#include <ArduinoIcon.h>
#include <Arial14.h>
#include <ks0108.h>
#include <ks0108_Arduino.h>
#include <ks0108_Mega.h>
#include <ks0108_Panel.h>
#include <ks0108_Sanguino.h>
#include <SystemFont5x7.h>


int factorPin = A3; 
int factor = 0;

boolean LCD = false; 

int xPin = A8; 
int yPin = A9; 

int turboPin = A0; 
float turbo = 0;

int lambdaPin = A10;
int lambda = 0;

int ledPin = 13;      // select the pin for the LED
int x = 0;  // variable to store the value coming from the sensor
int y = 0;  // variable to store the value coming from the sensor

int oldX, oldY; //to clean previous G-Force position 

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);  
  
  Serial.begin(9600);
  Serial1.begin(9600);
  
  if(LCD){
      GLCD.Init(NON_INVERTED);   // initialise the library, non inverted writes pixels onto a clear screen
      GLCD.ClearScreen();
      GLCD.SelectFont(System5x7); // switch to fixed width system font 
  }
}



void loop() {
  
   
  /**
  * G-Force
  */
  x = analogRead(xPin);    
  y = analogRead(yPin); 
  
  Serial1.write("<y");
  Serial1.write(x);
  Serial1.write(">");
  
  Serial1.write("<x");
  Serial1.write(125-((x-230)/2));
  //Serial1.write(x);
  Serial1.write(">");  
  
  if(LCD)
    drawGforce(x,y);


  /**
  * Turbo Pressure
  */
  turbo = analogRead(turboPin);  
   
  turbo = turbo - 167;

  if(turbo < 0) 
    turbo = 0.0;
  
  //correcao
  factor = 750;

  float bar = turbo / factor;
  
  if(LCD)
    drawTurbo(bar);
  
  Serial.println( int( (bar - int(bar) )*10));
 
 
  /**
  * Lambda Sensor
  * 1v - 204 (rich/max value)
  * Adjust: lambda/2 = 0-100 range
  **/  
  lambda = analogRead(lambdaPin);
  
  Serial1.write("<l");
  Serial1.write(lambda/2);
  Serial1.write(">");
  
  
  delay(40);
 
}

void drawTurbo(float pressure){
 
    GLCD.FillRect(0,0,37,28,WHITE);
    GLCD.CursorTo(0,0);     // positon cursor  
    GLCD.Puts("Turbo");  
    
  
    GLCD.CursorTo(0,1);     // positon cursor  
    GLCD.SelectFont(Arial_14); // switch to fixed width system font 
    GLCD.PrintNumber( int(pressure) );  
    GLCD.Puts(".");  
    GLCD.PrintNumber( int((pressure - int(pressure) )*10));  
    GLCD.Puts(" bar"); 
    //GLCD.CursorTo(0,4);     // positon cursor  
    //GLCD.Puts("T");
    //GLCD.PrintNumber(turbo2);
  
}

void drawGforce(int x, int y){
    
      GLCD.SelectFont(System5x7);
     
       //left up
      GLCD.DrawLine(40, 24, 40, 20, BLACK); //y
      GLCD.DrawLine(40, 20, 44, 20, BLACK); //x
      //right up  
      GLCD.DrawLine(85, 24, 85, 20, BLACK); //y
      GLCD.DrawLine(80, 20, 84, 20, BLACK); //x
    
    
      //left down
      GLCD.DrawLine(40, 48, 40, 44, BLACK); //y
      GLCD.DrawLine(40, 48, 44, 48, BLACK); //x
      //right down  
      GLCD.DrawLine(85, 48, 85, 44, BLACK); //y
      GLCD.DrawLine(80, 48, 84, 48, BLACK); //x
    
      
      GLCD.CursorTo(15,6);     // positon cursor  
      GLCD.Puts("1g"); 
    
    //  GLCD.DrawRect(44,17,40,30,BLACK); //1g
      
      GLCD.CursorTo(18,7);     // positon cursor  
      
    //  GLCD.Puts("2g"); 
      
    
      //G-Force Square
      oldX = 125-((x-230)/2);
      oldY = 64-((y-230)/4);
      
      GLCD.FillRect(oldX,oldY,5,5,WHITE);
      
      GLCD.FillRect(125-((x-230)/2),64-((y-230)/4),5,5,BLACK); 
    
      //G-Force text
      
      GLCD.CursorTo(14,0);     // positon cursor  
      GLCD.Puts("G-Force");  
      GLCD.CursorTo(14,1);     // positon cursor  
      GLCD.PrintNumber(x);  
      GLCD.Puts(","); 
      GLCD.PrintNumber(y);  
    
      /*factor = analogRead(factorPin); 
      GLCD.CursorTo(0,6);     // positon cursor  
      GLCD.Puts("F");
      GLCD.PrintNumber(factor);
      GLCD.Puts("  ");
      */
}

