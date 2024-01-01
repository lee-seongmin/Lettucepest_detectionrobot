#include <Servo.h> //Servo 라이브러리

Servo servo;      // Servo 클래스로 servo객체 생성
int value = 0;    // 각도를 조절할 변수 value

// 모터속도인가변수 중앙, 우측, 좌측 순
int spd_c=0; int spd_r=0; int spd_l=0; 

#define EN_MOTOR  6   // 모터의 Enable & PWM
#define IN1_MOTOR  5      // 구동 모터 제어선 IN1
#define IN2_MOTOR  4      // 구동 모터 제어선 IN2
#define IN4_PUMP 9        // 펌프 제어
#define EN_PUMP 10       // 펌프 제어
#define JETSON_PIN 8

const int ANGLE = 45; // 다른 방향에서 직진으로 바뀔 때 돌아와야할 각도
int L_Line = A0;      // 왼쪽 라인트레이서 센서는 A0 핀에 연결
int C_Line = A1;      // 가운데 라인트레이서 센서는 A1 핀에 연결
int R_Line = A2;      // 오른쪽 라인트레이서 센서는 A2 핀에 연결

int SL = 1; int SC = 1; int SR = 1; // 라인트레이싱용 변수

void setup()  
{ 
  Serial.begin(9600); // 시리얼 모니터 사용
  servo.attach(7);     //맴버함수인 attach : 핀 설정 digital 7
  servo.write(ANGLE); // 정면을 바라보는 각도값
  pinMode(EN_MOTOR,OUTPUT); // 모터 출력을 위한 설정
  pinMode(IN1_MOTOR,OUTPUT); // 모터 방향을 위한 설정
  pinMode(IN2_MOTOR,OUTPUT); // 모터 방향을 위한 설정
  pinMode(EN_PUMP,OUTPUT);  // 펌프 출력을 위한 설정
  pinMode(IN4_PUMP,OUTPUT);  // 펌프 출력을 위한 설정
  pinMode(JETSON_PIN, INPUT);
  digitalWrite(IN4_PUMP,LOW);   // 멈춤
}  

void motor_speed(int spd)  
{  
  analogWrite(EN_MOTOR,spd);  // 모터 속도값 할당
}  

void motor_dir(int dir)  
{
  if (dir == 0) // forward 
  {    
     digitalWrite(IN2_MOTOR,HIGH);  // 모터 정방향 회전
     digitalWrite(IN1_MOTOR,LOW);    
  }    
}  

void loop()  
{ 
  int L = digitalRead(L_Line);
  int C = digitalRead(C_Line);
  int R = digitalRead(R_Line);

  Serial.print("digital L : "); 
  Serial.print(L); 
  Serial.print(" C : "); 
  Serial.print(C); 
  Serial.print(" R : "); 
  Serial.print(R); 
  Serial.print("   ");
  Serial.println("시작");
  //Wire.onReceive(receiveEvent);  // 데이터 수신 시 이벤트 핸들러 등록

  if (digitalRead(JETSON_PIN)==true)
  {
    // 젯슨 나노에서 데이터 받아서 병충해 인식
    Serial.print("병충해");
    motor_speed(0);
    delay(100);
    digitalWrite(EN_PUMP, HIGH); // pump 가동
    digitalWrite(IN4_PUMP, HIGH);
    delay(2000);
    digitalWrite(EN_PUMP, LOW); // pump 끄기
    digitalWrite(IN4_PUMP, LOW);
  }
  
  if ( L == LOW && C == LOW && R == LOW ) // 0 0 0
  {           
    motor_dir(0); // 정방향 회전
    L = SL; C = SC; R = SR;
  }
  
  if ( L == LOW && C == HIGH && R == LOW ) // 0 1 0
  {
    servo.write(ANGLE); //value값의 각도로 회전. ex) value가 90이라면 90도 회전
    motor_dir(0); //정방향 회전
    Serial.println("직진");
    spd_c+=3;
    if(spd_c > 54)
    {
      spd_c = 54;        
    }

    motor_speed(spd_c);
    delay(100);
  }

  else if (L == LOW && R == HIGH ) // 0 0 1, 0 1 1 우
  {        
  //    Serial.println("우회전");
    
    if (value < ANGLE)
    {
      servo.write(ANGLE);
    }     
    value+=2;
    
    if(value > ANGLE+6)
    {
      value = ANGLE+6;
    }

    servo.write(value); //value값의 각도로 회전. ex) value가 90이라면 90도 회전
    delay(20);
    
    spd_r+=25;
    if(spd_r > 125)
    {
      spd_r=125;
    }
    
    motor_speed(spd_r);
    delay(50);
  }
  
  else if (L == HIGH && R == LOW ) // 1 0 0, 1 1 0
  {                 
    Serial.println("좌회전");
    motor_dir(0);
    if (value < ANGLE)
    {
      servo.write(ANGLE);
    }     
    
    value-=2;
    if(value < -ANGLE-6)
    {
      value=-(ANGLE+6);
    }
    
    servo.write(value); //value값의 각도로 회전. ex) value가 90이라면 90도 회전
    delay(20);
    
    spd_l+=25;
    if(spd_l > 125)
    {
      spd_l=125;
    }
    
    motor_speed(spd_l);
    delay(50);
  }
  
  else if ( L == HIGH && C == HIGH && R == HIGH ) // 1 1 1, 1 0 1
  {
    motor_dir(0);
    Serial.println("정지");
    motor_speed(0);
    delay(100);
  }
    
  SL = L; SC = C; SR = R;
}  
