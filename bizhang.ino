int period=3;//步骤控制
#include<stdio.h>

//-------------------------------------------跑马灯
#include <Adafruit_NeoPixel.h>
#define PIN 10   
#define PIN_NUM 2 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIN_NUM, PIN, NEO_GRB + NEO_KHZ800); 

//------------------------------------------红外测距传感器
#include <Wire.h> 
#include<math.h>
int GP2D12 = 0;//把夏普GP2D12红外测距传感器连接在A0
int val; //存储从GP2D12红外测距传感器读到的值 
float temp;//存储由传感器读取值，通过计算处理后的浮点型距离值
//当前距离
int mDistance = 0;

//-----------------------------------------------舵机
#include <Servo.h> 
Servo myServo;  //创建一个舵机控制对象
int mAngleNum = 0;//当前角度
char mFront = 0;//当前是正向旋转还是反向旋转
#define servo_pin SDA//I2C

//--------------------------------------------------红外
#include<IRremote.h>
#define IR_PIN 12
decode_results results;
IRrecv irrecv(IR_PIN);
#define irhead 33464415
#define irback 33478695
#define irleft 33480735
#define irright 33460335
#define period_1  33456255
#define period_2 33439935


//-----------------------------------------电机部分
//D6，D8控制1A，1B的电机
#define OUT1A 6
#define OUT1B 8
//D5，D7控制2A，2B的电机
#define OUT2A 5
#define OUT2B 7

//------------小车避障
int rightDistance=0,leftDistance = 0,middleDistance=0;

//--------------------------------------------小车方向控制
void left()
{
  digitalWrite(OUT1A, HIGH);
  digitalWrite(OUT1B, LOW);
  digitalWrite(OUT2A, HIGH);
  digitalWrite(OUT2B, LOW);
}
void right()
{
  digitalWrite(OUT1A, LOW);
  digitalWrite(OUT1B, HIGH);
  digitalWrite(OUT2A, LOW);
  digitalWrite(OUT2B, HIGH);
}

void back()
{
 digitalWrite(OUT1A,HIGH);
 digitalWrite(OUT1B,LOW);
 digitalWrite(OUT2A,LOW);
 digitalWrite(OUT2B,HIGH);
}

void head()
{
 digitalWrite(OUT1A,LOW);
 digitalWrite(OUT1B,HIGH);
 digitalWrite(OUT2A,HIGH);
 digitalWrite(OUT2B,LOW);
}

void STOP()
{
 digitalWrite(OUT1A,LOW);
 digitalWrite(OUT1B,LOW);
 digitalWrite(OUT2A,LOW);
 digitalWrite(OUT2B,LOW);
}

