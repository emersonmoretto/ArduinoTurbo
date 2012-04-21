/* FreqPeriodCounter
 * Version 6-2-2012
 * Copyright (C) 2011  Albert van Dalen http://www.avdweb.nl
 * 
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses

Release Notes
17-12-2011 timeFunctionPtr to select millis or micros instead of bool variable
17-12-2011 New function ready()

 
               <------------- period ------------>
                  pulseWidth
                 _____________                      ______________                     
               ||             ||                  ||              ||  
               ||             ||  pulseWidthLow   ||              ||
 ______________||             ||__________________||              ||______________
          
 periodCount   0                                  1
 transientTime ^              ^                   ^               ^
 level                1                 0                  1                0       
 debounceTime  <-->           <-->                <-->            <--> 
                                                                  <- elapsedTime ->
*/
          
#include "FreqPeriodCounter.h"

FreqPeriodCounter::FreqPeriodCounter(byte pin, unsigned long (*timeFunctionPtr)(), unsigned debounceTime):
time(0), transientTime(0), elapsedTime(0), pulseWidth(0), pulseWidthLow(0), period(0), periodCount(0), level(0), lastLevel(0), 
pin(pin), readyVal(0), debounceTime(debounceTime), timeFunctionPtr(timeFunctionPtr)
{ 
}

bool FreqPeriodCounter::poll()
{ time = timeFunctionPtr();
  elapsedTime = time - transientTime; 
  level = digitalRead(pin);
  if((level != lastLevel) & (elapsedTime > debounceTime))
  { lastLevel = level;
    transientTime = time;
    if(level == HIGH) 
    { pulseWidthLow = elapsedTime;
      period = pulseWidth + pulseWidthLow;
      if(periodCount++ > 1)
      { periodCount = 0;
        readyVal = true;
        return true; // return true if a complete period is measured
      }
    }
    else pulseWidth = elapsedTime;  
  }
  return false;
}

bool FreqPeriodCounter::ready()
{ bool returnVal = readyVal; 
  readyVal = false; // reset after read
  return returnVal;
}

unsigned long FreqPeriodCounter::hertz(unsigned int precision)
{ if (timeFunctionPtr == micros) return (unsigned long)precision*1000000/period;
  else return (unsigned long)precision*1000/period;
}






