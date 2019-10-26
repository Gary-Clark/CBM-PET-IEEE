//Author  : G Clark
//Date    : 23 October 2019
//Version : 1.0
//Changes : Initial working version
//
//Author  : G Clark
//Date    : 26 October 2019
//Version : 1.1
//Changes : Changed the end of program from detecting three 0x00 to reading the size of the file
//          and pulling EOI down for the last byte. This will ensure any machine code after the
//          basic program is also sent.
//          Built in LED set to follow bit 7 of the loading index variable so it flashes as a program
//          is loaded.
//
const int DIO1 = 37;
const int DIO2 = 36;
const int DIO3 = 35;
const int DIO4 = 34;
const int DIO5 = 33;
const int DIO6 = 32;
const int DIO7 = 31;
const int DIO8 = 30;

const int EOI   = 28;
const int DAV   = 27;
const int NRFD  = 26;
const int NDAC  = 25;
const int IFC   = 24;
const int SQR   = 23;
const int ATN   = 22;
const int CS    = 53;

#include <SPI.h>
#include <SD.h>

byte file[500];
Sd2Card   card;
SdVolume  volume;
SdFile    root;

    
void setup() {
File root;

  digitalWrite(NRFD,LOW);   // and pull low to signal the device isnt ready
  pinMode(NRFD,OUTPUT);     // First job, set NRFD as an output
  pinMode(DIO1,INPUT);      // setup the dataport as an input ready to accept commands
  pinMode(DIO2,INPUT);
  pinMode(DIO3,INPUT);
  pinMode(DIO4,INPUT);
  pinMode(DIO5,INPUT);
  pinMode(DIO6,INPUT);
  pinMode(DIO7,INPUT);
  pinMode(DIO8,INPUT);
  pinMode(EOI,LOW);
  pinMode(EOI,INPUT);
  pinMode(DAV,INPUT);
  pinMode(ATN,INPUT);
  digitalWrite(NDAC,LOW);   // Pull data not accepted low
  pinMode(NDAC,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("CBM disk drive :");

  if (!SD.begin(CS)) {
    Serial.println("SD card initialization failed!");
    while (1);
  }
  root = SD.open("/");
  printDirectory(root, 0);

  delay(500);
  pinMode(NRFD,INPUT);      // Let NRFD go high to signal startup complete
}

void ATN_(){
byte addr,command;

// get listner address
  while(digitalRead(DAV) == HIGH){}
  pinMode(NRFD,OUTPUT);             // set line to indicate not ready for data
  addr = ~PINC ;
  pinMode(NDAC,INPUT);              // allow to go high
  while(digitalRead(DAV) == LOW){}
  if (addr != 0x29) {               // if the ATN message is for another device
    pinMode(NRFD,INPUT);            // Set NRFD & NDAC to inputs to allow the addressed
    pinMode(NDAC,INPUT);            // device to control them
    return;                         // and exit to loop waiting for ATN
  }
  pinMode(NDAC,OUTPUT);
  pinMode(NRFD,INPUT);
    
// get command
  while(digitalRead(DAV) == HIGH){}
  pinMode(NRFD,OUTPUT);
  delay(5);
  command = ~PINC;
  Serial.print("Command received = ");
  Serial.println(command,HEX);
  pinMode(NDAC,INPUT);
  while(digitalRead(DAV) == LOW){}
  delay(5);
  pinMode(NDAC,OUTPUT);

  if (command == 0xF1) {Save();}
  if (command == 0xF0) {Load();}
  if (command == 0x6F) {Serial.println("channel 15 command received");}
  
  return;
}

void Save(){
byte addr,command,drive;
bool EOI_;
char file_name[] = "00000000.PRG";
int index;
File file_;   

Serial.println("Saving");
    pinMode(NRFD,INPUT);
// loop for file name
    Serial.print("File name = ");
    index = 0;
    EOI_ = HIGH;
    while (EOI_ == HIGH){
      delay(1);
      while(digitalRead(DAV) == HIGH){}
      if (digitalRead(EOI) == LOW){
        EOI_ = LOW;
      }
      pinMode(NRFD,OUTPUT);
      delay(1);
      file_name[index] = (~PINC);
      Serial.print(char(file_name[index]));
      index=index+1;
      pinMode(NDAC,INPUT);   //Allow NDAC to go high, data read
      while(digitalRead(DAV) == LOW){}
      pinMode(NDAC,OUTPUT);  //NDAC low for next read
      pinMode(NRFD,INPUT);   //NRFD high to signal ready
   }
    Serial.println("");
       
    while(digitalRead(DAV) == HIGH){} 
    pinMode(NRFD,OUTPUT);
    command = (~PINC);
    pinMode(NDAC,INPUT);
    while(digitalRead(DAV) == LOW){}
    pinMode(NDAC,OUTPUT);
    pinMode(NRFD,INPUT);

    while(digitalRead(DAV) == HIGH){} 
    pinMode(NRFD,OUTPUT);
    command = (~PINC);
    pinMode(NDAC,INPUT);
    while(digitalRead(DAV) == LOW){}
    pinMode(NDAC,OUTPUT);
    pinMode(NRFD,INPUT);

    while(digitalRead(DAV) == HIGH){} 
    pinMode(NRFD,OUTPUT);
    command = (~PINC);
    pinMode(NDAC,INPUT);
    while(digitalRead(DAV) == LOW){}
    pinMode(NDAC,OUTPUT);

    if (SD.exists(file_name)){
      SD.remove(file_name);
    }
    file_ = SD.open(file_name, FILE_WRITE);
    pinMode(NRFD,INPUT);
    
    index = 0;
    EOI_ = HIGH;
    while (EOI_ == HIGH){
      while(digitalRead(DAV) == HIGH){}
      if (digitalRead(EOI) == LOW){
        EOI_ = LOW;
      }
      pinMode(NRFD,OUTPUT);
      file_.write(~PINC);
      digitalWrite(LED_BUILTIN, bitRead(PINC,7));
      pinMode(NDAC,INPUT);   //Allow NDAC to go high, data read
      while(digitalRead(DAV) == LOW){}
      pinMode(NDAC,OUTPUT);  //NDAC low for next read
      pinMode(NRFD,INPUT);   //NRFD high to signal ready
    }
    file_.close();
    digitalWrite(LED_BUILTIN, LOW);   
    while(digitalRead(DAV) == HIGH){}
    pinMode(NRFD,OUTPUT); // set low
    delay(1);
    addr = ~PINC ;
    pinMode(NDAC,INPUT);  // allow to go high
    while(digitalRead(DAV) == LOW){}
    pinMode(NDAC,OUTPUT);

    pinMode(NRFD,INPUT);
    while(digitalRead(DAV) == HIGH){}
    pinMode(NRFD,OUTPUT); // set low
    delay(1);
    addr = ~PINC ;
    pinMode(NDAC,INPUT);  // allow to go high
    while(digitalRead(DAV) == LOW){}
    pinMode(NDAC,OUTPUT);

    pinMode(NRFD,INPUT);
    while(digitalRead(DAV) == HIGH){}
    pinMode(NRFD,OUTPUT); // set low
    delay(1);
    addr = ~PINC ;
    pinMode(NDAC,INPUT);  // allow to go high
    while(digitalRead(DAV) == LOW){}
    pinMode(NDAC,OUTPUT);
    
    pinMode(NRFD,INPUT);
    while(digitalRead(DAV) == HIGH){}
    pinMode(NRFD,OUTPUT); // set low
    delay(1);
    addr = ~PINC ;
    pinMode(NDAC,INPUT);  // allow to go high
    while(digitalRead(DAV) == LOW){}
    pinMode(NDAC,OUTPUT);
    
    pinMode(NRFD,INPUT);
    delay(200);
    Serial.println("save complete");
    return;
}

void Load(){
byte addr,command,data,drive;
bool EOI_;
char file_name[] = "00000000.PRG";
unsigned int index,offset;
unsigned int size_;
File file_;
File dir;
bool eof;

 byte footer[] = {0xDD,0x04,0xFF,0xFF,0x42,0x4C,0x4F,0x43,0x4B,0x53,0x20,0x46,0x52
                 ,0x45,0x45,0x2E,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20
                 ,0x20,0x20,0x20,0x00,0x00,0x00};
 byte header[] = {0x01,0x04,0x1F,0x04,0x01,0x00,0x12,0x22,0x50,0x45,0x54,0x44,0x49
                 ,0x53,0x4B,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x22,0x20
                 ,0x38,0x38,0x20,0x32,0x43,0x00};
 byte fname[]  = {0x3F,0x04,0x01,0x00,0x20,0x20,0x20,0x22,0x41,0x41,0x41,0x41,0x41
                 ,0x41,0x41,0x41,0x22,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20
                 ,0x50,0x52,0x47,0x20,0x20,0x00};                     
Serial.println("Loading");
drive=0;
// loop for file name
    index = 0;
    EOI_ = HIGH;
    Serial.print("Looking for file ");
    while (EOI_ == HIGH){
      pinMode(NRFD,INPUT);   //NRFD high to signal ready
      delay(5);
      while(digitalRead(DAV) == HIGH){}
      if (digitalRead(EOI) == LOW){
        EOI_ = LOW;
      }
      pinMode(NRFD,OUTPUT);
      file_name[index] = (~PINC);
      Serial.print(char(file_name[index]));
      index=index+1;
      pinMode(NDAC,INPUT);   //Allow NDAC to go high, data read
      while(digitalRead(DAV) == LOW){}
      pinMode(NDAC,OUTPUT);  //NDAC low for next read
   }
// Check if file exists and send error if not needs to go here
// if filename is $ then need to send directory
    
    pinMode(NRFD,INPUT);
    while(digitalRead(DAV) == HIGH){}
    pinMode(NRFD,OUTPUT); // Stop data
    addr = ~PINC ;                              // UNL 0x3f
    pinMode(NDAC,INPUT);  // Set accept flag
    while(digitalRead(DAV) == LOW){}
    delay(1);
    pinMode(NDAC,OUTPUT); // Clear accept flag
    delay(10);
    
    pinMode(NRFD,INPUT);
    while(digitalRead(DAV) == HIGH){}
    pinMode(NRFD,OUTPUT); // Stop data
    addr = ~PINC ;                              // TAG 08 0x48
    pinMode(NDAC,INPUT);  // Set accept flag
    while(digitalRead(DAV) == LOW){}
    delay(1);
    pinMode(NDAC,OUTPUT); // Clear accept flag
    delay(10);
    
    pinMode(NRFD,INPUT);
    while(digitalRead(DAV) == HIGH){}
    delay(1);
    pinMode(NRFD,OUTPUT); // Stop data
    addr = ~PINC ;                              // SCG 00 0x60
    pinMode(NDAC,INPUT);  // Set accept flag low
    while(digitalRead(DAV) == LOW){}
    delay(1);
    pinMode(NDAC,OUTPUT); // Clear accept flag
    delay(10);

// Step 10 to 21 - Device is now the talker
    Serial.println("");
    Serial.println("Sending file to the PET");
    DDRC = B11111111;
    pinMode(NRFD,INPUT);
    pinMode(NDAC,INPUT);
    digitalWrite(DAV,LOW);
//    digitalWrite(EOI,HIGH);
//    pinMode(EOI,OUTPUT);
    pinMode(DAV,INPUT);
//    Serial.print("file name = ");
//    Serial.println(file_name);

// #########################Send directory list########################################    
    if (strcmp(file_name,"$0000000.PRG")==0) {
    Serial.println("dir command");
    EOI_ = HIGH;
    while (EOI_ == HIGH){
      index = 0;
      
        for (index = 0;index <32;index++){
          data=header[index];
          PORTC= ~data;
          while(digitalRead(NRFD) == 0){}
          pinMode(DAV,OUTPUT);
          while(digitalRead(NDAC) == LOW){}
          pinMode(DAV,INPUT);
          }

    eof = false;
    offset = 0x043F;
    dir = SD.open("/");
    File entry =  dir.openNextFile();
    while(true){
      File entry =  dir.openNextFile();
      Serial.println(entry.name());
      if (! entry) {break;}
      char* names = entry.name();
      fname[0] = lowByte(offset);
      fname[1] = highByte(offset);
      fname[2] = lowByte(entry.size());
      fname[3] = highByte(entry.size());
      for(int i=0;i<=7;i++){
        fname[i+8] = names[i];}
      offset = offset + 0x20;
      entry.close();
      
         Serial.println("");
         for (index = 0;index <32;index++){
          data=fname[index];
          PORTC= ~data;
          while(digitalRead(NRFD) == 0){}
          pinMode(DAV,OUTPUT);
          while(digitalRead(NDAC) == LOW){}
          pinMode(DAV,INPUT);
          }
        }
        Serial.println("file names sent");
        for (index = 0;index <32;index++){
          data=footer[index];
          PORTC= ~data;
          while(digitalRead(NRFD) == 0){}
          pinMode(DAV,OUTPUT);
          while(digitalRead(NDAC) == LOW){}
          pinMode(DAV,INPUT);
          }
        EOI_ = LOW;
        pinMode(EOI,OUTPUT);
        delay(10);
        }
      pinMode(DAV,OUTPUT);
      while(digitalRead(NDAC) == LOW){}
      delay(0);
 
      pinMode(DAV,INPUT);
      pinMode(EOI,INPUT);
      }
// ####################################Send file ############################################
          
    else if (SD.exists(file_name)){
      EOI_ = HIGH;
      index = 0;
      file_ = SD.open(file_name, FILE_READ);
      size_ = file_.size();

      while (EOI_ == HIGH) {         
        digitalWrite(LED_BUILTIN, bitRead(index,7));
        data=file_.read();
        PORTC= ~data;
        while(digitalRead(NRFD) == 0){}
        index = index +1;
        if (index == (size_)){
          EOI_ = LOW;
          pinMode(EOI,OUTPUT);
        }      
        pinMode(DAV,OUTPUT);
 //       delay(1);
        while(digitalRead(NDAC) == LOW){}
        pinMode(DAV,INPUT);
      }
    pinMode(EOI,INPUT);
    file_.close();
    digitalWrite(LED_BUILTIN, LOW);
    }
    else {
      Serial.println("");
      Serial.println("File not found");
      pinMode(EOI,OUTPUT);
      pinMode(DAV,OUTPUT);
      while(digitalRead(NDAC) == LOW){}
      pinMode(DAV,INPUT);
      pinMode(EOI,INPUT);
    }
 
// steps 22 to 25
    digitalWrite(NRFD,LOW);
    pinMode(NRFD,OUTPUT);
    pinMode(NDAC,OUTPUT);
    pinMode(EOI,INPUT);
    pinMode(DAV,INPUT); 
    DDRC = B00000000;
    
  MCUSR |= _BV(PUD);    
    delay(10);
    digitalWrite(NRFD,HIGH);
    while(digitalRead(DAV) == HIGH){}
    pinMode(NRFD,OUTPUT); // set low
    delay(5);
    addr = ~PINC ;                              // UNT
    pinMode(NDAC,INPUT);  // allow to go high
    while(digitalRead(DAV) == LOW){}
    pinMode(NDAC,OUTPUT); 
    pinMode(NRFD,INPUT);

    while(digitalRead(DAV) == HIGH){}
    pinMode(NRFD,OUTPUT); // set low
    delay(1);
    addr = ~PINC ;                              // LAG 08
    pinMode(NDAC,INPUT);  // allow to go high
    while(digitalRead(DAV) == LOW){}
    pinMode(NDAC,OUTPUT); 
    pinMode(NRFD,INPUT);

    while(digitalRead(DAV) == HIGH){}
    pinMode(NRFD,OUTPUT); // set low
    delay(1);
    addr = ~PINC ;                              // SCG 00
    pinMode(NDAC,INPUT);  // allow to go high
    while(digitalRead(DAV) == LOW){}
    pinMode(NDAC,OUTPUT); 
    pinMode(NRFD,INPUT);

    while(digitalRead(DAV) == HIGH){}
    pinMode(NRFD,OUTPUT); // set low
    delay(1);
    addr = ~PINC ;                              // UNL
    pinMode(NDAC,INPUT);  // allow to go high
    while(digitalRead(DAV) == LOW){}
    pinMode(NDAC,OUTPUT); 
    pinMode(NRFD,INPUT);
        
    Serial.println("load complete");

return;
}

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  byte Control;
  if (digitalRead(ATN) == 0) {
      pinMode(NRFD,INPUT);
      pinMode(NDAC,OUTPUT); 
      ATN_();
   }
 }
