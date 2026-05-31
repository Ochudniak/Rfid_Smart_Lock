/*
  Elektroniczny zamek RFID z kodem PIN
  Autor: Patryk Zaremba
  Licencja: CC0 1.0 Universal

  Wykonane elementy:
  - LCD 16x2 pokazuje stan zamka i wpisywany kod
  - joystick pozwala wybierac cyfry oraz tryb dodawania/usuwania
  - poprawny kod PIN otwiera zamek serwem
  - bledny kod wlacza alarm buzzerem i czerwona diode
  - autoryzowana karta RFID wylacza alarm
  - zielona dioda sygnalizuje otwarcie zamka
  - po otwarciu mozna ponownie zamknac zamek przyciskiem joysticka

  Piny:
  LCD: RS=D4, E=D5, D4=D6, D5=D7, D6=A2, D7=A3
  LED zielona=D2, LED czerwona=A5, buzzer=D8
  RFID RC522: SDA/SS=D10, RST=D3, MOSI=D11, MISO=D12, SCK=D13
  Joystick: X=A1, Y=A4, SW=A0
  Serwo: D9
*/

#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <string.h>

// LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(4, 5, 6, 7, A2, A3);

// Piny elementow wykonawczych i wejsc
#define LED_GREEN 2
#define LED_RED A5
#define BUZZER_PIN 8
#define RFID_SS_PIN 10
#define RFID_RST_PIN 3
#define JOY_X A1
#define JOY_Y A4
#define JOY_SW A0
#define SERVO_PIN 9

// Obiekty bibliotek
Servo lockServo;
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);

// Haslo i karta dostepu
const char correctPassword[] = "12345";
const int passwordLength = 5;
byte allowedUID[] = {0xD6, 0x02, 0xF6, 0x05};

// Pozycje serwa
const int position_locked = 0;
const int position_unlocked = 90;

// Nuty do melodyjki po dobrym kodzie
const int c4 = 261;
const int d4 = 294;
const int e4 = 330;
const int f4 = 349;
const int g4 = 392;
const int a4 = 440;
const int b4 = 494;
const int c5 = 523;
const int d5 = 587;
const int e5 = 659;
const int f5 = 698;
const int g5 = 784;
const int a5 = 880;
const int rest = 0;

// Prosta melodyjka sukcesu
int melody[] = {
  f4, a4, c5, f5, e5, d5,
  c5, d5, e5, f5, d5, c5, a4,
  rest, a4, f4, g4, a4
};

int durations[] = {
  4, 4, 2, 2, 8, 8,
  4, 8, 8, 4, 4, 4, 2,
  2, 4, 4, 4, 1
};

// Zmienne do wpisywania kodu
int num = 0;
int num_pos = 0;
char enteredPassword[6] = "";

// Linie wyswietlane na LCD
char lineDel[17] = "LOCKED       DEL";
char lineAdd[17] = "LOCKED       ADD";

// Flagi stanu programu
bool del = false;
bool unlocked = false;

// Sprawdzenie czy przylozona karta jest dozwolona
bool cardOk() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return false;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return false;
  }

  // Jezeli UID ma inny rozmiar, karta jest odrzucana
  if (rfid.uid.size != 4) {
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return false;
  }

  // Porownanie UID karty z zapisanym UID
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != allowedUID[i]) {
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
      return false;
    }
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  return true;
}

// Krotki sygnal dzwiekowy
void beep(int frequency, int durationMs) {
  tone(BUZZER_PIN, frequency);
  delay(durationMs);
  noTone(BUZZER_PIN);
}

// Wypisanie dwoch linii na LCD
void lcdText(const char* firstLine, const char* secondLine) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(firstLine);
  lcd.setCursor(0, 1);
  lcd.print(secondLine);
}

// Zamiana liczby 0-9 na znak cyfry
char numChoose(int value) {
  if (value < 0) {
    value = 0;
  }

  if (value > 9) {
    value = 9;
  }

  return '0' + value;
}

// Odczyt joysticka
void readJoystick(int &x, int &y, int &sw) {
  x = analogRead(JOY_X);
  y = analogRead(JOY_Y);
  sw = digitalRead(JOY_SW);
}

// Wyczyszczenie wpisanego hasla
void clearPassword() {
  for (int i = 0; i < 6; i++) {
    enteredPassword[i] = '\0';
  }

  num_pos = 0;
  num = 0;
  del = false;
}

// Odtwarzanie melodyjki po poprawnym kodzie
void playSuccessMelody() {
  int size = sizeof(melody) / sizeof(melody[0]);

  for (int i = 0; i < size; i++) {
    int noteDuration = 1000 / durations[i];

    if (melody[i] == rest) {
      noTone(BUZZER_PIN);
    } else {
      tone(BUZZER_PIN, melody[i], noteDuration);
    }

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZER_PIN);
  }
}

