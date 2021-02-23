# This file is executed on every boot (including wake-boot from deepsleep)

import esp # esp32 specific, replace with your machines lib
import os

#esp.osdebug(None)

#import webrepl
import machine
import network
import upip

machine.freq(240000000) # specific to esp32, change to whatever you machine has max frequency

wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect('SSID', 'password')

#webrepl.start()
