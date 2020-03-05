#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include "DebugMacros.h"

#include <SoftwareSerial.h>
SoftwareSerial s(14,12); //D5,D6

const int inputPin = 13;
int value = 0;
int data;
String izena;

const char *ssid = "13";
const char *password = "12345678";

const char* host = "script.google.com";
const char *GScriptId = "AKfycbx_ptPJGEpJ0sQE7hXoOX8cqTQDQiC5liYTaNuO";
const int httpsPort = 443;

const char* fingerprint = "";
String url;

String payload_base =  "{\"command\": \"appendRow\", \
                    \"sheet_name\": \"Sheet1\", \
                    \"values\": ";
String payload = "";

HTTPSRedirect* client = nullptr;

unsigned int free_heap_before = 0;
unsigned int free_stack_before = 0;

void setup() {
  Serial.begin(9600);
  s.begin(9600);

  free_heap_before = ESP.getFreeHeap();
  free_stack_before = ESP.getFreeContStack();
  
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  value = digitalRead(inputPin);  
    if (value == LOW) {
    data=s.read();
    Serial.println(data);
    if (data==1)
    {
      izena = "Lander";
      Serial.println(izena);
    }
    else if (data==2){
     izena = "Jon";
      Serial.println(izena);
    }
    else if (data==3){
     izena = "Abierto";
      Serial.println(izena);
    }
    else if (data==0){
     izena = "Abierto";
      Serial.println(izena);
    }
    else {
      izena = "Abierto";
    Serial.println(izena);
    }

  static int error_count = 0;
  static int connect_count = 0;

  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  
  Serial.print("Connecting to ");
  Serial.println(host);

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
       flag = true;
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
  //String val = valor; 
  url = String("/macros/s/") + GScriptId + "/exec?value=" + izena;
  client->GET(url, host);
 
  delete client;
  client = nullptr;
  
  }
   else {return;}
 
}
