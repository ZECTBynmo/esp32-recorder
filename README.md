# ESP32 Recorder

## Deps

Setup tools

git clone --recursive https://github.com/espressif/esp-idf
cd esp-idf
./install.sh

add source ~/projects/esp-idf/export.sh to zsh

## Setup project

idf.py set-target esp32
idf.py build

## Build and flash

Find usb device
ls /dev/tty.*

idf.py -p /dev/tty.usbserial-0001 flash
idf.py -p /dev/tty.usbserial-0001 monitor

(quitting monitor is ctrl + ])