//--------------------------------------------------红外测距
float  Distance_test()
{
val = analogRead(GP2D12); 
//通过以下算式，把传感器读取值处理成浮点型距离值
temp=14629/(pow(float(val),1.082));
if(temp<20||temp>150)//---------传感器有效范围是20-150cm
temp=0;

mDistance=temp;
 return (temp);
}
//------------------------------------------避障前进
  void period2()
{
 myServo.write(90);//初始化
 delay(500); 
 middleDistance = Distance_test();
     
    Serial.print("middleDistance=");
    Serial.println(middleDistance);
 
    if(middleDistance<=30)
    {     
      STOP();
      delay(300);                         
      myServo.write(5);          //左
      delay(1000);      
      rightDistance = Distance_test();
      Serial.print("rightDistance=");
      Serial.println(rightDistance);
      delay(300);
       myServo.write(90);            //复位  
      delay(1000);                                                  
      myServo.write(175);              //右
      delay(1000); 
      leftDistance = Distance_test();
      Serial.print("leftDistance=");
      Serial.println(leftDistance);
      delay(300);
      myServo.write(90);              
      delay(1000);
      if(rightDistance>leftDistance&&rightDistance>30)  
      {
        right();
        delay(300);
        head();
        delay(300);
       }
       else if(rightDistance<leftDistance&&leftDistance>30)
       {
        left();
        delay(300);
        head();
        delay(300);
       }
       else
       {
        back();
        delay(300);
        STOP();
       }
    }  
    else
      head();
      delay(300);
      STOP();                     
     period=3;
} 

  void period3()//红外遥控
  {
      
      if (irrecv.decode(&results)){
    Serial.println(results.value,DEC);
  switch(results.value)
  {
  
  case irhead:
  strip.setPixelColor(0, strip.Color(0, 0, 255));//情况1：当按下前进按钮时，左右两个LED灯同时亮起蓝灯（安装的时候注意，左侧的为0，右侧的为1）
  strip.setPixelColor(1, strip.Color(0, 0, 255));
  strip.show();  
  delay(300);
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show(); 
  strip.setPixelColor(1, strip.Color(0, 0, 0));
  strip.show();  
  head();
  Serial.println("HEAD");
  delay(300);
  STOP();
  break;

   case irback:
    strip.setPixelColor(0, strip.Color(255, 0, 0));//情况2：当按下后退按钮时，左右两个LED灯同时亮起红灯，以提示车后的行人注意（红灯是比较醒目的颜色）
  strip.setPixelColor(1, strip.Color(255, 0, 0));
  strip.show();   
  delay(300);
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show();  
  strip.setPixelColor(1, strip.Color(0, 0, 0));
  strip.show();  
  back();
  Serial.println("BACK");
  delay(300);
  STOP();
  break;

 case irleft:
  strip.setPixelColor(1, strip.Color(0, 255, 0));
  strip.show();   
  delay(100);
  strip.setPixelColor(1, strip.Color(0, 0, 0));
  strip.show();
  delay(100);  
   strip.setPixelColor(1, strip.Color(0, 255, 0));
  strip.show();   
  delay(100);
  strip.setPixelColor(1, strip.Color(0, 0, 0));
  strip.show();  
  left();
  Serial.println("LEFT");
  delay(300);
  STOP();
  break;

   case irright: 
  strip.setPixelColor(0, strip.Color(0, 255, 0)); //情况4：当按下右拐的按钮时，小车右侧亮绿灯，左侧不亮灯
  strip.show();                                
  delay(100);
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show();
  delay(100);  
   strip.setPixelColor(0, strip.Color(0, 255, 0));
  strip.show();   
  delay(100);
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show();  
  right();
  Serial.println("RIGHT");
  delay(300);
  STOP();
  break;

  case period_1:
  period=1;
  strip.setPixelColor(0, strip.Color(100, 100, 100));
  strip.setPixelColor(1, strip.Color(100, 100, 100));
  strip.show();  
  delay(300);
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show(); 
  strip.setPixelColor(1, strip.Color(0, 0, 0));
  strip.show();  
  break;
  
  case period_2:
  period=2;
  strip.setPixelColor(0, strip.Color(200, 200, 200));
  strip.setPixelColor(1, strip.Color(200, 200, 200));
  strip.show();  
  delay(300);
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show(); 
  strip.setPixelColor(1, strip.Color(0, 0, 0));
  strip.show();  
  break;
  }
  
  irrecv.resume();
  }
    }

void setup()
{
  pinMode(OUT1A, OUTPUT);//驱动
  pinMode(OUT1B, OUTPUT);
  pinMode(OUT2A, OUTPUT);
  pinMode(OUT2B, OUTPUT);
  
   Serial.begin(9600);  // 初始化串口通信
    
        irrecv.enableIRIn();//启动遥控器

  pinMode(A0, INPUT);//红外
  
   myServo.attach(servo_pin); //舵机
   myServo.write(90);//舵机初始化
   
    strip.begin();   //跑马灯
}

void loop()
{
switch (period)
{
  //case 1:
  //period1();
  //break;
  
  case 2:
  {
  period2();
  break;
  }

  case 3:
  {
    period3();
  break;
  }
  }
}
