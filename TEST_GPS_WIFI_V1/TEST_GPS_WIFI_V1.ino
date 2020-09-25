    #include <SoftwareSerial.h>
#include <Adafruit_GPS.h>
#include <math.h>
#include <stdlib.h>
#include <floatToString.h>

 
#define DEBUG true

Adafruit_GPS GPS(&Serial2);
HardwareSerial mySerial = Serial2;
#define GPSECHO  true

void setup()
{
  Serial.begin(115200);
  Serial1.begin(9600); // your esp's baud rate might be different
  GPS.begin(9600);
  
  delay(5000);
  Serial.println("TEST GPS & WIFI"); Serial.println("");
    
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);

  // LIGNE DE COMMANDE WIFI 
  sendCommand("AT+RST\r\n",500,DEBUG); // reset module
  sendCommand("AT+CWMODE=3\r\n",500,DEBUG); // configure as access point
  sendCommand("AT+CWSAP=\"TwimEsp\",\"12345678\",5,2\r\n",500,DEBUG);
  delay(10000);
  sendCommand("AT+CIFSR\r\n",500,DEBUG); // get ip address
  sendCommand("AT+CIPMUX=1\r\n",500,DEBUG); // configure for multiple connections
  sendCommand("AT+CIPSERVER=1,80\r\n",500,DEBUG); // turn on server on port 80

  // LIGNE DE COMMANDE GPS 
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  
  Serial.println("WIFI READY");
  Serial.println("GPS READY");
  delay(1000);
}

uint32_t timer = millis();
 
void loop()
{

  // PARTIE GPS

  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO))
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) { 
    timer = millis(); // reset the timer
    
    if (GPS.fix) {
      Serial.print("Location: ");
      //Serial.print(GPS.latitude, 4); // Valeur en DMS
      Serial.print(convertDegMinToDecDeg(GPS.latitude)); // Valeur en DD 
      Serial.print(GPS.lat); 
      Serial.print(", "); 
      //Serial.print(GPS.longitude, 4); // Valeur en DMS
      Serial.print(convertDegMinToDecDeg(GPS.longitude)); // Valeur en DD
      Serial.println(GPS.lon);
    }
  }
  
  // PARTIE WIFI
  
  if(Serial1.available()) // check if the esp is sending a message 
  {
 
    
    if(Serial1.find("+IPD,"))
    {
     delay(200); // wait for the serial buffer to fill up (read all the serial data)
     // get the connection id so that we can then disconnect
     int connectionId = Serial1.read()-48; // subtract 48 because the read() function returns 
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48
          
     Serial1.find("pin="); // advance cursor to "pin="
          
     int pinNumber = (Serial1.read()-48); // get first number i.e. if the pin 13 then the 1st number is 1
     int secondNumber = (Serial1.read()-48);
     if(secondNumber>=0 && secondNumber<=9)
     {
      pinNumber*=10;
      pinNumber +=secondNumber; // get second number, i.e. if the pin number is 13 then the 2nd number is 3, then add to the first number
     }
     
     digitalWrite(pinNumber, !digitalRead(pinNumber)); // toggle pin    

     char buffer[35];
     float a = convertDegMinToDecDeg(GPS.latitude);
     String b = floatToString(buffer, a , 8);

     a = convertDegMinToDecDeg(GPS.longitude);
     String c = floatToString(buffer, a , 11);
     
     sendHTTPResponse(connectionId,b,c);
     
     // make close command
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     
     sendCommand(closeCommand,500,DEBUG); // close connection
    }
  }
}
 
/*
* Name: sendData
* Description: Function used to send data to Serial2.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the Serial2 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    int dataSize = command.length();
    char data[dataSize];
    command.toCharArray(data,dataSize);
           
    Serial1.write(data,dataSize); // send the read character to the Serial2
    if(debug)
    {
      Serial.println("\r\n====== HTTP Response From Arduino ======");
      Serial.write(data,dataSize);
      Serial.println("\r\n========================================");
    }
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(Serial1.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = Serial1.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}
 
/*
* Name: sendHTTPResponse
* Description: Function that sends HTTP 200, HTML UTF-8 response
*/
void sendHTTPResponse(int connectionId, String content, String content1)
{
     
     // build HTTP response
     String httpResponse;
     String httpHeader;
     
     // HTTP Header
     httpHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n"; 
     httpHeader += "Content-Length: ";
     httpHeader += content.length() + content1.length();
     httpHeader += "\r\n";
     httpHeader +="Connection: close\r\n\r\n";
     
     httpResponse = httpHeader + content + " / " + content1 + " ";// There is a bug in this code: the last character of "content" is not sent, I cheated by adding this extra space
     sendCIPData(connectionId,httpResponse);
     
}
 
/*
* Name: sendCIPDATA
* Description: sends a CIPSEND=<connectionId>,<data> command
*
*/
void sendCIPData(int connectionId, String data)
{
   String cipSend = "AT+CIPSEND=";
   cipSend += connectionId;
   cipSend += ",";
   cipSend +=data.length();
   cipSend +="\r\n";
   sendCommand(cipSend,500,DEBUG);
   sendData(data,500,DEBUG);
}
 
/*
* Name: sendCommand
* Description: Function used to send data to Serial2.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the Serial2 (if there is a reponse)
*/
String sendCommand(String command, const int timeout, boolean debug)
{
    String response = "";
           
    Serial1.print(command); // send the read character to the Serial2
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(Serial1.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = Serial1.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}


// Convert DMS -> DD
double convertDegMinToDecDeg (float degMin) {
double min = 0.0;
double decDeg = 0.0;
 
//get the minutes, fmod() requires double
min = fmod((double)degMin, 100.0);
 
//rebuild coordinates in decimal degrees
degMin = (int) ( degMin / 100 );
decDeg = degMin + ( min / 60 );
 
return decDeg;
}
