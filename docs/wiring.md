# Schemat podłączenia

## LCD 16x2 bez I2C

LCD pracuje w trybie 4-bitowym. Piny `D0-D3` LCD zostają niepodłączone.

| LCD | Arduino |
|---|---|
| GND / VSS | GND |
| VCC / VDD | 5V |
| VO / V0 | środkowa nóżka potencjometru 10kΩ |
| RS | D4 |
| RW | GND |
| E | D5 |
| D0 | nie podłączać |
| D1 | nie podłączać |
| D2 | nie podłączać |
| D3 | nie podłączać |
| D4 | D6 |
| D5 | D7 |
| D6 | A2 |
| D7 | A3 |
| BLA / LED+ | 5V przez rezystor |
| BLK / LED- | GND |

Potencjometr:
- jedna skrajna nóżka do 5V,
- druga skrajna nóżka do GND,
- środkowa nóżka do VO/V0 LCD.

## RFID RC522

| RC522 | Arduino |
|---|---|
| SDA / SS | D10 |
| SCK | D13 |
| MOSI | D11 |
| MISO | D12 |
| IRQ | nie podłączać |
| GND | GND |
| RST | D3 |
| 3.3V | 3.3V |

Uwaga: RC522 nie podłączać do 5V.

## Joystick

| Joystick | Arduino |
|---|---|
| GND | GND |
| VCC | 5V |
| VRx | A1 |
| VRy | A4 |
| SW | A0 |

## Serwo

| Serwo | Arduino |
|---|---|
| pomarańczowy / żółty | D9 |
| czerwony | 5V |
| brązowy / czarny | GND |

Jeżeli serwo działa niestabilnie, użyć osobnego zasilania 5V i połączyć GND z Arduino.

## LED-y

Zielona LED:

| Element | Podłączenie |
|---|---|
| anoda, dłuższa nóżka | D2 przez rezystor |
| katoda, krótsza nóżka | GND |

Czerwona LED:

| Element | Podłączenie |
|---|---|
| anoda, dłuższa nóżka | A5 przez rezystor |
| katoda, krótsza nóżka | GND |

## Buzzer

| Buzzer | Arduino |
|---|---|
| + | D8 |
| - | GND |

## Wspólna masa

Wszystkie moduły muszą mieć wspólne GND:

```text
GND Arduino
├── GND LCD
├── GND RC522
├── GND joysticka
├── GND buzzera
├── GND LED
└── GND serwa / zewnętrznego zasilania
```
