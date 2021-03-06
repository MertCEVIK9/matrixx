#include <stdio.h>
#include <linux/types.h>
#include "libfahw-filectl.h"
#include "libfahw-spi.h"
#include "libfahw-oled.h"
#include "libfahw-gpio.h"
#include "common.h"

static int gCmdDatPin = -1;
static int gResetPin = -1;
static unsigned char spiMode = 0;
static unsigned char spiBits = 8;
static unsigned int spiSpeed = 25000000;
static short spiDelay;
// 8 x 16 font
static unsigned char OLEDCharMap[]=
{
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//  0
        0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x30,0x00,0x00,0x00,//! 1
        0x00,0x10,0x0C,0x06,0x10,0x0C,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//" 2
        0x40,0xC0,0x78,0x40,0xC0,0x78,0x40,0x00,0x04,0x3F,0x04,0x04,0x3F,0x04,0x04,0x00,//# 3
        0x00,0x70,0x88,0xFC,0x08,0x30,0x00,0x00,0x00,0x18,0x20,0xFF,0x21,0x1E,0x00,0x00,//$ 4
        0xF0,0x08,0xF0,0x00,0xE0,0x18,0x00,0x00,0x00,0x21,0x1C,0x03,0x1E,0x21,0x1E,0x00,//% 5
        0x00,0xF0,0x08,0x88,0x70,0x00,0x00,0x00,0x1E,0x21,0x23,0x24,0x19,0x27,0x21,0x10,//& 6
        0x10,0x16,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//' 7
        0x00,0x00,0x00,0xE0,0x18,0x04,0x02,0x00,0x00,0x00,0x00,0x07,0x18,0x20,0x40,0x00,//( 8
        0x00,0x02,0x04,0x18,0xE0,0x00,0x00,0x00,0x00,0x40,0x20,0x18,0x07,0x00,0x00,0x00,//) 9
        0x40,0x40,0x80,0xF0,0x80,0x40,0x40,0x00,0x02,0x02,0x01,0x0F,0x01,0x02,0x02,0x00,//* 10
        0x00,0x00,0x00,0xF0,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x1F,0x01,0x01,0x01,0x00,//+ 11
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xB0,0x70,0x00,0x00,0x00,0x00,0x00,//, 12
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,//- 13
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,//. 14
        0x00,0x00,0x00,0x00,0x80,0x60,0x18,0x04,0x00,0x60,0x18,0x06,0x01,0x00,0x00,0x00,/// 15
        0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00,//0 16
        0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//1 17
        0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00,//2 18
        0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00,//3 19
        0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00,//4 20
        0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00,//5 21
        0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00,//6 22
        0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,//7 23
        0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00,//8 24
        0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00,//9 25
        0x00,0x00,0x00,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,//: 26
        0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x60,0x00,0x00,0x00,0x00,//; 27
        0x00,0x00,0x80,0x40,0x20,0x10,0x08,0x00,0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x00,//< 28
        0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,//= 29
        0x00,0x08,0x10,0x20,0x40,0x80,0x00,0x00,0x00,0x20,0x10,0x08,0x04,0x02,0x01,0x00,//> 30
        0x00,0x70,0x48,0x08,0x08,0x08,0xF0,0x00,0x00,0x00,0x00,0x30,0x36,0x01,0x00,0x00,//? 31
        0xC0,0x30,0xC8,0x28,0xE8,0x10,0xE0,0x00,0x07,0x18,0x27,0x24,0x23,0x14,0x0B,0x00,//@ 32
        0x00,0x00,0xC0,0x38,0xE0,0x00,0x00,0x00,0x20,0x3C,0x23,0x02,0x02,0x27,0x38,0x20,//A 33
        0x08,0xF8,0x88,0x88,0x88,0x70,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x11,0x0E,0x00,//B 34
        0xC0,0x30,0x08,0x08,0x08,0x08,0x38,0x00,0x07,0x18,0x20,0x20,0x20,0x10,0x08,0x00,//C 35
        0x08,0xF8,0x08,0x08,0x08,0x10,0xE0,0x00,0x20,0x3F,0x20,0x20,0x20,0x10,0x0F,0x00,//D 36
        0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x20,0x23,0x20,0x18,0x00,//E 37
        0x08,0xF8,0x88,0x88,0xE8,0x08,0x10,0x00,0x20,0x3F,0x20,0x00,0x03,0x00,0x00,0x00,//F 38
        0xC0,0x30,0x08,0x08,0x08,0x38,0x00,0x00,0x07,0x18,0x20,0x20,0x22,0x1E,0x02,0x00,//G 39
        0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20,//H 40
        0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//I 41
        0x00,0x00,0x08,0x08,0xF8,0x08,0x08,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,0x00,//J 42
        0x08,0xF8,0x88,0xC0,0x28,0x18,0x08,0x00,0x20,0x3F,0x20,0x01,0x26,0x38,0x20,0x00,//K 43
        0x08,0xF8,0x08,0x00,0x00,0x00,0x00,0x00,0x20,0x3F,0x20,0x20,0x20,0x20,0x30,0x00,//L 44
        0x08,0xF8,0xF8,0x00,0xF8,0xF8,0x08,0x00,0x20,0x3F,0x00,0x3F,0x00,0x3F,0x20,0x00,//M 45
        0x08,0xF8,0x30,0xC0,0x00,0x08,0xF8,0x08,0x20,0x3F,0x20,0x00,0x07,0x18,0x3F,0x00,//N 46
        0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,0x00,//O 47
        0x08,0xF8,0x08,0x08,0x08,0x08,0xF0,0x00,0x20,0x3F,0x21,0x01,0x01,0x01,0x00,0x00,//P 48
        0xE0,0x10,0x08,0x08,0x08,0x10,0xE0,0x00,0x0F,0x18,0x24,0x24,0x38,0x50,0x4F,0x00,//Q 49
        0x08,0xF8,0x88,0x88,0x88,0x88,0x70,0x00,0x20,0x3F,0x20,0x00,0x03,0x0C,0x30,0x20,//R 50
        0x00,0x70,0x88,0x08,0x08,0x08,0x38,0x00,0x00,0x38,0x20,0x21,0x21,0x22,0x1C,0x00,//S 51
        0x18,0x08,0x08,0xF8,0x08,0x08,0x18,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//T 52
        0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//U 53
        0x08,0x78,0x88,0x00,0x00,0xC8,0x38,0x08,0x00,0x00,0x07,0x38,0x0E,0x01,0x00,0x00,//V 54
        0xF8,0x08,0x00,0xF8,0x00,0x08,0xF8,0x00,0x03,0x3C,0x07,0x00,0x07,0x3C,0x03,0x00,//W 55
        0x08,0x18,0x68,0x80,0x80,0x68,0x18,0x08,0x20,0x30,0x2C,0x03,0x03,0x2C,0x30,0x20,//X 56
        0x08,0x38,0xC8,0x00,0xC8,0x38,0x08,0x00,0x00,0x00,0x20,0x3F,0x20,0x00,0x00,0x00,//Y 57
        0x10,0x08,0x08,0x08,0xC8,0x38,0x08,0x00,0x20,0x38,0x26,0x21,0x20,0x20,0x18,0x00,//Z 58
        0x00,0x00,0x00,0xFE,0x02,0x02,0x02,0x00,0x00,0x00,0x00,0x7F,0x40,0x40,0x40,0x00,//[ 59
        0x00,0x0C,0x30,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x06,0x38,0xC0,0x00,//\ 60
        0x00,0x02,0x02,0x02,0xFE,0x00,0x00,0x00,0x00,0x40,0x40,0x40,0x7F,0x00,0x00,0x00,//] 61
        0x00,0x00,0x04,0x02,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//^ 62
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,//_ 63
        0x00,0x02,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//` 64
        0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x19,0x24,0x22,0x22,0x22,0x3F,0x20,//a 65
        0x08,0xF8,0x00,0x80,0x80,0x00,0x00,0x00,0x00,0x3F,0x11,0x20,0x20,0x11,0x0E,0x00,//b 66
        0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x0E,0x11,0x20,0x20,0x20,0x11,0x00,//c 67
        0x00,0x00,0x00,0x80,0x80,0x88,0xF8,0x00,0x00,0x0E,0x11,0x20,0x20,0x10,0x3F,0x20,//d 68
        0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x22,0x13,0x00,//e 69
        0x00,0x80,0x80,0xF0,0x88,0x88,0x88,0x18,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//f 70
        0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x6B,0x94,0x94,0x94,0x93,0x60,0x00,//g 71
        0x08,0xF8,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//h 72
        0x00,0x80,0x98,0x98,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//i 73
        0x00,0x00,0x00,0x80,0x98,0x98,0x00,0x00,0x00,0xC0,0x80,0x80,0x80,0x7F,0x00,0x00,//j 74
        0x08,0xF8,0x00,0x00,0x80,0x80,0x80,0x00,0x20,0x3F,0x24,0x02,0x2D,0x30,0x20,0x00,//k 75
        0x00,0x08,0x08,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//l 76
        0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x20,0x3F,0x20,0x00,0x3F,0x20,0x00,0x3F,//m 77
        0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x3F,0x21,0x00,0x00,0x20,0x3F,0x20,//n 78
        0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00,//o 79
        0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x00,0x80,0xFF,0xA1,0x20,0x20,0x11,0x0E,0x00,//p 80
        0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x0E,0x11,0x20,0x20,0xA0,0xFF,0x80,//q 81
        0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x20,0x20,0x3F,0x21,0x20,0x00,0x01,0x00,//r 82
        0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x33,0x24,0x24,0x24,0x24,0x19,0x00,//s 83
        0x00,0x80,0x80,0xE0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x1F,0x20,0x20,0x00,0x00,//t 84
        0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20,//u 85
        0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x01,0x0E,0x30,0x08,0x06,0x01,0x00,//v 86
        0x80,0x80,0x00,0x80,0x00,0x80,0x80,0x80,0x0F,0x30,0x0C,0x03,0x0C,0x30,0x0F,0x00,//w 87
        0x00,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x20,0x31,0x2E,0x0E,0x31,0x20,0x00,//x 88
        0x80,0x80,0x80,0x00,0x00,0x80,0x80,0x80,0x80,0x81,0x8E,0x70,0x18,0x06,0x01,0x00,//y 89
        0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00,0x21,0x30,0x2C,0x22,0x21,0x30,0x00,//z 90
        0x00,0x00,0x00,0x00,0x80,0x7C,0x02,0x02,0x00,0x00,0x00,0x00,0x00,0x3F,0x40,0x40,//{ 91
        0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,//| 92
        0x00,0x02,0x02,0x7C,0x80,0x00,0x00,0x00,0x00,0x40,0x40,0x3F,0x00,0x00,0x00,0x00,//} 93
        0x00,0x06,0x01,0x01,0x02,0x02,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//~ 94
};

