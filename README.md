# MUSIC-8903-2016
# Assignment 2: Vibrato
# Group: Liang and Rithesh

1, Design a class interface based on the template MyProject.h

The VibratoIf class is the interface. User is able to set the vibrato parameter including delay (in sec), modulation width (in sec), and modulation frequency (in Hz) through the interface.

2, Implement a fractional delay line with linear interpolation (DAFX: vibrato.m)

The output of the vibrato for each sample equals to: delayline(i+1)*fraction + delayline(i)*(1-fraction)

3, Design and implement an LFO class to work with the fractional delay line above.

The LFO was implemented by using 'sin' function which is from the <math.h>

4, Implement main command line function



5, verify functionality for 2 different input audio files by comparison with matlab 

	1, zero input:
	2, sinunoid input:

6, write the following test functions to ensure functionality
	1, output equals delayed input when modulation amplitude is 0
	2, DC input stays DC ouput regardless of parametrization
	3, varying input block size
	4, zero input signal
	5, additional test to verify an yet untested correct functionality
