#include <SPI.h>
#include<SoftwareSerial.h>
#include <Ethernet.h>
#include "DHT.h"

SoftwareSerial mySerial(13, 12);

#define fan 6
#define DHTPIN 7 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE); 
          

//------------------VARIABLE DEFINITION AND ASSIGNMENT-------------------------------------------------

float Temperature;
float Humidity;
float maxTemp;
float maxHum;

String message;

//------------------------------------------------------------------------------
char server[] = "192.168.43.137";
IPAddress ip(192, 168, 43, 138);
byte gateway[] = { 192, 168, 43, 1 };
byte subnet[] = { 255, 255, 255, 0 };
String readString;
EthernetClient client;

//-------------------------------------SETUP FUNCTION--------------------------------------------------
void setup() {
  //  mySerial.begin(9600);
  Serial.begin(9600);
  dht.begin();
  pinMode(fan, OUTPUT);
  digitalWrite(fan, HIGH);
  Serial.println("FAN ON . . .");

  delay(500);

  Serial.println("..................................Max/Min set values................................................");
  Getting_From_phpmyadmindatabase();
  delay(1000); //changed from 2000ms
  Serial.print("MaxTemp=");
  Serial.println(maxTemp);
  Serial.print("MaxHum=");
  Serial.println(maxHum);
  
  Serial.println(".................End of MaxTemp/Min set values..............");
  delay(1000); //changed from 2000ms
}

//---------------------------LOOP FUNCTION----------------------------
void loop() {
  Humidity = dht.readHumidity(); 
  Temperature = dht.readTemperature(); // Read temperature as Celsius (the default)
  float f = dht.readTemperature(true);  // Read temperature as Fahrenheit (isFahrenheit = true)

  // Check if any readings failed and exit early (to try again).
  if (isnan(Humidity) || isnan(Temperature)) {
    Serial.println(F("Failed to read from sensor!"));
    message="GERIS Agri-Solutions: Failed to read from Sensor. Please check for possible misconnections or errors";
    delay(30000);
    return;
  }
     Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+256777885833\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  mySerial.print("Gladys :"); //text content
  delay(20);
  //mySerial.print("Project"); //text content
  updateSerial();
  mySerial.write(26);


}
//----------------------------------------------------------------------------
void Getting_From_phpmyadmindatabase()   //CONNECTING WITH MYSQL
{
  Serial.println();
  Serial.println("Send2");
  Serial.println();
  if (client.connect(server, 80)) {
    Serial.println("Getting_Data_connected");

    // Make a HTTP request
    client.println("GET https://localhost/GERIS/conditions.php");

    //    client.print("GET /GERIS/tempIn.php?");     //YOUR URL
    //    client.print("&Temperatur=");
    //    client.print(Temperature);
    client.print(" ");      //SPACE BEFORE HTTP/1.1
    client.print("HTTP/1.1");
    client.println();
    client.println("Host: 192.168.43.138");
    client.println("Connection: close");
    client.println();
    //updateSerial2();
}
  
  else {
    // if you didn't get a connection to the server:
    Serial.println("Getting_Data_connection failed");
    message = "GERIS APP: Failed to fetch data from database. Check internet connection.";
    
  }
  
}
//-----------------------------------------------------------------------------------------------------

void Sending_To_phpmyadmindatabase()//CONNECTING WITH MYSQL
{
  if (client.connect(server, 80)) {
    Serial.println("Send_Data_connected");

    // Make a HTTP request
    client.print("GET /GERIS/data_input.php?");     //YOUR URL
    client.print("&Temperature=");
    client.print(Temperature);
    client.print("&Humidity=");
    client.print(10);
    //    client.print(Humidity);
    
    client.print("&Fan=");
    //fanState();
    Serial.print(F("Humidity: "));
    Serial.print(Humidity);
    Serial.println("%");
    Serial.print(F("Temperature: "));
    Serial.print(Temperature);
    Serial.println(F("°C "));
    client.print(" ");      //SPACE BEFORE HTTP/1.1
    client.print("HTTP/1.1");
    client.println();
    client.println("Host: 192.168.43.138");
    client.println("Connection: close");
    client.println();

    }
  
 else {
    // if you didn't get a connection to the server:
    Serial.println("Send_Data_connection failed");
    message = "GERIS APP: Failed to send data from database. Check internet connection";
    //sendSMS();
  }
}

//------------------------------------FAN STATE FUNCTION----------------------------------------
void fanState() {
  if (Temperature >= maxTemp) {
    digitalWrite(fan, LOW);
    Serial.println("Fan = ON");
    char Fan[] = "ON";
    client.print(Fan);
  }
  else {
    digitalWrite(fan, HIGH);
    Serial.println("Fan = OFF");
    char Fan[] = "OFF";
    client.print(Fan);
  }
}



////GSM FUNCTION FOR SENDING NOTIFICATION/MESSAGES TO CLIENT
void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }

}


