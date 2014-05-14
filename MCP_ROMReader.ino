#include <math.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>


Adafruit_MCP23017 mcp1;
Adafruit_MCP23017 mcp2;

struct MPin {
  MPin(Adafruit_MCP23017 *m, int p) : mcp(m), pin(p) {
  }
  
  Adafruit_MCP23017* mcp;
  int pin;
};

MPin ADDRESS[13] = {
  MPin(&mcp1, 8),
  MPin(&mcp1, 9),
  MPin(&mcp1, 10),
  MPin(&mcp1, 11),
  MPin(&mcp1, 12),
  MPin(&mcp1, 13),
  MPin(&mcp1, 14),
  MPin(&mcp1, 15),
  MPin(&mcp1, 0),
  MPin(&mcp1, 1),
  MPin(&mcp1, 2),
  MPin(&mcp1, 3),
  MPin(&mcp1, 4),  
};
MPin DATA[8] = {
  MPin(&mcp2, 8),
  MPin(&mcp2, 9),
  MPin(&mcp2, 10),
  MPin(&mcp2, 7),
  MPin(&mcp2, 6),
  MPin(&mcp2, 5),
  MPin(&mcp2, 4),
  MPin(&mcp2, 3),
};
int cePin = 2;
int oePin = 4;
int wePin = 3;

int mem = 0;

#define NSIZE(a) (sizeof(a)/sizeof(a[0]))

void setup()
{
  mcp1.begin(0x0);
  mcp2.begin(0x4);

  delay(6000);

  Serial.begin(9600);
  
  pinMode(cePin, OUTPUT);
  pinMode(oePin, OUTPUT);
  pinMode(wePin, OUTPUT);
  
  for (int i = 0; i < NSIZE(ADDRESS); i++) {
    ADDRESS[i].mcp->pinMode(ADDRESS[i].pin, OUTPUT);
  }
  
  digitalWrite(cePin, HIGH);
  digitalWrite(oePin, HIGH);
  digitalWrite(wePin, HIGH);

  read_setup();
  for (int i = 0; i < 8192; i++) {  
    Serial.write(read1(i));
    delay(10);
  }

  Serial.end();
}

void loop()
{
}


void set_address_pins(word addr)
{
  for (int i = NSIZE(ADDRESS) - 1; i >= 0; i--) {    
      ADDRESS[i].mcp->digitalWrite(ADDRESS[i].pin, (addr >> i) & 0x1);
  } 
}

void read_setup()
{
  //set ce HIGH
  digitalWrite(cePin, HIGH);

  //set data pins to input
  for (int i = 0; i < NSIZE(DATA); i++) {
    DATA[i].mcp->pinMode(DATA[i].pin, INPUT);
  }

  //set ce low
  digitalWrite(cePin, LOW);
  //tACE

  //set rw high
  digitalWrite(wePin, HIGH);

  //set oe low
  digitalWrite(oePin, LOW);
  //tAOE
}

byte read1(word loc)
{
  //set address pins
  set_address_pins(loc);

  byte data = 0x0;
  for (int i = NSIZE(DATA) - 1; i >= 0; i--) {    
    if (DATA[i].mcp->digitalRead(DATA[i].pin) != LOW) {
      data |= (0x1 << i);
    }
  } 

  return data;
}

void write_ce(byte val, word loc)
{
  //set ce HIGH
  digitalWrite(cePin, HIGH);
  //set rw to high
  digitalWrite(oePin, HIGH);
  
  //set data pins to output
  for (int i = 0; i < NSIZE(DATA); i++) {
    DATA[i].mcp->pinMode(DATA[i].pin, OUTPUT);
  }
  
  //write address pins
  set_address_pins(loc);
  //set rw to low
  digitalWrite(wePin, LOW);
  //tAS
  
  //set ce low
  digitalWrite(cePin, LOW);

  //write data pins
  for (int i = NSIZE(DATA) - 1; i >= 0; i--) {
    DATA[i].mcp->digitalWrite(DATA[i].pin, (val >> i) & 0x1);
  }
  //tDW

  //set ce high
  digitalWrite(cePin, HIGH);
  //set rw high
  digitalWrite(oePin, LOW);
  //tDH
}
