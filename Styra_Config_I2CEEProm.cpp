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

#include "Styra_Config_I2CEEProm.h"

#define ACTION_SIZE 30

void StyraConfigI2CEEProm::init() {
  if (! _initialized) {
    Wire.begin(); 
    _initialized = true;
  }

}

void StyraConfigI2CEEProm::begin() {
  this->init();
  this->_num_configs = 0;
  this->_conf_table = this->getConfigTableFromEEProm();
  if (this->_conf_table.record_type != CONFIG_TABLE) {
    return;
  }
  this->_num_configs = this->_conf_table.num_configs;
  this->_active_config_index = 0;
  this->_conf_info = this->getConfigInfoFromEEProm(this->_conf_table.config[this->_active_config_index]);
}

void StyraConfigI2CEEProm::storeMacro(ButtonMacro value, uint8_t button) {
  storeButtonMacro(value,this->_conf_info.offset + button);
}

void StyraConfigI2CEEProm::storeButtonMacro(ButtonMacro value, uint16_t page) {
  uint16_t EEPROM_address = page * 64;
  byte* p = (byte*)(void*)&value;

  uint8_t bytes_to_send;
  uint8_t this_pass;
  bytes_to_send = sizeof(value);
  while (bytes_to_send > 0)  {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write((int)(EEPROM_address >> 8)); //writes the MSB
    Wire.write((int)(EEPROM_address & 0xFF)); //writes the LSB
    if (bytes_to_send < 16) {
      this_pass = bytes_to_send;
    } else {
      this_pass = 16;
    }
    for (uint8_t i = 0; i < this_pass; i++) {
      Wire.write(*p++); //write data byte
    }
    Wire.endTransmission();
    while (getWriteStatus() != 0) delay(10);
    bytes_to_send -= this_pass;
    EEPROM_address += this_pass;
  }
}

void StyraConfigI2CEEProm::storeConfigTable(ConfigTable value) {
  uint16_t EEPROM_address = 0 ;/* The ConfigTable is always stored at address 0 */
  byte* p = (byte*)(void*)&value;

  uint8_t bytes_to_send;
  uint8_t this_pass;
  bytes_to_send = sizeof(value);
  while (bytes_to_send > 0)  {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write((int)(EEPROM_address >> 8)); //writes the MSB
    Wire.write((int)(EEPROM_address & 0xFF)); //writes the LSB
    if (bytes_to_send < 16) {
      this_pass = bytes_to_send;
    } else {
      this_pass = 16;
    }
    for (uint8_t i = 0; i < this_pass; i++) {
      Wire.write(*p++); //write data byte
    }
    Wire.endTransmission();
    while (getWriteStatus() != 0) delay(10);
    bytes_to_send -= this_pass;
    EEPROM_address += this_pass;
  }
}

void StyraConfigI2CEEProm::storeConfigInfo(ConfigInfo value,uint16_t page) {
  uint16_t EEPROM_address = page * 64;
  byte* p = (byte*)(void*)&value;

  uint8_t bytes_to_send;
  uint8_t this_pass;
  bytes_to_send = sizeof(value);
  while (bytes_to_send > 0)  {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write((int)(EEPROM_address >> 8)); //writes the MSB
    Wire.write((int)(EEPROM_address & 0xFF)); //writes the LSB
    if (bytes_to_send < 16) {
      this_pass = bytes_to_send;
    } else {
      this_pass = 16;
    }
    for (uint8_t i = 0; i < this_pass; i++) {
      Wire.write(*p++); //write data byte
    }
    Wire.endTransmission();
    while (getWriteStatus() != 0) delay(10);
    bytes_to_send -= this_pass;
    EEPROM_address += this_pass;
  }
}


ButtonMacro StyraConfigI2CEEProm::getMacro(uint8_t button) {
  ButtonMacro value;
  byte* p = (byte*)(void*)&value;
  uint16_t EEPROM_address = (this->_conf_info.offset + button) * 64;

  uint8_t this_pass;
  uint8_t bytes_to_receive = sizeof(value);
  while (bytes_to_receive > 0) {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write((int)(EEPROM_address >> 8)); //writes the MSB
    Wire.write((int)(EEPROM_address & 0xFF)); //writes the LSB
    Wire.endTransmission();
    if (bytes_to_receive < 16) {
      this_pass = bytes_to_receive;
    } else {
      this_pass = 16;
    }
    Wire.requestFrom(DEVICE_ADDRESS,(int)this_pass);
    while (Wire.available()) {
      *p++ = Wire.read();  //get data byte
    }
    bytes_to_receive -= this_pass;
    EEPROM_address += this_pass;
  }
  return value;
}

