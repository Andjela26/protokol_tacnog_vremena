#include <WiFi.h>
#include "TM1637.h" //klasa za rad sa drajverom LED displeja
const int CLK_PIN = 24;       //clk pin displeja
const int DIO_PIN = 25;       //dio pin displeja
const int START_PIN = PUSH1;  //SW2

char ssid[] = "energia";      //ime WiFi mreze (SSID)
char password[] = "launchpad"; //lozinka
unsigned int localPort = 123;
unsigned char HH=0;
unsigned char MM=0;
unsigned char SS=0;
const int UDP_PCK_SIZE = 48;    //velicina UDP bafere
byte packetBuffer[UDP_PCK_SIZE]; //bafer za dolazne UDP pakete
TM1637 tm1637(CLK_PIN, DIO_PIN);  //instanciranje objekta klase TM1637
WiFiUDP udp;
IPAddress ip(192,168,1,3);        //IP adresa odredisnog hosta
int T;
void displayBinaryNumber(unsigned long num){
  unsigned d;
  for(int i = 3; i >= 0; i--){
    d = num % 10;
    tm1637.display(i, d);
    num /= 10;
  }
}

void setup() {
  tm1637.init();               //inicijalizacija drajvera LED displeja
  tm1637.set(BRIGHT_TYPICAL);  //podrazumevani nivo osvetljaja
  tm1637.point(true);
  pinMode(START_PIN, INPUT); 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
    delay(300);
  while (WiFi.localIP() == INADDR_NONE) 
    delay(300);
  Serial.println("Connected");
  //otvaranje UDP porta za slusanje:
  udp.begin(localPort);  
}

void loop() {
    if (digitalRead(START_PIN== HIGH)){
      udp.beginPacket(ip,123);
      udp.write(packetBuffer,48);
      udp.write(0x1B);
      udp.endPacket();
    }
    int packetSize = udp.parsePacket();
    if (packetSize > 0){
      int i=0;
      while(i<39)
      {
        int x = udp.read();
        i++;
      }

      int NS = NULL;
      NS |= udp.read();
      int j=0;
      while(j<3){
         NS <<= 8;
         NS |= udp.read();
         j++;
      }
      HH = NS / 3600;
      HH = (HH + 1) % 24;
      MM =(NS %3600)/60;
      SS = NS % 60 ;   
      
    }
    delay(1000);
    if (SS==59)
    {
      SS=0;
      MM=MM+1;
    }else{
      SS=SS+1;
    }
    if(MM=59){
      MM=0;
      HH=HH+1;
    }
    if(HH=24){
      HH=0;
    }
    T=HH*100+MM;
    displayBinaryNumber(T);
    
}
