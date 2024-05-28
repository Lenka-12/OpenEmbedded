# OpenEmbedded
## Warm up on UCT dev board 001
- Below is basic functionality
- Microcontroller is send to sleep mode if 60s pass without button press
- Use interrupts for detecting button press
- You may use DMA with timer trigger to read ADCs
| Button Label          |Function                                         |
| ----------------------|-------------------------------------------------|
| SW0 press             | Read POT1 and POT2 and the display values on LCD|
| SW1 press             | Read temperature sensor and display value on LCD|
| SW2 press             | Toggles LED0-LED4 every 1s                      |
| SW3 press             | Wake up microcontroller from deep sleep         |  