# PID Thermostat

This is a PID thermostat for the arduino.  It supports independent
tuning parameters for multiple hardware 'profiles', and includes
an autotune mode to estimate the PID parameters for a given setpoint.


## Installation

### Hardware

* DS18-series (I'm using the DS18B20) temperature sensor
* Solid-state relay rated for your power source
* 16x2 LCD display (I'm using the LCD-09052 from Sparkfun)
* 4 buttons (I think mine are normally closed)

### Software

OSX:

``` sh
sudo pip install ino
brew install picocom
```

Other OS's will have their own picocom install process.  Picocom is only
really necessary if you want to use `ino serial`.  I prefer to use 
`screen /dev/tty.foo`, FWIW.

### Sketch

``` sh
# get it
git clone git@github.com:kerinin/arduino-thermostat.git ./
cd arduino-thermostat
git submodule init && git submodule update

# deploy it
ino build
ino upload
```


## Using

You'll want to take a look at the pin settings and either adjust
your wiring to match or change them to reflect your hardware.

The up & down buttons cycle through the settings menus, the left & right
change values.  Set your setpoint and it should start driving the SSR

If you're seeing lots of oscillations or periodic temperature spikes followed 
by long ramp down times, try running an autotune cycle.  This works by pushing
the power to 100% and waiting for the temperature to go outside a temperature
band around your setpoint (controlled by the 'Tuning Range' parameter).  It
then drops the power to 0% and waits for the temperature to fall below the
temperature band.  Rinse and repeat until a 'stable' oscillation develops, at
which point the PID parameters are set based on the amplitude and frequency of
that oscillation.

The PID and autotune features use the awesome PID and PID_Autotune libraries
by Bret Beauregard.  For more information on how they work take a look at
[this post about the PID library](http://brettbeauregard.com/blog/2011/04/improving-the-beginners-pid-introduction/)
and [this post about the Autotune library](http://brettbeauregard.com/blog/2012/01/arduino-pid-autotune-library/)


## Contributing

Please do!  Fork the repo, make some changes and send me a pull request!


## Future Directions

* Profile-specific target temp
* I'd like the temperatures to ramp from their current value to the target value over some time period based on 
  the tuning parameters.  This should reduce oscillation and overshoot.
* Refactor the different files into self-contained classes.  (Stop sharing global state!!!)
* Improve the MenuBackend library's UI handling.  Try to remove that case statement in the display code
* Import / Export profile data from console
