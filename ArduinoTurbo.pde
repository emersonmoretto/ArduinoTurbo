#include <ArduinoIcon.h>
#include <Arial14.h>
#include <ks0108.h>
#include <ks0108_Arduino.h>
#include <ks0108_Mega.h>
#include <ks0108_Panel.h>
#include <ks0108_Sanguino.h>
#include <SystemFont5x7.h>


int injectorPin = 13;

int factorPin = A3; 
int factor = 0;

boolean LCD = false; 

int xPin = A8; 
int yPin = A9; 

int turboPin = A0; 
float turbo = 0;

int lambdaPin = A10;
int lambda = 0;

int x = 0;  // variable to store the value coming from the sensor
int y = 0;  // variable to store the value coming from the sensor

int oldX, oldY; //to clean previous G-Force position 

void setup() {
  
  // declare the injectorPin as an OUTPUT:
  pinMode(injectorPin, OUTPUT);  

  Serial.begin(9600);
  Serial1.begin(9600);

  if(LCD){
    GLCD.Init(NON_INVERTED);   // initialise the library, non inverted writes pixels onto a clear screen
    GLCD.ClearScreen();
    GLCD.SelectFont(System5x7); // switch to fixed width system font 
  }
}


int i=0;
int updatePause=0;

void loop() {

  /**
   * Pulse Width with Analog Pot
   * 
   * 
   * Govval = analogRead(Govpin);    
   * Govval = map(Govval, 0, 1023, 1,1000); 
   * 
   * val = analogRead(potpin);             
   * val = map(val, 0, 1023, 0, 20);     
   * digitalWrite(InjectorPin, HIGH);   // set the injector on
   * delay( 1000 - Govval + val);                  // on pulse width
   * digitalWrite(InjectorPin, LOW);    // set the injector off
   * delay(25-val);                  // off width
   * 
   * updatePause++;
   * if( updatePause > 8 )
   * {
   * // UpdateLCD();
   * updatePause = 0;
   * }
   */


  //val = map(val, 49, 57, 0, 179);     // scale it to use it with the servo (value between 0 and 180)

  Serial.println("loop");


  /**
   * Lambda Sensor
   * 1v - 204 (rich/max value)
   * Adjust: lambda/2 = 0-100 range
   **/
  lambda = analogRead(lambdaPin)/2;

  Serial1.write("<l");
  Serial1.write(lambda);
  Serial1.write(">");


  /**
  * Pulse Width Calc - based on Lambda
  */
  // I'm a suplementary injection
  if(bar > 0){
      
    // poor? go fuel!  
    if(lambda < 70){
      digitalWrite(injectorPin,HIGH);
      fuelCount = 1;
    }
    if(lambda < 50){
      digitalWrite(injectorPin,HIGH);
      fuelCount = 2;  
    }
    if(lambda < 30){
      digitalWrite(injectorPin,HIGH);
      fuelCount = 3;
    }
    
  }
  
  fuelCount--;
  
  
  
  if(fuelCount == 0)
    digitalWrite(injectorPin,LOW);
  


  if( i % 8 == 0 ){ // each 40ms we will read sensors (Turbo and G-Force)

    if(Serial1.available() > 0){
      Serial.println("chegando do BT");
      Serial.println(Serial1.read());
    }


    Serial.println("sensors");
    /**
     * G-Force
     */
    x = analogRead(xPin);    
    y = analogRead(yPin); 

    // to range
    x = x - 250;
    if(x < 0)
      x = 0;

    if(x > 254)
      x = 255;

    y = y - 250;
    if(y < 0)
      y = 0;

    if(y > 254)
      y = 255;  


    Serial1.write("<y");
    Serial1.write(y);
    Serial1.write(">");

    Serial.print("<x");
    Serial.print(x);
    Serial.println(">");  

    Serial1.write("<x");
    Serial1.write(x);
    Serial1.write(">");  

    if(LCD) drawGforce(x,y);


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

    if(LCD) drawTurbo(bar);

    Serial.println( int( (bar - int(bar) )*10));

    Serial1.write("<t");
    Serial1.write(int( (bar - int(bar) )*10));
    Serial1.write(">");

  } 

  i++; 
  delay(5);

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




