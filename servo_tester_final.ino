/*
 * Arduino 16-Channel Servo Tester - Ultra Memory Optimized
 * Minimal footprint version for Arduino Nano
 * Added pot-controlled menu scrolling
 */

#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_PWMServoDriver.h>

// Hardware pins
#define POT_PIN A0
#define BTN_SELECT 2
#define BTN_OK 3
#define BTN_BACK 4

// Servo constants
#define SERVO_MIN 150
#define SERVO_MAX 600
#define SERVO_CENTER 375

// Use minimal display buffer
U8G2_SH1106_128X64_NONAME_1_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Menu states - use defines to save RAM
#define MAIN_MENU 0
#define CENTRE_MODE 1
#define MANUAL_MODE 2
#define AUTO_MODE 3
#define SELECT_SERVO 4
#define INDIVIDUAL_SERVO 5

// Global variables - minimal set
uint8_t currentState = MAIN_MENU;
uint8_t selectedMenuItem = 0;
uint8_t selectedServo = 0;
uint8_t currentAngle = 90;
bool autoDirection = true;
unsigned long lastUpdate = 0;

// Pot smoothing
uint8_t lastMenuIndex = 255;
uint8_t lastServoIndex = 255;

void setup() {
  display.begin();
  pwm.begin();
  pwm.setPWMFreq(50);
  
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);
  
  // Center servos
  for (uint8_t i = 0; i < 16; i++) {
    pwm.setPWM(i, 0, SERVO_CENTER);
  }
  
  // Splash screen
  display.firstPage();
  do {
    display.setFont(u8g2_font_8x13_tr);
    display.drawStr(15, 25, "SERVO TESTER");
    display.drawStr(30, 45, "16-Channel");
  } while (display.nextPage());
  delay(2000);
}

void loop() {
  bool selectBtn = readButton(BTN_SELECT);
  bool okBtn = readButton(BTN_OK);
  bool backBtn = readButton(BTN_BACK);
  
  // --- Pot-controlled scrolling ---
  if (currentState == MAIN_MENU) {
    int potVal = analogRead(POT_PIN);
    uint8_t newIndex = map(potVal, 0, 1023, 0, 3);
    if (newIndex != lastMenuIndex) {
      selectedMenuItem = newIndex;
      lastMenuIndex = newIndex;
    }
  }
  
  if (currentState == SELECT_SERVO) {
    int potVal = analogRead(POT_PIN);
    uint8_t newIndex = map(potVal, 0, 1023, 0, 15);
    if (newIndex != lastServoIndex) {
      selectedServo = newIndex;
      lastServoIndex = newIndex;
    }
  }

  switch (currentState) {
    case MAIN_MENU:
      if (selectBtn) selectedMenuItem = (selectedMenuItem + 1) % 4;
      if (okBtn) {
        currentState = selectedMenuItem + 1;
        if (currentState == CENTRE_MODE) centerServos();
        if (currentState == AUTO_MODE) {
          currentAngle = 0;
          autoDirection = true;
        }
      }
      drawMainMenu();
      break;
      
    case CENTRE_MODE:
      if (okBtn || backBtn) currentState = MAIN_MENU;
      drawCentreMode();
      break;
      
    case MANUAL_MODE:
      currentAngle = map(analogRead(POT_PIN), 0, 1023, 0, 180);
      setAllServos(currentAngle);
      if (backBtn) currentState = MAIN_MENU;
      drawManualMode();
      break;
      
    case AUTO_MODE:
      if (millis() - lastUpdate > 50) {
        if (autoDirection) {
          currentAngle += 2;
          if (currentAngle >= 180) autoDirection = false;
        } else {
          currentAngle -= 2;
          if (currentAngle <= 0) autoDirection = true;
        }
        setAllServos(currentAngle);
        lastUpdate = millis();
      }
      if (okBtn || backBtn) currentState = MAIN_MENU;
      drawAutoMode();
      break;
      
    case SELECT_SERVO:
      if (selectBtn) selectedServo = (selectedServo + 1) % 16;
      if (okBtn) currentState = INDIVIDUAL_SERVO;
      if (backBtn) currentState = MAIN_MENU;
      drawSelectServo();
      break;
      
    case INDIVIDUAL_SERVO:
      currentAngle = map(analogRead(POT_PIN), 0, 1023, 0, 180);
      pwm.setPWM(selectedServo, 0, map(currentAngle, 0, 180, SERVO_MIN, SERVO_MAX));
      if (backBtn) currentState = SELECT_SERVO;
      if (okBtn) currentState = SELECT_SERVO;
      drawIndividualServo();
      break;
  }
  
  delay(50);
}

bool readButton(uint8_t pin) {
  static unsigned long lastPress = 0;
  static uint8_t lastPin = 255;
  
  if (!digitalRead(pin) && (millis() - lastPress > 200 || lastPin != pin)) {
    lastPress = millis();
    lastPin = pin;
    return true;
  }
  return false;
}

// ---------- DRAW FUNCTIONS ----------

