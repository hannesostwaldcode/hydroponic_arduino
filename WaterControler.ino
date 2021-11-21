//Water Level Values
int Liquid_level=0;
int State=-1;

//Mhz Sender Switch
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();



//Neo Pixel Stuff

#include <NeoPixelBus.h>

const uint16_t PixelCount = 4;
const uint8_t PixelPin = 3; 
#define colorSaturation 128

//Humidity and Temp Sensor
#include "DHT.h"
#define DHTPIN 12

#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);


NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

//Color Configs
RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

//Buzzer
int buzzer = 8;
int waterLevel = 6;

//Servos/Pump for Nutrients
#include <Servo.h>
Servo servo;
Servo servo2;
int GoalSalinity = 10000;
int UnderBorder = 5000;

//Prototypes
float Frequenzcounter () ;
void MakeBuzz (int);
void CheckTemp ();
void CheckSalinity();
void CheckWaterLevel();


void setup(){
  Serial.begin(9600);
  
  dht.begin();
  
  strip.Begin();
  strip.SetPixelColor(1, blue);
  strip.Show();
  
  pinMode(waterLevel,INPUT);
  
  pinMode(buzzer, OUTPUT);
  mySwitch.setPulseLength(301);
  mySwitch.enableTransmit(7);
}

void loop(){
 
  
unsigned long runner = 0;
unsigned long LoopLength = 12*3600*1000;
while (runner < LoopLength)
{
  Serial.println("Loop 1");
  unsigned long start = millis();
  Serial.println(runner < LoopLength);
  CheckTemp();
  CheckWaterLevel();
  delay(30*1000);
  runner = runner+(millis()-start);
  Serial.print("Runner: "); Serial.println(runner);
}

Serial.println("Loop 2");
CheckSalinity();

}

void CheckSalinity(){
  strip.SetPixelColor(2, red);
  strip.Show();
  float frqenz = Frequenzcounter();
  Serial.println(frqenz);
  if (frqenz <= UnderBorder){
    strip.SetPixelColor(2, white);
    strip.Show();
    while (frqenz < GoalSalinity)
    {
      servo.attach(9);
      servo2.attach(10);
      
      
      servo.write(720);
      
      delay(2000);

      servo2.write(360);
      delay(2000);
      //Wait for Salinity Rise
      delay(30000);
      frqenz = Frequenzcounter();
      //DEBUG
      frqenz = 20000;
    }
    strip.SetPixelColor(2, black);
    strip.Show();
    
    
}

}

void CheckWaterLevel() {
Liquid_level=digitalRead(waterLevel);
if (Liquid_level != State){
  strip.SetPixelColor(1, white);
  strip.Show();
  float temp_liquidLevel = 0;
  for(int i=0; i<10; i++){
  temp_liquidLevel = temp_liquidLevel+digitalRead(waterLevel);
  delay(500);
  }
  temp_liquidLevel = temp_liquidLevel/10;
  Serial.print("Liquid_Calc= ");Serial.println(temp_liquidLevel);
  if (temp_liquidLevel <= 0.5){
    mySwitch.send("000000010101000101010001");
      strip.SetPixelColor(0, green);
      strip.Show();
      State=0;
  } else {
    mySwitch.send("000000010101000101010100");
      strip.SetPixelColor(0, red);
      strip.Show();
      State=1;
      MakeBuzz(5);
  }
  State=Liquid_level;
}
Serial.print("Liquid_level= ");Serial.println(State);
strip.SetPixelColor(1, blue);
 strip.Show();
}

void CheckTemp (){
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  float hic = dht.computeHeatIndex(t, h, false);
  Serial.println(h); Serial.println(t); Serial.println(hic);
  if (t < 15){
    strip.SetPixelColor(3, blue);
    strip.Show();
  }else if(t >= 15 && t <= 25){
    strip.SetPixelColor(3, green);
    strip.Show();
  } else {
    strip.SetPixelColor(3, red);
    strip.Show();
  }
}



void MakeBuzz(int sec){
  for (int j = 0; j < sec; j++){
    for(int i=0;i<50;i++)
    {
      digitalWrite(buzzer,HIGH);
      delay(1);//wait for 1ms
      digitalWrite(buzzer,LOW);
      delay(1);//wait for 1ms
    }
    //output another frequency
    for(int i=0;i<50;i++)
    {
      digitalWrite(buzzer,HIGH);
      delay(2);//wait for 2ms
      digitalWrite(buzzer,LOW);
      delay(2);//wait for 2ms
    }
  }

}
