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

#ifndef __STYRA_CONFIG_H__
#define __STYRA_CONFIG_H__

#include <Arduino.h>

/* Define configuration record types */
#define BUTTON_MACRO 100
#define CONFIG_INFO 101
#define CONFIG_TABLE 102

/* Define action type for each button */
#define CLICK 1
#define DBL_CLICK 2
#define SEQUENCE 3
#define LATCH 4
#define PRESS 5

typedef struct {
    byte action[28];
    byte action_type;
    byte unused[2];
    byte record_type;
} ButtonMacro;

typedef struct {
    char name[18]; /* friendly name of the key configuration */
    uint16_t num_macros; /* store the number of button macros defined */
    uint16_t offset; /* store memory page offset from 0 for start of first button macro */
    byte unused[9];
    byte record_type;
} ConfigInfo;

typedef struct {
    uint16_t config[14]; /* store memory page address of ConfigInfo record for each key configuration */
    byte num_configs;
    byte unused[2];
    byte record_type;
} ConfigTable;

class StyraMacro
{
public:
    void setMacro(ButtonMacro macro) {
        _current_macro = macro;
        _index = 0;
    }
    byte getActionType() {
        return _current_macro.action_type;
    }
    byte getNextKey() {
        if (_index < 28) {
            return _current_macro.action[_index++];
        } else {
            return 0;
        }
    }
    void resetKeyIndex() {
        _index = 0;
    }
private:
    ButtonMacro _current_macro;
    uint8_t _index;
};

class StyraConfig
{
public:
    virtual void begin()= 0;
    /* Return the number of key configurations defined
     * in the ConfigTable */
    virtual uint8_t getNumConfigs() = 0;
    
    /* Return the ButtonMacro for the specified button number
     * within the currently active key configuration. */
    virtual ButtonMacro getMacro(uint8_t button) = 0;
    
    /* Store the ButtonMacro for the specified button number
     * within the currently active key configuration */
    virtual void storeMacro(ButtonMacro macro, uint8_t button) = 0;
    
    /* Return the ConfigInfo record for the currently
     * active key configuration */
    virtual ConfigInfo getConfigInfo() = 0;
    
    /* Change the active key configuration to the one specified
     * by an index into the ConfigTable.config[] array */
    virtual void setActiveConfig(uint8_t index) = 0;
    
    /* Return an index into the ConfigTable.configs[] array
     * for the currently active key configuration */
    virtual uint8_t getActiveConfig() = 0;
    
    /* Update the EEProm with the specified ConfigTable record */
    virtual void storeConfigTable(ConfigTable value) = 0;
    
    /* Update the EEProm with a ConfigInfo record at the
     * specified memory page. */
    virtual void storeConfigInfo(ConfigInfo value, uint16_t page) = 0;
    
    /* Update the EEProm with a ButtonMacro record at the
     * specified memory page. */
    virtual void storeButtonMacro(ButtonMacro macro, uint16_t page) = 0;
    
    /* Return the ConfigTable record from the EEProm */
    virtual ConfigTable getConfigTableFromEEProm() = 0;
    
    /* Return a ConfigInfo record from the specified memory page */
    virtual ConfigInfo getConfigInfoFromEEProm(uint16_t page) = 0;
    
    /* Return a ButtonMacro record from the specified memory page */
    virtual ButtonMacro getButtonMacroFromEEProm(uint16_t page) = 0;
    
};

#endif /* __STYRA_CONFIG_H__ */
