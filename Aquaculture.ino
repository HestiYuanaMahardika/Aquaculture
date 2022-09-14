#include<OneWire.h>
#include<DallasTemperature.h>
#include<LiquidCrystal_I2C.h>
#define MakanPertama                DateTime(0, 1, 1,  1, 10, 0, 0)//Disetting jam 7 pagi >> ubah ini untuk settingan waktu makan pertama
#define MakanKedua                  DateTime(0, 1, 1,  1, 11, 0, 0)//Disetting jam 5 sore >> ubah ini untuk settingan waktu makan kedua
#define pinservo                    6
#define waktuservo_ON               1000//milidetik
#define servo_ON                    20//derajat
#define servo_OFF                   60//derajat
#include <Wire.h>
#include "RTC_DS3231_RP.h" 
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16,2);
OneWire wiring(8);
DallasTemperature sensor(&wiring);
int resval = 0;
int respin = A3;
const int ldr = 7;
const int led = 13;
const int relayPin = 9;
const int relayPin2= 5;
Servo iniservo;
char weekDay[][7] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu" };   
byte detikSebelumnya;
char buf[17];



void setup(){
  sensor.begin();
  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  pinMode(ldr,INPUT);//define pin as input sensor
  pinMode(led,OUTPUT);//define pin as output sensor
  pinMode(relayPin,OUTPUT);
  pinMode(relayPin2,OUTPUT);
  iniservo.attach(pinservo);
  iniservo.write(servo_OFF);
  Wire.begin();
  rtc.begin();
  Wire.beginTransmission(0x3F);

  if (Wire.endTransmission())
  
  lcd.setCursor(3, 0);
  lcd.print("PAKAN IKAN");//untuk tampilan nama project
  lcd.setCursor(4, 1);
  lcd.print("OTOMATIS");//untuk tampilan nama project
  delay(3000);
  lcd.clear();

  Serial.println("Sistem mulai");
  sprintf(buf, "Set waktu 1 = %02d:%02d (%lu)", MakanPertama.hour(), MakanPertama.minute(), MakanPertama.get());
  Serial.println(buf);
  sprintf(buf, "Set waktu 2 = %02d:%02d (%lu)", MakanKedua.hour(), MakanKedua.minute(), MakanKedua.get());
  Serial.println(buf);
}

void loop()
{  
  DateTime now = rtc.now();
  rtc.convertTemperature(); 
  sensor.setResolution(9);
  sensor.requestTemperatures();
  float dataSuhu = sensor.getTempCByIndex(0);
  lcd.clear();
  lcd.setCursor(12,0);
  lcd.print("temp");

  lcd.setCursor(10,1);
  lcd.print(dataSuhu, 1);
  lcd.print((char)233);
  lcd.print("C");
  delay(1000);
  resval = analogRead(respin);

  lcd.setCursor(0,1);
  if (resval <=100){
    lcd.println("Emty");
  }
  else if(resval>100 && resval <=300){
    lcd.println("low");
  }
    else if(resval>300 && resval <=330){
    lcd.println("med");
    }
    else if(resval>330){
    lcd.println("High");
  }
if(resval >=      300){
  digitalWrite(relayPin2,HIGH);
}
else if(resval < 100){
  digitalWrite(relayPin2,LOW);
}
  
  int y =digitalRead(ldr);//read the sensor
if (y==1){
  digitalWrite(relayPin,LOW);//turn on led
}else{digitalWrite(relayPin,HIGH );//turn off led
}
delay(500);

if (detikSebelumnya != now.second())
  {
    sprintf(buf, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());

    lcd.setCursor(0, 0);
    lcd.print(buf);
    Serial.print(buf);
    detikSebelumnya = now.second();
    uint32_t epoch = now.get() % 86400;//hanya jam menit detik
   
    if ((epoch == MakanPertama.get()) ||
        (epoch == MakanKedua.get()))
    {
      char buf[17];
      sprintf(buf, "SEKARANG: %02d:%02d", now.hour(), now.minute());//untuk tampilan saat waktu makan
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(buf);
      lcd.setCursor(0, 1);
      lcd.print("Waktunya Makan!!");//untuk tampilan saat waktu makan
      Serial.println(buf);
      
      //tambah atau kurangi program ini untuk setting berapa kali servo berjalan
      //di setting 2X
      iniservo.write(servo_ON);
      delay(waktuservo_ON);
      iniservo.write(servo_OFF);
      delay(1000); 
      iniservo.write(servo_ON);
      delay(waktuservo_ON);
      iniservo.write(servo_OFF);
      delay(1000); 
      lcd.clear();
    }
  }
}
