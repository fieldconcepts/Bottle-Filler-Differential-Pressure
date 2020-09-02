# Bottle-Filler-Differential-Pressure

  Automatic Bottle Filler - DP
  ----------------------------------------
  
  NXP MPX5010DP Pressure Sensor (x2)
  1/4" Plastic Metal Solenoid Valve 2 Way DC 12V 0-120 PSI (x2)
  Arduino UNO (x1)
  12v Power adaptor 240VAC (x1)
  LDR Light Dependent Resistor (x2)
  Green LED (x2)
  Red LED (x2)
  
  ---------------------------------------

  Ardunio controlled automatic bottle filler that is controlled 
  via a serial interface from a PC (9600 Baud). 
  
  The filling station uses LDRs to detect the presence of an empty bottle. 
  The LDR act as an interlock and will not allow filling to commence unless
  bottle is detected. A red LED is turned ON when bottle is detected. 
  
  Operator can commnece filling by sending comand from PC (f). 
  A green LED is turned on during fill. The differential pressure sensor port is 
  connected to tubing that is set to the desired fill depth in the bottle. 
  As fluid fills the bottle it will eventually reach the fill line and pressure 
  will spike on the  sensor above the set threshold. The solenoid shuts off and green LED turns
  off. Filling is stopped. Manual overide by sending (q) command.

  LED test function to ensure all LEDS are good.

  Zeroing sensor function before operating.
