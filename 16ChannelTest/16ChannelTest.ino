#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SDA_1 13 // For MPU
#define SCL_1 12
TwoWire I2Cone = TwoWire(0);

// Servo minimum and maximum pulse lengths (these will vary with your servos)
#define SERVOMIN  150  // Minimum pulse length count (corresponds to 0 degrees)
#define SERVOMAX  600  // Maximum pulse length count (corresponds to 180 degrees)

// Servo channels
#define SERVO_1_CHANNEL 0  // Servo 1 connected to PCA9685 channel 0
#define SERVO_2_CHANNEL 4  // Servo 2 connected to PCA9685 channel 1
#define SERVO_3_CHANNEL 8  // Servo 3 connected to PCA9685 channel 2
#define SERVO_4_CHANNEL 12  // Servo 4 connected to PCA9685 channel 3

// Function to convert angle to pulse length for servos
int angleToPulse(int angle) {
  int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  return pulse;
}

// our servo # counter
uint8_t servonum = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Adafruit MPU6050 test!");

  I2Cone.begin(SDA_1, SCL_1, 100000); 
  // Initialize the PWM driver (without the second argument)
  bool status1 = pwm.begin();
  if (!status1) {
    Serial.println("Failed to find 16Channel chip");
    while (1) {
      delay(10);
    }
  }

  Serial.println("16 channel Servo test!");
  
  // Set the PWM frequency to 60 Hz
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
}

void loop() {
  // Slowly sweep all four servos from 0 to 180 degrees
  for (int angle = 0; angle <= 180; angle++) {
    int pulselen1 = angleToPulse(angle);
    int pulselen2 = angleToPulse(angle);
    int pulselen3 = angleToPulse(angle);
    int pulselen4 = angleToPulse(angle);
    
    // Set the pulse length for all four servos
    pwm.setPWM(SERVO_1_CHANNEL, 0, pulselen1);
    pwm.setPWM(SERVO_2_CHANNEL, 0, pulselen2);
    pwm.setPWM(SERVO_3_CHANNEL, 0, pulselen3);
    pwm.setPWM(SERVO_4_CHANNEL, 0, pulselen4);

    // Display the current angle
    Serial.print("Servos at angle: ");
    Serial.println(angle);

    delay(20);  // Adjust delay to control the speed of the movement
  }

  // Slowly sweep all four servos back from 180 to 0 degrees
  for (int angle = 180; angle >= 0; angle--) {
    int pulselen1 = angleToPulse(angle);
    int pulselen2 = angleToPulse(angle);
    int pulselen3 = angleToPulse(angle);
    int pulselen4 = angleToPulse(angle);
    
    // Set the pulse length for all four servos
    pwm.setPWM(SERVO_1_CHANNEL, 0, pulselen1);
    pwm.setPWM(SERVO_2_CHANNEL, 0, pulselen2);
    pwm.setPWM(SERVO_3_CHANNEL, 0, pulselen3);
    pwm.setPWM(SERVO_4_CHANNEL, 0, pulselen4);

    // Display the current angle
    Serial.print("Servos at angle: ");
    Serial.println(angle);

    delay(20);  // Adjust delay to control the speed of the movement
  }
}


