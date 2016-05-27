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
    /* Return next byte of macro or 0 when finished */
    virtual void storeMacro(ButtonMacro macro, byte button) = 0;
    virtual ButtonMacro getMacro(byte button) = 0;
    virtual uint8_t getNumConfigs() = 0;
    virtual ConfigInfo getConfigInfo() = 0;
    virtual void setActiveConfig(uint8_t index) = 0;
    virtual uint8_t getActiveConfig() = 0;
    
};

#endif /* __STYRA_CONFIG_H__ */
