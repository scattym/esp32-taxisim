Taxi simulator and gps unit test runner for the esp32
=============================================================

Starts three FreeRTOS tasks:
  - One task for transmitting taxi data over uart.
  - One task for receiving from the uart.
  - One task for monitoring for touch events.

When a touch event is received the esp32 will cycle to the next
section of taxi data. Moving between vacancy, occupied and print.

Make sure the xstensa tools are in your path and that the env
var IDF_PATH is set.

This has been used in two configurations, firstly as a device to test meitrack GPS taxi parsing behaviour.

![Serial](https://github.com/scattym/esp32-taxisim/blob/master/misc/ConnectivityMeitrack.png?raw=true)

Also with an in house designed 4G GPS unit based on the simcom 5360 chipset.

![Serial](https://github.com/scattym/esp32-taxisim/blob/master/misc/ConnectivitySimcom.png?raw=true)
