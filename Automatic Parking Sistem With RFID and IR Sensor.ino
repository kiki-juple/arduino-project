


/* ------------------------------------------------------------------------------------------------|
  Signal Pin Mifare Pin Arduino Uno R3/Nano V3/ Pro---|
  -------------------------------------------------------------------------------------------------|
  RST/Reset RST 9 |
  SPI SS SDA(SS) 10 |
  SPI MOSI MOSI 11 |
  SPI MISO MISO 12 |
  SPI SCK SCK 13 |
*/
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define RST_PIN 9 // Inisialisasi pin yang digunakan untuk rst/reset
#define SS_PIN 10 // inisialisasi pin yang digunakan untuk pin SS (SDA) mifare

Servo masuk;
Servo keluar;

byte i = 0;
String seri = ""; //variable untuk menampung nomor seri
MFRC522 mfrc522(SS_PIN, RST_PIN);

LiquidCrystal_I2C lcd(0x27, 16, 2);

int irmasuk = 8;
int irkeluar1 = 3;
int irkeluar2 = 6;
int irkeluar3 = 7;

int total = 4;
int space;

int kunciportal = 0;

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  pinMode (irmasuk, INPUT);
  pinMode (irkeluar1, INPUT);
  pinMode (irkeluar2, INPUT);
  pinMode (irkeluar3, INPUT);

  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522

  masuk.attach(4); //pin yang digunakan untuk servo
  keluar.attach(5);
  masuk.write(0); //menetapkan keadaan awal servo
  keluar.write(0);

  space = total;

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("=SISTEM  PARKIR=");
  lcd.setCursor(0, 1);
  lcd.print("    OTOMATIS    ");
  delay(2000);
  lcd.clear();

}

void pintumasuk() {

  MFRC522::MIFARE_Key key;
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) //membaca nomor seri kartu
    return;
  for (i = 0; i < mfrc522.uid.size; i++) {
    seri += mfrc522.uid.uidByte[i];
  }
  Serial.println();
  //rumah nomor yang berada dalam “” dengan nomor seri RFID anda

  if (seri == "2602152" || seri == "414215743" || seri == "18316122396" && kunciportal == 1) {
    Serial.print("Nomor Seri anda");
    Serial.println(seri);
    if (space > 0) {
      kunciportal = 0;
      masuk.write(90);
      space = space - 1;
    }
    else {
      lcd.setCursor (0, 0);
      lcd.print("TIDAK ADA");
      lcd.setCursor (0, 1);
      lcd.print("SPACE KOSONG");
      delay (2000);
      lcd.clear();
    }
    seri = "";
  }
  else { // kondisi jika nomor seri kartu tidak sesuai
    lcd.setCursor(0, 0);
    lcd.print("MAAF KARTU ANDA");
    lcd.setCursor(0, 1);
    lcd.print("TIDAK TERDAFTAR");
    delay(2000);
    lcd.clear();
    Serial.print("Nomor Seri anda");
    Serial.println(seri);
    Serial.println("Mohon maaf, kartu anda tidak terdaftar");
    seri = "";
  }
  seri = ""; //mengosongkan variable seri
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
void pintukeluar() {
  if (digitalRead(irkeluar1) == LOW && digitalRead(irkeluar2) == LOW) {
    delay (3000) ;
    keluar.write(90);
  }
  if (digitalRead(irkeluar3) == LOW) {
    keluar.write(0);
    space = space + 1;
    if (space > 4) {
      space = 4;
    }
  }
}
void loop() {
  pintumasuk();
  
  if (digitalRead(irmasuk) == LOW) {
    masuk.write(0);
  }
  
  lcd.setCursor (0, 0);
  lcd.print("TOTAL SPACE: ");
  lcd.print(total);
  lcd.setCursor (0, 1);
  lcd.print("KOSONG: ");
  lcd.print(space);
  delay(2000);
  lcd.clear();

  pintukeluar();
}
