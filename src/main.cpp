#include <Arduino.h>
#include <SPI.h>  
#include <RF24.h>
#include <U8glib.h>
#include <RH_NRF24.h>
#include <nRF24L01.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

const int IN_GAR = 4;
const int IN_PUM = 2;
const int IN_CHI = 6;

int currentTime;

bool estate = 0;
int place = 0;
const int RH_RF24_MAX_MESSAGE_LEN = 25;
bool flagRx;

int modeA = 0;
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

RH_NRF24 nrf24(10,9);
RHReliableDatagram manager(nrf24, CLIENT_ADDRESS);

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI 


void oledUpdate() {

  if (flagRx){
    char var[5];
    
    sprintf(var,"%d",analogRead(A0)/128);
    
    u8g.firstPage();  
    do {
      u8g.setFont(u8g_font_unifont);
      u8g.drawStr(0,12,mode);
      u8g.drawStr(0,24,pom);
      u8g.drawStr(0,36,jar);
      u8g.drawStr(0,48, pou);
      if (modeA == 0){
        u8g.drawStr(100,12,var);
      }
    } while( u8g.nextPage() );
  }
  else{
    u8g.firstPage();  
    do {
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr(20,12,"UNREACHED");
    } while( u8g.nextPage() );

  }
}


void txData(){

  uint8_t data[25] ;
  char rxMessage[12];

  sprintf(data,"%i%d%d%d",modeA,gar,pum,chi);

  nrf24.send(data, sizeof(data));
  nrf24.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  int x = 0;
  flagRx = 0;

  if (nrf24.waitAvailableTimeout(500))
  { 
    // Should be a reply message for us now   
    if (nrf24.recv(buf, &len))
    {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
      sprintf(rxMessage,"%s",buf);
      flagRx = 1;
    }
    else
    {
      Serial.println("recv failed");
      flagRx = 0;
    }
  }
  else
  {
    Serial.println("No reply, is nrf24_encrypted_server running?");
  }
  delay(400);

}

void rxData(){

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


  currentState = digitalRead(8);
  currentState_1 = digitalRead(IN_GAR);
  currentState_2 = digitalRead(IN_PUM);
  currentState_3 = digitalRead(IN_CHI);
  currentTime = millis();

  if (currentState != lastState && currentState ==1)
  {
    if (modeA == 0){
      modeA = 1;
      sprintf(mode,"mode : MANU");
    }
    else if (modeA == 1){
      modeA = 2;
      sprintf(mode,"mode : EXTRA");
    }
    else if (modeA == 2){
      modeA = 0;
      sprintf(mode,"mode : AUTO");
    }
    txData();
  }

  if (currentState_3 != lastState_3 && currentState_3 ==1)
  {
    if (chi){
      chi = 0;
     
      sprintf(pou,"chicken:OFF");
    }
    else {
      sprintf(pou,"chicken: ON");
      chi = 1;
 
      
    }
    txData();
  }

  if (currentState_1 != lastState_1 && currentState_1 ==1)
  {
    if (gar){
      sprintf(jar,"garden:OFF");
      gar = 0;

    }
    else {
      sprintf(jar,"garden: ON");
      gar = 1;

    }
    txData();
  }

  if (currentState_2 != lastState_2 && currentState_2 ==1)
  {
    if (pum){
      sprintf(pom,"pump : OFF");
      pum = 0;
    }
    else {
      sprintf(pom,"pump :  ON");
      pum = 1;
    }
    txData();
  }


  lastState = currentState; 
  lastState_1 = currentState_1;
  lastState_2 = currentState_2;
  lastState_3 = currentState_3;

}