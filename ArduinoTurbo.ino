boolean LCD = false; 

// PIN
int injectorPin = 8;
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

int i=0;
int updatePause=0;
int window=30;

int TURBO_THRESHOLD = 1;
int LAMBDA_THRESHOLD = 9;


//



void setup() {
  
  // declare the injectorPin as an OUTPUT:
  pinMode(injectorPin, OUTPUT);  

  Serial.begin(9600);
  Serial1.begin(9600);

  delay(2000);
}

void fuelInject(int percent){
  
        if(percent == 0){
          delay(window);
          return;
        }
  
  	int MAX_CYCLE = 5;
  	int MIN_CYCLE_WIDTH = 4; // quanto menor, mais ele vai abrir e fechar
	
	float prop = ( (float) window * ( (float) percent/100));		
		
	// se for muito pequeno a proporcao
	//if(MIN_CYCLE_WIDTH > prop)
	    //MIN_CYCLE_WIDTH = (int) prop;
		
	float mod = 0;
	float div = 0;
	int i = MAX_CYCLE;
		
	for(; i >= 1 ; i--){
		
		mod = (int) prop % i;
		div = prop / i;
			
		if(mod == 0 && div >= MIN_CYCLE_WIDTH){
			break;
		}
	}		
		
	if(i==0) i++;
		
	//adjust do fit the ONLY cycle to half of window
	float gap = 0;
	if(i==1)
	    gap = (window - prop) / (i+1);
	else
	    gap = (window - prop) / (i);
		
	float qtdGaps = (window - prop) / gap;

        char foo1[100];

        sprintf(foo1,"Janela: %i ms com  %i percent, Tempo de injecao %i ms ",window,percent, round(prop));
        Serial.println(foo1);
  
        char foo2[100];
        sprintf(foo2,"%i ciclo(s) de %i ms com intervalos de %i",i, (int) div, (int) gap);
        Serial.println(foo2);
  
                // System.out.print("Janela: "+window + "ms com "+percent+"%. Tempo de injecao: "+prop+"ms ==> " );
                // System.out.println(i +" ciclo(s) de "+div+"ms com intervalos de "+gap+" iniciando em "+gap+"ms");
				
	int soma = 0;
	for(int j=0; j < qtdGaps+i ; j++){
		if(j % 2 == 0){
			soma+=gap;
                        delay(gap);                        
			Serial.print("sleep ");
                        Serial.println(gap);
		}else{			
	        	soma+=div;
                        digitalWrite(injectorPin,HIGH);
                        delay(div);
                        digitalWrite(injectorPin,LOW);
		        Serial.print("FUEL por ");
                        Serial.println(div);
		}
	}
	Serial.print("soma:");
        Serial.println(soma);

        //algo deu MUITO errado... sleepando durante a janela
        if(soma == 0){
          Serial.println("nao foi possivel calcular a injecao");
          delay(window);
        }
}


void loop() {
  
  //Lambda MAPA
  //TODO fazer a regulagem do mapa via Android
              //0 , 1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12
  int mapa[] = {90, 80, 70, 60, 50, 40, 40, 30, 20, 20, 20, 0,  0};
  
  
  digitalWrite(injectorPin,LOW);
  i++;
  delay(500);
  
  lambda = 122;
  
  //if( i % 8 == 0 ){
   // each 40ms we will read sensors (Turbo and G-Force)

    if(Serial1.available() > 0){
      Serial.print("Chegando do BT: ");
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

   // Serial.print("l:");
   // Serial.println(lambda);
    
    
    /**
    DEV
    */
    if(Serial.available() > 0){
      Serial.println("chegando na serial");
      lambda = Serial.read(); 
      lambda-=48;     
    }
    
    
    
    
    /**
    * G-Force
    */
    readGForce();
   
    /**
     * Turbo Pressure
     */
    turbo = analogRead(turboPin);  
   // Serial.println(turbo);
    turbo -= 310; // starting with 1.9v and I don't no why
    turbo = map(turbo,0,1024,0,25);    
    //Serial.println((int)turbo);
  
    Serial1.write("<t");
    Serial1.write((int)turbo);
    Serial1.write(">");

     
    /**
    * Fuel Inject
    */
   // Serial.println(lambda);
    if(lambda <= LAMBDA_THRESHOLD){
    //if(turbo >= TURBO_THRESHOLD && lambda <= LAMBDA_THRESHOLD){

      // atuando entre 20% e 90%
      //int percent = map(lambda, 0, LAMBDA_THRESHOLD, 2, 9);
      
      // Usando mapa de injecao
      fuelInject(mapa[lambda]);

    }else{  /// Nao preciso injetar nada!!!
    
      delay(window); // dormindo...
    
    }

}


void readGForce(){

    x = analogRead(xPin);    
    y = analogRead(yPin); 

    // to range
    x = x - 250;
    if(x < 0) x = 0;
    if(x > 254) x = 255;

    y = y - 250;
    if(y < 0) y = 0;
    if(y > 254) y = 255;

    Serial1.write("<y");
    Serial1.write(y);
    Serial1.write(">");

    Serial1.write("<x");
    Serial1.write(x);
    Serial1.write(">");  
}