# 📖 Lab 2: Upravljanje potrošnjom energije korištenjem sleep modova

**Poveznica za projekt:** [Wokwi projekt](https://wokwi.com/projects/427765641826069505)

## 1. Opis zadatka
Cilj ovog laboratorijskog zadatka je implementacija upravljanja potrošnjom energije na mikrokontroleru ATmega328P (Arduino Uno) korištenjem sleep modova. U napravljenom programu dolazi do e treptanja crvene LED diode, ulazak u sleep modove i buđenje putem tipke(INT0 PREKID) ili Watchdog timera.

**Korištene funkcionalnosti:**
- ✅ Treptanje LED diode tijekom 5 sekundi
- ✅ Ulazak u sleep mode (Power-down, Idle, Standby) nakon zadatka
- ✅ Buđenje putem:
  - Tipke (INT0 prekid)
  - Watchdog timera (~4s interval)
- ✅ Simulacija potrošnje energije u Serial Monitoru

## 2. Hardverske komponente
Ovaj program koristi slijedeće komponete u Wokwi simulatoru:

| Komponente          | Model           | Uloga                     |
|---------------------|-----------------|---------------------------|
| Mikrokontroler      | Arduino Uno     | Glavni upravljački sklop  |
| LED dioda           | Crvena LED      | Signalizacija aktivnosti  |
| Tipka (Button)      | Tactile Switch  | Aktivacija prekida (INT0) |

## 3. Slika spojeva
Slijedeća slika pokazuje način povezivanja komponenti unutar Wokwi simulatora:

![Arduino Setup](sleep.png)


## 4. Opis rješenja
Za pisanje koda se koristi Arduino C++.

### 📌 Treptanje LED diode
- LED na pinu 13 trepti 5 sekundi (500 ms ON, 500 ms OFF).
- Potrošnja u aktivnom stanju simulirana je kao ~5 mA.

### 📌 Sleep modovi
- **Power-down:** Najniža potrošnja (~0.1 µA), oscilator isključen.
- **Idle:** Viša potrošnja (~1.5 mA), timeri aktivni.
- **Standby:** Niska potrošnja (~1 µA), oscilator uključen za brže buđenje.
- Mikrokontroler prelazi u sleep mode odmah nakon treptanja, isključujući ADC za dodatnu uštedu.

### 📌 Mehanizmi buđenja
- **Tipka (INT0):** Povezana na pin 2, koristi padajući brid za prekid. Softverski debouncing sprječava višestruke ispise.
- **Watchdog timer:** Postavljen na ~4 sekunde, automatski budi mikrokontroler.

### 📌 Simulacija potrošnje
- Potrošnja se ispisuje u Serial Monitoru temeljem podataka iz datasheet-a ATmega328P:
  - Aktivno: 5 mA
  - Power-down: 0.0001 mA
  - Idle: 1.5 mA
  - Standby: 0.001 mA

## 5. Zaključak
Ovaj laboratorijski zadatak uspješno demonstrira upravljanje potrošnjom energije na ATmega328P mikrokontroleru.

### 🔹 Sleep modovi značajno smanjuju potrošnju između aktivnih perioda. 
### 🔹 INT0 prekid i Watchdog timer omogućuju fleksibilno buđenje. 
### 🔹 Simulacija potrošnje pomaže u razumijevanju različitih režima. 


### 🛠 Autor
Dan Hamin
### 🎓 Predmet
Razvoj Ugradbenih Sustava
### 🏫 Fakultet
Tehničko Veleučilište u Zagrebu
