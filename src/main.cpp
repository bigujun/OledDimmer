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
#include <U8g2lib.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Dimmer.h>





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
}config;

void defaultConfig(){
  Config _default;
  config = _default;
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
}

//DIMMERS

Dimmer dimmer(4);


// OLED

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const char *string_list =
  "Status\n"
  "Potencia\n"
  "Tensao\n"
  "Salvar\n"
  "Sobre";

const char *string_list_tensao =
  "220V\n"
  "127V";

uint8_t current_selection = 0;



//MAIN ----------
void setup(void) {
  loadConfig();
  dimmer.begin();
  u8g2.begin(/*Select=*/ A0, /*Right/Next=*/ A1, /*Left/Prev=*/ A2, /*Up=*/ U8X8_PIN_NONE, /*Down=*/ U8X8_PIN_NONE, /*Home/Cancel=*/ U8X8_PIN_NONE); // Arduboy 10 (Production)
  u8g2.setFont(u8g2_font_9x18B_tf);
}


void loop(void) {

  current_selection = u8g2.userInterfaceSelectionList(
    "Configuracoes",
    current_selection,
    string_list);

  if ( current_selection == 0 ) {
    u8g2.userInterfaceMessage(
    "Nothing selected.", "", "", " ok ");
  }else if (current_selection == 1){
    uint8_t state = u8g2.userInterfaceMessage("Status", "", "", " OFF \n ON ");
    if(state==2){
      dimmer.on();
    }else{
      dimmer.off();
    }
  }else if(current_selection == 2 ){
    u8g2.userInterfaceInputValue("Potência\n", NULL, &config.pot, 0, 100, 3, "%");
  }else if ( current_selection == 3 ){
    config.volt = u8g2.userInterfaceSelectionList("Tensão",config.volt,string_list_tensao);
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
