#include <Regexp.h>
#include <extEEPROM.h>
#include <Embedis.h>
#include <Entropy.h>
#include <Keyboard.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <SPI.h>
#include <HID.h>
#include <string.h>
//#include <uart.h>

const String lcd1 = "****************\n\r* PassGen9     *\n\r****************\n\r";
const int _MAX_BUFFER_LEN_ = 80;
int _NAME_ = 0;
const int _NAME_MAX_ =15;
const int _URL_MAX_ = 256;
const String google = "https://www.google.ca";
const String pw_str = "Password :";

//Globals
char pw[9];
String google_user;
//byte buff2[_NAME_MAX_];
char buff2[_NAME_MAX_];


// meaningful constants
byte SC_A = 0x04;              byte SC_SPACE = 0x2c;            byte MOD_SHIFT = 0xf1;
byte SC_B = 0x05;              byte SC_ENTER = 0x28;            byte MOD_ALT = 0xf2;
byte SC_C = 0x06;              byte SC_BCKSPC = 0x2a;           byte MOD_CTRL = 0xf0;
byte SC_D = 0x07;              byte SC_TAB = 0x2b;
byte SC_E = 0x08;              byte SC_ESCAPE = 0x29;
byte SC_F = 0x09;              byte SC_ESC = 0x29;
byte SC_G = 0x0a;              byte SC_RIGHT_ARROW = 0xef;
byte SC_H = 0x0b;              byte SC_LEFT_ARROW = 0x50;
byte SC_I = 0x0c;              byte SC_DOWN_ARROW = 0x51;
byte SC_J = 0x0d;              byte SC_UP_ARROW = 0x52;
byte SC_K = 0x0e;              
byte SC_L = 0x0f;              
byte SC_M = 0x10;              
byte SC_N = 0x11;              
byte SC_O = 0x12;              
byte SC_P = 0x13;              
byte SC_Q = 0x14;              
byte SC_R = 0x15;              
byte SC_S = 0x16;              
byte SC_T = 0x17;              
byte SC_U = 0x18;              
byte SC_V = 0x19;              
byte SC_W = 0x1a;              
byte SC_X = 0x1b;              
byte SC_Y = 0x1c;              
byte SC_Z = 0x1d;

void sendSCByte(byte byteToSend)
{
 // This function sends single scancode bytes.
 // If using this function to send MOD bytes,
 // keep in mind the above notes.
 Serial.print(byteToSend);
 //delay(25);
}

void sendSCString(String stringToSend)
{
 byte charByte;
 int lengthOfString = stringToSend.length();
 for ( int i = 0; i < lengthOfString; i++)
 {
   charByte = stringToSend.charAt(i);
   if(charByte >= 0x61 && charByte <= 0x7a)
     sendSCByte(charByte - 0x5d);
   else if(charByte >= 0x41 && charByte <= 0x5a)
   {
     sendSCByte(MOD_SHIFT);
     sendSCByte(charByte - 0x3d);
   }
   else if(charByte == 0x20)
     sendSCByte(SC_SPACE);
 }
}

// This routine will convert a 6 bit value (0-63) to an acceptible password character
char mapChar(uint32_t parm)
{
  char retval;
  if (parm < 10)           // map 0..9 to ascii 0..9
    retval = char(48 + parm);
  else if (parm < 11)      // map 10 to -
    retval = '-';
  else if (parm < 12)      // map 11 to +
    retval = '.';
  else if (parm < 38)      // map 12 to 37 to ascii A..Z
    retval = char(53 + parm);
  else if (parm < 64)      // map 38 to 63 to ascii a..z
    retval = char(59 + parm);
  else
    retval = 0;            // if parm is invalid return null  
  return(retval);
}

