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

void can_read(uint8_t reg, uint8_t *reg_data, uint8_t len){
  
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

void can_send(uint8_t id, uint8_t *data, uint8_t len){
  can_write(0x31, (id>>3)); //SIDH
  can_write(0x32, (id<<5));

  for (uint8_t i = 0; i < len; i++) {
        can_write(0x36 + i, data[i]); // Rejestry TXBnD0 do TXBnD7
    }

    // Ustaw długość danych
    can_write(0x35, len); // Rejestr TXBnDLC

    // Zainicjuj transmisję
    can_write(0x30, 0x08); // TXREQ - Rozpocznij nadawanie
}

uint8_t can_receive(uint8_t *data, uint8_t *len) {
    uint8_t status = 0;

    // Sprawdź flagę RXnIF w CANINTF (np. RXB0IF dla bufora RXB0)
    can_read(0x2C, &status, 1); // CANINTF
    if (status & 0x01) {        // Sprawdź RXB0IF
        // Odczytaj długość danych
        can_read(0x65, len, 1); // RXBnDLC

        // Odczytaj dane
        for (uint8_t i = 0; i < *len; i++) {
            can_read(0x66 + i, &data[i], 1); // RXBnD0 do RXBnD7
        }

        // Wyczyść flagę RXB0IF
        can_write(0x2C, 0x00); // Wyczyszczenie CANINTF
        return 1; // Wiadomość odebrana
    }
    return 0; // Brak wiadomości
}

int main(){

uint8_t arr=4;
spi_arduino_init();
can_init();
while(1){
  can_send(0x123, arr, 1);
  _delay_ms(10000);
  
}


  return 0;
}
