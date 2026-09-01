# ESP32 + HUB75 32x32 LED Panel

Driving a P7.62 32x32 RGB LED matrix with an ESP32 DevKit and the
[PxMatrix](https://github.com/2dom/PxMatrix) library.

## Hardware

- ESP32 DevKit, 30-pin, ESP-WROOM-32 module
- P7.62-16S-V1 panel, 32x32, 1/16 scan
- 5V 2.5A supply (2A is the practical minimum; more headroom if you plan to
  light large areas at full brightness)

## Wiring

The panel's HUB75 input header on this board is labelled `R0` rather than `R1`,
so it follows the R0 variant of the PxMatrix wiring table.

| Panel (HUB75 input) | ESP32 | Notes |
|---|---|---|
| R0 | D13 | Hardware SPI MOSI, cannot be moved |
| CLK | D14 | Hardware SPI SCK, cannot be moved |
| STB | D22 | `P_LAT` |
| OE | RX2 (GPIO16) | `P_OE` |
| A | D19 | `P_A` |
| B | D23 | `P_B` |
| C | D18 | `P_C` |
| D | D5 | `P_D` — needed for 1/16 scan |
| GND | GND | Signal ground, separate from the power ground |

Jumpers between the panel's input header (PI) and output header (PO). These
chain the shift registers so a single data line feeds the whole panel. Each
wire goes from one header to the other, offset by one position:

| PI | PO |
|---|---|
| R1 | R0 |
| G0 | R1 |
| G1 | G0 |
| B0 | G1 |
| B1 | B0 |

The output header has no silkscreen. To confirm its orientation, power
everything down and use a multimeter's continuity mode: with one probe on a
known GND pin of the input header, probe the output header. GND is common
between the two, so the pins that beep tell you the pin ordering. On this
panel they land on the right-hand column of rows 2, 4 and 8, matching the
input header exactly.

Power the panel from its own 5V terminal, not through the ESP32's pins. The
ESP32 can run from the same supply via `VIN` — never `3V3`. Keep USB
disconnected while external power is applied.

## Software

Arduino IDE, with the esp32 core at **2.0.17**. Core 3.x changed the hardware
timer API (`timerBegin`, `timerAttachInterrupt`), which PxMatrix still uses in
its old form, so 3.x will not compile.

Board: ESP32 Dev Module.

Libraries:

- PxMatrix — install from the GitHub master branch via
  `Sketch > Include Library > Add .ZIP Library`. The Library Manager release is
  older and is missing `SHIFTREG_ABC` among other things.
- Adafruit GFX Library
- Adafruit BusIO (pulled in as a dependency of GFX)

## Sketches

`color_test/` cycles the panel through red, green and blue. Start here when
bringing up new hardware — it separates addressing problems from colour
channel problems.

`hello/` draws static text.

`scroll/` scrolls a longer message horizontally.

## Things that cost time

The scan value in `begin()` must match the panel. This one is 1/16, so
`begin(16)`. Getting it wrong lights only a fraction of the rows: `begin(8)`
on a 1/16 panel lights 8 rows and pairs them up, which looks like doubled
lines rather than an obvious error.

The constructor needs one address pin per scan bit. 1/16 needs A through D;
passing only A through C silently limits you to 8 addressable rows even if
`begin(16)` is correct.

Draw into the buffer *before* enabling the timer interrupt. Drawing after
`timerAlarmEnable()` races the ISR and the result is noise with your intended
content faintly visible underneath.

`flushDisplay()` did not reliably clear the buffer here. `clearDisplay()`
followed by `fillScreen(black)` did. The panel's shift registers can also hold
state across a reset, so a full power cycle is worth trying if a previous
sketch's output persists.

A USB cable that charges but does not carry data produces a missing serial
port, which looks exactly like a dead board. Swap the cable before concluding
anything else.
