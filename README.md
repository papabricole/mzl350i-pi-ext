# mzl350i-pi-ext
Display driver for the Raspberry Pi Tontec MZJD35H-PI-EXT LCD module.

## How it works
It grab the framebuffer and send the bytes to the LCD display.

## Getting Started

### Configure the config.txt file
Edit the framebuffer configuration to set the screen resolution:

    sudo nano /boot/config.txt

and then find the lines:
  
    #framebuffer_width=1280 
    #framebuffer_height=720 

change to:
 
    framebuffer_width=480 
    framebuffer_height=320

### Get the source code and build

    git clone git@github.com:mojocorp/mzl350i-pi-ext.git
    cd mzl350i-pi-ext/src
    make

### Quick test

    ./mzl350i

### Install

    make install

### Uninstall

    make uninstall

## License



