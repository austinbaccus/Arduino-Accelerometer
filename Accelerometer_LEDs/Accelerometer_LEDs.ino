#include <SD.h>
#include <Wire.h>

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;
int potPin = 2;

void setup() 
{
  Serial.begin(9600);
  Wire.begin();         
  setupMPU();

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(13, OUTPUT);
  
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(8, LOW);
  digitalWrite(13, LOW);
}
 
void loop() 
{
  recordAccelRegisters();
  float gForce = getAccelMagnitude();
  handleLEDs(gForce);
}

// returns the magnitude of the acceleration (using the pythagorean theorem)
float getAccelMagnitude()
{
  gForceX = accelX / 16384.0; // 16384.0 is the conversion factor to get a digital reading from an analog one. 
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
  float gForceMagnitude = sqrt(gForceX * gForceX + gForceY * gForceY + gForceZ * gForceZ); // sqrt(x^2 + y^2 + z^2)
  Serial.print(gForceMagnitude);
  Serial.print("\n");
  return gForceMagnitude;
}

void handleLEDs(float gForce)
{
  // There are 5 LEDs on the breadboard, two green, two yellow, and one red (in that order).
  // These LEDs turn on when you accelerate the device. The more acceleration, the more LEDs that will be turned on.
  // This method calculates how many LEDs to turn on.
  
  int potentiometerVal = analogRead(potPin);    // read the value from the sensor
  
  // processedGForce is a value that makes it easier to determine how many LEDs to light up. 
  // This takes the absolute value of the g-force so that you don't have to deal with both negative and positive magnitudes and making if-statements for both cases.
  // This also subtracts 1 from that g-force value, because when the accelerometer is at rest it is always experiencing 1 g due to gravity). But, we don't want the LEDs to be lit up all the time when it's not moving, so this subtracts 1 and makes the default value 0 when at rest.
  // Finally, this multiplies the g-force value by the voltage of the potentiometer. The potentiometer value is the LED's sensitivity setting, with the potentiometer controlling how much acceleration is needed for the LEDs to light up.
  float processedGForce = (abs(gForce) - 1) * ((double)potentiometerVal / 1025.0); 
  
  if (processedGForce > 0.1) digitalWrite(9, HIGH); // if processed g's > 0.1, light up the first green LED
  else digitalWrite(9, LOW);
  
  if (processedGForce > 0.2) digitalWrite(10, HIGH); // if processed g's > 0.2, light up the second green LED
  else digitalWrite(10, LOW);
  
  if (processedGForce > 0.3 )digitalWrite(11, HIGH); // if processed g's > 0.3, light up the first yellow LED
  else digitalWrite(11, LOW);
  
  if (processedGForce > 0.4) digitalWrite(8, HIGH); // if processed g's > 0.4, light up the second yellow LED
  else digitalWrite(8, LOW);
  
  if (processedGForce > 0.5) digitalWrite(13, HIGH); // if processed g's > 0.5, light up the red LED
  else digitalWrite(13, LOW);
}

void recordAccelRegisters()
{
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
  
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
}

void setupMPU()
{
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();  
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
  Wire.endTransmission(); 
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission(); 
}
