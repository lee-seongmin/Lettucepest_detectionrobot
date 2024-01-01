#include <Servo.h>

Servo servo;  // 서보 객체 생성

int servoPin = 9;  // 서보 모터의 신호선을 연결한 아두이노 핀 번호

int angle = 0;    // 서보 모터의 각도 변수

void setup() {
  servo.attach(servoPin);  // 서보 모터 핀 설정
}

void loop() {
  // 0도부터 180도까지 회전
  for (angle = 0; angle <= 180; angle += 5) {
    servo.write(angle);   // 서보 모터를 지정한 각도로 회전
    delay(50);            // 50ms 동안 대기
  }

  // 180도부터 0도까지 회전
  for (angle = 180; angle >= 0; angle -= 5) {
    servo.write(angle);   // 서보 모터를 지정한 각도로 회전
    delay(50);            // 50ms 동안 대기
  }
}
