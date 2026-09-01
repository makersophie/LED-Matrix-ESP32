// Horizontally scrolling text on a 32x32 P7.62 HUB75 panel.
// The panel is only 32px wide, so anything longer than 5 characters
// (6px per character in the default font) has to scroll.

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

const char* message = "YAY WE'VE FINALLY MADE IT";
int textWidth = 25 * 6;   // characters * 6px
int xPos = 32;            // start just off the right edge
int yPos = 8;             // 0 = top, text is 8px tall

void IRAM_ATTR display_updater() {
  portENTER_CRITICAL_ISR(&timerMux);
  display.display(30);
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  Serial.begin(115200);

  display.begin(16);
  display.setBrightness(150);
  display.clearDisplay();
  display.fillScreen(display.color565(0, 0, 0));

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &display_updater, true);
  timerAlarmWrite(timer, 4000, true);
  timerAlarmEnable(timer);

  Serial.println("ready");
}

void loop() {
  display.fillScreen(display.color565(0, 0, 0));
  display.setTextColor(display.color565(0, 255, 255));
  display.setCursor(xPos, yPos);
  display.print(message);

  xPos--;
  if (xPos < -textWidth) {
    xPos = 32;
  }

  delay(80);   // smaller = faster scroll
}
