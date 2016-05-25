/**
 * Styra_Config provides standard macro configs for Styra prototypes.
 * 
 * Copyright (C) 2016  Luke Hindman <luke@platypuscreations.net>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */ 

#include "Styra_Config_InternalEEProm.h"

#define ACTION_SIZE 30
//#define SERIAL_DEBUG

InternalEEPromConfig::InternalEEPromConfig() {
        _initialized = false;
}


void InternalEEPromConfig::begin() {
  _initialized = true;
  this->_num_configs = 0;
  this->_conf_table = this->getConfigTableFromEEProm();
  if (this->_conf_table.record_type != CONFIG_TABLE) {
    return;
  }
  this->_num_configs = this->_conf_table.num_configs;
  this->_active_config_index = 0;
  this->_conf_info = this->getConfigInfoFromEEProm(this->_conf_table.config[this->_active_config_index]);
}

void InternalEEPromConfig::storeMacro(ButtonMacro value, uint8_t button) {
  storeButtonMacro(value,this->_conf_info.offset + button);
}

void InternalEEPromConfig::storeButtonMacro(ButtonMacro value, uint16_t page) {
  uint16_t EEPROM_address = page * sizeof(ButtonMacro);
  byte* p = (byte*)(void*)&value;

  for (int i = 0; i < sizeof(ButtonMacro); i++) {
    EEPROM.write(EEPROM_address++,p[i]);
  }

}

void InternalEEPromConfig::storeConfigTable(ConfigTable value) {
  uint16_t EEPROM_address = 0 ;/* The ConfigTable is always stored at address 0 */
  byte* p = (byte*)(void*)&value;

  for (int i = 0; i < sizeof(ConfigTable); i++) {
    EEPROM.write(EEPROM_address++,p[i]);
  }
}

void InternalEEPromConfig::storeConfigInfo(ConfigInfo value,uint16_t page) {
  uint16_t EEPROM_address = page * sizeof(ConfigInfo);
  byte* p = (byte*)(void*)&value;

  for (int i = 0; i < sizeof(ConfigInfo); i++) {
    EEPROM.write(EEPROM_address++,p[i]);
  }
}


ButtonMacro InternalEEPromConfig::getMacro(uint8_t button) {
  ButtonMacro value;
  byte* p = (byte*)(void*)&value;
  uint16_t EEPROM_address = (this->_conf_info.offset + button) * sizeof(ButtonMacro);

  for (int i = 0; i < sizeof(ButtonMacro); i++) {
    p[i] = EEPROM.read(EEPROM_address++);
  }

  return value;
}

ButtonMacro InternalEEPromConfig::getButtonMacroFromEEProm(uint16_t page) {
  ButtonMacro value;
  byte* p = (byte*)(void*)&value;
  uint16_t EEPROM_address = page * sizeof(ButtonMacro);

  for (int i = 0; i < sizeof(ButtonMacro); i++) {
    p[i] = EEPROM.read(EEPROM_address++);
  }

  return value;
}


ConfigInfo InternalEEPromConfig::getConfigInfoFromEEProm(uint16_t page) {
  ConfigInfo value;
  byte* p = (byte*)(void*)&value;
  uint16_t EEPROM_address = page * sizeof(ConfigInfo);

  for (int i = 0; i < sizeof(ConfigInfo); i++) {
    p[i] = EEPROM.read(EEPROM_address++);
  }

  return value;
}

ConfigTable InternalEEPromConfig::getConfigTableFromEEProm() {
  ConfigTable value;
  byte* p = (byte*)(void*)&value;
  uint16_t EEPROM_address = 0;

  for (int i = 0; i < sizeof(ConfigTable); i++) {
    p[i] = EEPROM.read(EEPROM_address++);
  }

  return value;
}

uint8_t InternalEEPromConfig::getNumConfigs() {
  return this->_conf_table.num_configs;
}
ConfigInfo InternalEEPromConfig::getConfigInfo() {
  return this->_conf_info;
}
void InternalEEPromConfig::setActiveConfig(uint8_t index) {
  if (index < this->_num_configs ) {
    this->_active_config_index = 0;
    this->_conf_info = this->getConfigInfoFromEEProm(this->_conf_table.config[this->_active_config_index]);  
  }
  return;
}
uint8_t InternalEEPromConfig::getActiveConfig() {
  return this->_active_config_index;
}

