#!/opt/local/bin/python
# Author:  Luke Hindman
# Date:  Sun Oct  4 11:15:56 MDT 2015
# Description:  This tool generates ButtonMacro configurations from python
#    configuration files.

from ConfigParser import ConfigParser
import re
import struct
import binascii
import os
import time
import serial
import sys
import getopt

import serial.tools.list_ports
from usbid.usbinfo import USBINFO


class ArduinoDevice:

    
    def __init__(self):
        self.idVendor = ""
        self.idProduct = ""
        self.productString = ""
        self.serialDevice = ""
        
    def __init__(self,idVendor,idProduct):
        self.idVendor = idVendor
        self.idProduct = idProduct
        self.vendorString = ""
        self.productString = ""
        self.serialDevice = ""
        
       

# Generated from ArduinoKeyCodes
# cat ArduinoKeyCodes.txt | awk 'BEGIN {print "{" } NR > 2 {print "\x27" $1 "\x27" ":" "\x27" $3 "\x27," } END {print "}"} '
arduinoKeycodeDict = {
    'KEY_LEFT_CTRL':'128',
    'KEY_LEFT_SHIFT':'129',
    'KEY_LEFT_ALT':'130',
    'KEY_LEFT_GUI':'131',
    'KEY_RIGHT_CTRL':'132',
    'KEY_RIGHT_SHIFT':'133',
    'KEY_RIGHT_ALT':'134',
    'KEY_RIGHT_GUI':'135',
    'KEY_UP_ARROW':'218',
    'KEY_DOWN_ARROW':'217',
    'KEY_LEFT_ARROW':'216',
    'KEY_RIGHT_ARROW':'215',
    'KEY_BACKSPACE':'178',
    'KEY_TAB':'179',
    'KEY_RETURN':'176',
    'KEY_ESC':'177',
    'KEY_INSERT':'209',
    'KEY_DELETE':'212',
    'KEY_PAGE_UP':'211',
    'KEY_PAGE_DOWN':'214',
    'KEY_HOME':'210',
    'KEY_END':'213',
    'KEY_CAPS_LOCK':'193',
    'KEY_F1':'194',
    'KEY_F2':'195',
    'KEY_F3':'196',
    'KEY_F4':'197',
    'KEY_F5':'198',
    'KEY_F6':'199',
    'KEY_F7':'200',
    'KEY_F8':'201',
    'KEY_F9':'202',
    'KEY_F10':'203',
    'KEY_F11':'204',
    'KEY_F12':'205'}

styraActionTypeDict = {
    'CLICK':1,
    'DBL_CLICK':2,
    'SEQUENCE':3,
    'LATCH':4,
    'PRESS':5,
    'TOGGLE_MOUSE':6,
    'WHEEL_LOCK':7
}

styraRecordTypeDict = {
    'BUTTON_MACRO':100,
    'CONFIG_INFO':101,
    'CONFIG_TABLE':102
}

arduinoMesgDict = {
    'READY':200,
    'CONFIG':201,
    'STORE_BUTTON_MACRO':202,
    'STORE_CONFIG_INFO':203,
    'STORE_CONFIG_TABLE':204,
    'GET_BUTTON_MACRO':205,
    'GET_CONFIG_INFO':206,
    'GET_CONFIG_TABLE':207
}

macro_action_size=28

#
# These hardcoded values need to be pulled from the config file
#
fix_config_count=1
fix_config_info_page=1
fix_config_offset=2


def getList(list_string, sep=',', chars=None, delimited=True):
    """Return a list from a ConfigParser option. By default,
       split on a comma and strip whitespaces."""
    if delimited:
        return [ chunk.strip(chars) for chunk in list_string.split(sep) ]
    else:
        return list(list_string)

def getKeyBytes(keys_string,action_type):
    if action_type == "SEQUENCE":
        print "Parsing String: " + keys_string
        key_list = getList(keys_string,delimited=False)
    else:
        key_list = getList(keys_string)
    key_bytes = []
    for k in key_list:
        if k in arduinoKeycodeDict.keys():
            key_bytes.append(int(arduinoKeycodeDict[k]))
        else:
            key_bytes.append(ord(k))
    
    while (len(key_bytes) < macro_action_size):
        key_bytes.append(0)
    
    return key_bytes





