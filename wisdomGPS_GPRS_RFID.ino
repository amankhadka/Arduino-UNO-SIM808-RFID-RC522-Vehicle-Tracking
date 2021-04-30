/* Tested on march 12 in my office with guidance from boss saurav sir.
 *  Discovered the problem is with the sim but not woith the code,
 *  GPS is successfully sent through GPRS in every 7 sec with delay 5 sec
 *  
 *  The sketch is updated for delay and tested ok for NTC sim all working great
 *  Preparing to test in ncell M2M sim
 *  Updated on 4-19-2021 for delayed transmit around 8-10 sec.
 */


//***************************************Header Files*****************************************

#include <SIM808.h>
#include <ArduinoLog.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

//**********************************Header Files Close****************************************



//**********************************Define PINs*********************************************** 
#define SS_PIN    10 // Configurable, see typical pin layout above
#define RST_PIN   9 // Configurable, see typical pin layout above
#define BUZ       8 ///< LED status to check various status of the module
#define SIM_PWR   7 ///< SIM808 PWRKEY
#define SIM_RST   6 ///< SIM808 RESET
#define SIM_TX    5 ///< SIM808 TXD
#define SIM_RX    4 ///< SIM808 RXD
#define SIM_STATUS 3 ///< SIM808 STATUS
#define IRQ_PIN     2 ///< RFID interrupt receive pin
#define statLED    A0 ///< LED status to check various status of the module

//**********************************Define PINs Close***************************************** 


//**********************************Define Others********************************************* 
#define SIM_SERIAL_TYPE  SoftwareSerial          ///< Type of variable that holds the Serial communication with SIM808
#define SIM_SERIAL    SIM_SERIAL_TYPE(SIM_TX, SIM_RX) ///< Definition of the instance that holds the Serial communication with SIM808    

#define SIM808_BAUDRATE 9600    ///< Controls the serial baudrate between the arduino and the SIM808 module
#define SERIAL_BAUDRATE 38400   ///< Controls the serial baudrate between the arduino and the computer
#define NL "\n"
#define cardFreq  8000  ///< Time out interval for every successive card read
#define APN "web"
#define deviceID "1"

typedef __FlashStringHelper* __StrPtr;

//**********************************Define Others Close*************************************** 


//********************************** Create instances****************************************

SIM_SERIAL_TYPE simSerial = SIM_SERIAL;
SIM808 sim808 = SIM808(SIM_RST, SIM_PWR, SIM_STATUS);

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

//********************************** End Instances******************************************

//********************************** Name spaces definination********************************

namespace strings {
    const char UNRECOGNIZED[] S_PROGMEM = "Unrecognized : %s" NL;
    const char UNKNOWN[] S_PROGMEM = "Unknown value";

    const char YES[] S_PROGMEM = "Yes";
    const char NO[] S_PROGMEM = "No";

    const char SUCCESS[] S_PROGMEM = "Success";
    const char FAILED[] S_PROGMEM = "Failed";
    const char ERROR[] S_PROGMEM = "Error";

    const char ON[] S_PROGMEM = "On";
    const char OFF[] S_PROGMEM = "Off";
    const char STATUS[] S_PROGMEM = "Status";
    const char ALL[] S_PROGMEM = "All";

    const char MAIN[] S_PROGMEM = "Main";
    const char GPS[] S_PROGMEM = "Gps";
    const char NETWORK[] S_PROGMEM = "Network";

    const char MINIMUM[] S_PROGMEM = "Minimum";
    const char FULL[] S_PROGMEM = "Full";
    const char RF_DISABLED[] S_PROGMEM = "Disabled";
};
//********************************** End Name spaces defination*******************************


//********************************** Program Starts***********************************

char response[100];
volatile bool bNewInt = false;
char buffer[128];
char RFIDtoSend[150];
String RFIDdata="";
byte regVal = 0x7F;
int conErrorCnt=0;


void setup() {

  delay(3000);
  USBserialSetup();
  
  Sim808serialSetup();

  RFIDsetup();

  pinMode(statLED, OUTPUT);
  
  PowerONSIM808();
  delay(3000);
  
  powerONGPS();
  delay(3000);

  powerONGPRS();
  delay(3000);

}//setup

