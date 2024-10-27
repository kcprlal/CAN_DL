#include<stdlib.h>
#include<avr/io.h>
#include <util/delay.h>

#define SS0 7
#define SS1 6

void spi_arduino_init()
{
//SPI CHIP SELECT
  DDRD |=(1<<6);
  DDRD |=(1<<7);
  PORTD &=(0<<6);
  PORTD &=(0<<7);
//SPI MISO
  DDRB &=(0<<4);
//SPI MOSI 
  DDRB|=(1<<3);
//SPI SCK
  DDRB|=(1<<5);


//wybor f zegara
  SPCR &=(0<<SPR1);
  SPCR |=(1<<SPR0);
//polarity i faza zegara
  SPCR &= (0<<CPOL);
  SPCR &= (0<<CPHA);
//master
  SPCR |=(1<<MSTR);
//data order LSB
  SPCR |=(1<<DORD);
//spi enable
  SPCR |=(1<<SPE);
//przerwania moze pozniej 
}
void spi_CAN_init_1()
{
  CAN_writereg()

}
uint8_t SPI_transfer(uint8_t data) {
	SPDR0 = data;
	while (!(SPSR0 & (1 << SPIF)));
	return SPDR0;
}
void CAN_writereg(uint8_t reg, uint8_t value) {
	PORTB &= ~(1 << SS0);
	SPI_transfer(reg & ~0x80);
 _dealy_ms(10);
	SPI_transfer(value);
 _dealy_ms(10);
	PORTB |= (1 << SS0);
}
void can_write()
{
  PORTD |=(1<<6);
  


  PORTD &=(0<<6);
}

void can_read(){
  PORTD |=(1<<7);

  PORTD &=(0<<7);
}

int main(){
spi_arduino_init();
spi_CAN_init_1()


  return 0;
}
