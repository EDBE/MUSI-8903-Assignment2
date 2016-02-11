# MUSIC-8903-2016
# Assignment 2: Vibrato
# Group: Liang and Rithesh

1, Design a class interface based on the template MyProject.h

The VibratoIf class is the interface. User is able to set the vibrato parameter including modulation width (in sec), and modulation frequency (in Hz) through the interface. In order to make sure the modulation width is valid, the VibratoIf would ask user to set the maximum delay length in second. The modulation width can not be greater than the half of the maximum delay length. This maximum delay length in second would be converted to number of samples to initialize the ring buffer.

2, Implement a fractional delay line with linear interpolation (DAFX: vibrato.m)

The output of the vibrato for each sample equals to: delayline(i+1)*fraction + delayline(i)*(1-fraction). We write the 'getFromWrite' function in the RingBuffer class in order to get the value at index offseting from the write index.

3, Design and implement an LFO class to work with the fractional delay line above.

The LFO was implemented by using 'sin' function which is from the 'math.h'

4, Implement main command line function



5, verify functionality for 2 different input audio files by comparison with matlab 

	1, Impulse signal as input:
	2, sinunoid signal as input:

6, write the following test functions to ensure functionality
	1, output equals delayed input when modulation amplitude is 0
	2, DC input stays DC ouput regardless of parametrization
	3, varying input block size
	4, zero input signal
	5, additional test to verify an yet untested correct functionality

Note:
	For our project, you may need to add the 'Vibrato.cpp' and 'VibratoIf.cpp' to the Compile Sources for building the following targets (at least in XCode):

		1. MUSI890Exec :
		      a.Vibrato.cpp
		      b.VibratoIf.cpp
		      c.MUSI8903Exec.cpp

		 2. TestExec:
		 	  a.Test.cpp
		 	  b.Test_MyVibrato.cpp