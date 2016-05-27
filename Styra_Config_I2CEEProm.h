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

#ifndef __STYRA_CONFIG_I2CEEProm_H__
#define __STYRA_CONFIG_I2CEEProm_H__

#include <Arduino.h>
#include <Wire.h>
#include "Styra_Config.h"


#define DEVICE_ADDRESS 0x50


class StyraConfigI2CEEProm:  public StyraConfig
{
public:
    StyraConfigI2CEEProm() {
        _initialized = false;
    }
    void init();
    void begin();
 
    /* Return the number of key configurations defined
     * in the ConfigTable */
    uint8_t getNumConfigs();
    
    /* Return the ButtonMacro for the specified button number
     * within the currently active key configuration. */
    ButtonMacro getMacro(uint8_t button);
    
    /* Store the ButtonMacro for the specified button number
     * within the currently active key configuration */
    void storeMacro(ButtonMacro macro, uint8_t button);
    
    /* Return the ConfigInfo record for the currently
     * active key configuration */
    ConfigInfo getConfigInfo();
    
    /* Change the active key configuration to the one specified
     * by an index into the ConfigTable.config[] array */
    void setActiveConfig(uint8_t index);
    
    /* Return an index into the ConfigTable.configs[] array
     * for the currently active key configuration */
    uint8_t getActiveConfig();
    
    /* Update the EEProm with the specified ConfigTable record */
    void storeConfigTable(ConfigTable value);
    
    /* Update the EEProm with a ConfigInfo record at the
     * specified memory page. */
    void storeConfigInfo(ConfigInfo value, uint16_t page);
    
    /* Update the EEProm with a ButtonMacro record at the
     * specified memory page. */
    void storeButtonMacro(ButtonMacro macro, uint16_t page);
    
    /* Return the ConfigTable record from the EEProm */
    ConfigTable getConfigTableFromEEProm();
    
    /* Return a ConfigInfo record from the specified memory page */
    ConfigInfo getConfigInfoFromEEProm(uint16_t page);
    
    /* Return a ButtonMacro record from the specified memory page */
    ButtonMacro getButtonMacroFromEEProm(uint16_t page);

private:
    uint8_t getWriteStatus();
    ConfigTable _conf_table;
    uint8_t _num_configs;
    uint8_t _active_config_index;
    ConfigInfo _conf_info;
    bool _initialized;

};

#endif /* __STYRA_CONFIG_I2CEEProm_H__ */
