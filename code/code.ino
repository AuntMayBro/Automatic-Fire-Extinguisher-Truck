#include <Servo.h>

// ================= PIN CONFIGURATION =================

// Motor Driver (L298N)
#define enA 10
#define in1 9
#define in2 8
#define in3 7
#define in4 6
#define enB 5

// Flame Sensors
#define ir_R A0
#define ir_F A1
#define ir_L A2

// Servo & Pump
#define SERVO_PIN A4
#define PUMP_PIN A5

// ================= VARIABLES =================
int speedMotor = 160;  
int s1, s2, s3;

Servo waterServo;

// ================= SETUP =================
void setup() {
  Serial.begin(9600);

  pinMode(ir_R, INPUT);
  pinMode(ir_F, INPUT);
  pinMode(ir_L, INPUT);

  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enB, OUTPUT);

  pinMode(PUMP_PIN, OUTPUT);

  waterServo.attach(SERVO_PIN);

  analogWrite(enA, speedMotor);
  analogWrite(enB, speedMotor);

  // Initial servo sweep (calibration)
  for (int angle = 90; angle <= 140; angle += 5)
    waterServo.write(angle);

  for (int angle = 140; angle >= 40; angle -= 5)
    waterServo.write(angle);

  for (int angle = 40; angle <= 90; angle += 5)
    waterServo.write(angle);

  delay(500);
}

// ================= MAIN LOOP =================
void loop() {
  readSensors();
  printSensorValues();
  fireLogic();
  delay(10);
}

// ================= SENSOR FUNCTIONS =================
void readSensors() {
  s1 = analogRead(ir_R);
  s2 = analogRead(ir_F);
  s3 = analogRead(ir_L);
}

void printSensorValues() {
  Serial.print(s1); Serial.print("\t");
  Serial.print(s2); Serial.print("\t");
  Serial.println(s3);
}

// ================= FIRE DETECTION LOGIC =================
void fireLogic() {

  // Fire detected on RIGHT
  if (s1 < 250) {
    stopMotors();
    sweepServo(90, 40);
    sweepServo(40, 90);
    moveBackward();
    delay(100);
  }

  // Fire detected in FRONT
  else if (s2 < 350) {
    stopMotors();
    sweepServo(90, 140);
    sweepServo(140, 40);
    sweepServo(40, 90);
    moveBackward();
    delay(100);
  }

  // Fire detected on LEFT (EXTINGUISH)
  else if (s3 < 250) {
    stopMotors();
    digitalWrite(PUMP_PIN, HIGH);
    sweepServo(90, 140);
    sweepServo(140, 90);
  }

  // Searching logic
  else if (s1 >= 251 && s1 <= 800) {
    digitalWrite(PUMP_PIN, LOW);
    turnRight();
    delay(200);
  }

  else if (s2 >= 251 && s2 <= 800) {
    digitalWrite(PUMP_PIN, LOW);
    turnLeft();
    delay(200);
  }

  else if (s3 >= 251 && s3 <= 800) {
    digitalWrite(PUMP_PIN, LOW);
    moveForward();
  }

  // No fire
  else {
    digitalWrite(PUMP_PIN, LOW);
    stopMotors();
  }
}

// ================= SERVO FUNCTION =================
void sweepServo(int startAngle, int endAngle) {
  if (startAngle < endAngle) {
    for (int angle = startAngle; angle <= endAngle; angle += 3) {
      waterServo.write(angle);
      delay(20);
    }
  } else {
    for (int angle = startAngle; angle >= endAngle; angle -= 3) {
      waterServo.write(angle);
      delay(20);
    }
  }
}

// ================= MOTOR FUNCTIONS =================
void moveForward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void moveBackward() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void turnRight() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void turnLeft() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void stopMotors() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}