// write 8 * 8 bitmap
static int OLEDWriteData(int devFD, unsigned char dat)
{       
    int ret = 0;
    unsigned char tx[] = {dat};
    int writeLen = ARRAY_SIZE(tx);
    int pin = gCmdDatPin;

    if (setGPIODirection(pin, GPIO_OUT) == -1) {
        ret = -1;
    }
    if (setGPIOValue(pin, GPIO_HIGH) == -1) {
        ret = -1;
    }       
    if (writeBytesToSPI(devFD, tx, writeLen, spiDelay, spiSpeed, spiBits) == -1) {
        ret = -1;
    }
    return ret;
}

static int OLEDWriteCmd(int devFD, unsigned char cmd)
{
    int ret = 0;
    unsigned char tx[] = {cmd};
    int writeLen = ARRAY_SIZE(tx);
    int pin = gCmdDatPin;

    if (setGPIODirection(pin, GPIO_OUT) == -1) {
        ret = -1;
    }
    if (setGPIOValue(pin, GPIO_LOW) == -1) {
        ret = -1;
    }
    if ((writeBytesToSPI(devFD, tx, writeLen, spiDelay, spiSpeed, spiBits)) == -1) {
        ret = -1;        
    }
    return ret;
}

static int OLEDSetPos(int devFD, int x, int y)
{ 
    int ret = 0;

    y = y/8 + y%8;
    if (OLEDWriteCmd(devFD, 0xb0+y) == -1) {
        ret = -1;
    }
    if (OLEDWriteCmd(devFD, ((x&0xf0)>>4)|0x10) == -1) {
        ret = -1;
    }
    if (OLEDWriteCmd(devFD, (x&0x0f)|0x01) == -1) {
        ret = -1;
    }
    return ret;
} 

