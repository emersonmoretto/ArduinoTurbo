
#include <ks0108.h>
#include "Arial14.h"         // proportional font
#include "SystemFont5x7.h"   // system font

int xPin = A8; 
int yPin = A9; 

int factorPin = A3; 
int factor = 0;

int turboPin = A0; 
float turbo = 0;

int ledPin = 13;      // select the pin for the LED
int x = 0;  // variable to store the value coming from the sensor
int y = 0;  // variable to store the value coming from the sensor

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);  
  Serial.begin(9600);
  GLCD.Init(NON_INVERTED);   // initialise the library, non inverted writes pixels onto a clear screen
  GLCD.ClearScreen();
  GLCD.SelectFont(System5x7); // switch to fixed width system font 
}

int oldX, oldY;

void loop() {
  
   
  /**
  * G-Force
  */

  GLCD.DrawRect(24,3,80,58,BLACK); //1g
  GLCD.CursorTo(14,6);     // positon cursor  
  GLCD.Puts("1g"); 
  
  GLCD.DrawRect(44,17,40,30,BLACK); //2g
  GLCD.CursorTo(17,7);     // positon cursor  
  GLCD.Puts("2g"); 
  
  oldX = 125-((x-230)/2);
  oldY = 64-((y-230)/4);
  
  GLCD.FillRect(oldX,oldY,5,5,WHITE);
   
  x = analogRead(xPin);    
  y = analogRead(yPin); 
  
  GLCD.FillRect(125-((x-230)/2),64-((y-230)/4),5,5,BLACK); 
  
  GLCD.CursorTo(14,2);     // positon cursor  
  GLCD.Puts("G-Force");  
  GLCD.CursorTo(14,3);     // positon cursor  
  GLCD.PrintNumber(x);  
  GLCD.Puts(","); 
  GLCD.PrintNumber(y);  


  factor = analogRead(factorPin); 
  GLCD.CursorTo(0,6);     // positon cursor  
  GLCD.Puts("F");
  GLCD.PrintNumber(factor);
  GLCD.Puts("  ");
  

 

  /**
  * Turbo Pressure
  */
  turbo = analogRead(turboPin);  
   
  float turbo2 = turbo - 167;

  if(turbo2 < 0) turbo2 = 0.0;
  
  float bar = turbo2 / factor;
  
  Serial.println( int( (bar - int(bar) )*10));
  
  GLCD.CursorTo(0,2);     // positon cursor  
  GLCD.Puts("Turbo");  
  GLCD.CursorTo(0,3);     // positon cursor  
  GLCD.PrintNumber( int(bar) );  
  GLCD.Puts(".");  
  GLCD.PrintNumber( int((bar - int(bar) )*10));  
  GLCD.Puts("bar"); 
  GLCD.CursorTo(0,4);     // positon cursor  
  GLCD.Puts("T");
  GLCD.PrintNumber(turbo2);
  
  delay(80);
  

}
