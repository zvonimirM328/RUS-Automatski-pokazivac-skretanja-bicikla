# 💤 AVR Sleep Mode – Upravljanje potrošnjom energije

[🔗 Otvori u Wokwi Simulatoru](https://wokwi.com/projects/427250141739098369)

## 1. 📌 Opis zadatka

Cilj projekta je implementacija upravljanja energijom pomoću **sleep modova** na **AVR mikrokontroleru (ATmega328P)**. Mikrokontroler izvodi osnovnu funkciju (treptanje LED-ice), a zatim prelazi u **niskopotrošni režim (Power-down mode)** do sljedećeg buđenja.

### ✅ Funkcionalnosti:

- Treptanje LED-ice tijekom 3 sekunde
- Ulazak u sleep mode nakon završetka zadatka
- Buđenje putem:
  - Tipke (INT0 prekid)
  - Watchdog timera (~4s interval)

---

## 2. ⚙️ Korištene komponente

| Komponenta        | Model           | Uloga                                       |
|------------------|------------------|---------------------------------------------|
| Mikrokontroler    | ATmega328P (Arduino Uno) | Upravljanje logikom i sleep modom |
| LED dioda         | Crvena (na PB5) | Signalizira aktivnost mikrokontrolera      |
| Tipkalo (Button)  | Tactile Switch (na PD2) | Aktivira INT0 prekid za buđenje            |

---

## 3. 🔌 Shema spoja (Wokwi)

- LED ➜ PB5 (Arduino Uno: Digital 13)  
- Tipkalo ➜ PD2 (Arduino Uno: Digital 2 / INT0)  
- Tipkalo koristi **internal pull-up** otpor



---

## 4. 💡 Funkcionalni opis

### 🔄 Glavni tok
- Mikrokontroler uključuje LED i trepće 3 skunde.
- Nakon toga gasi LED i prelazi u **Power-down sleep mode**.

### 🌙 Sleep mod
- Koristi se `SLEEP_MODE_PWR_DOWN` za minimalnu potrošnju.
- `sleep_bod_disable()` koristi se za dodatno smanjenje potrošnje (isključuje brown-out detektor).

### ⏰ Prekidi za buđenje
- **INT0 (tipka na PD2):** Vanjski prekid (falling edge) budi mikrokontroler.
- **Watchdog Timer:** Prekida svakih ~4 sekunde, također budi mikrokontroler.

---

## 5. 🧪 Testiranje

| Scenarij | Očekivano ponašanje |
|----------|----------------------|
| LED treptanje | LED bljeska 3 sekunde |
| Sleep mode | LED se gasi, Arduino ulazi u low-power |
| Pritisak tipke | LED ponovno počinje treptati |
| Automatsko buđenje | LED se aktivira svakih ~4s ako tipka nije pritisnuta |

---

## 6. 📚 Zaključak

Projekt demonstrira kako koristiti sleep modove na AVR mikrokontroleru kako bi se značajno **smanjila potrošnja energije** bez kompromisa u funkcionalnosti.

### 🔧 Moguće nadogradnje:
- Analiza i usporedba svih AVR sleep modova
- Mjerenje realne potrošnje putem strujnog mjerača

---

## 👨‍💻 Autor

**Zvonimir Mlinarić**  
🎓 *Razvoj Ugradbenih Sustava*  
🏫 *Tehničko Veleučilište Zagreb*

---

## 🗂️ Projekt struktura

