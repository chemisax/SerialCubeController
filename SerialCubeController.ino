//Serial communication protocol
char messageStart = '<'; // ASCII: 60 The message will always start with this character
char messageEnd = '>'; // ASCII: 62 The message will always end with this character
char dataSeparator = '?'; // ASCII: 63 The different data of the message is separated using this character
char ok = 'k'; // ASCII: 107  Send when the message is received and validated
char error = '!'; // ASCII: 33 Send when an error has ocurred and the message is not valid
char on = '1'; // ASCII 48 On character in ASCII from serial connection
char push = 'P'; //When received in the correct format, this character fires the push function that will update the frame
int messageSize = 38; //Default message size
int conectionSpeed = 57600; //bauds
int serialBufferSize;
char serialBuffer[38];

byte tempFrame[4][4][2] = {
{{B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}},
{{B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}},
{{B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}},
{{B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}}};

//Animation
byte frame[4][4][2] = {
{{B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}},
{{B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}},
{{B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}},
{{B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}, {B10101010,B10101010}}};

//Cube multiplexer serial connection
int latch = 12;
int clock = 11;
int data = 10;

//Pins for the layers of the cubes
int layers[4] = {9,8,7,6};

//Time to wait after the latch signal has been sent and before turning on the layer
//In microseconds. default: 25
int offset = 25;

//Time that the layer will stay turned on. This number represents 1/4 of the refresh rate
//default: 2500ms, this equals a refresh rate of 1ms
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
  serialBufferSize = Serial.available();
  if (serialBufferSize == messageSize) {
    for (int i=0;i<serialBufferSize;i++) serialBuffer[i] = Serial.read();
    parseData();
  } else if (serialBufferSize > messageSize) {
    flushBuffer(serialBufferSize);
  }
  writeCubes();
}

void parseData () {
  //Check Integrity
  if (serialBuffer[0] == messageStart && serialBuffer[2] == dataSeparator && serialBuffer[4] == dataSeparator && serialBuffer[37] == messageEnd) {
    if (serialBuffer[1] == '0' || serialBuffer[1] == '1') {
      int workingCube;
      int workingLayer;
      
      workingCube = (serialBuffer[1] == '1') ? 1 : 0;  
      
      if (serialBuffer[3] == '0' || serialBuffer[3] == '1') {
        //Received valid data
        workingLayer = (serialBuffer[3] == '0') ? 0 : 2;
        for (int i=0; i<16; i++) tempFrame[workingCube][workingLayer][i] = (serialBuffer[i+5] == on) ? 1 : 0;
        for (int i=0; i<16; i++) tempFrame[workingCube][workingLayer+1][i] = (serialBuffer[i+21] == on) ? 1 : 0;
        //Serial.write(ok);
      } else {
        //Serial.write(error);
      }
    } else {
      //Serial.write(error);
    }
  } else if (serialBuffer[0] == push && serialBuffer[2] == push && serialBuffer[37] == push) {
    //Push the data to the real array to update the frame
  } else {
    //Serial.write(error);
  }
}

void flushBuffer (int until) {
  //Serial.println(error);
  char flushs;
  for (int i=0;i<until;i++) flushs = Serial.read(); 
}

//Write te image on the cube the image on the cube
void writeCube () {
  for (int i=0; i<4; i++) {
       digitalWrite(latchP, LOW);
       if (i==0) digitalWrite(layers[3],LOW);
       else digitalWrite(layers[i-1],LOW);
       
       for (int j=0;j<4;j++) {
         shiftOut(data, clock, MSBFIRST, frame[j][i][0]);
         shiftOut(data, clock, MSBFIRST, frame[j][i][1]);
       }
       
       digitalWrite(latch, HIGH);
       delayMicroseconds(offset);
       digitalWrite(layers[i], HIGH);
       delayMicroseconds(frametime);
     }
}
