Taxi simulator and gps unit test runner for the esp32
=============================================================

Starts three FreeRTOS tasks:
  - One task for transmitting taxi data over uart.
  - One task for receiving from the uart.
  - One task for monitoring for touh events.

When a touch event is received the esp32 will cycle to the next
section of taxi data. Moving between vacancy, occupied and print.
