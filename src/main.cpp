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
const int POT = A0;

int currentTime;
int lastTime = 0;

bool estate = 0;
int place = 0;
const int RH_RF24_MAX_MESSAGE_LEN = 25;
bool flagRx;
char messageRx[30];
char messageTx[30];


int garden = 6;
int chicken = 7;
int pump = 5;
int mode = 8;
bool oldstate_g, oldstate_c, oldstate_p, oldstate_m;
bool gardenstate, chickenstate, pumpstate, modestate;


int modeA = 1;
bool niv1;
bool niv2;
int level;

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

bool screen1 = 0;
bool screen2 = 1;
bool screen3;
bool screen4;

bool noDisplay;


// variables oled
char percent = '%';
char strmode[13];
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


RF24 radio(10, 9); // CE, CSN
const byte addresses [][6] = {"00001", "00002"};    //Setting the two addresses. One for transmitting and one for receiving

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI 


void oledUpdate() {
  if (noDisplay){
    u8g.firstPage();  
      do {
      } while( u8g.nextPage() );
    }
  

  else if (flagRx){
    if (screen1){
 
      if (modeA == 1){
        sprintf(strmode,"Mode:AUTO");
      }
      else if(modeA == 2){
        sprintf(strmode,"Mode:EXTRA");
      }
      else if(modeA ==3){
        sprintf(strmode,"Mode:MANU");
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
        u8g.setContrast(16);
        u8g.drawStr(1,16,strmode);
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
      
      sprintf(niv,"Level:%i/95 cm",(97 - level)) ;

      u8g.firstPage();  
      do {
        u8g.setFont(u8g_font_unifont_0_8);
        u8g.setContrast(16);
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
        u8g.setContrast(16);
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
        u8g.setContrast(16);
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
    u8g.setContrast(16);
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

  if (radio.available())
  {
    // Should be a message for us now   
    flagRx = 1;

    radio.read(messageRx, sizeof(messageRx));
    //sprintf(messageRx,"A%d%d%i%d%d%d%d%03dB",digitalRead(IN_NIV2),digitalRead(IN_NIV1),modeA,gar,pum,chi,overheatPump,currentLev);

    niv2 = messageRx[1] - '0';
    niv1 = messageRx[2] - '0';
    modeA = messageRx[3] - '0';
    gar = messageRx[4] - '0';
    pum = messageRx[5] - '0';
    chi = messageRx[6] - '0';
    overheatpump = messageRx[7] - '0';
    level = (messageRx[8] - '0')*100 + (messageRx[9] - '0')*10 + (messageRx[10] - '0');
  }
}

void txData(){
   if (digitalRead(pump) && !oldstate_p){
  Serial.println("pump");
  oldstate_p = 1;
 }
 else {
  oldstate_p = 0;
 }
 if (digitalRead(chicken) && !oldstate_c){
  Serial.println("chicken");
  oldstate_c = 1;
 }
 else {
  oldstate_c = 0;
 }
 if (digitalRead(garden) && !oldstate_g){
  Serial.println("garden");
  oldstate_g = 1;
 }
 else {
  oldstate_g = 0;
 }
 if (digitalRead(mode) && !oldstate_m){
  Serial.println("mode");
  oldstate_m = 1;
 }
 else {
  oldstate_m = 0;
 }

 if (oldstate_c || oldstate_g || oldstate_m || oldstate_p){
  sprintf(messageTx,"%d-%d-%d-%d",oldstate_c, oldstate_g, oldstate_m, oldstate_p);
  radio.stopListening();                                //This sets the module as transmitter
  radio.write(&messageTx, sizeof(messageTx));               //Sending the data
  Serial.println(messageTx);
  delay(20);
  radio.startListening();
 }
}



void setup() 
{
  Serial.begin(9600);
  //pinMode(POT,INPUT);

  radio.begin();                            //Starting the radio communication
  radio.openWritingPipe(addresses[0]);      //Setting the address at which we will send the data
  radio.openReadingPipe(1, addresses[1]);   //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_HIGH);            //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
  radio.startListening();                   //This sets the module as receiver

  radio.setChannel(90);
}

void loop()
{
  oledUpdate();
  rxData();
  txData();

  
  currentTime = millis();
/*
  if (currentTime - lastTime >= 60000){ // shutdown OLED after 60 sec
    lastTime = currentTime;
    noDisplay = 1;
    flagRx = 0;

  }
*/

  if (overheatpump){
    noDisplay = 1;
    screen4 = 1;
    screen1 = 0;
    screen2 = 0;
    screen3 = 0;
  }
  else{
    
    if ((analogRead(POT)/80) <= 1){
      screen1 = 0;
      screen2 = 1;
      screen3 = 0;
    }
    else if ((analogRead(POT)/80) <= 2){
      screen1 = 0;
      screen2 = 0;
      screen3 = 1;
    }
    else if ((analogRead(POT)/80) <= 3){
      screen1 = 1;
      screen2 = 0;
      screen3 = 0;
    }
    Serial.println(analogRead(POT));
  }
  


}