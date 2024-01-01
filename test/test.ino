#include <Servo.h> //Servo 라이브러리

Servo servo;      // Servo 클래스로 servo객체 생성

// 모터속도인가변수 중앙, 우측, 좌측 순
int spd_c=30; int spd_r=0; int spd_l=0; 

#define EN_MOTOR  6   // 모터의 Enable & PWM
#define IN1_MOTOR  5      // 구동 모터 제어선 IN1
#define IN2_MOTOR  4      // 구동 모터 제어선 IN2
#define JET_PUMP 8
#define IN3_PUMP 9 // 펌프 제어
#define IN4_PUMP 10
#define EN_PUMP 11 // 펌프 제어
#define ARD_DIST 3

//const int INTERRUPT_PIN = 2;  // 인터럽트 핀

int value = 45;
const int ANGLE = 45; // 다른 방향에서 직진으로 바뀔 때 돌아와야할 각도
int L_Line = A4;      // 왼쪽 라인트레이서 센서는 A4 핀에 연결
int C_Line = A2;      // 가운데 라인트레이서 센서는 A2 핀에 연결
int R_Line = A0;     // 오른쪽 라인트레이서 센서는 A0 핀에 연결

int SL = 0; int SC = 0; int SR = 0; // 라인트레이싱용 변수
int cnt=0; int cnt_r=0; int cnt_l=0;

int cnt_mov = 0;

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
  pinMode(IN3_PUMP,OUTPUT);
//  pinMode(ARD_DIST,OUTPUT);
  pinMode(JET_PUMP,INPUT);
    
  digitalWrite(IN4_PUMP,LOW);   // 멈춤
  digitalWrite(IN3_PUMP,LOW); 
  digitalWrite(IN2_MOTOR,HIGH);  // 모터 정방향 회전
  digitalWrite(IN1_MOTOR,LOW);
}  

void motor_speed(int spd)  
{  
  analogWrite(EN_MOTOR,spd);  // 모터 속도값 할당
}  

void pump_on(int JET, int cnt_mov)
{
  JET = digitalRead(JET_PUMP);
  Serial.print("젯슨나노");
  Serial.println(JET);
  Serial.print("정지횟수 : ");
  Serial.println(cnt_mov);
  if(JET==1 && cnt_mov==1)
  {
    motor_speed(0);
    delay(250);
    digitalWrite(EN_PUMP,HIGH);
    digitalWrite(IN3_PUMP, HIGH);
    digitalWrite(IN4_PUMP, LOW);
    analogWrite(EN_PUMP, 255);
    delay(750);          // 모터 상태 업데이트
    digitalWrite(EN_PUMP,LOW);
    digitalWrite(IN4_PUMP,LOW);   //정지
    digitalWrite(IN3_PUMP,LOW);
    delay(750);
    motor_speed(60);
    delay(250);
  }

  else if(cnt_mov==1 && JET==0)
  {
    motor_speed(0);
    delay(250);
    delay(750);           // 모터 상태 업데이트
    digitalWrite(EN_PUMP,LOW);
    digitalWrite(IN4_PUMP,LOW);   //정지
    digitalWrite(IN3_PUMP,LOW);
    delay(750);
    motor_speed(60);
    delay(250);
  }
  
  else if(cnt_mov==2 && JET==1)
  {
    motor_speed(0);
    delay(250);
    digitalWrite(EN_PUMP,HIGH);
    digitalWrite(IN3_PUMP, HIGH);
    digitalWrite(IN4_PUMP, LOW);
    analogWrite(EN_PUMP, 255);
    delay(750);          // 모터 상태 업데이트
    digitalWrite(EN_PUMP,LOW);
    digitalWrite(IN4_PUMP,LOW);   //정지
    digitalWrite(IN3_PUMP,LOW);
    delay(750);
    motor_speed(60);
    delay(250); 
  }
  else if(cnt_mov==2 && JET==0)
  {
    motor_speed(0);
    delay(250);
    delay(750);           // 모터 상태 업데이트
    digitalWrite(EN_PUMP,LOW);
    digitalWrite(IN4_PUMP,LOW);   //정지
    digitalWrite(IN3_PUMP,LOW);
    delay(750);
    motor_speed(60);
    delay(250);
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
  Serial.println(" 시작");
  
  int JET=0;
  if ( L == LOW && C == LOW && R == LOW ) // 0 0 0
  {           
    L = SL; C = SC; R = SR;
  }
  
  if(L==LOW && C==HIGH && R==LOW) // 0 1 0
  {
    servo.write(ANGLE); //value값의 각도로 회전. ex) value가 90이라면 90도 회전
//    jet_on(JET);
    Serial.println("직진");
    
    cnt+=1;
    if(cnt>6)
    {
      motor_speed(0);
      cnt=0;
      spd_c=30;
      cnt_mov++;
      pump_on(JET, cnt_mov);
    }
    
    spd_c+=6;
    if(spd_c > 54)
    {
      spd_c = 54; //54  
    }

    motor_speed(spd_c);
    delay(500);
//    pause(JET);
  }
  
  else if (L == LOW && R == HIGH ) // 0 0 1, 0 1 1 우
  {
    Serial.println("우회전");
    
    value+=15;
    if(value>ANGLE*2)
    {
      value = ANGLE*2;
    }

    servo.write(value); //value값의 각도로 회전. ex) value가 90이라면 90도 회전
    delay(20);
//    jet_on(JET);
    
    spd_r+=10;
    if(spd_r > 70)
    {
      spd_r=70;
    }
   
    motor_speed(spd_r);
    delay(80); //50

    if(cnt_r>10)
    {
      motor_speed(0);
      cnt_mov++;
      cnt_r=0;
      pump_on(JET, cnt_mov);
    }
  }
  
  else if (L == HIGH && R == LOW ) // 1 0 0, 1 1 0
  {                 
    Serial.println("좌회전");
    value-=15;
    if(value<0)
    {
      value = 0;
    }
    
    servo.write(value); //value값의 각도로 회전. ex) value가 90이라면 90도 회전
    delay(20);
//     jet_on(JET);
    cnt_l+=1;
    spd_l+=10; // 20
    if(spd_l > 70)
    {
      spd_l=70; //100
    }
    
    motor_speed(spd_l);
    delay(80); //50
    if(cnt_l>10)
    {
      motor_speed(0);
      cnt_l=0;
      cnt_mov++;
      pump_on(JET, cnt_mov);
    }
  }
  
  else if ( L == HIGH && C == HIGH && R == HIGH ) // 1 1 1
  {
    servo.write(ANGLE); //value값의 각도로 회전. ex) value가 90이라면 90도 회전
    Serial.println("정지");
    motor_speed(0);
    delay(100);
  }
  
  SL = L; SC = C; SR = R;
}  
