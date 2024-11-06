#include<stdlib.h>
#include<avr/io.h>

void spi_arduino_init()
{
  //SPI CHIP SELECT
  DDRD |=(1<<PD6);
  DDRD |=(1<<PD7);
  //tutaj zmiana z jakeigos powodu trzrba skontrolowac
  PORTD |=(1<<PD6);
  PORTD |=(1<<PD7);
//SPI MISO
  DDRB &=~(1<<PB4);
//SPI MOSI 
  DDRB|=(1<<PB3);
//SPI SCK
  DDRB|=(1<<PB5);


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

uint8_t SPI_transfer(uint8_t data) {
	SPDR0 = data;
	while (!(SPSR0 & (1 << SPIF)));
	return SPDR0;
}
//spraedzic czy slave wybierany jest za pomoca 0 czy 1?

void can_write(uint8_t reg, uint8_t value){
  PORTD &=~(1<<PD6);
  SPI_transfer(reg & ~0x80);
 _dealy_ms(10);
	SPI_transfer(value);
 _dealy_ms(10);
  PORTD |=(1<<PD6);
}

void can_read(uint8_t reg, void *reg_data, uint8_t len){
  PORTD &=~(1<<PD7);
  SPI_transfer(reg | 0x80);
		while(len--)
		{
			*(uint8_t*)reg_data=SPI_transfer(0x00);
			(uint8_t*)reg_data++;
		}

  PORTD |=(1<<PD7);
}

int main(){
uint8_t read_data;
spi_arduino_init();



  return 0;
}
