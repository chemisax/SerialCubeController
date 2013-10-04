//Serial communication
char messageStart = '<'; // ASCII: 60 The message will always start with this character
char messageEnd = '>'; // ASCII: 62 The message will always end with this character
char dataSeparator = '?'; // ASCII: 63 The different data of the message is separated using this character
char ok = 'k'; // ASCII: 107  Send when the message is received and validated
char error = '!'; // ASCII: 33 Send when an error has ocurred and the message is not valid
char on = '1'; // ASCII 48 On character in ASCII from serial connection
char push = 'P'; //When received in the correct format, this character fires the push function that will update the frame
int messageSize = 38; //Default message size
int conectionSpeed = 57600; //bauds. default 57600
int serialBufferSize;
char serialBuffer[38];

//tempFrame default = 11111111 Binary
byte tempFrame[4][4][2] = {
{{255,255}, {255,255}, {255,255}, {255,255}},
{{255,255}, {255,255}, {255,255}, {255,255}},
{{255,255}, {255,255}, {255,255}, {255,255}},
{{255,255}, {255,255}, {255,255}, {255,255}}};  

//Frame default = 1010101010 Binary
byte frame[4][4][2] = 
{{{170,170}, {170,170}, {170,170}, {170,170}},
{{170,170}, {170,170}, {170,170}, {170,170}},
{{170,170}, {170,170}, {170,170}, {170,170}},
{{170,170}, {170,170}, {170,170}, {170,170}}};

//Cube multiplexer serial connection
int latch = 12;
int clock = 11;
int data = 10;

//Pins for the layers of the cubes
int layers[4] = {9,8,7,6};

// Time to wait after the latch signal has been sent and before turning on the layer
// default: 25us
int offset = 25;

//Time that the layer will stay turned on. This number represents 1/4 of the refresh rate
//default: 2500us, this equals a refresh rate of 1ms + 100us offset
int frametime = 2500;

void setup () {
  Serial.begin(conectionSpeed);
  
  pinMode(latch,OUTPUT);
  pinMode(clock,OUTPUT);
  pinMode(data,OUTPUT);
  for (int i=0; i<4; i++) pinMode(layers[i],OUTPUT);
    
  //Turn off the LED on pin 13
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
}

void loop () {
  checkSerialBuffer();
  writeCubes();
}

void checkSerialBuffer () {
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
    
    //Check if cube number is within acceptable range
    if (serialBuffer[1] == '0' || serialBuffer[1] == '1' || serialBuffer[1] == '2' || serialBuffer[1] == '3') {
      
      int workingCube, workingLayer, temp;
      
      //Convert ASCII to int
      switch (serialBuffer[1]) {
       case '0': workingCube = 0; break;
       case '1': workingCube = 1; break;
       case '2': workingCube = 2; break;
       case '3': workingCube = 3;
      }
      
      if (serialBuffer[3] == '0' || serialBuffer[3] == '1') {
        //Received valid data
        workingLayer = (serialBuffer[3] == '0') ? 0 : 2;
        
        for (int i=0; i<16; i++) {
          temp = (serialBuffer[i+5] == on) ? 1 : 0;
          if (i<=7) {
            if (temp == 0) bitSet(tempFrame[workingCube][workingLayer][0], 7-i);
            else bitClear(tempFrame[workingCube][workingLayer][0], 7-i);
          } else {
             if (temp == 0) bitSet(tempFrame[workingCube][workingLayer][1], 7-(i-8));
             else bitClear(tempFrame[workingCube][workingLayer][1], 7-(i-8)); 
          }          
        }
        
        for (int i=0; i<16; i++) {
          temp = (serialBuffer[i+21] == on) ? 1 : 0;
          if (i<=7) {
            if (temp == 0) bitSet(tempFrame[workingCube][workingLayer+1][0], 7-i);
            else bitClear(tempFrame[workingCube][workingLayer+1][0], 7-i);
          } else {
             if (temp == 0) bitSet(tempFrame[workingCube][workingLayer+1][1], 7-(i-8));
             else bitClear(tempFrame[workingCube][workingLayer+1][1], 7-(i-8)); 
          }          
        }
        //Serial.write(ok);
      } else {
        //Serial.write(error);
      }
    } else {
      //Serial.write(error);
    }
  } else if (serialBuffer[0] == push && serialBuffer[2] == push && serialBuffer[37] == push) {
    pushFrame();
  } else {
    //Serial.write(error);
  }
}

//move the values from the temp frame to the real frame
void pushFrame () {
  for (int i=0;i<4;i++) for (int j=0;j<4;j++) for (int k=0;k<2;k++) frame[i][j][k] = tempFrame[i][j][k];
}

void flushBuffer (int until) {
  char flushs;
  for (int i=0;i<until;i++) flushs = Serial.read(); 
}

//Write te image on the cube the image on the cube
void writeCubes () {
  for (int i=0; i<4; i++) {
       digitalWrite(latch, LOW);
       if (i==0) digitalWrite(layers[3],LOW);
       else digitalWrite(layers[i-1],LOW);
       
       for (int j=0;j<4;j++) for (int k=0; k<2; k++) shiftOut(data, clock, MSBFIRST, frame[j][i][k]);
       
       digitalWrite(latch, HIGH);
       delayMicroseconds(offset);
       digitalWrite(layers[i], HIGH);
       delayMicroseconds(frametime);
     }
}