ButtonMacro StyraConfigI2CEEProm::getButtonMacroFromEEProm(uint16_t page) {
  ButtonMacro value;
  byte* p = (byte*)(void*)&value;
  uint16_t EEPROM_address = page * 64;

  uint8_t this_pass;
  uint8_t bytes_to_receive = sizeof(value);
  while (bytes_to_receive > 0) {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write((int)(EEPROM_address >> 8)); //writes the MSB
    Wire.write((int)(EEPROM_address & 0xFF)); //writes the LSB
    Wire.endTransmission();
    if (bytes_to_receive < 16) {
      this_pass = bytes_to_receive;
    } else {
      this_pass = 16;
    }
    Wire.requestFrom(DEVICE_ADDRESS,(int)this_pass);
    while (Wire.available()) {
      *p++ = Wire.read();  //get data byte
    }
    bytes_to_receive -= this_pass;
    EEPROM_address += this_pass;
  }
  return value;
}


ConfigInfo StyraConfigI2CEEProm::getConfigInfoFromEEProm(uint16_t page) {
  ConfigInfo value;
  byte* p = (byte*)(void*)&value;
  uint16_t EEPROM_address = page * 64;

  uint8_t this_pass;
  uint8_t bytes_to_receive = sizeof(value);
  while (bytes_to_receive > 0) {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write((int)(EEPROM_address >> 8)); //writes the MSB
    Wire.write((int)(EEPROM_address & 0xFF)); //writes the LSB
    Wire.endTransmission();
    if (bytes_to_receive < 16) {
      this_pass = bytes_to_receive;
    } else {
      this_pass = 16;
    }
    Wire.requestFrom(DEVICE_ADDRESS,(int)this_pass);
    while (Wire.available()) {
      *p++ = Wire.read();  //get data byte
    }
    bytes_to_receive -= this_pass;
    EEPROM_address += this_pass;
  }
  return value;
}

ConfigTable StyraConfigI2CEEProm::getConfigTableFromEEProm() {
  ConfigTable value;
  byte* p = (byte*)(void*)&value;
  uint16_t EEPROM_address = 0;

  uint8_t this_pass;
  uint8_t bytes_to_receive = sizeof(value);
  while (bytes_to_receive > 0) {
    Wire.beginTransmission(DEVICE_ADDRESS);
    Wire.write((int)(EEPROM_address >> 8)); //writes the MSB
    Wire.write((int)(EEPROM_address & 0xFF)); //writes the LSB
    Wire.endTransmission();
    if (bytes_to_receive < 16) {
      this_pass = bytes_to_receive;
    } else {
      this_pass = 16;
    }
    Wire.requestFrom(DEVICE_ADDRESS,(int)this_pass);
    while (Wire.available()) {
      *p++ = Wire.read();  //get data byte
    }
    bytes_to_receive -= this_pass;
    EEPROM_address += this_pass;
  }

  return value;
}

uint8_t StyraConfigI2CEEProm::getNumConfigs() {
  return this->_conf_table.num_configs;
}
ConfigInfo StyraConfigI2CEEProm::getConfigInfo() {
  return this->_conf_info;
}
void StyraConfigI2CEEProm::setActiveConfig(uint8_t index) {
  if (index < this->_num_configs ) {
    this->_active_config_index = 0;
    this->_conf_info = this->getConfigInfoFromEEProm(this->_conf_table.config[this->_active_config_index]);  
  }
  return;
}
uint8_t StyraConfigI2CEEProm::getActiveConfig() {
  return this->_active_config_index;
}

uint8_t StyraConfigI2CEEProm::getWriteStatus() {
  uint8_t status;
  Wire.beginTransmission(DEVICE_ADDRESS);
  status = Wire.endTransmission();
  return status;

}