#typedef struct {
#    char name[18];
#    uint16_t num_macros;
#    uint16_t offset;
#    byte unused[9];
#    byte record_type;
#} ConfigInfo;
def getPackedConfigInfo(config_name,num_macros,offset):
    s = struct.Struct('18B H H 9B B')
    byte_list = []
    
    # Convert the name to a list of no more than 18 characters
    name_bytes = []
    for c in list(config_name)[:18]:
        name_bytes.append(ord(c))
        
    while (len(name_bytes) < 18):
        name_bytes.append(0)

    print name_bytes
    byte_list = byte_list + name_bytes
    
    # Store the number of macros
    byte_list.append(int(num_macros))
    
    # Store the page offset for the first button_macro
    byte_list.append(int(offset))
    
    # Append unused bytes as place holders
    for i in range(9):
        byte_list.append(255)
        
    # Set the record type
    byte_list.append(styraRecordTypeDict['CONFIG_INFO'])
    
    return s.pack(*byte_list)  
    

#typedef struct {
#    byte config[28];
#    byte num_configs;
#    byte unused[2];
#    byte record_type;
#} ConfigTable;
def getPackedConfigTable():
    s = struct.Struct('14H B 2B B')
    byte_list = []
    
    # Store page location of each config to byte
    # Default has 1 configuration stored at page 1
    config_location_bytes=[]
    config_location_bytes.append(fix_config_info_page)
    # Zero out the remaining bytes
    while (len(config_location_bytes) < 14):
        config_location_bytes.append(0)
    byte_list = byte_list + config_location_bytes

    # num_config
    byte_list.append(fix_config_count)
    
    # unused bytes
    byte_list.append(255)
    byte_list.append(255)
    
    # set the record_type
    byte_list.append(styraRecordTypeDict['CONFIG_TABLE'])
    
    return s.pack(*byte_list)
    
def getPackedButtonMacro(keys_string,action_type):
    s = struct.Struct('28B B 2B B')
    byte_list = getKeyBytes(keys_string,action_type)[:28]
    byte_list.append(styraActionTypeDict[action_type])
    byte_list.append(255)
    byte_list.append(255)
    byte_list.append(styraRecordTypeDict['BUTTON_MACRO'])
    print ("Byte List:  %s" % byte_list)
    #values = tuple(byte_list)
    #print ("Values: %s" % values)
    return s.pack(*byte_list)

def displayMenu():
    os.system('clear')
    print "*************************************"
    print "* Styra Keyboard Configuration Tool *"
    print "*************************************"
    print ""
    print "u - Upload config to keyboard"
    print "d - Download config from keyboard"
    print "x - Exit"

def arduinoReset(arduino):
    # reset the arduino
    arduino.setDTR(level=False)
    time.sleep(0.5)
    # ensure there is no stale data in the buffer
    arduino.flushInput()
    arduino.setDTR()
    time.sleep(0.5)

def arduinoHandshake(arduino):
    arduinoReset(arduino)
    print "Waiting for Arduino to initialize..."
    while 1:
        try:
            if struct.unpack("B", arduino.read(1))[0] != arduinoMesgDict['READY']:
                print "."
                time.sleep(0.5)
            else:
                break
        except Exception as msg:
            print "Device disconnected"
    arduino.flushInput()
    print "Entering configuration mode..."
    arduino.write(struct.pack("B",arduinoMesgDict['CONFIG']))
    while struct.unpack("B", arduino.read(1))[0] != arduinoMesgDict['READY']:
        print "."
        time.sleep(0.5)
    arduino.flushInput()

