Thermostat
=============

This is a thermostat for the arduino using the DS18B20 digital
temperature sensor and a SSR and using the Liquid Crystal library
for 4-pin display output.

My hardware has two buttons for control - Mode & Select

Using
-------------
You'll want to take a look at the pin settings and either adjust
your wiring to match or change them to reflect your hardware.

Aside from that it's pretty much plug&play.  If you set the target mode
to 'constant' you can tell it what temperature you want to aim for.

You can also define custom temperature profiles.  Doing so simply requires
that you define a float array with the time & temperature values you want to
use and add a temperature profile to load that array.  The relevant lines 
from the existing code:

Constants - if you add a new mode, be sure to increase the mode count as
well
    #define tModeConstant 0
    #define tModeSmokedPork 1
    #define tModeSmokedBrisket 2
    #define tModeCount 3

setting up arrays - this uses explicit slopes, but you can add points to
the beginning and end and use Catmull-Rom splines instead.
    double timeArrayPork[5] =   {0,  5,   10,  360, 540};
    double tempArrayPork[5] =   {60, 220, 228, 225, 215};
    double slopesArrayPork[5] = {0,  0,   0,   0,   0};  

In reset_spline - setting up the spline.  Sets the initial temperature to the
current temperature
    case tModeSmokedPork:
      tempArrayPork[0] = temperature;
      tempCurve.setPoints( timeArrayPork, tempArrayPork, slopesArrayPork, 5 );

In target_mode_loop():
    case tModeSmokedPork:
      lcd.print( "=> Pulled Pork" ); break;


