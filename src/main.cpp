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
int lastTime;

bool estate = 0;
int place = 0;
const int RH_RF24_MAX_MESSAGE_LEN = 25;
bool flagRx;
char rxMessage[10];
char level[4];


int modeA = 1;
bool niv1;
bool niv2;

bool gar;
bool pum;
bool chi;
bool overheatpump;
bool lastState;
bool currentState;
bool lastState_1;
bool currentState_1;
bool lastState_2;
bool currentState_2;
bool lastState_3;
bool currentState_3;

bool screen1 = 1;
bool screen2;
bool screen3;
bool screen4;

bool noDisplay;


// variables oled
char percent = '%';
char mode[13];
char jar[12];
char pom[12];
char pou[12];
char lev1[14];
char lev2[14];
char niv[24];
char info_1[24];
char info_2[24];
char info_3[24];
char info_4[24];


uint8_t buf[RH_RF24_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);

RH_NRF24 nrf24(10,9);

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI 


void oledUpdate() {
  if (noDisplay){
    u8g.firstPage();  
      do {
      } while( u8g.nextPage() );
    }
  

  else if (flagRx){
    if (screen1){
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

      if (pum){
        sprintf(pom,"Pump:ON");
      }
      else{
        sprintf(pom,"Pump:OFF");
      }
    
      u8g.firstPage();  
      do {
        u8g.setFont(u8g_font_unifont);
        u8g.setContrast(analogRead(A0)/4);
        u8g.drawStr(1,16,mode);
        u8g.drawStr(1,32,pom);
        u8g.drawStr(0,48,jar);
        u8g.drawStr(0,64,pou);
      } while( u8g.nextPage() );
    }

    else if (screen2){
      if (niv1){
        sprintf(lev1,"Niveau 1 : V");
      }
      else{
        sprintf(lev1,"Niveau 1 : X");
      }

      if (niv2){
        sprintf(lev2,"Niveau 2 : V");
      }
      else{
        sprintf(lev2,"Niveau 2 : X");
      }
      //sprintf(niv,"Level:%d",((94-atoi(level))/94)*100); 
      
      sprintf(niv,"Level:%i/95 cm",(97 - atoi(level))) ;

      u8g.firstPage();  
      do {
        u8g.setFont(u8g_font_unifont_0_8);
        u8g.setContrast(analogRead(A0)/4);
        u8g.drawStr(0,16,"Level management");
        u8g.drawStr(0,32,niv);
        u8g.drawStr(0,48,lev2);
        u8g.drawStr(0,64,lev1);
      } while( u8g.nextPage() );
    }
    
    else if(screen3){
      if (modeA == 1){
        sprintf(info_1,"mode AUTO");
        sprintf(info_2,"garden at nightfall");
        sprintf(info_3,"auto refill");
        sprintf(info_4,"chiken/2h");
      }
      else if(modeA == 0){
        sprintf(info_1,"mode MANU");
        sprintf(info_2,"everything is possible");
        sprintf(info_3,"c'est koh-lanta");
        sprintf(info_4,"chiken/2h");
      }
      else if(modeA == 2){
        sprintf(info_1,"mode EXTRA");
        sprintf(info_2,"manual garden");
        sprintf(info_3,"auto refill");
        sprintf(info_4,"chiken/2h");
      }
      

      u8g.firstPage();  
      do {
        u8g.setFont(u8g_font_6x10);
        u8g.setContrast(analogRead(A0)/4);
        u8g.drawStr(0,12,"info");
        u8g.drawStr(0,25,info_1);
        u8g.drawStr(0,38,info_2);
        u8g.drawStr(0,51,info_3);
        u8g.drawStr(0,64,info_4);
      } while( u8g.nextPage() );
    }

    else if (screen4){
      u8g.firstPage();  
      do {
        u8g.setFont(u8g_font_6x10);
        u8g.setContrast(analogRead(A0)/4);
        u8g.drawStr(0,12,"WARNING:");
        u8g.drawStr(0,25,"possible pump failure");
        u8g.drawStr(0,38,"check wire/pump/valve");
        u8g.drawStr(0,51,"sensor/battery ");
        u8g.drawStr(0,64,"use MANU to reboot");
      } while( u8g.nextPage() );
    }
    
  }
  else{
    u8g.firstPage();  
    do {
    u8g.setFont(u8g_font_unifont);
    u8g.setContrast(analogRead(A0)/4);
    u8g.drawStr(20,12,"UNREACHED");
    u8g.setFont(u8g_font_5x8r);
    u8g.drawStr(0,20,"1:check device location");
    u8g.drawStr(0,30,"2:check antenna access.");
    u8g.drawStr(0,40,"3:wait 2 min max");
    u8g.drawStr(0,50,"4:try again/check device");
    u8g.drawStr(0,60,"5:call Arthur");
    

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

    level[0] = rxMessage[8];
    level[1] = rxMessage[9];
    level[2] = rxMessage[10];

    Serial.println(level);
      
    float proto = atof(rxMessage);
    Serial.println(proto);

    if (proto >= 1000000){
      proto -= 1000000;
      niv2 = 1;
    }
    else{
      niv2 = 0;
    }

    if (proto >= 100000){
      proto -= 100000;
      niv1 = 1;
    }
    else{
      niv1 = 0;
    }

    if (proto >= 30000){
      Serial.println("MODE AUTO RX");
      proto -= 30000;
      modeA = 0;
    }
    else if (proto >= 20000){
      Serial.println("MODE EXTRA RX");
      proto -= 20000;
      modeA = 2;
    }
    else if (proto >= 10000){
      Serial.println("MODE MANU RX");
      proto -= 10000;
      modeA = 1;
    }

    Serial.println(proto);
    
    if (proto >= 1000){
      gar =1;
      proto -= 1000;
      Serial.println("garden");
    }
    else{
      gar =0;
      Serial.println("GARDEN");
    }   
    Serial.println(proto);
    if (proto >= 100){
      pum = 1;
      proto -= 100;
      Serial.println("pump");
    }  
    else{
      pum = 0;
      Serial.println("PUMP");
    }
    Serial.println(proto);


    if (proto >= 10){
      chi = 1;
      proto -= 10;
    }
    else{
      chi = 0;
    }
    if (proto >= 1){
      overheatpump = 1;
      proto -= 1;
    }
    else{
      overheatpump =0;
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

nrf24.setChannel(0x90);

  Serial.println("2");
}

void loop()
{
  oledUpdate();
  rxData();

  currentState_1 = digitalRead(IN_GAR);
  currentTime = millis();

  if (currentTime - lastTime >= 60000){ // shutdown OLED after 60 sec
    lastTime = currentTime;
    noDisplay = 1;
    flagRx = 0;

  }

  if ((currentState_1 != lastState_1 && currentState_1 ==1) )
  {
    noDisplay = 0;
    lastTime = currentTime;

    if (overheatpump){
      screen4 = 1;
      screen1 = 0;
      screen2 = 0;
      screen3 = 0;
    }
    else{
      if (screen1){
        screen1 = 0;
        screen2 = 1;
        screen3 = 0;
      }
      else if (screen2){
        screen1 = 0;
        screen2 = 0;
        screen3 = 1;
      }
      else if (screen3){
        screen1 = 1;
        screen2 = 0;
        screen3 = 0;
      }
    }
  }

  lastState_1 = currentState_1;

}