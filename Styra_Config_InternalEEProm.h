/* I2CEEPromStyraConfig.h
 * Author:  Luke Hindman
 * Date: Sun Aug 30 17:44:54 MDT 2015
 * Description:  Provide a standard configuration 
 *   Styra Keyboard devices that can provide either hard coded,
 *   eeprom or vfram stored button configurations. */

#ifndef __STYRA_CONFIG_InternalEEProm_H__
#define __STYRA_CONFIG_InternalEEProm_H__

#include <Arduino.h>
#include <EEPROM.h>

#include "Styra_Config.h"

class StyraConfigInternalEEProm:  public StyraConfig
{
public:
    StyraConfigInternalEEProm();
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
    ConfigTable _conf_table;
    uint8_t _num_configs;
    uint8_t _active_config_index;
    ConfigInfo _conf_info;
    bool _initialized;

};

#endif /* __STYRA_CONFIG_InternalEEProm_H__ */
