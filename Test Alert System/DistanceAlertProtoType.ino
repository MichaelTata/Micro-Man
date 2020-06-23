
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>



WiFiUDP Udp;
unsigned int localUdpPort = 4210;     // local port to listen on
char packetMsg[255];             // buffer for incoming packets
char  replyPacket[] = "Received. "; 

char remoteIp[] = "192.168.0.32";     // IP of server. Temporarily testing on local subnet so default to 192.168.0.-- 

////////////////
char accessPoint[] = "*******";
char accessPointPass[] = "********";
////////////////

static const uint8_t echoD7 = 13;
static const uint8_t trigD8 = 15;

long duration, cm, inches;
long prev;

void setup() {

  //Serial Port begin
  Serial.begin (9600);
  //Define inputs and outputs
  pinMode(trigD8, OUTPUT);
  pinMode(echoD7, INPUT);

  prev = 0;

  //Flash LED for visual confirmation of activity
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  Serial.println();
  
  WiFi.begin(accessPoint, accessPointPass);
  
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  
  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

}

void loop() {

  //Prime Sensor with low pulse then signal with high
  digitalWrite(trigD8, LOW);
  delayMicroseconds(5);
  digitalWrite(trigD8, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigD8, LOW);
  
  
  pinMode(echoD7, INPUT);
  duration = pulseIn(echoD7, HIGH);

  
  cm = (duration/2) / 29.1;
  inches = (duration/2) / 74;

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();


  //If distance has increased by more than threshold, send an alert to the server. 
  if(inches - prev > 2)
  {
        
    
        Udp.beginPacket(remoteIp,localUdpPort);

        String msgString;
        msgString = String("Alert from Device. Device IP:" + WiFi.localIP().toString());


        Serial.print(msgString);
        msgString.toCharArray(packetMsg, 255);
        
        Udp.write(packetMsg);
          
        if(Udp.endPacket() == 1)
        {
          Serial.printf("Packet send successful\n");
        }
        else
        {
          Serial.printf("Packet failed to send\n");
        }

        Udp.endPacket();
  }
  prev = inches;
 
  digitalWrite(LED_BUILTIN, HIGH);   
  delay(1000);                       
  digitalWrite(LED_BUILTIN, LOW);    
  delay(1000);                       

}
