# BonnMotionDA
BonnMotion Disaster Area support for OMNet++ with INET4

This is a rough patch for OMNet++/INET4 in order to introduce support of BonnMotion Disaster Area scenario files.

In order to support device entering and exiting the simulation, you should listen to "mobilityNodeRadioOn" and "mobilityNodeRadioOff" events that will be dispatched from the BonnMotionDAMobility class. 

