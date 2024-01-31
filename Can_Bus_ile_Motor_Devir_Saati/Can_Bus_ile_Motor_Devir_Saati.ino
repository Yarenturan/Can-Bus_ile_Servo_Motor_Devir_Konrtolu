#include <Canbus.h>
#include <defaults.h>
#include <global.h>
#include <mcp2515.h>
#include <mcp2515_defs.h>
#include <Servo.h>

Servo myservo;
int pos = 0;
int rpm = 0;

void setup() {
  myservo.attach(3); // Servo kontrol pini
  Serial.begin(115200);
  Serial.println("Can Read - Testing receival of CAN Bus message");
  delay(1000);

  if(Canbus.init(CANSPEED_500))
    Serial.println("Can Init ok");
  else
    Serial.println("Can't init CAN");

  delay(1000);
}

void loop() {
  obd2Request();
  delay(5);
  obd2Reply();

  pos = map(rpm, 0, 7000, 0, 180);
  myservo.write(pos);
}

void obd2Request() {
  tCAN message;
  message.id = 0x7DF;
  message.header.rtr = 0;
  message.header.length = 8;
  message.data[0] = 0x02;
  message.data[1] = 0x01;
  message.data[2] = 0x0C;
  message.data[3] = 0x55;
  message.data[4] = 0x55;
  message.data[5] = 0x55;
  message.data[6] = 0x55;
  message.data[7] = 0x55;

  Canbus.sendFrame(message); // CAN mesajını gönder
}

void obd2Reply() {
  tCAN message;
  if (Canbus.receiveFrame(&message)) {
    Serial.print("Received CAN message. ID: 0x");
    Serial.print(message.id, HEX);
    Serial.print(", Data: ");
    for (int i = 0; i < message.header.length; i++) {
      Serial.print(message.data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    rpm = (message.data[3] << 8) | message.data[4];
  }
}

