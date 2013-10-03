char messageStart = '<'; // ASCII: 60
char messageEnd = '>'; // ASCII: 62
char dataSeparator = '?'; // ASCII: 63 
char ok = 'k'; // ASCII: 107
char error = '!'; // ASCII: 33
char on = '1'; // ASCII 48
char push = 'P';

int messageSize = 38;


int serialBufferSize;
char serialBuffer[38];

byte tempFrame[2][4][16];

byte frame[8][2] = 
{{B11111111,B11111111},{B11111111,B11111111},{B11111111,B11111111},{B11111111,B11111111},
{B11111111,B11111111},{B11111111,B11111111},{B11111111,B11111111},{B11111111,B11111111}};


//CUBE 1
int latchPin = 3;
int clockPin = 4;
int dataPin = 2;
int layers[4] = {5,6,7,8};
int layers1[4] = {12,13,0,1};

int offset = 25; //microseconds
int frametime = 2500; //microseconds



void setup () {
  Serial.begin(57600);
  Serial.write("HELLO");
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
}

void loop () {
  serialBufferSize = Serial.available();
  if (serialBufferSize == messageSize) {
    for (int i=0;i<serialBufferSize;i++) serialBuffer[i] = Serial.read();
    parseData();
  } else if (serialBufferSize > messageSize) {
    flushBuffer(serialBufferSize);
  }
}

void parseData () {
  //Check Integrity
  if (serialBuffer[0] == messageStart && serialBuffer[2] == dataSeparator && serialBuffer[4] == dataSeparator && serialBuffer[37] == messageEnd) {
    if (serialBuffer[1] == '0' || serialBuffer[1] == '1') {
      int workingCube;
      int workingLayer;
      
      if (serialBuffer[1] == '0') workingCube = 0;
      if (serialBuffer[1] == '1') workingCube = 1;      
      
      if (serialBuffer[3] == '0' || serialBuffer[3] == '1') {
        if (serialBuffer[3] == '0') workingLayer = 0;
        else if (serialBuffer[3] == '1') workingLayer = 2;
        
        // VALID DATA
        for (int i=0; i<16; i++) {
          if(serialBuffer[i+5] == on) tempFrame[workingCube][workingLayer][i] = 1;
          else tempFrame[workingCube][workingLayer][i+5] = 0;
        }
        
        for (int i=0; i<16; i++) {
          if(serialBuffer[i+21] == on) tempFrame[workingCube+1][workingLayer][i] = 1;
          else tempFrame[workingCube][workingLayer][i+21] = 0;
        }
        
        //Serial.write(ok);
      } else {
        //Serial.write(error);
      }
    } else {
      //Serial.write(error);
    }
  } else if (serialBuffer[0] == push && serialBuffer[2] == push && serialBuffer[37] == push) {
    //PUSH DATA
    //Serial.println("push");
  } else {
    //Serial.write(error);
  }
}

void flushBuffer (int until) {
  Serial.println(error);
  char flushs;
  for (int i=0;i<until;i++) flushs = Serial.read(); 
}


void writeCube () {
  for (int i=0; i<4; i++) {
       digitalWrite(latchPin, LOW);
       if (i==0) digitalWrite(layers[3],LOW);
       else digitalWrite(layers[i-1],LOW);
       shiftOut(dataPin, clockPin, MSBFIRST, frame[i][0]);
       shiftOut(dataPin, clockPin, MSBFIRST, frame[i][1]);
       digitalWrite(latchPin, HIGH);
       delayMicroseconds(offset);
       digitalWrite(layers[i], HIGH);
       
       digitalWrite(latchPin1, LOW);
       if (i==0) digitalWrite(layers1[3],LOW);
       else digitalWrite(layers1[i-1],LOW);
       shiftOut(dataPin1, clockPin1, MSBFIRST, frame[i][0]);
       shiftOut(dataPin1, clockPin1, MSBFIRST, frame[i][1]);
       digitalWrite(latchPin1, HIGH);
       delayMicroseconds(offset);
       digitalWrite(layers1[i], HIGH);
      
       delayMicroseconds(frametime);
     }  
}
