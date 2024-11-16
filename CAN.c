#include<stdlib.h>
#include<avr/io.h>
#include<util/delay.h>

#define CANCTRL 0x0f
#define CNF1 0x2A
#define CNF2 0x29
#define CNF3 0x28
#define RXB0CNTRL 0x60

void spi_arduino_init()
{
  //SPI CHIP SELECT
  DDRB |=(1<<PB2);
  //tutaj zmiana z jakeigos powodu trzrba skontrolowac
  PORTB |=(1<<PB2);
//SPI MISO
  DDRB &=~(1<<PB4);
//SPI MOSI 
  DDRB|=(1<<PB3);
//SPI SCK
  DDRB|=(1<<PB5);


//wybor f zegara
  SPCR &=~(1<<SPR1);
  SPCR |=(1<<SPR0);
//polarity i faza zegara
  SPCR &=~(1<<CPOL);
  SPCR &=~(1<<CPHA);
//master
  SPCR |=(1<<MSTR);
//data order MSB
  SPCR &=~(1<<DORD);
//spi enable
  SPCR |=(1<<SPE);
//przerwania moze pozniej 
}

uint8_t SPI_transfer(uint8_t data) {
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}
//spraedzic czy slave wybierany jest za pomoca 0 czy 1?

void can_write(uint8_t reg, uint8_t value){
  PORTB &=~(1<<PB2);
  SPI_transfer(0x02);
  SPI_transfer(reg);
	SPI_transfer(value);
  PORTB |=(1<<PB2);
}

void can_read(uint8_t reg, void *reg_data, uint8_t len){
  
  PORTB &=~(1<<PB2);
  SPI_transfer(0x03);
  SPI_transfer(reg);

		while(len--)
		{
			 *(uint8_t*)reg_data = SPI_transfer(0x00); // Cast to uint8_t*
        reg_data = (uint8_t*)reg_data + 1;        // Increment as uint8_t*
        _delay_ms(10);
		}

  PORTB |=(1<<PB2);
}

void can_standard(){
  for(int i = 0; i<=0x0b; i++){
    can_write(i, 0);
  }
  for(int i = 0x10; i<=0x1b; i++){
    can_write(i, 0);
  }
  for(int i = 0x20; i<=0x27; i++){
    can_write(i, 0);
  }
  for(int i = 0x30; i<=0x3D; i++){
    can_write(i, 0);
  }
  for(int i = 0x40; i<=0x4d; i++){
    can_write(i, 0);
  }
  for(int i = 0x50; i<=0x5d; i++){
    can_write(i, 0);
  }
}

void can_init(){
  PORTB &= ~(1 << PB2);
  can_write(CANCTRL, 0x80);
  can_write(CNF1, 0x41); 
  can_write(CNF2, 0xFB);
  can_write(CNF3, 0x86);
  can_standard();
  can_write(RXB0CNTRL, 0x64);
  can_write(CANCTRL, 0x00);
  PORTB |=(1<<PB2);
}

int main(){

DDRD |=(1<<PD2);
PORTD &= ~(1 << PD2);

uint8_t canctrl;
spi_arduino_init();
can_init();
can_read(CANCTRL, &canctrl, 1);
_delay_ms(100);
 if ((canctrl & 0xE0) == 0x00) { // 000 dla trybu normalnego
        PORTD |= (1 << PD2);        // Ustawienie PD2 na wysoki
    } else {
        PORTD &= ~(1 << PD2);       // Ustawienie PD2 na niski
    }

  return 0;
}
