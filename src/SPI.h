#ifndef _SPI_H_
#define _SPI_H_

#include <cstdint>
#include <stdint.h>

#define NULL 0


class SPIClass{
	public:
		SPIClass();
		SPIClass(int8_t CS, // todo, figure out if this should move
				uint8_t spi_clock, 
				uint8_t spi_miso, 
				uint8_t spi_mosi,
				uint32_t freq,               // frequency
				uint8_t SPI_MODE              // data mode
 		);

		SPIClass(int8_t CS, // todo, figure out if this should move
				uint32_t freq,               // frequency
				SPIClass * spi              // data mode
 		);
		bool begin();
		bool transfer(uint8_t d);
		void setClockDivider(uint8_t d);

		void endTransaction();
		void beginTransaction();
};

SPIClass SPI;
 
#endif
