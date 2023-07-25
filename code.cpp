#include <Adafruit_LiquidCrystal.h>
#include <Servo.h>



Adafruit_LiquidCrystal lcd(0x20);
Servo gateServo;


// Button and Force Sensors pins
const int buttonPin = 2;
int buttonState = 0;

//int forceSensorPin = 4;
#define FORCESENSORPIN A0
int forceSensorReading = 0;

// Ultrasonic sensor 1 pins
int trigUltrasonic1Pin = 10;
int echoUltrasonic1Pin = 11;
long durationUltrasonic1, inchesUltrasonic1;
int isUltrasonic1Active = 0;

// Ultrasonic sensor 2 pins
int trigUltrasonic2Pin = 8;
int echoUltrasonic2Pin = 9;
long durationUltrasonic2, inchesUltrasonic2;
int isUltrasonic2Active = 0;

int carCount = 0;
int isGateOpen = 0;
int initialCarCount;

void setup()
{
  Serial.begin(9600);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.print("Cars in the park");

  // Generate random beginning car count
  initialCarCount = random(100);
  lcd.setCursor(0, 1);
  lcd.print(initialCarCount);

  // Configure sensors and servo
  configureUltrasonicSensors();
  configureButton();
  configureGateServo();
}

void loop()
{
  // Read button state and force sensor value
  buttonState = digitalRead(buttonPin);
  forceSensorReading = analogRead(FORCESENSORPIN);

  // Measure distances using ultrasonic sensors
  measureDistance(trigUltrasonic1Pin, echoUltrasonic1Pin, durationUltrasonic1, inchesUltrasonic1);
  measureDistance(trigUltrasonic2Pin, echoUltrasonic2Pin, durationUltrasonic2, inchesUltrasonic2);

  // Check conditions and perform actions accordingly
  if (shouldOpenGate())
  {
    openGate();
  }
  else if (isCarApproachingUltrasonic2())
  {
    isUltrasonic2Active = 1;
  }
  else if (hasCarPassedSensors())
  {
    closeGate();
    updateCarCount();
  }
  else if (shouldCloseGate())
  {
    closeGate();
  }
}

void configureUltrasonicSensors()
{
  // Configure ultrasonic sensor pins
  pinMode(trigUltrasonic1Pin, OUTPUT);
  pinMode(echoUltrasonic1Pin, INPUT);
  pinMode(trigUltrasonic2Pin, OUTPUT);
  pinMode(echoUltrasonic2Pin, INPUT);
}

void configureButton()
{
  // Configure button pin
  pinMode(buttonPin, INPUT);
}

void configureGateServo()
{
  // Attach servo and set initial position
  gateServo.attach(3);
  gateServo.write(0);
}

void measureDistance(int trigPin, int echoPin, long& duration, long& inches)
{
  // Measure distance using ultrasonic sensor
  // Generate an ultrasonic pulse
  digitalWrite(trigPin, HIGH); // Set the trig pin to HIGH
  delayMicroseconds(10); // Wait for 10 microseconds
  digitalWrite(trigPin, LOW); // Set the trig pin back to LOW

  // Measure the duration of the echo pulse
  pinMode(echoPin, INPUT); // Set the echo pin as INPUT
  duration = pulseIn(echoPin, HIGH); // Measure the duration of the pulse with HIGH level on the echo pin

  // Convert the duration to inches
  inches = (duration / 2) / 74;  // Calculate the distance in inches using the duration of the echo pulse

}

bool shouldOpenGate()
{
  // Check if gate should be opened
  float forceReading = forceSensorReading * (5.0 / 1023.0); // Convert analog reading to voltage (assuming 5V reference)
  float forceInNewton = forceReading * 1.96; // Convert voltage to force in newtons (calibration factor: 1V = 1.96 N)

  return (inchesUltrasonic1 < 125 && forceSensorReading > 80 && buttonState == HIGH);
}

bool isCarApproachingUltrasonic2()
{
  // Check if a car is approaching Ultrasonic Sensor 2
  return (inchesUltrasonic1 >= 125 && inchesUltrasonic2 < 125);
}

bool hasCarPassedSensors()
{
  // Check if a car has passed both ultrasonic sensors
  return (inchesUltrasonic1 >= 125 && inchesUltrasonic2 >= 125 && isGateOpen == 1 && isUltrasonic2Active == 1);
}

bool shouldCloseGate()
{
  // Check if gate should be closed
  return (inchesUltrasonic1 >= 125 && inchesUltrasonic2 >= 125 && isUltrasonic2Active == 0);
}

void openGate()
{
  // Open the gate by moving the servo to 90 degrees
  gateServo.write(90);
  isGateOpen = 1;
  isUltrasonic2Active = 0;
}

void closeGate()
{
  // Close the gate by moving the servo to 0 degrees
  gateServo.write(0);
}

void updateCarCount()
{
  // Increment the car count, update the LCD display, and reset variables
  carCount++;
  isGateOpen = 0;
  isUltrasonic2Active = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cars in the park");
  lcd.setCursor(0, 1);
  lcd.print(carCount + initialCarCount);
}
