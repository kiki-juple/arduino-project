#include "DHT.h"
#define DHTPIN D4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#include <Servo.h>

Servo myservo;

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>   //including the library of SimpleTimer
SimpleTimer timer;

char auth[] = "JVBxFZFNTYXJWCOmG1-2q7hU6A1pwggX";
char ssid[] = "JUPLE";
char pass[] = "12345678";

#define lampu     D5
#define humidifer D6
#define kipas     D7
#define servo     D8

WidgetLED ledlampu(V6);
WidgetLED ledhumidifer(V7);
WidgetLED ledkipas(V8);
WidgetLED ledservo(V9);

float suhu, kelembaban;

int otomatis = 0;

int jam, menit, detik;
int lock1 = 0, lock2 = 0;

void tasktimer() {
  // baca dht11
  suhu = dht.readTemperature();
  kelembaban = dht.readHumidity();
  // kondisi led blynk
  if (digitalRead(lampu) == 0) ledlampu.on();
  else ledlampu.off();
  if (digitalRead(humidifer) == 0) ledhumidifer.on();
  else ledhumidifer.off();
  if (digitalRead(kipas) == 0) ledkipas.on();
  else ledkipas.off();

  // menampilkan data dht11 ke blynk
  Blynk.virtualWrite(V0, suhu );
  Blynk.virtualWrite(V1, kelembaban);
  // kondisi on off berdasarkan suhu dan kelembaban
  if (suhu >= 37 && suhu <= 40) lock1 = 0;
  if (kelembaban >= 50 && kelembaban <= 60) lock2 = 0;

  if (suhu < 37) {
    if (lock1 == 0)digitalWrite(lampu, LOW); // on
    lock1 = 1;
  }
  if (suhu > 40) {
    if (lock1 == 0)digitalWrite(lampu, HIGH); // off
    lock1 = 1;
  }

  if (kelembaban < 50) {
    if (lock2 == 0)digitalWrite(humidifer, LOW); // on
    lock2 = 1;
  }
  if (kelembaban > 60) {
    if (lock2 == 0)digitalWrite(humidifer, HIGH); // off
    lock2 = 1;
  }
  if (suhu > 40 or kelembaban > 60) {
    digitalWrite(kipas, LOW); // on
  }
  else if (suhu < 40 or kelembaban < 60) {
    digitalWrite(kipas, HIGH); // off
  }


  // mode otomatis servo aktif
  if (otomatis == 1) {
    // timer
    detik++;
    if (detik > 59) {
      menit++;
      detik = 0;
    }
    if (menit > 59) {
      jam++;
      menit = 0;
    }
    if (jam > 23) {
      jam = 0;
    }
    // 4x dalam 24 jam
    if (jam == 6 && menit == 0 && detik == 0) myservo.write(65); // kiri
    if (jam == 12 && menit == 0 && detik == 0) myservo.write(105); // kanan
    if (jam == 18 && menit == 0 && detik == 0) myservo.write(65); // kiri
    if (jam == 0 && menit == 0 && detik == 0) myservo.write(105); // kanan

  }
  Serial.print(" T: ");
  Serial.print(suhu);
  Serial.print(" H: ");
  Serial.print(kelembaban);
  Serial.print(" Timer: ");
  Serial.print(jam);
  Serial.print(":");
  Serial.print(menit);
  Serial.print(":");
  Serial.print(detik);
  Serial.println();
}

// tombol manual lampu
BLYNK_WRITE(V2) {
  int pinValue = param.asInt();

  if (pinValue == 1) {
    if (digitalRead(lampu) == HIGH) digitalWrite(lampu, LOW); // on
    else digitalWrite(lampu, HIGH); // off
  }

}
// tombol manual hum
BLYNK_WRITE(V3) {
  int pinValue = param.asInt();

  if (pinValue == 1) {
    if (digitalRead(humidifer) == HIGH) digitalWrite(humidifer, LOW); // on
    else digitalWrite(humidifer, HIGH); // off
  }

}
// tombol manual fan
BLYNK_WRITE(V4) {
  int pinValue = param.asInt();

  if (pinValue == 1) {
    if (digitalRead(kipas) == HIGH) digitalWrite(kipas, LOW); // on
    else digitalWrite(kipas, HIGH); // off
  }

}

// tombol manual servo
BLYNK_WRITE(V5) {
  int pinValue = param.asInt();

  if (pinValue == 0) {
    otomatis = 0;
    myservo.write(85); // center
    ledservo.off();
  }
  if (pinValue == 1) {
    otomatis = 1;
    myservo.write(105); // kanan
    ledservo.on();
  }

}



void setup()
{
  // Debug console
  pinMode(lampu, OUTPUT);
  pinMode(humidifer, OUTPUT);
  pinMode(kipas, OUTPUT);
  digitalWrite(lampu, HIGH); //off
  digitalWrite(humidifer, HIGH); //off
  digitalWrite(kipas, HIGH); //off

  myservo.attach(servo);
  myservo.write(90); // center

  Serial.begin(115200);
  dht.begin();
  Blynk.begin(auth, ssid, pass);

  timer.setInterval(1000, tasktimer);

}

void loop()
{
  Blynk.run();
  timer.run();

}
