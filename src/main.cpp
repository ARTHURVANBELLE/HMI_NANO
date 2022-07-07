#include <Arduino.h>
#include <SPI.h>  
#include <RF24.h>
#include <U8glib.h>
#include <RH_NRF24.h>
#include <nRF24L01.h>

const int IN_GAR = 4;
const int IN_PUM = 2;
const int IN_CHI = 6;

int currentTime;

bool estate = 0;
int place = 0;
const int RH_RF24_MAX_MESSAGE_LEN = 25;

bool modeA;
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
char mode[12];
char jar[12];
char pom[12];
char pou[12];

/*
RF24 myRadio (9, 10);
byte addresses[][6] = {"0"};
*/

RH_NRF24 nrf24(10,9);

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI 


struct package {
  int mode;
  bool chi;
  bool pum;
  bool gar;
  bool niv1;
  bool niv2;
  int lev;
};

typedef struct package Package;
Package dataRecieve;
Package dataTransmit;

char sent[15] = " HELLO THERE ";

void oledUpdate() {

  char var[5];

  sprintf(var,"%d",analogRead(A0));
  
  u8g.firstPage();  
  do {
    u8g.setFont(u8g_font_unifont);
    u8g.drawStr(0,12,mode);
    u8g.drawStr(0,24,pom);
    u8g.drawStr(0,36,jar);
    u8g.drawStr(0,48, pou);
    u8g.drawStr(90,12,var);
  } while( u8g.nextPage() );
}


void txData(){
  //delay(200);
/*
  myRadio.stopListening();
  myRadio.openWritingPipe(addresses[0]);
  myRadio.write(&sent, sizeof(sent));
  myRadio.openReadingPipe(1, addresses[0]);
  //myRadio.startListening();
*/

  uint8_t data[] = " hello there";
  nrf24.send(data, sizeof(data));
  nrf24.waitPacketSent();
}

void rxData(){

}

void setup() 
{
  
  Serial.begin(115200);
  Serial.println("1");
/*
  delay(1000);
  
  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS );
  
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();
*/
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
    if (modeA){
      modeA = 0;
      dataTransmit.mode = 0;
      sprintf(mode,"mode : MANU");
    }
    else {
      sprintf(mode,"mode : AUTO");
      modeA = 1;
      dataTransmit.mode = 1;
    }
    txData();
  }

  if (currentState_3 != lastState_3 && currentState_3 ==1)
  {
    if (chi){
      chi = 0;
      dataTransmit.chi = chi;
      sprintf(pou,"chicken:OFF");
    }
    else {
      sprintf(pou,"chicken: ON");
      chi = 1;
      dataTransmit.chi = chi;
      
    }
    txData();
  }

  if (currentState_1 != lastState_1 && currentState_1 ==1)
  {
    if (gar){
      sprintf(jar,"garden:OFF");
      gar = 0;
      dataTransmit.gar = gar;
    }
    else {
      sprintf(jar,"garden: ON");
      gar = 1;
      dataTransmit.gar = gar;
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