boolean LCD = false; 

// PIN
int injectorPin = 13;
int turboPin = A0; 
int xPin = A8; 
int yPin = A9; 
int lambdaPin = A10;

// Global vars
float turbo = 0;
int lambda = 0;
int x = 0;  // variable to store the value coming from the sensor
int y = 0;  // variable to store the value coming from the sensor

int oldX, oldY; //to clean previous G-Force position 

void setup() {
  
  // declare the injectorPin as an OUTPUT:
  pinMode(injectorPin, OUTPUT);  

  Serial.begin(9600);
  Serial1.begin(9600);


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

 // Serial.println("loop");


  
  //Serial.write(lambda);

  /**
  * Pulse Width Calc - based on Lambda
  */
  // I'm a suplementary injection
  /*
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
  
  
  //fuelCount--;
  
  
  
  if(fuelCount == 0)
    digitalWrite(injectorPin,LOW);
  
*/

  if( i % 8 == 0 ){ // each 40ms we will read sensors (Turbo and G-Force)

    if(Serial1.available() > 0){
      Serial.println("chegando do BT");
      Serial.println(Serial1.read());
    }
    
   /**
   * Lambda Sensor
   * 1v - 204 (rich/max value)
   * Adjust: lambda/2 = 0-100 range
   **/
  lambda = analogRead(lambdaPin)/2;

  lambda = map(lambda, 0, 122, 0, 12); 
  Serial1.write("<l");
  Serial1.write(lambda);
  Serial1.write(">");

 // Serial.print("<l");
  //Serial.print(lambda);
  //Serial.println(">");
  
    
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

   // Serial.print("<x");
   // Serial.print(x);
   // Serial.println(">");  

    Serial1.write("<x");
    Serial1.write(x);
    Serial1.write(">");  
  

    /**
     * Turbo Pressure
     */
    turbo = analogRead(turboPin);  

    Serial.println(turbo);
  
    Serial1.write("<t");
    Serial1.write(turbo);
    Serial1.write(">");

  } 

  i++; 
  delay(30);

}




