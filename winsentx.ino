#include <SPI.h> //penyertaan library header penggunaan interface SPI
#include <nRF24L01.h> //penyertaan library header nRF24L01
#include <RF24.h> //penyertaan library header RF24const byte Address[6] = " 00009 " ;
#include <MQUnifiedsensor.h>

#define         Board                   ("Arduino UNO")
#define         PinMQ4                  (A1)
#define         Type1                   ("MQ-4") //MQ4
#define         Voltage_Resolution      (5)
#define         ADC_Bit_Resolution      (10)   // For arduino UNO/MEGA/NANO
#define         RatioMQ4CleanAir        (4.4)  //RS / R0 = 4.4 ppm

MQUnifiedsensor MQ4(Board, Voltage_Resolution, ADC_Bit_Resolution, PinMQ4, Type1);

int pinSensor = A0;
RF24 radio(9, 10);
const uint64_t pipe = 123456;

struct Pack{
  int CO;
  float CH4;
} packet;

void setup(void) {
  Serial.begin(9600);
  pinMode(pinSensor, INPUT);
  analogReference(EXTERNAL);
  radio.begin();
  radio.openWritingPipe(pipe);

  MQ4.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ4.init();
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for (int i = 1; i <= 10; i ++)
  {
    MQ4.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ4.calibrate(RatioMQ4CleanAir);
    Serial.print(".");
  }
  MQ4.setR0(calcR0 / 10);
  Serial.println("  done!.");
}

void loop(void) {
  float vadc = (float)(2.5 / 1024 * analogRead(pinSensor));
  packet.CO = (int)((625 * (vadc)) - 260)-16;
  if (packet.CO < 0)
  {
    packet.CO = 0;
  }

  MQ4.update();
  MQ4.setA(1012.7); MQ4.setB(-2.786); // Configurate the ecuation values to get CH4 concentration
  packet.CH4 = MQ4.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup

 
  Serial.print("PPM CO= ");
  Serial.println(packet.CO);
  Serial.print("PPM CH4= ");
  Serial.println(packet.CH4);
  delay(1000);

  radio.write(&packet, sizeof(packet));
  delay(1000);
}
