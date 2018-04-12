#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier

// Connect Display via BRZO I2C Lib
#include <brzo_i2c.h>
#include "SH1106Brzo.h"

SH1106Brzo  display(0x3c, 5, 4);

// VEML6070 I2C address is 0x38(56)
#define VEML_I2C_ADDR 0x38 //0x38 and 0x39

//Integration Time
#define IT_1_2 0x0 //1/2T
#define IT_1   0x1 //1T
#define IT_2   0x2 //2T
#define IT_4   0x3 //4T

void setup() {
  Serial.begin(9600);

  display.init();

  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);

  // For VEML6070

  Wire.begin(2, 14);
  Wire.beginTransmission(VEML_I2C_ADDR);
  Wire.write((IT_1 << 2) | 0x02);
  Wire.endTransmission();
  delay(500);
}


void loop() {
  Wire.pins(2, 14);
  int raw = getUV();
  float uvI = raw / 65536.0

              Wire.pins(5, 4);
  display.clear();
  display.drawString(0, 0, "RAW ");
  display.drawString(0, 12, String(raw));
  display.drawString(0, 22, "UV ");
  display.drawString(0, 34, String(uvI));
  display.display();

  delay(500);
}

float getUV() {
  byte msb = 0, lsb = 0;
  uint16_t uv;

  Wire.requestFrom(VEML_I2C_ADDR + 1, 1); //MSB
  delay(1);
  if (Wire.available())
    msb = Wire.read();

  Wire.requestFrom(VEML_I2C_ADDR + 0, 1); //LSB
  delay(1);
  if (Wire.available())
    lsb = Wire.read();

  uv = (msb << 8) | lsb;
  Serial.println(uv);
  Serial.println(uv, DEC);
  return uv;
}
