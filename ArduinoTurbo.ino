#include "FreqPeriodCounter.h"

const byte counterPin = 3; 
const byte counterInterrupt = 1; // = pin 3
FreqPeriodCounter counter(counterPin, millis, 0);



boolean LCD = false; 

// PIN
int injectorPin1 = 8;
int injectorPin2 = 9;

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

//TODO implementar a alteracao disso via Android/BT
int TURBO_THRESHOLD = 1;
int LAMBDA_THRESHOLD = 9;


void setup() {
  
  // declare the injectorPin as an OUTPUT:
  pinMode(injectorPin1, OUTPUT);  
  pinMode(injectorPin2, OUTPUT);  
  
  Serial.begin(9600);
  Serial1.begin(9600);

  attachInterrupt(counterInterrupt, counterISR, CHANGE);

  delay(2000);
}

void counterISR(){
  counter.poll();
}

/**
Metodo que recebe o mapa de injecao fracionado em 1 ms e atua nos bicos injetores
*/
void fuelInject(int mapa1[], int mapa2[]){
    
  boolean b1 = false;
  boolean b2 = false;
  
  Serial.println("start");
  for(int i=0 ; i < 30 ; i++){
  
   // if(mapa1[i] == 0 || mapa2[i] == 0)
      delay(1);
    
    if(mapa1[i] == 1){
      if(!b1) digitalWrite(injectorPin1,HIGH);
      b1 = true;
      Serial.print(1);
    }else{
      if(b1) digitalWrite(injectorPin1,LOW);
      b1 = false;
      Serial.print(0);
    }    
        
    if(mapa2[i] == 1){
      if(!b2) digitalWrite(injectorPin2,HIGH);
      b2 = true;     
    }else{
      if(b2) digitalWrite(injectorPin2,LOW);
      b2 = false;
    }
  }
  
  Serial.println("end");
  // seguranca
  digitalWrite(injectorPin1,LOW);
  digitalWrite(injectorPin2,LOW);  
  
}

/**
Metodo que faz a predicao de como serao feitos os ciclos de abertura e fechamento do bico injetor
Recebe o percentual a ser injetado e preenche o mapa de injecao fracionado em 1ms
*/
boolean predictInject(int mapa[], int percent){
  
        if(percent == 0){
          delay(window);
          return false;
        }
       
        
        for(int k=0; k<30 ; k++)
          mapa[k] = 9;
  
  	int MAX_CYCLE = 6;
  	int MIN_CYCLE_WIDTH = 2; // quanto menor, mais ele vai abrir e fechar
	
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
			

/*
Aqui vamos converter a predicao do algoritmo em intervalos e ciclos de injecao para um mapa de 0 e 1
ex: 
5 ciclo(s) de 3 ms com intervalos de 3
tem que resultar nisso:
0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1
*/	
	int soma = 0;
        int f=0;
	for(int j=0; j < qtdGaps+i ; j++){
		if(j % 2 == 0){ // eh um gap?? - sempre comeca com gap
			soma+=gap;
                        
			Serial.print("sleep ");
                        Serial.println(gap);
                        
                        for(int k=0; k<gap ; k++){
                          mapa[f+k] = 0;
                        } 
                        f+=gap;
                        
		}else{	// injetando?
  		
	        	soma+=div;
                        
                        for(int k=0; k < div ; k++){
                          mapa[f+k] = 1;
                        }
                        f+=div;
                       // digitalWrite(injectorPin,LOW);
                                
                        Serial.print("FUEL por ");
                        Serial.println(div);
		}
	}
        Serial.println("----");
        Serial.println(qtdGaps);

        Serial.println("----");
        for(int k=0; k<30 ; k++){
          Serial.print(mapa[k]);
          Serial.print(",");
        } 
        Serial.println("----");        
	Serial.print("soma:");
        Serial.println(soma);

        //algo deu MUITO errado... sleepando durante a janela
        if(soma == 0){
          Serial.println("nao foi possivel calcular a injecao");          
        }
        return true;
        
}

  //Lambda MAPA
              //0 , 1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12
  int mapa[] = {80, 70, 60, 50, 40, 30, 20, 20, 10, 0, 0, 0, 0};
  int mapa2[] = {60, 60, 50, 40, 40, 30, 20, 10, 0, 0, 0, 0, 0};  
  
void loop() {
  
  digitalWrite(injectorPin1,LOW);
  digitalWrite(injectorPin2,LOW);
  
  i++;
  delay(20);
  
  lambda = 122;
  
  /**
  * RPM
  fator 0.6 (map)
  1000 33hz 
  2000 70hz 66
  3000 100hz 99
  4000 130hz 132
  5000 170hz 165
  6000 200hz 199
  7000 233hz 233
  */
  if(counter.ready()){
    Serial.print(counter.hertz());
    Serial.println(" hz");
    Serial1.write("<r");
    Serial1.write( map(counter.hertz(), 0, 233, 0, 140));
    Serial1.write(">");
  }
  
    /**
    * Recebendo novo Mapa via BT
    */
    if(Serial1.available() > 0){
      
      char c = Serial1.read();
      Serial.print("Chegando do BT: ");
      Serial.println(c);
      
      // Mapa do bico 1
      if(c == 'm'){
       Serial.println("Chegando mapa do bico 1");
        int buff[] = {0,0,0};
        int j=0,k=0;
      
        while(Serial1.available() > 0){
          int c = Serial1.read();        
          
          if(c == 110){
            Serial.println("mapa 2");
            break;
          }
        
          if(c == ','){
            // ler e converter os valores, [0]*10+[1]
            int item = (buff[0] - 48) * 10;
            mapa[k] = item;  
            k++;
            Serial.println(item);            
            j=0;          
          }else{
            buff[j] = c;
            j++;
          }        
        }
      }
      
      // Mapa do bico 2
      if(c == 110){
       Serial.println("Chegando mapa do bico 2");
        int buff[] = {0,0,0};
        int j=0,k=0;
      
        while(Serial1.available() > 0){
          int c = Serial1.read();        
        
          if(c == ','){
            // ler e converter os valores, [0]*10+[1]
            int item = (buff[0] - 48) * 10;
            mapa2[k] = item;  
            k++;
            Serial.println(item);            
            j=0;          
          }else{
            buff[j] = c;
            j++;
          }        
        }
      }
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

    if(i%50 == 0){
      Serial.print("l:");
      Serial.println(lambda);
    }
    
    /********************************************************************
    DEV
    /*******************************************************************/
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

    /********************************************************************
    DEV
    /*******************************************************************/
    if(lambda <= LAMBDA_THRESHOLD){ // DEBUG
    
    // Verificacao de Thresholds para atuar!
    //if(turbo >= TURBO_THRESHOLD && lambda <= LAMBDA_THRESHOLD){

      int mapaDiscretoBico1[window];
      int mapaDiscretoBico2[window]; 
       
      // Aqui fazemos a predicao da injecao que preenche o mapa de injecao em valores discretos fragmentados a cada 1 ms
      // ou seja, mandamos o percentual a ser injetado e ele preenche o mapa assim: 0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1
      // onde 0=bico fechado e 1=bico aberto
      predictInject(mapaDiscretoBico1, mapa[lambda]); // Bico 1
      predictInject(mapaDiscretoBico2, mapa2[lambda]); // Bico 2
      
      // Atuando
      fuelInject(mapaDiscretoBico1, mapaDiscretoBico2);
      
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


