#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

RF24 radio (9, 10);
const uint64_t pipe = 123456;

int pinBuzzer = 3;
int hijau1 = 4;
int merah1 = 5;
int hijau2 = 6;
int merah2 = 7;

struct Pack {
  int CO;
  float CH4;
} packet;

void setup(void) {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  pinMode(pinBuzzer, OUTPUT);
  pinMode(merah1, OUTPUT);
  pinMode(hijau1, OUTPUT);
  pinMode(merah2, OUTPUT);
  pinMode(hijau2, OUTPUT);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();

  lcd.setCursor(0, 0);
  lcd.print("MONITORING");
  lcd.setCursor(0, 1);
  lcd.print("GAS BERACUN");
  delay(2000);
  lcd.clear();
}

void loop(void) {
  if (radio.available()) {
    bool selesai = false;

    while (!selesai) {
      radio.read(&packet, sizeof(packet));
      delay(1000);
      Serial.print("PPM CO= ");
      Serial.println(packet.CO);
      lcd.clear();
      delay(1);
      lcd.setCursor(0, 0);
      lcd.print("CO: ");
      lcd.print(packet.CO);
      lcd.print(" PPM");
      delay(1000);
      if (packet.CO > 10)
      {
        digitalWrite(pinBuzzer, HIGH);
        digitalWrite(merah1, HIGH);
        digitalWrite(hijau1, LOW);
      }
      else
      {
        digitalWrite(pinBuzzer, LOW);
        digitalWrite(hijau1, HIGH);
        digitalWrite(merah1, LOW);
      }
      if (packet.CH4 > 20) {
        Serial.print("CH4 = ADA");
        lcd.setCursor(0, 1);
        lcd.print("CH4 = ADA");
        digitalWrite(pinBuzzer, HIGH);
        digitalWrite(merah2, HIGH);
        digitalWrite(hijau2, LOW);
      }
      else {
        Serial.print("CH4 = TIDAK ADA");
        lcd.setCursor(0, 1);
        lcd.print("CH4 = TIDAK ADA");
        digitalWrite(pinBuzzer, LOW);
        digitalWrite(merah2, LOW);
        digitalWrite(hijau2, HIGH);
      }
    }
  }
}
