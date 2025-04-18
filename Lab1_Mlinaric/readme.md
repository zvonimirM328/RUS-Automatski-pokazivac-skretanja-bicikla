<h1>📖 Lab1: Prekidi u Ugradbenim Sustavima</h1>

Poveznica za projekt: https://wokwi.com/projects/427246398407736321

<h2>1. Opis zadatka</h2>

Cilj ovog laboratorijskog rada je implementacija prekida (interrupts) na Arduino Mega 2560 pomoću tipki, tajmera i ultrazvučnog senzora. Svaki prekid aktivira određenu LED diodu kako bi se vizualizirala aktivnost.

Korištene funkcionalnosti:

✅ Prekidi pomoću vanjskih tipki (INT0, INT1, INT2)

✅ Prekid tajmera Timer1 za periodične radnje

✅ Prekid putem ultrazvučnog senzora (HC-SR04) ako je objekt bliži od 100 cm

<h2>2. Hardverske komponente</h2>

Za realizaciju sustava korišteni su sljedeći dijelovi:

| **Komponenta**        | **Model**            | **Uloga**                     |
|-----------------------|---------------------|-------------------------------|
| **Mikrokontroler**    | Arduino Mega 2560   | Glavni upravljački sklop      |
| **LED diode**         | 5x (različite boje) | Signalizacija prekida         |
| **Tipke (Buttons)**   | 3x Tactile Switch   | Aktivacija prekida            |
| **Ultrazvučni senzor**| HC-SR04             | Mjerenje udaljenosti          |
| **Logički analizator**| WOKWI LOGIC         | Analiza signala (simulacija)  |

<h2>3. Slika spojeva</h2>

Sljedeća slika prikazuje način povezivanja komponenti:

![schema](https://github.com/user-attachments/assets/30ef2200-4a63-4290-aa3f-af72a8bc4b27)

<h2>4. Opis rješenja</h2>

Kod implementira prekide za tipke, tajmer i senzor koristeći Arduino C++.

📌 Vanjski prekidi (tipke)

Svaka tipka je povezana na prekidni ulaz (INT0, INT1, INT2).

Kada se tipka pritisne, LED dioda trepće 10 puta.

Implementiran je debounce kako bi se izbjegle neželjene višestruke aktivacije.

📌 Tajmerski prekid

Timer1 se koristi za generiranje prekida svakih 1 sekundu.

Timer aktivira LED diodu (LED_TIMER) i poziva funkciju za provjeru udaljenosti senzora.

📌 Ultrazvučni senzor (HC-SR04)

Timer prekid aktivira mjerenje udaljenosti pomoću HC-SR04 senzora.

Ako je objekt bliže od 100 cm, aktivira se prekid senzora i narančasta LED dioda trepće dok se ne udalji objekt.

<h2>5. Zaključak</h2>

Ovaj laboratorijski zadatak uspješno demonstrira korištenje prekida na Arduino Mega 2560.

🔹 Prekidi omogućuju brzu i efikasnu reakciju sustava na događaje.

🔹 Debounce logika sprječava neželjene višestruke prekide uzrokovane mehaničkim vibracijama tipki.

🔹 Tajmer omogućuje periodične provjere, čime se osigurava pravovremena detekcija udaljenosti.

Moguće buduće nadogradnje:

✅ Implementacija više razina alarma na temelju udaljenosti.

🛠 Autor: Zvonimir Mlinarić

🎓 Predmet: Razvoj Ugradbenih sustava

🏫 Fakultet: Tehničko Veleučilište Zagreb
