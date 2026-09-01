// Diagnostic: cycles the whole panel through red, green, blue.
//
// Use this when bringing up a new panel. Each stage should be a clean,
// uniform single colour across all 32 rows.
//
//   Mixed / random colours  -> PI-to-PO jumper order is wrong
//   Only some rows lit      -> wrong scan value in begin(), or a missing
//                              address line (A-D)
//   Nothing lit             -> check power, and the CLK / R0 SPI lines

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

  display.begin(16);
  display.setBrightness(100);
  display.clearDisplay();

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &display_updater, true);
  timerAlarmWrite(timer, 4000, true);
  timerAlarmEnable(timer);

  Serial.println("ready");
}

void loop() {
  display.fillScreen(display.color565(255, 0, 0));
  delay(3000);
  display.fillScreen(display.color565(0, 255, 0));
  delay(3000);
  display.fillScreen(display.color565(0, 0, 255));
  delay(3000);
}
