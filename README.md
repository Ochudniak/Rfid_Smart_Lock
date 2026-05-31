# Elektroniczny zamek RFID z kodem PIN

Projekt przedstawia elektroniczny zamek sterowany kodem PIN wpisywanym joystickiem oraz kartą RFID. Po wpisaniu poprawnego kodu serwomechanizm otwiera zamek, zapala się zielona dioda LED i odtwarzana jest krótka melodyjka. Po wpisaniu błędnego kodu uruchamia się alarm dźwiękowy oraz czerwona dioda LED. Alarm działa do momentu przyłożenia autoryzowanej karty RFID.

Projekt został przygotowany jako projekt edukacyjny na Arduino.

## Licencja

Projekt udostępniony jest na licencji **Creative Commons Zero 1.0 Universal (CC0)**. Oznacza to, że kod, instrukcję i dokumentację można kopiować, modyfikować oraz rozwijać bez pytania autora o zgodę.

## Autor

Patryk Zaremba

## Funkcje projektu

- wpisywanie kodu PIN za pomocą joysticka,
- wyświetlanie statusu i wpisywanego kodu na ekranie LCD 16x2,
- otwieranie zamka serwomechanizmem po poprawnym kodzie,
- sygnalizacja poprawnego kodu zieloną diodą LED,
- melodyjka po poprawnym kodzie,
- alarm po błędnym kodzie,
- sygnalizacja błędnego kodu czerwoną diodą LED,
- wyłączenie alarmu za pomocą autoryzowanej karty RFID,
- ponowne zamknięcie zamka przyciskiem joysticka.

## Elementy

- Arduino UNO,
- LCD 16x2 bez I2C,
- potencjometr 10kΩ do kontrastu LCD,
- czytnik RFID RC522,
- karta lub brelok RFID,
- joystick analogowy z przyciskiem,
- serwomechanizm,
- buzzer,
- zielona dioda LED,
- czerwona dioda LED,
- rezystory do diod LED,
- przewody,
- płytka stykowa,
- kartonowa obudowa / model drzwi.

## Schemat podłączenia

Szczegółowy opis podłączenia znajduje się w pliku:

[`docs/wiring.md`](docs/wiring.md)

Najważniejsze piny:

| Element | Pin Arduino |
|---|---|
| LCD RS | D4 |
| LCD E | D5 |
| LCD D4 | D6 |
| LCD D5 | D7 |
| LCD D6 | A2 |
| LCD D7 | A3 |
| Zielona LED | D2 |
| Czerwona LED | A5 |
| Buzzer | D8 |
| Serwo | D9 |
| RFID SDA/SS | D10 |
| RFID RST | D3 |
| RFID MOSI | D11 |
| RFID MISO | D12 |
| RFID SCK | D13 |
| Joystick X | A1 |
| Joystick Y | A4 |
| Joystick SW | A0 |

## Biblioteki

W Arduino IDE potrzebne są biblioteki:

- `LiquidCrystal` — standardowa biblioteka Arduino,
- `SPI` — standardowa biblioteka Arduino,
- `Servo` — standardowa biblioteka Arduino,
- `MFRC522` — biblioteka do czytnika RFID RC522.

Bibliotekę `MFRC522` można zainstalować przez:

**Arduino IDE → Library Manager → wyszukaj: MFRC522 → Install**

## Instrukcja wykonania krok po kroku

### 1. Przygotowanie LCD

1. Podłącz LCD zgodnie z tabelą w `docs/wiring.md`.
2. Podłącz pin `VO/V0` LCD do środkowej nóżki potencjometru.
3. Skrajne nóżki potencjometru podłącz do `5V` i `GND`.
4. Piny podświetlenia LCD:
   - `BLA` / `LED+` do `5V` przez rezystor,
   - `BLK` / `LED-` do `GND`.
5. Uruchom program i wyreguluj kontrast potencjometrem.

### 2. Podłączenie RFID RC522

1. Podłącz czytnik RFID do pinów SPI Arduino.
2. Zasilanie czytnika RC522 podłącz do **3.3V**, nie do 5V.
3. Pin `IRQ` zostaw niepodłączony.
4. Masa czytnika musi być połączona z masą Arduino.

### 3. Podłączenie joysticka

1. Podłącz `VRx` joysticka do `A1`.
2. Podłącz `VRy` joysticka do `A4`.
3. Podłącz `SW` joysticka do `A0`.
4. W kodzie przycisk joysticka działa z `INPUT_PULLUP`, więc wciśnięcie daje stan `LOW`.

### 4. Podłączenie LED, buzzera i serwa

1. Zieloną diodę LED podłącz do `D2` przez rezystor.
2. Czerwoną diodę LED podłącz do `A5` przez rezystor.
3. Buzzer podłącz do `D8` i `GND`.
4. Serwo:
   - przewód sygnałowy do `D9`,
   - czerwony do `5V`,
   - brązowy/czarny do `GND`.
5. Jeżeli serwo resetuje Arduino, zasil serwo z osobnego 5V i połącz masę z masą Arduino.

### 5. Wgranie programu

1. Otwórz plik `src/RFID_Smart_Lock/RFID_Smart_Lock.ino` w Arduino IDE.
2. Wybierz płytkę **Arduino UNO**.
3. Wybierz poprawny port COM.
4. Wgraj program.

### 6. Obsługa

1. Po uruchomieniu ekran jest wygaszony.
2. Naciśnij joystick, aby rozpocząć wpisywanie kodu.
3. Wychylenie joysticka w prawo/lewo zmienia wybraną cyfrę.
4. Wciśnięcie joysticka dodaje cyfrę.
5. Wychylenie joysticka w dół przełącza tryb usuwania.
6. Wychylenie joysticka w górę wraca do trybu dodawania.
7. Poprawny kod `12345` otwiera zamek.
8. Błędny kod uruchamia alarm.
9. Alarm można wyłączyć tylko autoryzowaną kartą RFID.
10. Po otwarciu naciśnij joystick, aby zamknąć zamek.

## Hasło i karta RFID

Domyślne hasło w kodzie:

```cpp
const char correctPassword[] = "12345";
```

Domyślna karta RFID:

```cpp
byte allowedUID[] = {0xD6, 0x02, 0xF6, 0x05};
```

Jeżeli używasz innej karty, odczytaj jej UID i podmień wartości w tablicy `allowedUID`.

## Wymagania zadania — kontrola

Projekt spełnia warunki widoczne w instrukcji:

- posiada kod źródłowy gotowy do umieszczenia w publicznym repozytorium,
- zawiera instrukcję wykonania krok po kroku,
- zawiera licencję CC0,
- projekt jest edukacyjny i możliwy do rozwijania przez innych uczniów,
- dokumentacja opisuje elementy, podłączenie i obsługę,
- program zawiera komentarze opisujące działanie najważniejszych bloków.

## Co pokazać na filmie / prezentacji

Na nagraniu warto pokazać:

1. kartkę z imieniem, nazwiskiem i numerem indeksu,
2. cały układ elektroniczny,
3. działanie LCD,
4. wpisanie błędnego kodu i uruchomienie alarmu,
5. wyłączenie alarmu kartą RFID,
6. wpisanie poprawnego kodu,
7. otwarcie zamka serwem,
8. ponowne zamknięcie zamka.

## Uwagi techniczne

- RC522 musi być zasilany z 3.3V.
- Wszystkie elementy muszą mieć wspólne GND.
- Jeżeli LCD pokazuje słabo tekst, ustaw kontrast potencjometrem.
- Jeżeli serwo powoduje reset Arduino, użyj osobnego zasilania 5V dla serwa.
