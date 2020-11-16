#ifndef _GPIO_H_
#define _GPIO_H_

#include <cstdint>

enum IOMODE {
	OUTPUT,
	INPUT
};

#define LOW 0
#define HIGH 1
#define SPI_MODE0 0

#define delay(a)  // todo, change to RTOS config

void pinMode(int p, IOMODE mode) {} // todo, change to port->pin
void digitalWrite(int p, bool mode) {}  // todo, change to port->pin
uint8_t digitalRead(int p) {}  // todo, change to port->pin
#endif