// Alarm po zlym kodzie, dziala do przylozenia poprawnej karty
void alarmUntilCard() {
  lcdText("WRONG PASSWORD", "SCAN CARD");
  digitalWrite(LED_RED, HIGH);

  while (true) {
    tone(BUZZER_PIN, 1000);
    digitalWrite(LED_RED, HIGH);
    delay(500);

    noTone(BUZZER_PIN);
    digitalWrite(LED_RED, LOW);
    delay(500);

    // W kazdej petli sprawdzana jest karta RFID
    if (cardOk()) {
      break;
    }
  }

  noTone(BUZZER_PIN);
  digitalWrite(LED_RED, LOW);
  lcdText("ALARM OFF", "CARD OK");
  delay(1000);
}

// Otwarcie zamka
void openLock() {
  digitalWrite(LED_GREEN, HIGH);
  lockServo.write(position_unlocked);
  unlocked = true;
  lcdText("UNLOCKED", enteredPassword);
  playSuccessMelody();
}

// Zamkniecie zamka
void closeLock() {
  lcdText("LOCKING", ".");
  delay(500);
  lcdText("LOCKING", "..");
  delay(500);
  lcdText("LOCKING", "...");
  delay(500);

  lockServo.write(position_locked);
  unlocked = false;
  digitalWrite(LED_GREEN, LOW);
  clearPassword();
}

// Sprawdzenie wpisanego hasla
bool checkPassword() {
  return strcmp(enteredPassword, correctPassword) == 0;
}

// Obsluga wpisywania kodu joystickiem
void passwordInputMode() {
  int x;
  int y;
  int sw;

  lcd.display();
  lcd.clear();

  // Petla dziala do wpisania hasla albo do zakonczenia czasu
  for (int i = 0; i < 30; i++) {
    readJoystick(x, y, sw);

    // Pokazanie aktualnego trybu na LCD
    if (del == false) {
      lcdText(lineAdd, enteredPassword);
      enteredPassword[num_pos] = numChoose(num);
    } else {
      lcdText(lineDel, enteredPassword);
    }

    delay(300);
    readJoystick(x, y, sw);

    if (sw == LOW) {
      i = 0;

      if (del == false) { // Dodajemy cyfre do kodu
        if (num_pos < passwordLength) {
          enteredPassword[num_pos] = numChoose(num);
          num_pos++;
          enteredPassword[num_pos] = '\0';
          num = 0;
        }
      } else { // Usuwamy ostatnia potwierdzona cyfre kodu
        if (num_pos > 0) {
          num_pos--;
          enteredPassword[num_pos] = '\0';
        }
      }

      delay(300);
    }
    else if (y > 700) { // Przelaczenie widoku na dodawanie cyfr
      i = 0;

      if (del == true) {
        del = false;
        enteredPassword[num_pos] = '\0';
        num = 0;
      }

      delay(300);
    }
    else if (y < 300) { // CPrzelaczenie widoku na usuwanie cyfr
      i = 0;

      if (del == false) {
        del = true;
        enteredPassword[num_pos] = '\0';
      }

      delay(300);
    }
    else if (x < 300) { // Zmiana wartosci obecnej cyfry o jedna w dol
      i = 0;
      num--;

      if (num < 0) {
        num = 9;
      }

      if (num_pos < passwordLength) {
        enteredPassword[num_pos] = numChoose(num);
      }

      delay(300);
    }
    else if (x > 700) { // Zmiana wartosci obecnej cyfry o jedna w gore
      i = 0;
      num++;

      if (num > 9) {
        num = 0;
      }

      if (num_pos < passwordLength) {
        enteredPassword[num_pos] = numChoose(num);
      }

      delay(300);
    }

    // Po wpisaniu calego hasla nastepuje sprawdzenie
    if (num_pos == passwordLength) {
      if (checkPassword()) { // Poprawne haslo
        openLock();
      } else { // Niepoprawne haslo
        alarmUntilCard();
      }

      break;
    }
  }
}

// Ekran oczekiwania na zamkniecie po otwarciu
void waitForLockButton() {
  int x;
  int y;
  int sw;

  while (unlocked == true) {
    lcdText("PRESS BUTTON TO", "LOCK");
    delay(300);

    readJoystick(x, y, sw);

    if (sw == LOW) {
      closeLock();
      break;
    }
  }
}

void setup() {
  // Ustawienie pinow wyjsc i wejsc
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(JOY_SW, INPUT_PULLUP);

  // Stan poczatkowy elementow
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
  noTone(BUZZER_PIN);

  // Uruchomienie LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.noDisplay();

  // Ustawienie serwa w pozycji zamknietej
  lockServo.attach(SERVO_PIN);
  lockServo.write(position_locked);

  // Uruchomienie komunikacji z RFID
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
}

void loop() {
  int x;
  int y;
  int sw;

  readJoystick(x, y, sw);

  // Nacisniecie joysticka uruchamia wpisywanie kodu
  if (sw == LOW && unlocked == false) {
    passwordInputMode();
  }

  // Po otwarciu czekamy na ponowne zamkniecie przyciskiem
  if (unlocked == true) {
    waitForLockButton();
  }

  // Po zakonczeniu pracy ekran jest czyszczony i wygaszany
  if (unlocked == false) {
    clearPassword();
    digitalWrite(LED_GREEN, LOW);
    lcd.clear();
    lcd.noDisplay();
  }
}