def getArduinoSerialDevice():
    device_list = []
    device_count = 0
    ports = list(serial.tools.list_ports.comports())
    print ports
    for p in ports:
        found = re.search("VID:PID=([A-Fa-f0-9]+):([A-Fa-f0-9]+)",p[2])
        if found is not None:
            idVendor = found.group(1)
            idProduct = found.group(2)
            arduino = ArduinoDevice(idVendor,idProduct)
            arduino.serialDevice = p[0]
            try:
                vendor_lookup = USBINFO.usb_ids[idVendor]
                arduino.vendorString = vendor_lookup[0]
                arduino.productString = vendor_lookup[1][idProduct]
            except KeyError as msg:
                print msg
            device_list.append(arduino)
    
    serial_selection = None
    if not device_list:
        print "No devices found"
    elif len(device_list) == 1:
        device = device_list[0]
        print "%d - %s %s (%s)" % (device_count, device.vendorString, device.productString, device.serialDevice)
        serial_selection =  device.serialDevice
    else:
        for device in device_list:
            print "%d - %s %s (%s)" % (device_count, device.vendorString, device.productString, device.serialDevice)
            device_count = device_count + 1
        while True:
            selection = int(raw_input("Select a device: "))
            if selection in range(device_count):
                serial_selection =  device_list[selection].serialDevice
                break
            
    return serial_selection


