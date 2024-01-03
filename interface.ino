/*******************************************************************
    A mix of different bits of code based on stuff by Brian Lough
 *******************************************************************/

// Make sure to copy the UserSetup.h file into the library as
// per the Github Instructions. The pins are defined in there.

#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>

// Defines

// CYD touchscreen SPI pins
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

// Button sizes
#define BUTTON_W 50
#define BUTTON_H 50

#define LDR_PIN 34

unsigned long ldrCheckDue = 0;
int ldrDelay = 1000;

SPIClass mySpi = SPIClass(HSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);
TFT_eSPI tft = TFT_eSPI();

ButtonWidget btn1 = ButtonWidget(&tft);
ButtonWidget btn2 = ButtonWidget(&tft);
ButtonWidget btn3 = ButtonWidget(&tft);
ButtonWidget btn4 = ButtonWidget(&tft);
MeterWidget lum = MeterWidget(&tft);

// Array of button instances to use in for() loops
ButtonWidget* btn[] = {&btn1, &btn2, &btn3, &btn4};;
uint8_t buttonCount = sizeof(btn) / sizeof(btn[0]);

//    btn2.drawSmoothButton(!btn2.getState(), 3, TFT_BLACK, btn2.getState() ? "OFF" : "ON");
//    Serial.print("Button toggled: ");

//    btn2.setPressTime(millis());

void btn1_pressAction(void)
{
  Serial.println(btn1.getState() ? "B1 turned OFF" : "B1 turned ON");
  btn1.drawSmoothButton(!btn1.getState(), 3, TFT_BLACK, btn1.getState() ? "B1 OFF" : "B1 ON");
  //if(!btn1.getState()) drawLDRValue(0);
  delay(100);
}

void btn2_pressAction(void)
{
  Serial.println(btn2.getState() ? "B2 turned OFF" : "B2 turned ON");
  btn2.drawSmoothButton(!btn2.getState(), 3, TFT_BLACK, btn2.getState() ? "B2 OFF" : "B2 ON");
  delay(100);
}

void btn3_pressAction(void)
{
  Serial.println(btn3.getState() ? "B3 turned OFF" : "B3 turned ON");
  btn3.drawSmoothButton(!btn3.getState(), 3, TFT_BLACK, btn3.getState() ? "B3 OFF" : "B3 ON");
  delay(100);
}

void btn4_pressAction(void)
{
  Serial.println(btn4.getState() ? "B4 turned OFF" : "B4 turned ON");
  btn4.drawSmoothButton(!btn4.getState(), 3, TFT_BLACK, btn4.getState() ? "B4 OFF" : "B4 ON");
  delay(100);
}

void initButtons() {
  //uint16_t x = (tft.width() - BUTTON_W) / 2;
  uint16_t x = 10;
  uint16_t y = 5;
  btn1.initButtonUL(x, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLACK, TFT_RED, "B1 OFF", 1);
  btn1.setPressAction(btn1_pressAction);
  btn1.drawSmoothButton(false, 3, TFT_BLACK);

  y = 65;
  btn2.initButtonUL(x, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLACK, TFT_RED, "B2 OFF", 1);
  btn2.setPressAction(btn2_pressAction);
  btn2.drawSmoothButton(false, 3, TFT_BLACK);

  y = 125;
  btn3.initButtonUL(x, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLACK, TFT_RED, "B3 OFF", 1);
  btn3.setPressAction(btn3_pressAction);
  btn3.drawSmoothButton(false, 3, TFT_BLACK);

  y = 185;
  btn4.initButtonUL(x, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLACK, TFT_RED, "B4 OFF", 1);
  btn4.setPressAction(btn4_pressAction);
  btn4.drawSmoothButton(false, 3, TFT_BLACK);
}

void drawLDRValue(int sensorValue) {
  int x = 180;
  int y = 5;
  int fontSize = 4;
  if(btn1.getState()) {
    char fmtSV[5];
    snprintf(fmtSV,sizeof(fmtSV),"%04d", sensorValue); 
    tft.drawString("LDR: "+String(fmtSV), x, y, fontSize);
  } else {
    tft.drawString("LDR: OFF  ", x, y, fontSize);
  }
}

void setup() {
  Serial.begin(115200);

  Serial.println("Initialising...");
  pinMode(LDR_PIN, INPUT);
  mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin(mySpi);
  tft.init();
  tft.invertDisplay(1);
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  initButtons();
  lum.setZones(75, 100, 50, 75, 25, 50, 0, 25);
  lum.analogMeter(70, 5, 2000, "Lum", "0", "500", "1000", "1500", "2000");
  //drawLDRValue(0);
}

void loop() {
  static uint32_t now = millis();
  if (millis() - now >= 50) {
    now = millis();
    if (ts.tirqTouched() && ts.touched()) {
      TS_Point p = ts.getPoint();
      for (uint8_t b = 0; b < buttonCount; b++) {
        // Rough calibration:
        // x: 3800 / 320 = 11.875
        // y: 3800 / 240 = 15.83
        if (btn[b]->contains((p.x/11.875), (p.y/15.83))) {
          btn[b]->press(true);
          btn[b]->pressAction();
        }
      }
    }
  }
  if (now > ldrCheckDue && btn1.getState()) {
    ldrCheckDue = now + ldrDelay;
    int sensorValue = analogRead(LDR_PIN);
    Serial.print("LDR: ");
    Serial.println(sensorValue);
    lum.updateNeedle(sensorValue, 0);
    //drawLDRValue(sensorValue);
  }
}