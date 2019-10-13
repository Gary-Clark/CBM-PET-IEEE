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

byte file[100];
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
  pinMode(EOI,INPUT);
  pinMode(DAV,INPUT);
  pinMode(ATN,INPUT);
  digitalWrite(NDAC,LOW);   // Pull data not accepted low
  pinMode(NDAC,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  Serial.println("CBM 8050 emulator :");

  if (card.init(SPI_HALF_SPEED, CS)) {
    Serial.println("Memory card checked ok");
  }
if (!SD.begin(CS)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

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
    return;
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

  if (command == 0xF1) { 
    Save(); 
  }
  if (command == 0xF0) { 
    Load(); 
  }
  return;
}

void Save(){
byte addr,command;
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
      delay(5);
      while(digitalRead(DAV) == HIGH){}
      if (digitalRead(EOI) == LOW){
        EOI_ = LOW;
      }
      pinMode(NRFD,OUTPUT);
      delay(5);
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
      digitalWrite(LED_BUILTIN, LOW);
      delay(0);
      while(digitalRead(DAV) == HIGH){}
      if (digitalRead(EOI) == LOW){
        EOI_ = LOW;
      }
      pinMode(NRFD,OUTPUT);
      delay(1);
      digitalWrite(LED_BUILTIN, HIGH);
      file_.write(~PINC);
      pinMode(NDAC,INPUT);   //Allow NDAC to go high, data read
      while(digitalRead(DAV) == LOW){}
      pinMode(NDAC,OUTPUT);  //NDAC low for next read
      pinMode(NRFD,INPUT);   //NRFD high to signal ready
    }
    file_.close();
    
    while(digitalRead(DAV) == HIGH){}
    pinMode(NRFD,OUTPUT); // set low
    delay(5);
    addr = ~PINC ;
    pinMode(NDAC,INPUT);  // allow to go high
    while(digitalRead(DAV) == LOW){}
    pinMode(NDAC,OUTPUT);

    pinMode(NRFD,INPUT);
    while(digitalRead(DAV) == HIGH){}
    pinMode(NRFD,OUTPUT); // set low
    delay(5);
    addr = ~PINC ;
    pinMode(NDAC,INPUT);  // allow to go high
    while(digitalRead(DAV) == LOW){}
    pinMode(NDAC,OUTPUT);

    pinMode(NRFD,INPUT);
    while(digitalRead(DAV) == HIGH){}
    pinMode(NRFD,OUTPUT); // set low
    delay(5);
    addr = ~PINC ;
    pinMode(NDAC,INPUT);  // allow to go high
    while(digitalRead(DAV) == LOW){}
    pinMode(NDAC,OUTPUT);
    
    pinMode(NRFD,INPUT);
    while(digitalRead(DAV) == HIGH){}
    pinMode(NRFD,OUTPUT); // set low
    delay(5);
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
byte addr,command,data;
bool EOI_;
char file_name[] = "00000000.PRG";
int index;
File file_;
Serial.println("Loading");

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
      delay(5);
      pinMode(NRFD,OUTPUT);
      delay(5);
      file_name[index] = (~PINC);
      Serial.print(char(file_name[index]));
      index=index+1;
      delay(5);
      pinMode(NDAC,INPUT);   //Allow NDAC to go high, data read
      while(digitalRead(DAV) == LOW){}
      delay(5);
      pinMode(NDAC,OUTPUT);  //NDAC low for next read
   }
// Check if file exists and send error if not needs to go here
// if filename is $ then need to send directory

    Serial.println("");
    Serial.println("file found");
    
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
    Serial.println("Sending file to the PET");
    DDRC = B11111111;
    pinMode(NRFD,INPUT);
    pinMode(NDAC,INPUT);
    digitalWrite(DAV,LOW);
    digitalWrite(EOI,HIGH);
    pinMode(EOI,OUTPUT);
    pinMode(DAV,INPUT);

    EOI_ = HIGH;
    index = 0;
    file_ = SD.open(file_name, FILE_READ);

    while (EOI_ == HIGH) {         
      digitalWrite(LED_BUILTIN, HIGH);
      while(digitalRead(NRFD) == 0){}
      data=file_.read();
      digitalWrite(DIO1,bitRead(~data,0));
      digitalWrite(DIO2,bitRead(~data,1));
      digitalWrite(DIO3,bitRead(~data,2));
      digitalWrite(DIO4,bitRead(~data,3));
      digitalWrite(DIO5,bitRead(~data,4));
      digitalWrite(DIO6,bitRead(~data,5));
      digitalWrite(DIO7,bitRead(~data,6));
      digitalWrite(DIO8,bitRead(~data,7));
      delay(1);
      digitalWrite(LED_BUILTIN, LOW);
      if (data == 0){
        index = index +1;
        }
      else {
        index = 0;
        }
      
      if( index == 3)  {
        EOI_ = LOW;
        digitalWrite(EOI,LOW);
        delay(10);
      }
      pinMode(DAV,OUTPUT);
      while(digitalRead(NDAC) == LOW){}
      delay(0);
 
      pinMode(DAV,INPUT);
      digitalWrite(EOI,HIGH);
    }

   file_.close();
 
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
