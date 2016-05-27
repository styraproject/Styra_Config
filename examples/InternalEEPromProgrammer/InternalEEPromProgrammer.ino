
#include <Wire.h>
#include "EEPROM.h"
#include <Styra_Config_InternalEEProm.h>
#include <Styra_Config.h>




/* Define programmer messages */
#define READY 200
#define CONFIG 201
#define STORE_BUTTON_MACRO 202
#define STORE_CONFIG_INFO 203
#define STORE_CONFIG_TABLE 204
#define GET_BUTTON_MACRO 205
#define GET_CONFIG_INFO 206
#define GET_CONFIG_TABLE 207

typedef union {
  
  struct {
    byte lower: 8;
    byte upper: 8;
  } bytes;
  uint16_t value;
  
} SERIAL_UINT16_T;

void setup() {
    int byte_count = 0;
    uint8_t byte_buffer = 0;
    SERIAL_UINT16_T int_buffer;
    int_buffer.value = 0;
    uint8_t tmp = 0;


    ButtonMacro macro;
    ConfigInfo info;
    ConfigTable table;
    byte * p;

    /* Connect to EEProm */
    StyraConfigInternalEEProm eeprom_cfg = StyraConfigInternalEEProm();
    eeprom_cfg.begin();

    Serial.begin(9600);
    while (!Serial);
    delay(1000);
    Serial.write(READY);
    
    /* Wait for CONFIG packet */
    while (1) {
      if (Serial.available() && Serial.read() == CONFIG) {
         break;
      }
    }
    Serial.write(READY);

    /* Wait for control instruction */
    while (1) {
      if (Serial.available()) {
        byte_buffer = Serial.read() ;
        if (byte_buffer == GET_CONFIG_TABLE) {

          table = eeprom_cfg.getConfigTableFromEEProm();
          Serial.write((char *) &table,sizeof(table));
                   
        } else if (byte_buffer == GET_CONFIG_INFO) {
          Serial.write(READY);
          /* Wait for page location for config  */
          while (1) {
            if (Serial.available() > 1) {
              int_buffer.bytes.lower = Serial.read();
              int_buffer.bytes.upper = Serial.read();
              break;
            }
          }

          info = eeprom_cfg.getConfigInfoFromEEProm(int_buffer.value);
          Serial.write((char *) &info,sizeof(info));
          
        } else if (byte_buffer == GET_BUTTON_MACRO) {
          Serial.write(READY);
          /* Wait for page location for macro  */
          while (1) {
            if (Serial.available() > 1) {
              int_buffer.bytes.lower = Serial.read();
              int_buffer.bytes.upper = Serial.read();
              break;
            }
          }
          
          macro = eeprom_cfg.getButtonMacroFromEEProm(int_buffer.value);
          Serial.write((char *) &macro,sizeof(macro));
          
        } else if (byte_buffer == STORE_CONFIG_TABLE) {
          Serial.write(READY);
          p = (byte *) &table;
          byte_count = 0;

          while (byte_count < sizeof(ConfigTable)){
            if (Serial.available()) {

              p[byte_count++] = Serial.read();
            }
          }

          eeprom_cfg.storeConfigTable(table);
          Serial.write(READY);
          

        } else if (byte_buffer == STORE_CONFIG_INFO) {
          Serial.write(READY);
          /* Wait for page location for ConfigInfo record  */
        
          while (1) {
            if (Serial.available() > 1) {
              int_buffer.bytes.lower = Serial.read();
              int_buffer.bytes.upper = Serial.read();
              break;
            }
          }
          Serial.write(READY);

          p = (byte *) &info;
          byte_count = 0;
          while (byte_count < sizeof(ConfigInfo)){
            if (Serial.available()) {
              p[byte_count++] = Serial.read();
            }
          }
          Serial.write(READY);
          eeprom_cfg.storeConfigInfo(info,int_buffer.value);
          Serial.write(READY);
        
        } else if (byte_buffer == STORE_BUTTON_MACRO) {
          Serial.write(READY);
          /* Wait for page location for ButtonMacro record  */
          while (1) {
            if (Serial.available() > 1) {
              int_buffer.bytes.lower = Serial.read();
              int_buffer.bytes.upper = Serial.read();
              break;
            }
          }
          Serial.write(READY);

          p = (byte *) &macro;
          byte_count = 0;
          while (byte_count < sizeof(ButtonMacro)){
            if (Serial.available()) {
              p[byte_count++] = Serial.read();
            }
          }
          eeprom_cfg.storeButtonMacro(macro,int_buffer.value);
          Serial.write(READY);
          
        }        
        
      }

    }


   
}

void loop() {
  
}