// This routine uses the Entropy library to obtain truly random 6 bit values
// and to map that to an eight character cryptographically secure password
char *getPassword(char *pw)
{
  char ch;
  int indx;
  uint32_t tmp;
  
  
  for (indx=0; indx<8; indx++)
  { 
    tmp = mapChar(Entropy.random(64));
    pw[indx] = (char) tmp;
  }
  pw[8] = 0;
  return(pw);
}
/* removed for infineon board test
int eepromPut(int addr, String key, int maxLen, String message){
  message = Serial.read();
  if(message.length() <= maxLen){
    //Eeprom write
    Serial.println( "Writing:" + key + " " + message); 
    EEPROM.put(_NAME_, message);
    Serial.print("OK");
  } else {
    Serial.print("!!");
    return 1;
  }
  return 0;
}*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo and Due
  }
  
  Entropy.initialize();  

  Serial.println("Setup: OK");
  Serial.print(lcd1);
  
}

void loop() {
  byte buff1[_MAX_BUFFER_LEN_];//255
  String stringATCommand;
  // put your main code here, to run repeatedly:
  Serial.print(pw_str);
  Serial.println(getPassword(pw));
  /*static char buffer1[80];
  if (readline(Serial.read(), buffer1, 80) > 0) {
    Serial.print("You entered: >");
    Serial.print(buffer1);
    Serial.println("<");
  }*/

  
  //Command Parser
  //while(!Serial.available()){
  //stringATCommand = 
  String cmd;
  if(cmd.length() > 0){
    // clear buffer
    cmd = "";
    for(int i; i<= _MAX_BUFFER_LEN_;i++)
    {
      buff1[i]= 0x00; // "";
    }
  }
  //cmd = Serial.readBytes( buff1, _MAX_BUFFER_LEN_);
  cmd = Serial.readBytesUntil('%', buff1, _MAX_BUFFER_LEN_);
  int address = 0;
  byte value;
  //String cmd = buff1;
  //Serial.println("Command buffer :" + cmd);
      String x; //used for reading in user inputs on serial line
        for(int i=0;i<=_MAX_BUFFER_LEN_;i++){
            switch(buff1[i])
            {
              case 'G': 
              i++;
              Serial.println("CMD received");
                switch(buff1[i])
                {
                  case 'T':
                    i++;
                    Serial.println("Test: OK");
                    break;
                  case 'E':
                    Serial.println("CMD received");
                    /*
                    //Get EEPROM
                    
                    ///dump eeprom to shell
                    while( address <= EEPROM.length() ){// read a byte from the current address of the EEPROM
                      value = EEPROM.read(address);
                      
                      //Serial.print(address);
                      //Serial.print("\t");
                      Serial.print(value, HEX);
                      Serial.print(" ");
                      //Serial.println();
                    
                      
                      //  Advance to the next address, when at the end restart at the beginning.
                    
                      //  Larger AVR processors have larger EEPROM sizes, E.g:
                      //  - Arduno Duemilanove: 512b EEPROM storage.
                      //  - Arduino Uno:        1kb EEPROM storage.
                      //  - Arduino Mega:       4kb EEPROM storage.
                    
                      //  Rather than hard-coding the length, you should use the pre-provided length function.
                      //  This will make your code portable to all AVR processors.
                     
                      if (address % 8 == 1){
                        Serial.println();
                      }
                      address = address + 1;
                      
                      if (address == EEPROM.length()) {
                        
                        //address = 0;
                      }
                      
                    }
                    Serial.println("\n\rEnd of EEPROM: OK")
                    */;
                    break;
                  case 'P':
                      //Output a password
                      Serial.println(getPassword(pw));
                      break;
                
              
                   default:
                      Serial.println("!!");
                   break;
                }
                break; //end of g input
                
               case 'S':
                i++;
                switch(buff1[i])
                {
                  case 'N':
                    //Send the device a name and save to eeprom
                    
                    Serial.print("Enter Name:");
                    //do
                    //{
                    //}while(Serial.readBytesUntil('$', buff2, _NAME_MAX_));
                    while (Serial.available() <= 0) {
                        //Serial.println("0,0,0");   // send an initial string
                        delay(300);
                    }
                    Serial.readBytesUntil('\n', buff2, _NAME_MAX_);
                    for(int k=0;k<_NAME_MAX_;k++){
                        Serial.print(buff2[k]);
                    }
                    Serial.println(' ');
                    //eepromPut(_NAME_, String(buff2), _NAME_MAX_, x);
                    for(int i; i<= _MAX_BUFFER_LEN_;i++)
                    {
                        buff1[i]= 0x00; // clear buffer
                    }
                  break;
                }
                break;
               case 'P':
                i++;
                //if(stringATCommand.substring(i,i+4) == 'M'){
                  //Send your message in format x.
                  //Serial.print("OK");
                //}
                break;
              }
          } // for loop end


          
          //} else {
            //Serial.println("Nothing to Do");
            //Serial.println("$:_" + buff1 + "_:$");
          //}
          
          
        
}//end main loop
 
