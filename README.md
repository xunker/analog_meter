#analog_meter.ino
##Control an analog meter/gauge using a serial port.

Using an Arduino-compatible microcontroller you can use this sketch
to make a serially-controllable analog meter.

Any analog panel meter should work, just make sure you choose the correct
resistors. For a 5v Arduino connected to a 50-microamp meter you would use
a 100k-ohm resistor. for a 1-milliamp meter connected to 5v, you'd use a 5k.

The best tutorial about selecting resistors and wiring up the meter I have
found is the [the Adafruit Analog Meter Clock tutorial](https://learn.adafruit.com/trinket-powered-analog-meter-clock/building-the-circuit).

You can use the included Python script (`meter_control.py`) to send commands
from the command line.

You can send commands direct via a serial connection, too, with a simple
command syntax. Please see the "command syntax" section in the comment header
of `analog_meter.ino` for details.

### Note for Arduino Leonardo and Micro users

In the `analog_meter.ino` file, find the following line below `delay(100);`:

```c
//serialEvent();
```

.. and remove the comment characters ("//") so it looks like this:

```c
serialEvent();
```