#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Create PCA9685 object
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

const int escChannel1 = 0;  // ESC connected to channel 0
const int escChannel2 = 1; 
const int minPulseWidth = 600;  // 1000 µs (1 ms) corresponds to 0% throttle
const int maxPulseWidth = 2500;  // 2000 µs (2 ms) corresponds to 100% throttle

void setup() {
  Serial.begin(115200);
  
  // Initialize PCA9685 and set PWM frequency to 50 Hz
  pwm.begin();
  pwm.setPWMFreq(50);
  
  delay(10);  // Allow PCA9685 to initialize
  Serial.println("Starting Calibration for 2 sec");
  
  // Arm ESC by sending minimum throttle signal
  setESC(escChannel1, 1000);
  setESC(escChannel2, 1000);
  delay(2000);  // Give ESC time to arm
}

void loop() {

  // Set motor speed to 100%
  Serial.println("Going for Full Throtle");
  setESC(escChannel1, calculatePulseWidth(70));
  setESC(escChannel2, calculatePulseWidth(70));

  // Keep motor running at 100%
  while (true);
}

// Function to convert percentage to pulse width in microseconds
int calculatePulseWidth(int percent) {
  return map(percent, 0, 100, minPulseWidth, maxPulseWidth);
}

// Function to send PWM signal to ESC
void setESC(int channel, int pulseWidth) {
  int pulseLength = map(pulseWidth, 0, 20000, 0, 4096);  // Convert to 12-bit value
  pwm.setPWM(channel, 0, pulseLength);
}
