char svr[20]="45.115.218.176";
//char svr[20]="3.6.94.176";
char port[7]="8001";

char aux_str[50];

//********************************* Start GPRS only and exit **************************************************

bool sendGPRSToSocket(char *data) 
{
    bool retVal = false;
    int cnt=0;
    // Gets Local IP Address
      Log.notice(S_F("Openning TCP..." NL));
      
      //prepare TCP command
      char TCPcmd[50];
      sprintf(TCPcmd,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"",svr,port);
      // Opens a TCP socket
      if (sendATcmd(TCPcmd, "CONNECT OK", "CONNECT FAIL", 30000) == 1)
      {
        Log.notice(S_F("Connected" NL));
      
        // Sends some data to the TCP socket
        sprintf(aux_str, "AT+CIPSEND=%d", strlen(data));
        if (sendATcmd(aux_str, ">", "ERROR", 10000) == 1)
        {
          socketSend(data);
          //Log.notice(S_F("Data successfully sent to server" NL))
          retVal=true;
        }
      
        // Closes the socket
        while(sendATcmd("AT+CIPCLOSE", "CLOSE OK", "ERROR", 10000)!=1){
          Log.notice(S_F("Connection not closed" NL));
          if(cnt>3)
            break;
          cnt++;
        }
      }
      else
      {
        Log.notice(S_F("Error openning the connection" NL));
        conErrorCnt++;
      }        
    return retVal;
}

//********************************* END GPRS ****************************************************

//********************************* AT Command function******************************************

int8_t sendATcmd(char* ATcommand, char* expected_answer1, char* expected_answer2, unsigned int timeout) 
{
  char resp[100];
  uint8_t x = 0, answer = 0;
  unsigned long previous;
  response[0]='\0';
  while (simSerial .available() > 0) simSerial .read();    // Clean the input buffer

  simSerial .println(ATcommand);    // Send the AT command

  x = 0;
  previous = millis();

  // this loop waits for the answer
  do {
    // if there are data in the UART input buffer, reads it and checks for the asnwer
    if (simSerial .available() != 0) {
      resp[x] = simSerial .read();
      x++;
      // check if the desired answer 1  is in the response of the module
      if (strstr(resp, expected_answer1) != NULL)
      {
        answer = 1;
      }
      // check if the desired answer 2 is in the response of the module
      else if (strstr(resp, expected_answer2) != NULL)
      {
        answer = 2;
      }
    }
  }
  // Waits for the asnwer with time out
  while ((answer == 0) && ((millis() - previous) < timeout));

  return answer;
}

//********************************* END AT Command function******************************************


//********************************* Socket Send ******************************************


void socketSend(char* datas) {
  String respo="";  
  while (simSerial .available() > 0) simSerial .read();    // Clean the input buffer

  simSerial .println(datas);    // Send the AT command
  delay(1000);

 // this loop waits for the answer
  while (simSerial.available() > 0) {
      respo=simSerial.readStringUntil('\n'); // writes in the string all the inputs till a comma
      if(respo.indexOf("P")>=0)
        break;
      else
        respo="";
  }
  
  respo.toCharArray(response,respo.length()+1);
}

//********************************* Socket Send end******************************************
