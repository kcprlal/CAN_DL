# CAN_DL
## Tydzień 1:
  - zmontowaliśmy ekipe do wykonania zadania
  - zebraliśmy elementy potrzebne do komunikacji przez magistarlę CAN  
  - na płytce ArduinoUno podłączyliśmy układ umożliwiający komunikację w trybie LOOP-BACK
  - skonfigurowaliśmy komunikację między płytką a modułami PmodCAN poprzez SPI
## Tydzień 2:
  - dodaliśmy funkcję umożliwiającą zczytywanie oraz zapis poprzez SPI
  - zaczynamy prace nad inicjalizacją Pmodów z planem przesłania pierwszych informacji w przyszłym tygodniu
  - zagłębialiśmy się w tematyce konfiguracji oraz działania magistrali CAN
## Tydzień 3:
- napisaliśmy funkcje odpowiedzialne za inicjalizację rejestrów kontrolnych CAN
- zmontowaliśmy układ składający się z dwóch płytek arduino i dwóch płytek pmod
- zczytaliśmy pierwsze informacje za pomocą CAN (losowy ciąg zer i jedynek)
## Tydzień 4:
- Pmody zostały zainicjalizowane i gotowe do działania
- Niestetety napotkaliśmy problemy z wgrywaniem programów do mikrokontrolera Leonarda ;(
## Tydzień 5:
- Zastąpiliśmy płytkę arduino Leonardo mikrokontrolerem ATMega328PB
- Napisaliśmy skrypty dotyczące spi oraz can dla mikrokontrolera, analogicznie do tych dla arduino Leonardo
- Nie udało się przesłać i odczytać informacji między płytkami, planujemy zlokalizować błąd przy pomocy oscyloskopu