static int OLEDReset(int devFD, int pin)
{
    clearLastError();
    int ret = -1;

    if ((ret = setGPIODirection(pin, GPIO_OUT)) == -1) {
        return -1;
    }
    if ((ret = setGPIOValue(pin, GPIO_LOW)) == -1) {
        return -1;
    }    
    usleep(100);
    if ((ret = setGPIOValue(pin, GPIO_HIGH)) == -1) {
        return -1;
    }
    return ret;
}

static int OLEDSPIInit(int devFD)
{
    clearLastError();
    if (setSPIDataMode(devFD, spiMode) == -1 ) {
        return -1;
    }
    if (setSPIWriteBitsPerWord(devFD, spiBits) == -1 ) {
        return -1;
    }
    if (setSPIMaxSpeed(devFD, spiSpeed) == -1 ) {
        return -1;
    }
    return 0;
}

EXPORT int OLEDInit(int cmdDatPin, int resetPin) 
{
    clearLastError();
    int ret = 0;
    int devFD;   
    
    gResetPin = resetPin;
    gCmdDatPin = cmdDatPin;
    if ((devFD = openHW(SPI0_PATH, O_RDWR)) < 0) {
        setLastError("Fail to open SPI device MAX7219");
        return -1;
    }
    if ((ret = OLEDSPIInit(devFD)) == -1) {
        setLastError("Fail to init SPI device OLED");
    }
    if (exportGPIOPin(gResetPin) || exportGPIOPin(gCmdDatPin)) {
        setLastError("Fail to request resetPin or DCPin");
    }
    if ((ret = OLEDReset(devFD, gResetPin)) == -1) {
        setLastError("Fail to reset OLED");
    }

    OLEDWriteCmd(devFD, 0xae);//--turn off oled panel
    OLEDWriteCmd(devFD, 0x00);//---set low column address
    OLEDWriteCmd(devFD, 0x10);//---set high column address
    OLEDWriteCmd(devFD, 0x40);//--set start line address  Set Mapping RAM Display       Start Line (0x00~0x3F)
    OLEDWriteCmd(devFD, 0x81);//--set contrast control register
    OLEDWriteCmd(devFD, 0xcf); // Set SEG Output Current Brightness
    OLEDWriteCmd(devFD, 0xa1);//--Set SEG/Column Mapping     0xa0???????????? 0xa1??????
    OLEDWriteCmd(devFD, 0xc8);//Set COM/Row Scan Direction   0xc0???????????? 0xc8??????
    OLEDWriteCmd(devFD, 0xa6);//--set normal display
    OLEDWriteCmd(devFD, 0xa8);//--set multiplex ratio(1 to 64)
    OLEDWriteCmd(devFD, 0x3f);//--1/64 duty
    OLEDWriteCmd(devFD, 0xd3);//-set display offset Shift Mapping RAM Counter (0x00~0x3F)
    OLEDWriteCmd(devFD, 0x00);//-not offset
    OLEDWriteCmd(devFD, 0xd5);//--set display clock divide ratio/oscillator frequency
    OLEDWriteCmd(devFD, 0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLEDWriteCmd(devFD, 0xd9);//--set pre-charge period
    OLEDWriteCmd(devFD, 0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLEDWriteCmd(devFD, 0xda);//--set com pins hardware configuration
    OLEDWriteCmd(devFD, 0x12);
    OLEDWriteCmd(devFD, 0xdb);//--set vcomh
    OLEDWriteCmd(devFD, 0x40);//Set VCOM Deselect Level
    OLEDWriteCmd(devFD, 0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLEDWriteCmd(devFD, 0x02);//
    OLEDWriteCmd(devFD, 0x8d);//--set Charge Pump enable/disable
    OLEDWriteCmd(devFD, 0x14);//--set(0x10) disable
    OLEDWriteCmd(devFD, 0xa4);// Disable Entire Display On (0xa4/0xa5)
    OLEDWriteCmd(devFD, 0xa6);// Disable Inverse Display On (0xa6/a7) 
    OLEDWriteCmd(devFD, 0xaf);//--turn on oled panel

/*    if (OLEDCleanScreen(devFD) == -1) {
        ret = -1;
    }
*/
    if (OLEDSetPos(devFD, 0, 0) == -1) {
        ret = -1;
    }
    if (ret == 0) {
        return devFD;
    } else {
        unexportGPIOPin(resetPin);
        unexportGPIOPin(cmdDatPin);
        return ret;
    }
}

EXPORT void OLEDDeInit(int devFD)
{
    clearLastError();
    unexportGPIOPin(gCmdDatPin);
    unexportGPIOPin(gResetPin);        
    gCmdDatPin = -1;
    gResetPin = -1;
    closeHW(devFD);    
}

EXPORT int OLEDDisp8x16Char(int devFD, int x, int y, char ch)
{
    clearLastError();
    int i = 0;
    int index = ch-32;           

    if (x<0 || x>(OLED_WIDTH-8) || y<0 || y>(OLED_HEIGHT-16)) {
        setLastError("Unsupported OLED coordinate(%d %d) x:0~%d y:0~%d", x, y, OLED_WIDTH, OLED_HEIGHT);
        return -1;
    }

    // printf("x=%3d, y=%3d\n", x, y);
    OLEDSetPos(devFD, x, y);    
    for (i=0; i<8; i++)     
        OLEDWriteData(devFD,OLEDCharMap[index*16+i]);
    // printf("x=%3d, y=%3d\n", x, y+8);
    OLEDSetPos(devFD, x, y+8);    
    for (i=0; i<8; i++)     
        OLEDWriteData(devFD,OLEDCharMap[index*16+i+8]); 

    return 0;
}

EXPORT int OLEDDisp8x16Str(int devFD, int x, int y, char ch[])
{
    clearLastError();
    int i = 0;
    while (ch[i] != '\0')
    {   
        if (OLEDDisp8x16Char(devFD, x, y, ch[i]) == -1) {
            setLastError("Fail to write 8x16 Char to OLED");
            return -1;
        }
        x+=8;
        i++;
    }
    return 0;
}

EXPORT int OLEDCleanScreen(int devFD)
{
    int x,y;
    unsigned char data = 0x00;
    int ret = 0;
    for (y=0; y<8; y++)
    {
        // set pos (y, 0)
        OLEDWriteCmd(devFD, 0xb0+y);
        OLEDWriteCmd(devFD, 0x00);
        OLEDWriteCmd(devFD, 0x10);
        for (x=0; x<OLED_WIDTH; x++) {
            if (OLEDWriteData(devFD, data) == -1) {
                ret = -1;
                break;
            }
        }
    }
    return ret;
} 
