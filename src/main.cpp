/**
 * @Author: Anderson Juncowski <anderson>
 * @Date:   2018-07-17T10:14:04-03:00
 * @Email:  andersonj@hotmail.rs
 * @Last modified by:   anderson
 * @Last modified time: 2018-07-17T10:14:16-03:00
 */


/**
  * Triac dimmer with SSD1306, needs a zeroCross detection circuit at pin 2 (INT0)
  *
  * By Bigujun
  */


#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
// #include <U8x8lib.h>
// #include "SSD1306Ascii.h"
// #include "SSD1306AsciiWire.h"
#include <EEPROM.h>
#include <Dimmer.h>

//PINOS
enum Pins{
  ledRed = A1,
  ledOrange = A0,
  btnLeft = 10,
  btnRight = 12,
  btnEnter = 11,
  btnComum = 9,
  triac = 4
};
//DIMMERS

Dimmer dimmer(Pins::triac);

//-----SAVE AND LOAD CONFIGS

static const int EEPROM_KEY=420; //Chave aleatória verificar se já salvou algo

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}


struct Config{
  int key=0;
  uint8_t pot=0;
  uint8_t volt=1;
  uint8_t state;
}config;

void defaultConfig(){
  Config _default;
  config = _default;
}

void applyConfig(){
  dimmer.set(config.pot,config.state==2?true:false);
}

void saveConfig(){
  config.key = EEPROM_KEY;
  EEPROM_writeAnything(0, config);
}

void loadConfig(){
  EEPROM_readAnything(0, config);
  if(config.key != EEPROM_KEY){
    defaultConfig();
  }
  applyConfig();
}


// OLED

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
// U8G2_SSD1306_128X64_ALT0_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
// U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
// U8X8_SSD1306_128X64_ALT0_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
// #define u8g2 u8x8
const char *string_list =
  "Status\n"
  "Potencia\n"
  "Tensao\n"
  "Salvar\n"
  "Sobre";

const char *string_list_tensao =
  "220V\n"
  "127V";

  const char *string_list_state =
    "DESLIGADO\n"
    "LIGADO";

uint8_t current_selection = 0;

// //OLDE SSD1306SSD1306ASCII
//
// #define I2C_ADDRESS 0x3C
// #define RST_PIN -1
// SSD1306AsciiWire oled;

//MAIN ----------
void setup(void) {
  pinMode(Pins::ledOrange,OUTPUT);
  digitalWrite(Pins::ledOrange,HIGH);
  pinMode(Pins::ledRed,OUTPUT);
  digitalWrite(Pins::ledRed,HIGH);
  pinMode(Pins::btnComum,OUTPUT);
  digitalWrite(Pins::btnComum,LOW);
  delay(500);
  digitalWrite(Pins::ledRed,LOW);
  digitalWrite(Pins::ledOrange,LOW);
  Wire.begin();
  Wire.setClock(100000L);
  // oled.begin(&Adafruit128x64, I2C_ADDRESS);
  // oled.setFont(utf8font10x16);
  // oled.clear();

  // oled.print("Hello world!");
  // dimmer.on();
  // while(1);
  u8g2.begin(/*Select=*/ Pins::btnEnter, /*Right/Next=*/ Pins::btnRight, /*Left/Prev=*/ Pins::btnLeft, /*Up=*/ U8X8_PIN_NONE, /*Down=*/ U8X8_PIN_NONE, /*Home/Cancel=*/ U8X8_PIN_NONE); // Arduboy 10 (Production)
  u8g2.setFont(u8g2_font_9x18B_tf);
  // u8x8.setFont(u8x8_font_chroma48medium8_r);

  // loadConfig();
  u8g2.clear();

  // dimmer.begin();
}

// void loop(){
//
//
//   static uint8_t value=0;
//   static bool up=true;
//   if(value>99)up=false;
//   else if(value<1)up=true;
//   value=up?value+1:value-1;
//   dimmer.set(value);
//   oled.clear();
//   oled.print(value);
//   delay(500);
// }


void loop(void) {

  current_selection = u8g2.userInterfaceSelectionList(
    "Configuracoes",
    current_selection,
    string_list);

  if (current_selection == 1){
    config.state = u8g2.userInterfaceSelectionList("Status", config.state, string_list_state);
    applyConfig();
  }else if(current_selection == 2 ){
    u8g2.userInterfaceInputValue("Potência\n", NULL, &config.pot, 0, 100, 3, "%");
    applyConfig();
  }else if ( current_selection == 3 ){
    config.volt = u8g2.userInterfaceSelectionList("Tensão",config.volt,string_list_tensao);
    applyConfig();
  }else if(current_selection==4){
    uint8_t save = u8g2.userInterfaceMessage("Salvar","Configurações?","",  " Ok \n Cancelar");
    if(save==1){
      saveConfig();
      current_selection=0;
    }
  }else if(current_selection==5){
    u8g2.userInterfaceMessage("SmartDimmer", "Develop by:","Bigujun", " Ok ");
  }

}
