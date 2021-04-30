//****************************Serial setup***********************************

//*************************** Serial setup for usb connect to computer*******

void USBserialSetup()
{
    //while(!Serial);

    Serial.begin(SERIAL_BAUDRATE);
    Log.begin(LOG_LEVEL_NOTICE, &Serial);

}

//****************************Serial setup for SIM808 module*****************

void Sim808serialSetup()
{

    simSerial.begin(SIM808_BAUDRATE);
    sim808.begin(simSerial);

}

//****************************END Serial setup for SIM808 module**************


//****************************SIM808 Power on******************************

void PowerONSIM808()
{
    Log.notice(S_F("Powering on SIM808..." NL));
    //sim808.powerOnOff(true);
    sim808.init();
    Log.notice(S_F("Power On SIM808 done" NL));

}

//****************************GPS Power on*********************************

void powerONGPS()
{ 
    bool poweredOn = sim808.powerOnOffGps(true);
    Log.notice(S_F("powered on %S : %S" NL), TO_F(strings::GPS), poweredOn ? TO_F(strings::YES) : TO_F(strings::NO));

}
//****************************GPS Power ON end*********************************

void powerONGPRS()
{
    char apn[15]="web";
    char *userP = NULL, *passP = NULL;
    bool success =false;
    do{
    success= sim808.enableGprs(apn,userP,passP);
    Log.notice(S_F("enable GPRS : %S" NL), success ? TO_F(strings::SUCCESS) : TO_F(strings::FAILED));
    LEDflashFast(2000);
    }while(!success); 
}

void RFIDsetup()
{
    SPI.begin();      // Initiate  SPI bus
    mfrc522.PCD_Init();   // Initiate MFRC522
  
    Serial.println(F("End RFID setup"));
}

void LEDflashFast(unsigned long timeDelay)
{ 
  unsigned long previous=millis();  
  do{
      digitalWrite(statLED, HIGH);
      delay(200);
      digitalWrite(statLED, LOW);
      delay(200);
    }while((millis()-previous)< timeDelay);
}
