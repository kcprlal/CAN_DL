#include<stdlib.h>
#include<avr/io.h>
#include<util/delay.h>

#define CANCTRL 0x0f
#define CNF1 0x2A
#define CNF2 0x29
#define CNF3 0x28
#define RXB0CNTRL 0x60
#define TXBxCTRL 0x30
#define RTS_TXB0 0x81
#define RTS_TXB1 0x82
#define RTS_TXB2 0x84
#define TXB0_ID_C  0x40  // Buffer 0 - Standard Identifier High Register
#define TXB0_DATA_C  0x41  // Buffer 0 - Data Byte Register
#define TXB1_ID_C  0x42  // Buffer 1 - Standard Identifier High Register
#define TXB1_DATA_C  0x43  // Buffer 1 - Data Byte Register
#define TXB2_ID_C  0x44  // Buffer 2 - Standard Identifier High Register
#define TXB2_DATA_C  0x45 


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

void SPI_write(uint8_t reg, uint8_t value){
  PORTB &=~(1<<PB2);
  SPI_transfer(0x02);
  SPI_transfer(reg);
	SPI_transfer(value);
  PORTB |=(1<<PB2);
}

void SPI_read(uint8_t reg, uint8_t *reg_data, uint8_t len){
  
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
    SPI_write(i, 0);
  }
  for(int i = 0x10; i<=0x1b; i++){
    SPI_write(i, 0);
  }
  for(int i = 0x20; i<=0x27; i++){
    SPI_write(i, 0);
  }
  for(int i = 0x30; i<=0x3D; i++){
    SPI_write(i, 0);
  }
  for(int i = 0x40; i<=0x4d; i++){
    SPI_write(i, 0);
  }
  for(int i = 0x50; i<=0x5d; i++){
    SPI_write(i, 0);
  }
}

void can_init(){
  PORTB &= ~(1 << PB2);
  SPI_write(CANCTRL, 0x80);
  SPI_write(CNF1, 0x41); 
  SPI_write(CNF2, 0xFB);
  SPI_write(CNF3, 0x86);
  can_standard();
  SPI_write(RXB0CNTRL, 0x64);
  SPI_write(CANCTRL, 0x00);
  PORTB |=(1<<PB2);
}

void can_send(uint8_t id, uint8_t data, uint8_t len){
  SPI_Write(TXB0_ID_C, id);
  SPI_Write(TXB0_DATA_C, data);
  SPI_Write(TXB1_ID_C, id);
  SPI_Write(TXB1_DATA_C, data+1);
  SPI_Write(TXB2_ID_C, id);
  SPI_Write(TXB2_DATA_C, data+1);

  PORTB &=~(1<<PB2);
  SPI_transfer(RTS_TXB0);
  PORTB |=(1<<PB2);
}

uint8_t can_receive(uint8_t *data, uint8_t *len) {
    uint8_t status = 0;

    // Sprawdź flagę RXnIF w CANINTF (np. RXB0IF dla bufora RXB0)
    SPI_read(0x2C, &status, 1); // CANINTF
    if (status & 0x01) {        // Sprawdź RXB0IF
        // Odczytaj długość danych
        SPI_read(0x65, len, 1); // RXBnDLC

        // Odczytaj dane
        for (uint8_t i = 0; i < *len; i++) {
            SPI_read(0x66 + i, &data[i], 1); // RXBnD0 do RXBnD7
        }

        // Wyczyść flagę RXB0IF
        SPI_write(0x2C, 0x00); // Wyczyszczenie CANINTF
        return 1; // Wiadomość odebrana
    }
    return 0; // Brak wiadomości
}

int main(){

uint8_t arr=4;
spi_arduino_init();
can_init();
while(1){
  can_send(0x12, arr, 1);
  _delay_ms(10000);
  
}


  return 0;
}
