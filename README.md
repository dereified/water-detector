Water Detector
==============

Water detector designed to minimise current consumption.

```
                                             -+-  -+-    -+-
                                              |    |    +-+
                                              V\\  V\\  - | 1N4007 +
-+ 4.5V    -+-                   -+-          - Gr - Rd ^ O< Buzzer
 |          |                     |    2x150o |    |    +-+
 = 100n     |            2x   +----+---+      R    R      |
 |          |            150o |   8    |      R    R      |
-+-         +O hi O--+----RR--+2 Tiny 7|-RR---|----|----|/ BC546
            +O lo O----+--RR--+3 AT85 6|-RR---|---|/    |\>
                     | |      |   4   5|-RR--|/   |\>     |
                     R R 2x   +---+----+ 3x  |\>    |     |
                     R R 470k     |      4k7   |    |     |
                    -+-+-        -+-          -+-  -+-   -+-

```

Detecting water is trivial, but an MCU-based solution was chosen to minimise current consumption for a device to be left unattended for years. Current consumption should be a few uA when water not detected and so for 3xAA cells self-discharge happen much sooner (~5 years).

Circuit is designed for a usually-dry sump, so current consumption is higher when water is detected (but still <1mA). Two water-marks are used to allow preventative (lwm) and reactive (hwm) detection.

Signals
-------

* 1 x Green flash (~1/min) --- all okay
* 2 x Green flash + buzz (~1/min) --- battery low
* 1 x Red flash (~15/min) --- LWM detects water
* 2 x Red flash + buzz (~15/min) --- HWM detects water

Programming
-----------

Ptrogrammed at 1MHz (internal clock, no bootloader) with a Sparkfun Tiny AVR Programmer (USBTinyISP board) programmed with the Arduino IDE and ATTinyCore extension (from github).

Operation
---------

Water detection triggers when ADC hits 10% to Vcc, ie resistance between detection pins ~47k. Should be fine for tap & rain water using ~1cm wire ends ~1cm apart. Tested on Northumbrian tap & rain water. Rain has a considerably higher resistance and so a harder challenge.

When HWM artificially tied to Vcc, debug mode is triggered. Low battery mode triggered at 3.3V, ie 1.1V/cell.

Debug Mode
----------

Debug mode gives a readout of three values. Each value comprises ten LED flashes (red=1, green=0) denoting a 10-bit binary ADC reading (MSB first). Between values, buzzer triggers. After all three, buzzer triggers twice. Values are:

1. HWM ADC value (likely all 1's or near enough as tied high)
2. LWM ADC value
3. Value of 1100/Vcc.

This is useful for tweaking low-battery detection and water detection in service.