void drawMainMenu() {
  const uint8_t visibleItems = 3;  // Number of items shown at once
  const char* menu[] = {"Centre", "Manual", "Auto", "Individual"};
  const uint8_t menuCount = 4;

  uint8_t startItem = 0;
  if (selectedMenuItem >= visibleItems) {
    startItem = selectedMenuItem - visibleItems + 1;
  }

  display.firstPage();
  do {
    display.setFont(u8g2_font_6x10_tr);
    display.drawStr(32, 10, "Servo Tester");

    for (uint8_t i = 0; i < visibleItems; i++) {
      uint8_t itemIndex = startItem + i;
      if (itemIndex >= menuCount) break;
      uint8_t y = 22 + i * 15;
      if (itemIndex == selectedMenuItem) display.drawStr(5, y, ">");
      display.drawStr(18, y, menu[itemIndex]);
    }

    uint8_t barHeight = 40;
    uint8_t barX = 120, barY = 15;
    display.drawFrame(barX, barY, 6, barHeight);

    uint8_t handleHeight = barHeight * visibleItems / menuCount;
    uint8_t handleY = barY + (barHeight - handleHeight) * startItem / (menuCount - visibleItems);
    display.drawBox(barX + 1, handleY, 4, handleHeight);

    //display.setFont(u8g2_font_4x6_tr);
    //display.drawStr(30, 62, "Select/OK/Back");
  } while (display.nextPage());
}

void drawCentreMode() {
  display.firstPage();
  do {
    display.setFont(u8g2_font_6x10_tr);
    display.drawStr(25, 15, "Centre Mode");
    display.drawStr(15, 30, "All Servos: 90");
    display.setFont(u8g2_font_8x13_tr);
    display.drawStr(55, 45, "90");
    display.setFont(u8g2_font_4x6_tr);
    display.drawStr(45, 62, "[OK] Back");
  } while (display.nextPage());
}

void drawManualMode() {
  display.firstPage();
  do {
    display.setFont(u8g2_font_6x10_tr);
    display.drawStr(20, 15, "Manual Mode");
    
    char buf[12];
    sprintf(buf, "Angle: %d", currentAngle);
    display.drawStr(30, 30, buf);
    
    drawSimpleGauge();
    
    display.setFont(u8g2_font_4x6_tr);
    display.drawStr(20, 62, "Pot controls");
  } while (display.nextPage());
}

void drawAutoMode() {
  display.firstPage();
  do {
    display.setFont(u8g2_font_6x10_tr);
    display.drawStr(30, 15, "Auto Mode");
    display.drawStr(15, 30, "Sweeping...");
    
    char buf[12];
    sprintf(buf, "Angle: %d", currentAngle);
    display.drawStr(30, 45, buf);
    
    display.setFont(u8g2_font_4x6_tr);
    display.drawStr(40, 62, "[OK] Stop");
  } while (display.nextPage());
}

void drawSelectServo() {
  const uint8_t visibleItems = 3;
  const uint8_t menuCount = 16;

  uint8_t startItem = 0;
  if (selectedServo >= visibleItems) {
    startItem = selectedServo - visibleItems + 1;
  }

  display.firstPage();
  do {
    display.setFont(u8g2_font_6x10_tr);
    display.drawStr(22, 15, "Select Servo");

    for (uint8_t i = 0; i < visibleItems; i++) {
      uint8_t itemIndex = startItem + i;
      if (itemIndex >= menuCount) break;
      uint8_t y = 30 + i * 15;
      if (itemIndex == selectedServo) display.drawStr(5, y, ">");
      char buf[12];
      sprintf(buf, "Servo %d", itemIndex);
      display.drawStr(20, y, buf);
    }

    uint8_t barHeight = 40;
    uint8_t barX = 120, barY = 25;
    display.drawFrame(barX, barY, 6, barHeight);

    uint8_t handleHeight = barHeight * visibleItems / menuCount;
    uint8_t handleY = barY + (barHeight - handleHeight) * startItem / (menuCount - visibleItems);
    display.drawBox(barX + 1, handleY, 4, handleHeight);

    //display.setFont(u8g2_font_4x6_tr);
    //display.drawStr(25, 62, "Select servo");
  } while (display.nextPage());
}

void drawIndividualServo() {
  display.firstPage();
  do {
    display.setFont(u8g2_font_6x10_tr);
    char buf[15];
    sprintf(buf, "Servo %d", selectedServo);
    display.drawStr(35, 15, buf);
    
    sprintf(buf, "Angle: %d", currentAngle);
    display.drawStr(30, 30, buf);
    
    drawSimpleGauge();
    
    //display.setFont(u8g2_font_4x6_tr);
    //display.drawStr(15, 62, "OK=Save Back");
  } while (display.nextPage());
}

void drawSimpleGauge() {
  uint8_t cx = 64, cy = 45, r = 12;
  
  for (uint8_t a = 0; a <= 180; a += 45) {
    float rad = (a + 180) * PI / 180.0;
    uint8_t x = cx + r * cos(rad);
    uint8_t y = cy + r * sin(rad);
    display.drawPixel(x, y);
  }
  
  float needleRad = (currentAngle + 180) * PI / 180.0;
  uint8_t nx = cx + (r - 2) * cos(needleRad);
  uint8_t ny = cy + (r - 2) * sin(needleRad);
  display.drawLine(cx, cy, nx, ny);
}

void centerServos() {
  for (uint8_t i = 0; i < 16; i++) {
    pwm.setPWM(i, 0, SERVO_CENTER);
  }
  currentAngle = 90;
}

void setAllServos(uint8_t angle) {
  uint16_t pulse = map(angle, 0, 180, SERVO_MIN, SERVO_MAX);
  for (uint8_t i = 0; i < 16; i++) {
    pwm.setPWM(i, 0, pulse);
  }
}
