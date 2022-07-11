#include <Arduino.h>
#include <SPI.h>  
#include <RF24.h>
#include <U8glib.h>
#include <RH_NRF24.h>
#include <nRF24L01.h>
#include <RHReliableDatagram.h>


const int IN_GAR = 4;
const int IN_PUM = 2;
const int IN_CHI = 6;

int currentTime;

bool estate = 0;
int place = 0;
const int RH_RF24_MAX_MESSAGE_LEN = 25;
bool flagRx = 0;
char rxMessage[10];


int modeA = 1;
bool niv1;
bool niv2;

bool gar;
bool pum;
bool chi;
bool lastState;
bool currentState;
bool lastState_1;
bool currentState_1;
bool lastState_2;
bool currentState_2;
bool lastState_3;
bool currentState_3;


// variables oled
char mode[13];
char jar[12];
char pom[12];
char pou[12];

uint8_t buf[RH_RF24_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);

RH_NRF24 nrf24(10,9);

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI 


void oledUpdate() {

  if (flagRx){
    char var[5];
    
    sprintf(var,"%d",analogRead(A0)/128);

    if (modeA == 1){
      sprintf(mode,"Mode:AUTO");
    }
    else if(modeA == 0){
      sprintf(mode,"Mode:MANU");
    }
    else if(modeA == 2){
      sprintf(mode,"Mode:EXTRA");
    }

    if (gar){
      sprintf(jar,"Garden:ON");
    }
    else{
      sprintf(jar,"Garden:OFF");
    }

    if (chi){
      sprintf(pou,"Chicken:ON");
    }
    else{
      sprintf(pou,"chicken:OFF");
    }

    u8g.firstPage();  
    do {
      u8g.setFont(u8g_font_unifont);
      u8g.drawStr(1,16,mode);
      u8g.drawStr(1,32,pom);
      //u8g.drawStr(0,48,jar);
      //u8g.drawStr(0,64,pou);
    } while( u8g.nextPage() );
  }
  else{
    u8g.firstPage();  
    do {
    u8g.setFont(u8g_font_unifont);
    u8g.setContrast(analogRead(A0)/4);
    u8g.drawStr(20,12,"UNREACHED");
    } while( u8g.nextPage() );

  }
}



void rxData(){

  if (nrf24.available())
  {
    // Should be a message for us now   
    

    if (nrf24.recv(buf, &len))
    {
      Serial.print("message: ");
      Serial.println((char*)buf);
      sprintf(rxMessage,"%s",(char*)buf);
    }
    Serial.println(rxMessage);
      
    int proto = atoi(rxMessage);
    Serial.println(proto);

    if (proto >= 20000){
      Serial.println("MODE EXTRA RX");
      proto -= 20000;
      modeA = 2;
    }
    else if (proto >= 10000){
      Serial.println("MODE MANU RX");
      proto -= 10000;
      modeA = 1;
    }
    else if(proto>= 30000){
      Serial.println("MODE AUTO RX");
      modeA = 0;
    }

    if (proto >= 1000){
      gar =0;
      proto -= 1000;
    }
    else{
      gar =1;
    }   

    if (proto >= 100){
      pum = 0;
      proto -= 100;
    }  
    else{
      pum = 1;
    }

    if (proto == 10){
      chi = 0;
    }
    else{
      chi = 1;
    }
    
    flagRx = 1;
  }
}



void setup() 
{
  
  Serial.begin(115200);
  Serial.println("1");

if (!nrf24.init())
  Serial.println("init failed");
// Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
if (!nrf24.setChannel(1))
  Serial.println("setChannel failed");
if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
  Serial.println("setRF failed");   
Serial.println("setup Done") ;

  Serial.println("2");
}

void loop()
{
  oledUpdate();
  rxData();

  currentState_1 = digitalRead(IN_GAR);

  lastState_1 = currentState_1;

}