if __name__ == '__main__':


    configFileName = 'button_macro.cfg'

    try:    
        opts, args = getopt.getopt(sys.argv[1:], "c:", ["config="])
    except getopt.GetOptError:
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-c", "--config"):
            configFileName = arg
        else:
            print "Invalid Options"
            sys.exit(2)

    if not os.path.exists(configFileName):
        print "Config file %s does not exist" % configFileName
        sys.exit(1)
    

    # Setup connection to arduino
    serial_device = getArduinoSerialDevice()
    arduino = serial.Serial(serial_device, 9600)
    
    # Reset the arduino and place it in config mode
    arduinoHandshake(arduino)
    
    running = True
    while running:
        displayMenu()
        while True:
            n = raw_input("\n\n--> ")
            if n == "u":
                
                
                config = ConfigParser()
                config.read(configFileName)
                
                buttonIndexList = []
                buttonSectionList = []
                for s in config.sections():
                    m = re.match("button([0-9]+)",s)
                    if m:
                        print "Button Section: %s" % m.group(0)
                        buttonSectionList.append(m.group(0))
                        buttonIndexList.append(int(m.group(1)))
                    else:
                        print "Not a button: %s" % s
                numMemoryPages = max(buttonIndexList) + 1
                
                print "Upload Config Table..."
                arduino.write(struct.pack("B",arduinoMesgDict['STORE_CONFIG_TABLE']))
                while struct.unpack("B", arduino.read(1))[0] != arduinoMesgDict['READY']:
                    print "."
                    time.sleep(0.5)
                arduino.flushInput()
                print "Writing..."
                arduino.write(getPackedConfigTable())
                while struct.unpack("B", arduino.read(1))[0] != arduinoMesgDict['READY']:
                    print "."
                    time.sleep(0.5)
                arduino.flushInput()
                print "Done..."
                
                print "Upload Config Info..."
                arduino.write(struct.pack("B",arduinoMesgDict['STORE_CONFIG_INFO']))
                while struct.unpack("B", arduino.read(1))[0] != arduinoMesgDict['READY']:
                    print "."
                    time.sleep(0.5)
                arduino.flushInput()
                arduino.write(struct.pack("H",fix_config_info_page))
                while struct.unpack("B", arduino.read(1))[0] != arduinoMesgDict['READY']:
                    print "."
                    time.sleep(0.5)
                arduino.flushInput()
                arduino.write(getPackedConfigInfo("Styra-Nav-01",numMemoryPages,fix_config_offset))
                while struct.unpack("B", arduino.read(1))[0] != arduinoMesgDict['READY']:
                    print "."
                    time.sleep(0.5)
                arduino.flushInput()
                
                for button in buttonSectionList:
                    print "\n\nUpload macro for %s" % button
                    index = int(re.match("button([0-9]+)",button).group(1))
                    arduino.write(struct.pack("B",arduinoMesgDict['STORE_BUTTON_MACRO']))
                    while struct.unpack("B", arduino.read(1))[0] != arduinoMesgDict['READY']:
                        sys.stdout.write('.')
                        sys.stdout.flush()
                        time.sleep(0.5)
                    arduino.flushInput()
                    # Send the button number (index) that the macro if associated with
                    print "\nStoring button offset %d" % (fix_config_offset+index)
                    arduino.write(struct.pack("H",fix_config_offset+index))
                    while struct.unpack("B", arduino.read(1))[0] != arduinoMesgDict['READY']:
                        sys.stdout.write('+')
                        sys.stdout.flush()
                        time.sleep(0.5)
                    arduino.flushInput()
                    # Send the macro itself
                    arduino.write(getPackedButtonMacro(config.get(button,'keys'),config.get(button,'action')))
                    while struct.unpack("B", arduino.read(1))[0] != arduinoMesgDict['READY']:
                        sys.stdout.write('>')
                        sys.stdout.flush()
                        time.sleep(0.5)
                    arduino.flushInput()
                
                print "Upload complete"
                time.sleep(5)
                break  # stops the loop
            
            elif n == "d":
                
                print "Download Config Table..."
                arduino.write(struct.pack("B",arduinoMesgDict['GET_CONFIG_TABLE']))
                print "Waiting"
                rx_data = arduino.read(32)
                print "Done waiting"
                configTable = struct.unpack('14H B 2B B', rx_data)
                arduino.flushInput()
                #print ":".join("{:02x}".format(ord(b)) for b in rx_data)
                #print configTable
                #time.sleep(10)
                if configTable[17] != styraRecordTypeDict['CONFIG_TABLE']:
                    print "Expected CONFIG_TABLE but received %d" % int(configTable[17])
                    break;
                # Process one or more configs
                for config_page in configTable[:13]:
                    if config_page == 0:
                        break
                    # Retrieve the ConfigInfo object from (config_index)
                    print "Download Config %d..." % int(config_page) 
                    arduino.write(struct.pack("B",arduinoMesgDict['GET_CONFIG_INFO']))
                    while struct.unpack("B", arduino.read(1))[0] != arduinoMesgDict['READY']:
                        print "."
                        time.sleep(0.5)
                    arduino.flushInput()
                    arduino.write(struct.pack("H",config_page))
                    configInfo = struct.unpack('18B H H 9B B', arduino.read(32))
                    if configInfo[29] != styraRecordTypeDict['CONFIG_INFO']:
                        print "Expected CONFIG_INFO but received %d" % int(configInfo[29])
                        break;
                    configName = ""
                    for c in configInfo[:17]:
                        if c == 0:
                            break
                        configName = configName + chr(c)

                    configNumMacros=int(configInfo[18])
                    configOffset=int(configInfo[19])
                    
                    print "Download %d ButtonMacros for Config %s (%d)" % (configNumMacros,configName,configOffset)
                    
                    for macro in range(configNumMacros):
                        print "Download Button Macro %d..." % macro
                        arduino.write(struct.pack("B",arduinoMesgDict['GET_BUTTON_MACRO']))
                        while struct.unpack("B", arduino.read(1))[0] != arduinoMesgDict['READY']:
                            print "."
                            time.sleep(0.5)
                        arduino.flushInput()
                        arduino.write(struct.pack("H",configOffset + macro))
                        buttonMacro = struct.unpack('28B B 2B B', arduino.read(32))
                        print buttonMacro
                        buttonKeys = buttonMacro[:27]
                        buttonAction = buttonMacro[28]
                        if buttonMacro[31] != styraRecordTypeDict['BUTTON_MACRO']:
                            print "Expected BUTTON_MACRO but received %d" % int(buttonMacro[31])
                            continue;
                
                
                print "Download complete"
                time.sleep(5)
                break
            elif n == "x":
                print "Exiting..."
                arduinoReset(arduino)
                running = False
                break
                
                    

    
    
    

        
        
    

    
    
    
    

    
    
    


