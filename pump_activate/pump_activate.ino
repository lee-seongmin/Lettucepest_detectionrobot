#define IN3_PUMP 8        // 펌프 제어
#define IN4_PUMP 9        // 펌프 제어
#define ENB 10 

void setup()
{
  Serial.begin(9600); // 시리얼 모니터 사용
  pinMode(IN3_PUMP,OUTPUT);  //IN1을 OUTPUT으로
  pinMode(IN4_PUMP,OUTPUT);  //IN2를 OUTPUT으로
  pinMode(ENB, OUTPUT);
  digitalWrite(IN3_PUMP,LOW);   // 멈춤
  digitalWrite(IN4_PUMP,LOW);
}

void loop()
{
  char data;
  if(Serial.available())
  {
    data = Serial.read();
    // ""=> 포인터
    if(data=='o')  //만약 버튼의 값이 HIGH라면
    {
      Serial.println("정작동");
      digitalWrite(IN3_PUMP, HIGH);   // 정방향회전
      digitalWrite(IN4_PUMP, LOW);
      analogWrite(ENB, 255);
      delay(1000);
    }
    else if(data=='x')  //만약 버튼의 값이 HIGH라면
    {
      Serial.println("역작동");
      digitalWrite(IN4_PUMP, HIGH);   // 정방향회전
      digitalWrite(IN3_PUMP, LOW);
      analogWrite(ENB, 255);
      delay(1000);
    }
    else
    {
      Serial.println("멈춤");
      analogWrite(ENB, 0);
      digitalWrite(IN3_PUMP,LOW);   // 멈춤
      digitalWrite(IN4_PUMP,LOW);
      delay(1000);
    }
  }
}
