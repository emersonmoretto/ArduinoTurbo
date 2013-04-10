
int injectorPin1 = 8;
int injectorPin2 = 9;

const int MAX_PSI = 3;//20;
const int MAX_RPM = 3;//7;
const int RPM_PRECISION = 1;//2; // multiply factor: MAX_RPM x RPM_PRECISION

int mapa[MAX_RPM * RPM_PRECISION][MAX_PSI];  

int incomingByte = 0; 

void setup(){
  
 Serial.begin(9600);
  
}


int getValueFromBuffer(int buff[]){
 
 int result = 0;

  if(buff[1] == -1)
     return buff[0]-48;
  else{
    result = (buff[0]-48)*10;
    result += buff[1]-48;
    return result;
  }
}


void readMap(){  
  
    int buff[] = {-1,-1};
    int j=0,k=0;
    int x=0,y=0;
    int sum = 0;
    boolean newMap = false;
    
    do{   
      
      int c = Serial.read();        
    
      if(c == ','){
        mapa[y][x] = getValueFromBuffer(buff);                          
        sum += mapa[y][x];    
        buff[0] = -1; buff[1] = -1;            
        x++;        
        j=0;              
      }else if(c == ';'){ // new line
        mapa[y][x] = getValueFromBuffer(buff);            
        sum += mapa[y][x];
        buff[0] = -1; buff[1] = -1;            
        y++;    
        x=0;  
        j=0;
      }else if(c == 'm' || c == '['){
        //Receiving new map  
        j=0;
      }else if(c >= 48 && c <= 57){ // the data!
        buff[j] = c;
        j++;            
      }
      newMap = true;    
      delay(2); // Little pause for next data
    
    }while(Serial.available());
    
    if(newMap){
    
      // DEBUG - printing received map
      for( int i=0; i < MAX_RPM * RPM_PRECISION ; i++){
        for( int j=0; j < MAX_PSI ; j++){
          Serial.print(mapa[i][j]);
          Serial.print(" ");
        }
        Serial.println(" ");
      }
      
      Serial.println(" ");
      Serial.print("New map saved! Checksum: ");
      Serial.println(sum);       
    }
    
}

void loop(){ 

  //m10,13,14;20,20,22;33,34,30; 
  
  
  // Is Bluetooth data Incoming??
  if(Serial.available())
  {      
      int c = Serial.read();

      // Receiving map
      if(c == 'm')
      {
         readMap();
      }
    }
    
    delay(1000);
    Serial.println("to aqui");
}
