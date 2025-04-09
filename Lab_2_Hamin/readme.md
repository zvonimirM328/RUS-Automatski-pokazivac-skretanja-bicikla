# 📖 Lab 2: Upravljanje potrošnjom energije korištenjem sleep modova

**Poveznica za projekt:** [Wokwi projekt](https://wokwi.com/projects/427765641826069505)

## 1. Opis zadatka
Cilj ovog laboratorijskog rada je implementacija upravljanja potrošnjom energije na mikrokontroleru ATmega328P (Arduino Uno) korištenjem sleep modova. Program omogućuje treptanje LED diode, ulazak u niskopotrošne režime i buđenje putem tipke ili Watchdog timera.

**Korištene funkcionalnosti:**
- ✅ Treptanje LED diode tijekom 5 sekundi
- ✅ Ulazak u sleep mode (Power-down, Idle, Standby) nakon zadatka
- ✅ Buđenje putem:
  - Tipke (INT0 prekid)
  - Watchdog timera (~4s interval)
- ✅ Simulacija potrošnje energije u Serial Monitoru

## 2. Hardverske komponente
Za realizaciju sustava korištene su sljedeće komponente u Wokwi simulatoru:

| Komponente          | Model           | Uloga                     |
|---------------------|-----------------|---------------------------|
| Mikrokontroler      | Arduino Uno     | Glavni upravljački sklop  |
| LED dioda           | Crvena LED      | Signalizacija aktivnosti  |
| Tipka (Button)      | Tactile Switch  | Aktivacija prekida (INT0) |

## 3. Slika spojeva
Na sljedećoj slici prikazan je način povezivanja komponenti unutar Wokwi simulatora:

sleep.png 


## 4. Opis rješenja
Kod u programu implementira upravljanje sleep modovima koristeći Arduino C++.

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

##5. Zaključak
Ovaj laboratorijski zadatak uspješno demonstrira upravljanje potrošnjom energije na ATmega328P mikrokontroleru.

🔹 Sleep modovi značajno smanjuju potrošnju između aktivnih perioda.
🔹 INT0 prekid i Watchdog timer omogućuju fleksibilno buđenje.
🔹 Simulacija potrošnje pomaže u razumijevanju različitih režima.
Moguće buduće nadogradnje:

✅ Dodavanje senzora za automatsko buđenje temeljem vanjskih uvjeta.
✅ Implementacija mjerenja stvarne potrošnje na fizičkom hardveru.
🛠 Autor
Dan Hamin
Datum: 09. travnja 2025.
🎓 Predmet
Razvoj Ugradbenih Sustava
🏫 Fakultet
Tehničko Veleučilište Zagreb
