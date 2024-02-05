# -*- coding: utf-8 -*-
"""
/******************************************************************* 
  ProtoStax Arduino Nano 33 BLE Sense RGB LED Control Central Device

  This is a example sketch for controlling the RGB LED on an 
  Arduino Nano 33 BLE Sense with Bluetooth over Python  
   
  Items used:
  Arduino Nano 33 BLE Sense
  ProtoStax for BreadBoard/Custom Boards - 
      - to house and protect the Nano and allow for other circuitry 
      --> https://www.protostax.com/collections/all/products/protostax-for-breadboard
  
  The Nano publishes a Bluetooth LE Client profile with Characteristics for the Red, Green, 
  and Blue components of the onboard RGB LED. These can be read and written to
  control the LED colors.

  This program toggles the R,G,B LEDs based on user input. Run the python program from your computer
  (PC, Mac or Linux) that has Bluetooth support and the requisite python packages - 
  you can then read and set the on/off states of the 3 colors. 
  
  The Red, Green and Blue colors of the onboard RGB LED can only be turned on or off. 
  It is not possible to use PWM to mix colors, unfortunately, based on how the Arduino 
  Nano BLE Sense board is configured.
  
  We write a value of 1 to turn on a color and 0 to turn it off. The user inputs 
  a string that can contain r,g,b (or any combination) and those colors will be toggled. 

  The Arduino Nano 33 BLE Sense is chockful of other sensors - you can similarly expose 
  those sensors data as Characteristics
 
  Written by Sridhar Rajagopal for ProtoStax
  BSD license. All text above must be included in any redistribution
 */
"""


import logging
import asyncio
import platform
import ast
import bleak

from bleak import BleakClient
from bleak import BleakScanner
from bleak import discover

# These values have been randomly generated - they must match between the Central and Peripheral devices
# Any changes you make here must be suitably made in the Arduino program as well

DEVICE_ID = 'EFC4216F-5A11-E3FC-1726-B19E1F430A18'
CHARACTERISTIC_UUID = '19b10001-e8f2-537e-4f6c-d104768a1214'

on_value = bytearray([0x01])
off_value = bytearray([0x00])

GESTURE_NONE  = -1,
GESTURE_UP    = 0,
GESTURE_DOWN  = 1,
GESTURE_LEFT  = 2,
GESTURE_RIGHT = 3

def getValue(on):
    if on:
        return on_value
    else:
        return off_value

async def setCharacteristic(client):
    global CHARACTERISTIC_UUID, DEVICE_ID, GESTURE_UP, GESTURE_DOWN, GESTURE_LEFT, GESTURE_RIGHT, on_value, off_value

    val = input('Enter up, down, left or right: ')
    print(val)

    if ('u' in val):
        await client.write_gatt_char(CHARACTERISTIC_UUID, on_value, response=False)
        val = await client.read_gatt_char(CHARACTERISTIC_UUID)
        print(f'Value: {val}')
    if ('d'in val):
        await client.write_gatt_char(CHARACTERISTIC_UUID, getValue(GESTURE_DOWN), response=False)
    if ('l' in val):
        await client.write_gatt_char(CHARACTERISTIC_UUID, getValue(GESTURE_LEFT), response=False)
    if ('r' in val):
        await client.write_gatt_char(CHARACTERISTIC_UUID, getValue(GESTURE_RIGHT), response=False)
    

async def run():
    global CHARACTERISTIC_UUID, DEVICE_ID

    async with BleakClient('cc:79:98:7e:6a:ae') as client:
        print(f'Connected to {DEVICE_ID}')
        val = await client.read_gatt_char(CHARACTERISTIC_UUID)
        print(f'Value: {val}')
        
        for service in client.services:
            print(f'Service: {service}')
            for char in service.characteristics:
                print(f'Characteristic: {char}')
                cc = char
                # characteristics = await client.read_gatt_char(char.uuid)
                
        # if (val == on_value):
        #     print ('RED ON')
        #     RED = True
        # else:
        #     print ('RED OFF')
        #     RED = False

        write_value = bytearray([0x01])
        await client.write_gatt_char(cc, write_value)
        val = await client.read_gatt_char(CHARACTERISTIC_UUID)
        print(f'Value: {val}')
        # while True:
        #     val = input('Enter up, down, left or right: ')
        #     print(val)

        #     if ('u' in val):
        #         # await client.write_gatt_char(CHARACTERISTIC_UUID, b"0", response=False)
        #     if ('d'in val):
        #         await client.write_gatt_char(CHARACTERISTIC_UUID, b"1", response=False)
        #     if ('l' in val):
        #         await client.write_gatt_char(CHARACTERISTIC_UUID, b"2", response=False)
        #     if ('r' in val):
        #         await client.write_gatt_char(CHARACTERISTIC_UUID, b"3", response=False)
        #     
        #     val = await client.read_gatt_char(CHARACTERISTIC_UUID)
        #     print(f'Value: {val}')

                    
loop = asyncio.get_event_loop()
try:
    loop.run_until_complete(run())
except KeyboardInterrupt:
    print('\nReceived Keyboard Interrupt')
finally:
    print('Program finished')
        