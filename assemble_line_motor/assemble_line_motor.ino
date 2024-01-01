#include <Servo.h> //Servo 라이브러리
#include <Wire.h> // I2C 라이브러리

// I2C 주소
#define I2C_ADDRESS 0x12 // I2C

Servo servo;      // Servo 클래스로 servo객체 생성

// 젯슨 나노로 부터 데이터 받았는지 확인하는 변수
int receivedData = -1; 

// 모터속도인가변수 중앙, 우측, 좌측 순
int spd_c=30; int spd_r=0; int spd_l=0; 

#define EN_MOTOR  6   // 모터의 Enable & PWM
#define IN1_MOTOR  5      // 구동 모터 제어선 IN1
#define IN2_MOTOR  4      // 구동 모터 제어선 IN2

#define JET_PUMP 8
#define IN4_PUMP 9        // 펌프 제어
#define EN_PUMP 10       // 펌프 제어

int value = 45;
const int ANGLE = 45; // 다른 방향에서 직진으로 바뀔 때 돌아와야할 각도
int L_Line = A4;      // 왼쪽 라인트레이서 센서는 A4 핀에 연결
int C_Line = A2;      // 가운데 라인트레이서 센서는 A2 핀에 연결
int R_Line = A0;     // 오른쪽 라인트레이서 센서는 A0 핀에 연결

int SL = 0; int SC = 0; int SR = 0; // 라인트레이싱용 변수
int cnt=0; int cnt_r=0; int cnt_l=0;

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
  pinMode(JET_PUMP,INPUT);
  
  digitalWrite(IN4_PUMP,LOW);   // 멈춤
  digitalWrite(IN2_MOTOR,HIGH);  // 모터 정방향 회전
  digitalWrite(IN1_MOTOR,LOW);
  Wire.begin(I2C_ADDRESS);  // I2C 통신 초기화
  Wire.onRequest(requestEvent);  // 데이터 요청 시 이벤트 핸들러 등록
  Wire.onReceive(receiveEvent); // 데이터 수신 시 이벤트 핸들러 등록
}  

// 데이터 요청 시 호출되는 이벤트 핸들러
void requestEvent() 
{
  // 데이터 전송
  byte data[] = {0x05};  // 전송할 데이터
  Wire.write(data, sizeof(data));  // 데이터 전송
}

// 데이터 수신 시 호출되는 이벤트 핸들러
void receiveEvent(int byteCount) 
{
  if(Wire.available()) 
  {
    // 데이터 읽기
    receivedData = Wire.read(); // 젯슨나노로 부터 받은 데이터 1임.
    Serial.println(receivedData);  // 받은 데이터 시리얼 모니터에 출력
    Serial.println("병충해감지정지");
    //motor_speed(0);
    delay(100);
  }
}

void motor_speed(int spd)  
{  
  analogWrite(EN_MOTOR,spd);  // 모터 속도값 할당
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

  //Wire.onReceive(receiveEvent);  // 데이터 수신 시 이벤트 핸들러 등록
  
  if ( L == LOW && C == LOW && R == LOW ) // 0 0 0
  {           
    L = SL; C = SC; R = SR;
  }
  
  if(L==LOW && C==HIGH && R==LOW) // 0 1 0
  {
    servo.write(ANGLE); //value값의 각도로 회전. ex) value가 90이라면 90도 회전

    if(receivedData<0) // 젯슨나노로 받아오는 receiveData가 1임
    {
      Serial.println("직진");
      
      cnt+=1;
      if(cnt>15)
      {
        motor_speed(0);
        cnt=0;
        spd_c=30;
        delay(3000);
      }
      
      spd_c+=2;
      if(spd_c > 60)
      {
        spd_c = 60; //54  
      }

      motor_speed(spd_c);
      delay(100);  
    }
    else
    {
      // 젯슨 나노에서 데이터 받아서 병충해 인식
      Serial.print("병충해");
      motor_speed(0);
      delay(100);
      
      digitalWrite(IN4_PUMP,HIGH);   // 정방향회전   
      receivedData=-1;
      delay(2000);

      // motor재가동
      digitalWrite(IN4_PUMP,LOW);   //정방향회전  
      delay(100);
      motor_speed(60);
      delay(100);
    }
  }
  
  else if (L == LOW && R == HIGH ) // 0 0 1, 0 1 1 우
  {        
    Serial.println("우회전");
    value+=2;
    if(value>ANGLE*2)
    {
      value = ANGLE*2;
    }

    servo.write(value); //value값의 각도로 회전. ex) value가 90이라면 90도 회전
    delay(20);
    
    cnt_r+=1;
    if(cnt_r>10)
    {
      motor_speed(0);
      cnt_r=0;
      delay(3000);
    }
    
    spd_r+=8;
    if(spd_r > 80)
    {
      spd_r=80;
    }
   
    motor_speed(spd_r);
    delay(80); //50
  }
  
  else if (L == HIGH && R == LOW ) // 1 0 0, 1 1 0
  {                 
    Serial.println("좌회전");

    value-=2;
    if(value<0)
    {
      value = 0;
    }
    
    servo.write(value); //value값의 각도로 회전. ex) value가 90이라면 90도 회전
    delay(20);
    
    cnt_l+=1;
    if(cnt_l>10)
    {
      motor_speed(0);
      cnt_l=0;
      delay(3000);
    }
    
    spd_l+=8;
    if(spd_l > 80)
    {
      spd_l=80;
    }
    
    motor_speed(spd_l);
    delay(80); //50
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
