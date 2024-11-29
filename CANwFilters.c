//po lewej
#include<stdlib.h>
#include<avr/io.h>
#include<util/delay.h>

#define RXFxSIDH0 0x00 // Filtr 0 - RXB0
#define RXFxSIDL0 0x01 // Filtr 0 - RXB0
#define RXFxSIDH1 0x04 // Filtr 1 - RXB0
#define RXFxSIDL1 0x05 // Filtr 1 - RXB0
#define RXFxSIDH2 0x08 // Filtr 2 - RXB1
#define RXFxSIDL2 0x09 // Filtr 2 - RXB1
#define RXFxSIDH3 0x10 // Filtr 3 - RXB1
#define RXFxSIDL3 0x11 // Filtr 3 - RXB1
#define RXFxSIDH4 0x14 // Filtr 4 - RXB1
#define RXFxSIDL4 0x15 // Filtr 4 - RXB1
#define RXFxSIDH5 0x18 // Filtr 5 - RXB1
#define RXFxSIDL5 0x19 // Filtr 5 - RXB1
#define RXM0xSIDH 0x20
#define RXM0xSIDL 0x21
#define RXM1xSIDH 0x24
#define RXM1xSIDL 0x25
#define CNF3 0x28
#define CNF2 0x29
#define CNF1 0x2A
#define CANCTRL 0x0F
#define CANINTF 0x2C
#define TXBxCTRL 0x30
#define TXB0_SIDH 0x31 // Standard Identifier High Register
#define TXB0_SIDL 0x32
#define TXB0DLC 0x35
#define TXB0_DATA 0x36
#define TXB1_SIDH 0x41 // Standard Identifier High Register
#define TXB1_SIDL 0x42
#define TXB1DLC 0x45
#define TXB1_DATA 0x46
#define TXB2_SIDH 0x51 // Standard Identifier High Register
#define TXB2_SIDL 0x52
#define TXB2DLC 0x55
#define TXB2_DATA 0x56
#define RXB0CNTRL 0x60
#define RXB0xSIDH 0x61
#define RXB0xSIDL 0x62
#define RXB0DLC 0x65
#define RXB0xDn 0x66 // Receive buffer może być od 0x66 do 0x6D
#define RXB1CTRL 0x70
#define RXB1xSIDH 0x71
#define RXB1xSIDL 0x72
#define RXB1DLC 0x75
#define RXB1xDn 0x76 // Receive buffer może być od 0x76 do 0x76
#define RTS_TXB0 0x81
#define RTS_TXB1 0x82
#define RTS_TXB2 0x84

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
			 *reg_data = SPI_transfer(0x00); // wysylamy cokolwiek zeby rejestr sie przesunal
      reg_data = reg_data + 1;        // Increment as uint8_t*
		}

  PORTB |=(1<<PB2);
}

void SPI_modify(uint8_t reg, uint8_t value, uint8_t mask){
  PORTB &=~(1<<PB2);
  SPI_transfer(0x05);
  SPI_transfer(reg);
  SPI_transfer(mask); //tam gdzie sa 1 rejestr zostaje modyfikowany
  SPI_transfer(value);
  PORTB |=(1<<PB2);
}
//NIE UZYWAC NA RAZIE BO TO JAKIS DZIWNY TRYB
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
  SPI_write(CNF1, 0x00);     // BRP = 0, SJW = 1 TQ
  SPI_write(CNF2, 0xB3);     // PROPSEG = 7 TQ, PHSEG1 = 4 TQ, BTLMODE = 1
  SPI_write(CNF3, 0x03);
  //jezeli ustawie konkretne id to pomimo filtra i maski przyjmie tylko
  //wiadomosci z ponizszymi id
  SPI_write(RXB0xSIDH, 0xaa); //ustaw id receive buffera
  SPI_write(RXB0xSIDL, 0x00);
  SPI_write(RXB1xSIDH, 0x12);
  SPI_write(RXB1xSIDL, 0x00);
  //can_standard();
  //jezeli chcemy zrobic rollover z rejestrow BUKT (2bit musi byc ustawiony na 1)
  //pozwala to po przepelnieniu rxb0 wpisac do rxb1
  //filtr z rxb0 zostanie zastosowany w rxb1(chyba ale tak na 90%)
  //SPI_write(RXB0CNTRL, 0x64);//przyjmuje wszystkie wiadomosci
  SPI_write(RXB0CNTRL, 0x04);//filtry dzialaja
  SPI_write(RXB1CTRL, 0x04);//filtry dzialaja
  SPI_write(CANCTRL, 0x00);//normal
  //SPI_write(CANCTRL, 0b01000000); // loopback
  SPI_write(CANINTF, 0);
  PORTB |=(1<<PB2);
}

