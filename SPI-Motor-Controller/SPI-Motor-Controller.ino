/*

Harry Beadle, Petros Karydogiannis
D4 Thames
SPI Motor Controller (spi-motor-controller.c)

*/

#include <Servo.h>

#define DEBUG

Servo Rotor1, Rotor2, Rotor3, Rotor4;
bool Update1, Update2, Update3, Update4;
int Pos1, Pos2, Pos3, Pos4;

enum {control, high, low} state, nstate;

struct {
  unsigned int control, high, low;
  unsigned int data;
  unsigned int angle;
} packet;

#define S_R1 0xF4
#define S_R2 0xF5
#define S_R3 0xF6
#define S_R4 0xF7

void tick_comms(void) 
{
  if (!Serial.available()) return;
  state = nstate;
  uint8_t data = Serial.read();
  switch (state) {
    case control:
      if ((data & 0xF0) == 0xF0) {
        packet.control  = data;
        nstate = high;
      }
      break;
    case high:
      packet.high = data;
      nstate = low;
      break;
    case low:
      packet.low = data;
      packet.data = (int) (packet.high << 8 | packet.low) & 0x0000FFFF;
      packet.angle = map(packet.data, 0, 65536, 1000, 2000);
      nstate = control;
#ifdef DEBUG
      Serial.print("Update ");
      Serial.print(packet.control, HEX);
      Serial.print(" to ");
      Serial.print(packet.angle);
      Serial.print(" with ");
      Serial.println(packet.data, HEX);
#endif      
      switch (packet.control) {
        case S_R1:
          Pos1 = packet.angle;
          Update1 = true;
          break;
        case S_R2:
          Pos2 = packet.angle;
          Update2 = true;
          break;
        case S_R3:
          Pos3 = packet.angle;
          Update3 = true;
          break;
        case S_R4:
          Pos4 = packet.angle;
          Update4 = true;
          break;
      }
      break;
  }
}

void setup() {
  // Start Serial
  Serial.begin(9600);
  
  // Attach the motors to their respective output pins.
  Rotor1.attach(P1_4);
  Rotor2.attach(P1_5);
  Rotor3.attach(P1_6);
  Rotor4.attach(P1_7);

  // Arm Motors
  Pos1 = 1000;
  Pos2 = 1000;
  Pos3 = 1000;
  Pos4 = 1000;
  Rotor1.writeMicroseconds(2000);
  Rotor2.writeMicroseconds(2000);
  Rotor3.writeMicroseconds(2000);
  Rotor4.writeMicroseconds(2000);
  delay(5200);
  Rotor1.writeMicroseconds(1000);
  Rotor2.writeMicroseconds(1000);
  Rotor3.writeMicroseconds(1000);
  Rotor4.writeMicroseconds(1000);
  delay(8200);
}

void loop() {
  tick_comms();
  if (Update1) {
    Rotor1.writeMicroseconds(Pos1);
    Update1 = !Update1;
  }
  if (Update2) {
    Rotor2.writeMicroseconds(Pos2);
    Update2 = !Update2;
  }
  if (Update3) {
    Rotor3.writeMicroseconds(Pos3);
    Update3 = !Update3;
  }
  if (Update4) {
    Rotor4.writeMicroseconds(Pos4);
    Update4 = !Update4;
  }
}