void loop() {

  char * parsedPos[8];
  char position[128]="";
  char temp[128];
  char dataToSend[150]="";
  String RFIDtag;
  RFIDtag="RFID,"+String(deviceID)+":";
  char RFIDchar[100];
  bool stat;
  unsigned long prev;
  
  if(!sim808.getGprsPowerState(&stat)){
    sim808.disableGprs();
    powerONGPRS();
  }
      
  digitalWrite(statLED, HIGH);
  sim808.getGpsPosition(position, 128);
  //delay(2000);
  //Log.notice(S_F("Position Data before: %s" NL),position);

  
  //////////////////////////////////////////////////Tokenize GPS satallite data /////////////////////////////////////////////////
  
  strcpy(temp,position);
  char* token = strtok(temp, ",");
  
  // loop through the string to extract all other tokens from position data
  int k=0;
  while(k<8) 
  {
    //Serial.println( " %s\n", token ); //printing each token
    parsedPos[k]=token;
    token = strtok(NULL, ",");
    k++;
  }

  ///////////////////////////////////////////////Tokenized Done ////////////////////////////////////////////////////////////////
    sprintf(dataToSend,"Pos,%s,%s,%s,%s,%s,%s,%s",deviceID,parsedPos[2],parsedPos[3],parsedPos[4],parsedPos[5],parsedPos[6],parsedPos[7]);
    Log.notice(S_F("Position Data: %s" NL),dataToSend);

  ///////////////////////////////////////////// Prepare data to send to TCP socket/////////////////////////////////////////////////
  if(strlen(parsedPos[4])>5)
  {
    digitalWrite(statLED, HIGH);
    if(sendGPRSToSocket(dataToSend))
      {
         Log.notice(S_F("Position sent to TCP server" NL));
         dataToSend[0]='\0';
         conErrorCnt=0;
      }
    else // IF no data sent to socket
      { 
        Log.notice(S_F("Position not sent to TCP server" NL));
        digitalWrite(statLED, LOW);
         dataToSend[0]='\0';
        delay(500);
        if(conErrorCnt > 8){
          sim808.disableGprs();
          delay(300);
          powerONGPRS();
          conErrorCnt=0;
        }
          LEDflashFast(5000);
      }
  }
  else //If there is no exact required GPS
  {
    Log.notice(S_F("No data from GPS..." NL));
    digitalWrite(statLED, HIGH);
    delay(1000);
    digitalWrite(statLED, LOW);
    delay(1000);
  } 
    // End handling GPRS data send

    ///////////////////////////////////////////// Prepare RFID data iF interrupt from RFID module and send it to TCP socket///

  prev=millis();  
  //if(readRFID())
  //{
    do
    {
      //Serial.println(F("There is card"));
      if(readRFID()){
        if(strlen(RFIDtoSend)<5)
        {
          RFIDdata=RFIDtag+RFIDdata;
        }
        RFIDdata.toCharArray(RFIDchar,RFIDdata.length()+1); 
        strcat(RFIDtoSend,RFIDchar);
        RFIDdata="";
                
        //Log.notice(S_F("RFID data to send:%s" NL),RFIDtoSend);
        //if(readRFID())
        prev=millis();
      }
      delay(1000);
    }while(millis() - prev < cardFreq);
  //}//End if for RFID 
  
  //****************************************RFID data send to TCP server*****************
  
  Log.notice(S_F("RFID Data: %s" NL),RFIDtoSend);
  
  if(strlen (RFIDtoSend) > 10)
  {
    digitalWrite(statLED, HIGH);
    if(sendGPRSToSocket(RFIDtoSend))
    {
      Log.notice(S_F("RFID Sent to TCP server" NL));
      RFIDtoSend[0]='\0';
    }
    else
    {
      Log.notice(S_F("RFID not Sent to TCP server" NL));
      digitalWrite(statLED, HIGH);
      delay(1000);
      digitalWrite(statLED, LOW);
      delay(1000);
    }
  }
  //delay(3000);
  
}//loop


void restartSim808()
{
    PowerONSIM808();
    delay(1000);
  
    powerONGPS();
    delay(1000);

    powerONGPRS();
    delay(1000);
}
