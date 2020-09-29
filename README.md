# Word Clock
Arduino code for Word Clock with RTC and a few effects. This is a DIY replica of the QlockTwo design.

[![Demo video](https://img.youtube.com/vi/Jz-r7BNQk3U/0.jpg)](https://www.youtube.com/watch?v=Jz-r7BNQk3U)

### Parts:
- WS2812B (110 leds)
- Arduino Nano
- DS3231 RTC (real time clock)
- (3) Buttons to adjust the time and to switch between effects
- 1000uF Capacitor for leds (20% white draws 500mA already)
- 470 Ohm resistor for data connection between arduino and WS2812B leds
- (3) 10k Ohm resistors for the buttons
- microUSB breakout board for power supply
- lasercut wooden enclosure

### Pins:
- D2, D3, D4 as input pins for the buttons
- D6 as data pin for the led strip
- Power supply through 5V pin (13) of Arduino Nano, which accepts regulated 5V as input.

### Troubleshooting:
- the RTC module I bought has a circuit to charge a LIR2032 battery. But it ships with a CR2032 non chargeable battery that will be damaged upon connecting the rtc to power. If you want to use the non-rechargeable CR2032 battery, you'll have to break up the charger circuit by <a href="https://campercontrol.org/step-1-assembling-control-box/modifying-rtc-module/">removing a resistor</a>, which is what I did too.
- The WS2812B leds draw a lot of current, thus they need a distributed power supply, and an appropriate capacitor. Here's a <a href="http://www.eerkmans.nl/wp-content/uploads/2016/02/neopixel_power_cheatsheet_v2.png">cheat sheet</a>. I'd also test them with dark colors first, like an rgb(5,20,20) cyan. Lowering their input voltage is not appropriate, as the leds have individual control circuits themselves, that need 5V to function properly. Setting the colors is one way of reducing intensity, and you can also set the brightness in the neopixel library through something like ```strip.setBrightness(50)``` Max is 255. When I first powered all 110 leds in series, only the first led would light up in green (regardless of the color I set), and the rest stood dark. No wonder, since the 110th led only saw 2.2 Volts. Powering every row of 11 leds in parallel solved the issue.
