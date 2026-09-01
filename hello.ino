// Static text on a 32x32 P7.62 HUB75 panel, driven by an ESP32 DevKit.
// See README for wiring and library versions.

#include <PxMatrix.h>

#define P_LAT 22
#define P_A   19
#define P_B   23
#define P_C   18
#define P_D   5
#define P_OE  16

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

PxMATRIX display(32, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D);

void IRAM_ATTR display_updater() {
  portENTER_CRITICAL_ISR(&timerMux);
  display.display(30);
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  Serial.begin(115200);

  display.begin(16);              // 1/16 scan
  display.setBrightness(150);     // 0-255

  // Clear before drawing. flushDisplay() alone was not enough.
  display.clearDisplay();
  display.fillScreen(display.color565(0, 0, 0));

  // Draw BEFORE enabling the timer, or the ISR races the drawing.
  display.setTextColor(display.color565(0, 255, 255));
  display.setCursor(1, 12);
  display.print("Hello");

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &display_updater, true);
  timerAlarmWrite(timer, 4000, true);
  timerAlarmEnable(timer);

  Serial.println("ready");
}

void loop() {
  delay(100);
}