void can_send0(uint8_t id, uint8_t data, uint8_t len){
  SPI_write(TXBxCTRL, 0);
  SPI_write(TXB0_SIDH, id);
  SPI_write(TXB0_SIDL, 0);
  SPI_write(TXB0_DATA, data);
  SPI_write(TXB0DLC, len & 0x0f);
  PORTB &=~(1<<PB2);
  SPI_transfer(RTS_TXB0); //tutaj wysyla txb0
  PORTB |=(1<<PB2);
}

void can_send1(uint8_t id, uint8_t data, uint8_t len){
  SPI_write(TXBxCTRL, 0);
  SPI_write(TXB1_SIDH, id);
  SPI_write(TXB1_SIDL, 0);
  SPI_write(TXB1_DATA, data);
  SPI_write(TXB1DLC, len & 0x0f);
  PORTB &=~(1<<PB2);
  SPI_transfer(RTS_TXB1); //tutaj wysyla txb1
  PORTB |=(1<<PB2);
}

void can_send2(uint8_t id, uint8_t data, uint8_t len){
  SPI_write(TXBxCTRL, 0);
  SPI_write(TXB2_SIDH, id);
  SPI_write(TXB2_SIDL, 0);
  SPI_write(TXB2_DATA, data);
  SPI_write(TXB2DLC, len & 0x0f);
  PORTB &=~(1<<PB2);
  SPI_transfer(RTS_TXB2); //tutaj wysyla txb2
  PORTB |=(1<<PB2);
}

//domyslny bufor
void can_receive0(uint8_t *data, uint8_t len) {
    SPI_read(RXB0xDn, data, len);
    SPI_modify(CANINTF, 0, 0x01); 
}

void can_receive1(uint8_t *data, uint8_t len) {
    SPI_read(RXB1xDn, data, len);
    SPI_modify(CANINTF, 0, 0x02); 
}
//funkcje dla identyfikatorow 8 bitowych, do dzialania programu na labie wystarczy
//ustaw 1 dla bitow znaczacych
void can_mask_set(uint8_t sidh){
  SPI_write(CANCTRL, 0x80);
  SPI_write(RXM0xSIDH, sidh);
  SPI_write(RXM0xSIDL, 0x00);
  SPI_write(CANCTRL, 0x00);
}
//ustawia filtr i jest on traktowany zgodnie z maska
void can_filter_set(uint8_t sidh){
  SPI_write(CANCTRL, 0x80);
  SPI_write(RXFxSIDH0, sidh); //domyslnie ustawiony w can int
  SPI_write(RXFxSIDL0, 0x00);
  SPI_write(RXFxSIDH1, sidh); //domyslnie ustawiony w can int
  SPI_write(RXFxSIDL1, 0x00);
  SPI_write(CANCTRL, 0x00);
}

int main(){
DDRD &=~(1<<PD0);

    spi_arduino_init();         // Inicjalizacja SPI
    can_init();                 // Inicjalizacja kontrolera CAN
uint8_t data = 0;
  while(1){
    can_send0(0x12, data, 1);    // Wyślij wiadomość ID=0x12, Data=0xAA, Len=1
    _delay_ms(2000); 
    data++;
    can_send0(0x12, 0xff, 1);
    _delay_ms(2000);
    can_send0(0x88, 0xaa, 1);
    _delay_ms(2000);
  }

    /*uint8_t canintf;
    SPI_read(CANINTF, &canintf, 1); // Sprawdzenie flagi przerwań
    if (canintf & 0x01) {           // Jeśli RX0IF ustawione
        can_receive(&data, 1);         // Odbierz wiadomość
    }

    PORTD = data; */                 // Wyświetl odebrane dane na PORTD
    return 0;
}
