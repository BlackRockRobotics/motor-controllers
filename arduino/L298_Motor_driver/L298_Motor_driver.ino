/* L298N DC motor example */

int IN1=8;
int IN2=9;
int ENA=3;
void setup()
{
 pinMode(IN1,OUTPUT);
 pinMode(IN2,OUTPUT);  
}
void loop()
{  
 analogWrite(ENA, 200);// motor speed  
 digitalWrite(IN1,LOW);// rotate forward
 digitalWrite(IN2,HIGH);
 delay(2000);  
 digitalWrite(IN1,HIGH);// rotate reverse
 digitalWrite(IN2,LOW);
 delay(2000);
}
