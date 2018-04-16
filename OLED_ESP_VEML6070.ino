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

char xStep = 0;
float yValue[120];

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


  for (char i = 0; i < 120; i++) {
    yValue[i] = 0;
  }
  delay(150);
}


void loop() {
  int raw = getUV();
  float uvi = raw / 65536.0;

  display.clear();
  header(raw, uvi);
  axis();
  chart(uvi);
  display.display();


  xStep += 1;
  if (xStep >= 120) {
    xStep = 0;

    for (char i = 0; i < 120; i++) {
      yValue[i] = 0;
    }
  }

  delay(100);
}

void header(int raw, float uvi) {
  display.drawString(0, 0, "Raw: ");
  display.drawString(display.getStringWidth("Raw: ") + 2, 0, String(raw));
  display.drawString(64, 0, "UV: ");
  display.drawString(display.getStringWidth("UV: ") + 66, 0, String(uvi));
}

void axis() {
  // X
  display.drawLine(4, 14, 4, 64);

  // Y
  display.drawLine(0, 60, 124, 60);
}

void chart(float uvi) {
  // 4.1818 = 60 / 11
  float y = 60.0 - 4.1818 * uvi * 400; // 400 - DEBUG
  if (y < 14.0) {
    y = 14.0;
  }
  yValue[xStep] = y;

  Serial.println(y);

  for (char i = 0; i < xStep; i++) {
    display.drawLine(i + 4, yValue[i], i + 4, 60);
  }

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
  return uv;
}
