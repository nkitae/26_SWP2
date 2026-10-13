#include <Servo.h>
#include <math.h>

#define PIN_SERVO 10
#define PIN_TRIG  12
#define PIN_ECHO  13

#define SND_VEL 346.0

// 차량 감지 거리 (cm)
#define DETECT_DISTANCE 20.0

// 서보모터 각도
#define DOWN_ANGLE 0
#define UP_ANGLE   90

// 차단기 이동 시간 (ms)
#define MOVING_TIME 2000

Servo myServo;

unsigned long moveStartTime = 0;

int startAngle = DOWN_ANGLE;
int stopAngle = DOWN_ANGLE;

bool isMoving = false;
bool isOpen = false;


// 초음파 센서로 거리 측정
float measureDistance() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long duration = pulseIn(PIN_ECHO, HIGH, 25000);

  // 초음파 신호를 받지 못한 경우
  if (duration == 0) {
    return -1.0;
  }

  // 왕복 시간을 이용해 거리 계산
  float distance = duration * SND_VEL / 20000.0;

  return distance;
}


// Sigmoid 함수
/*float sigmoid(float x) {
  return 1.0 / (1.0 + exp(-x));
}*/

// 3차 스무스 스텝 함수
float smoothstep(float t) {
  return 3.0 * t * t - 2.0 * t * t * t;
}



// 차단기 부드럽게 움직이기
void moveBarrier(int targetAngle) {
  startAngle = myServo.read();
  stopAngle = targetAngle;

  moveStartTime = millis();
  isMoving = true;
}


void setup() {
  Serial.begin(9600);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  myServo.attach(PIN_SERVO);
  myServo.write(DOWN_ANGLE);

  delay(500);
}


void loop() {
  float distance = measureDistance();

  Serial.print("Distance: ");
  Serial.println(distance);

  bool vehicleDetected =
      (distance > 0 && distance <= DETECT_DISTANCE);

  if (vehicleDetected && !isOpen) {
    moveBarrier(UP_ANGLE);
    isOpen = true;
  }

  else if (!vehicleDetected && isOpen) {
    moveBarrier(DOWN_ANGLE);
    isOpen = false;
  }


  if (isMoving) {
    unsigned long elapsed = millis() - moveStartTime;

    float t = (float)elapsed / MOVING_TIME;

    if (t >= 1.0) {
      myServo.write(stopAngle);
      isMoving = false;
    } else {
      // sigmoid
      /*float x = 12.0 * t - 6.0;

      float s0 = sigmoid(-6.0);
      float s1 = sigmoid(6.0);

      float s = (sigmoid(x) - s0) / (s1 - s0); */

      float s = smoothstep(t);

      // 시작 각도에서 목표 각도까지 이동
      float angle =
          startAngle + (stopAngle - startAngle) * s;

      myServo.write((int)angle);
    }
  }

  delay(30);
}
