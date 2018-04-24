#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>
#include <string.h>

const char* ssid = "RaspberryBroker";
const char* password = "12345678";
const char* mqtt_server = "192.168.0.103";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
boolean newData = false;
char *stringPos;

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (Serial.available()>0) {
      char *data;
      char *topic = (char *)malloc(100);
      
      recvWithEndMarker();
      if (strstr(receivedChars, "sensornode")){
        data = strstr(receivedChars, " ") + 1;
        strcpy(data-2, "\0");
        strcpy(topic, receivedChars);        
        client.publish(topic, data);
      }     
      newData = false;
  }
}

void recvWithEndMarker() {
 static byte ndx = 0;
 char endMarker = '\n';
 char rc;
 
 while (Serial.available() > 0 && newData == false) {
   rc = Serial.read();
  
   if (rc != endMarker) {
     receivedChars[ndx] = rc;
     ndx++;
     if (ndx >= numChars) {
       ndx = numChars - 1;
     }
   } else {
       receivedChars[ndx] = '\0'; // terminate the string
       ndx = 0;
       newData = true;
     }
  }
}
