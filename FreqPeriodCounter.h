#ifndef FREQPERIODCOUNTER_H 
#define FREQPERIODCOUNTER_H
 
/* FreqPeriodCounter
 * Version 6-2-2012
 * Copyright (C) 2011  Albert van Dalen http://www.avdweb.nl
 * 
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses 
 */

#include <Arduino.h>
 
class FreqPeriodCounter
{    
public:
  FreqPeriodCounter(byte pin, unsigned long (*timeFunctionPtr)(), unsigned debounceTime); 
  bool poll();
  bool ready();
  unsigned long hertz(unsigned int precision=1);

  unsigned long period, pulseWidth, pulseWidthLow, elapsedTime;
  bool level; 
  
protected:
  unsigned long time, transientTime;
  unsigned debounceTime; 
  byte periodCount, pin; 
  bool lastLevel, readyVal; 
  unsigned long (*timeFunctionPtr)();
};
#endif
