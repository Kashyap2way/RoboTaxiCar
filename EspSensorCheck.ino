#include <NewPing.h>
#include <ESP32Servo.h>

// Define Ultrasonic Sensor Parameters
#define MAX_DISTANCE 400  // Max distance for ultrasonic sensors

// Define TRIG and ECHO pins for 6 sensors
#define TRIG_PIN_1 22
#define ECHO_PIN_1 23
#define TRIG_PIN_2 2
#define ECHO_PIN_2 4
#define TRIG_PIN_3 16
#define ECHO_PIN_3 17
#define TRIG_PIN_4 5
#define ECHO_PIN_4 18
#define TRIG_PIN_5 19
#define ECHO_PIN_5 21
#define TRIG_PIN_6 32
#define ECHO_PIN_6 35

// Define Servo and ESC pins
#define SERVO_PIN_1 13
#define SERVO_PIN_2 12
#define ESC_PIN_1 26
#define ESC_PIN_2 25

// Create sensor objects for the ultrasonic sensors
NewPing sonar1(TRIG_PIN_1, ECHO_PIN_1, MAX_DISTANCE);
NewPing sonar2(TRIG_PIN_2, ECHO_PIN_2, MAX_DISTANCE);
NewPing sonar3(TRIG_PIN_3, ECHO_PIN_3, MAX_DISTANCE);
NewPing sonar4(TRIG_PIN_4, ECHO_PIN_4, MAX_DISTANCE);
NewPing sonar5(TRIG_PIN_5, ECHO_PIN_5, MAX_DISTANCE);
NewPing sonar6(TRIG_PIN_6, ECHO_PIN_6, MAX_DISTANCE);

// Create Servo objects for the servos and ESCs
Servo servo1;
Servo servo2;
Servo esc1;
Servo esc2;

int pos = 0;  // Servo position variable

void setup() {
Serial.begin(115200);

// Allocate timers for ESP32 servo control
ESP32PWM::allocateTimer(0);
ESP32PWM::allocateTimer(1);
ESP32PWM::allocateTimer(2);
ESP32PWM::allocateTimer(3);

// Attach servos and ESCs to their respective pins with a standard 50 Hz period
servo1.setPeriodHertz(50);
servo2.setPeriodHertz(50);
esc1.setPeriodHertz(50);
esc2.setPeriodHertz(50);

servo1.attach(SERVO_PIN_1, 500, 2400);  // Adjust pulse width range as needed for servo
servo2.attach(SERVO_PIN_2, 500, 2400);
esc1.attach(ESC_PIN_1, 1000, 2000);     // Typical pulse width range for ESCs
esc2.attach(ESC_PIN_2, 1000, 2000);
}

void loop() {
// Sweep the servos between 0 and 180 degrees
for (pos = 80; pos <= 120; pos += 1) {
    servo1.write(pos);
    servo2.write(pos);
    delay(15);
}

for (pos = 120; pos >= 80; pos -= 1) {
    servo1.write(pos);
    servo2.write(pos);
    delay(15);
}

// Control ESCs with mapped values (example: control based on ultrasonic sensor input)
int distance1 = sonar1.ping_cm();
int distance2 = sonar2.ping_cm();
int distance3 = sonar3.ping_cm();
int distance4 = sonar4.ping_cm();
int distance5 = sonar5.ping_cm();
int distance6 = sonar6.ping_cm();
//int esc1Throttle = map(distance1, 0, 100, 1000, 2000);  // Map distance to throttle
//int esc2Throttle = map(distance2, 0, 100, 1000, 2000);

//esc1.writeMicroseconds(esc1Throttle);  // Set throttle for ESC 1
//esc2.writeMicroseconds(esc2Throttle);  // Set throttle for ESC 2

// Log distances for debugging
Serial.print("1: "); Serial.print(distance1); Serial.print(" cm, ");
Serial.print("2: "); Serial.print(distance2); Serial.print(" cm, ");
Serial.print("3: "); Serial.print(distance3); Serial.print("cm, ");
Serial.print("4: "); Serial.print(distance4); Serial.print(" cm, ");
Serial.print("5: "); Serial.print(distance5); Serial.print(" cm, ");
Serial.print("6: "); Serial.print(distance6); Serial.println("cm");

delay(100);  // Delay between updates
}
