#include "Adafruit_EPD.h"
#include "Adafruit_IL0373.h"

#define BUSY_WAIT 500

#ifdef USE_EXTERNAL_SRAM
Adafruit_IL0373::Adafruit_IL0373(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO, int8_t BUSY) : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY){
#else
Adafruit_IL0373::Adafruit_IL0373(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, BUSY) {
	bw_buf = (uint8_t *)malloc(width * height / 8);
	red_buf = (uint8_t *)malloc(width * height / 8);
#endif
	bw_bufsize = width * height / 8;
	red_bufsize = bw_bufsize;
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
#ifdef USE_EXTERNAL_SRAM
Adafruit_IL0373::Adafruit_IL0373(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t SRCS, int8_t BUSY) : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY) {
#else
Adafruit_IL0373::Adafruit_IL0373(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EPD(width, height, DC, RST, CS, BUSY) {
	bw_buf = (uint8_t *)malloc(width * height / 8);
	red_buf = (uint8_t *)malloc(width * height / 8);
#endif
	bw_bufsize = width * height / 8;
	red_bufsize = bw_bufsize;
}

void Adafruit_IL0373::busy_wait(void)
{
	if(busy > -1)
		while(digitalRead(busy)); //wait for busy low
	else
		delay(BUSY_WAIT);
}

void Adafruit_IL0373::begin(bool reset)
{
	uint8_t buf[5];
	Adafruit_EPD::begin(reset);
			
	buf[0] = 0x07;
	buf[1] = 0x00;
	buf[2] = 0x0A;
	buf[3] = 0x00;
	EPD_command(IL0373_POWER_SETTING, buf, 4);
		
	buf[0] = 0x07;
	buf[1] = 0x07;
	buf[2] = 0x07;
	EPD_command(IL0373_BOOSTER_SOFT_START, buf, 3);
}

void Adafruit_IL0373::update()
{
	EPD_command(IL0373_DISPLAY_REFRESH);
	
	busy_wait();
	
	delay(10000);
	
	//power off
	uint8_t buf[4];
	
	buf[0] = 0x17;
	EPD_command(IL0373_CDI, buf, 1);
	
	buf[0] = 0x00;
	EPD_command(IL0373_VCM_DC_SETTING, buf, 0);
	
	buf[0] = 0x02;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	EPD_command(IL0373_POWER_SETTING);
	
	EPD_command(IL0373_POWER_OFF);
	
	delay(10000);
}

void Adafruit_IL0373::powerUp()
{
	uint8_t buf[3];
	 
	EPD_command(IL0373_POWER_ON);
	busy_wait();
	delay(200);
	
	buf[0] = 0xCF;
	EPD_command(IL0373_PANEL_SETTING, buf, 1);
	
	buf[0] = 0x37;
	EPD_command(IL0373_CDI, buf, 1);
	
	buf[0] = 0x29;
	EPD_command(IL0373_PLL, buf, 1);

	buf[0] = height() & 0xFF;
	buf[1] = (height() >> 8) & 0xFF;
	buf[2] = width() & 0xFF;
	buf[3] = (width() >> 8) & 0xFF;
	EPD_command(IL0373_RESOLUTION, buf, 4);
			
	buf[0] = 0x0A;
	EPD_command(IL0373_VCM_DC_SETTING, buf, 1);
	delay(20);
}

void Adafruit_IL0373::display()
{
	powerUp();
	
#ifdef USE_EXTERNAL_SRAM
	uint8_t c;
	
	sram.csLow();
	//send read command
	fastSPIwrite(K640_READ);
	
	//send address
	fastSPIwrite(0x00);
	fastSPIwrite(0x00);
	
	//first data byte from SRAM will be transfered in at the same time as the EPD command is transferred out
	c = EPD_command(EPD_RAM_BW, false);
	
	dcHigh();
	
	for(uint16_t i=0; i<bw_bufsize; i++){
		c = fastSPIwrite(c);
	}
	csHigh();
	sram.csHigh();
	
	delay(2);
	
	sram.csLow();
	//send write command
	fastSPIwrite(K640_READ);
	
	uint8_t b[2];
	b[0] = (bw_bufsize >> 8);
	b[1] = (bw_bufsize & 0xFF);
	//send address
	fastSPIwrite(b[0]);
	fastSPIwrite(b[1]);
	
	//first data byte from SRAM will be transfered in at the same time as the EPD command is transferred out
	c = EPD_command(EPD_RAM_RED, false);
	
	dcHigh();
	
	for(uint16_t i=0; i<red_bufsize; i++){
		c = fastSPIwrite(c);
	}
	csHigh();
	sram.csHigh();
	
#else
	//write image
	EPD_command(EPD_RAM_BW, false);
	dcHigh();

	for(uint16_t i=0; i<bw_bufsize; i++){
		fastSPIwrite(bw_buf[i]);
	}
	csHigh();
	
	EPD_command(EPD_RAM_RED, false);
	dcHigh();
		
	for(uint16_t i=0; i<red_bufsize; i++){
		fastSPIwrite(red_buf[i]);
	}
	csHigh();

#endif
	update();
}
		
void Adafruit_IL0373::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
	return;
	
	uint8_t *pBuf;

	// check rotation, move pixel around if necessary
	switch (getRotation()) {
		case 1:
		EPD_swap(x, y);
		x = WIDTH - x - 1;
		break;
		case 2:
		x = WIDTH - x - 1;
		y = HEIGHT - y - 1;
		break;
		case 3:
		EPD_swap(x, y);
		y = HEIGHT - y - 1;
		break;
	}
	//make our buffer happy
	x = (x == 0 ? 1 : x);

	uint16_t addr = ( (width() - x) * height() + y)/8;

#ifdef USE_EXTERNAL_SRAM
	if(color == EPD_RED){
		//red is written after bw
		addr = addr + bw_bufsize;
	}
	uint8_t c = sram.read8(addr);
	pBuf = &c;
#else
	if(color == EPD_RED){
		pBuf = red_buf + addr;
	}
	else{
		pBuf = bw_buf + addr;
	}
#endif
	// x is which column
	switch (color)
	{
		case EPD_WHITE:   *pBuf |= (1 << (7 - y%8)); break;
		case EPD_RED:
		case EPD_BLACK:   *pBuf &= ~(1 << (7 - y%8)); break;
		case EPD_INVERSE: *pBuf ^= (1 << (7 - y%8)); break;
	}
#ifdef USE_EXTERNAL_SRAM
	sram.write8(addr, *pBuf);
#endif
	
}

void Adafruit_IL0373::clearBuffer()
{
#ifdef USE_EXTERNAL_SRAM
	sram.erase(0x00, bw_bufsize + red_bufsize, 0xFF);
#else
	memset(bw_buf, 0xFF, bw_bufsize);
	memset(red_buf, 0xFF, red_bufsize);
#endif
}

void Adafruit_IL0373::clearDisplay() {
	clearBuffer();
	display();
	delay(100);
	display();
}