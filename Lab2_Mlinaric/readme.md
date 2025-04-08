<h1>📖 Lab2: Upravljanje potrošnjom energije mikrokontrolera korištenjem Sleep moda</h1>

🔗 Poveznica za projekt: [https://wokwi.com/projects/427581015265206366](https://wokwi.com/projects/427696344752524289)

<h2>1. Opis zadatka</h2>

Cilj ovog laboratorijskog rada je implementacija sleep modova na Arduino Uno mikrokontroleru kako bi se smanjila potrošnja energije tijekom neaktivnih perioda rada sustava.

Program koristi LED diodu koja periodično trepće, a zatim mikrokontroler prelazi u sleep mode do sljedeće aktivnosti.

Korištene funkcionalnosti:

✅ Periodično treptanje LED diode (5 sekundi rada)

✅ Uvođenje mikrokontrolera u niskopotrošni režim (Sleep mode)

✅ Buđenje putem vanjskog prekida (tipkalo)

✅ Automatsko buđenje pomoću watchdog timera

<h2>2. Hardverske komponente</h2>

Komponenta	Model	Uloga
Mikrokontroler	Arduino Uno	Upravljanje sustavom i sleep modovima
LED dioda	Ugrađena (pin 13)	Vizualna signalizacija rada
Tipkalo (button)	Tactile Switch	Buđenje iz sleep moda (prekid INT0)
Pull-up otpornik	Interni (INPUT_PULLUP)	Održavanje stabilnog stanja na tipki
Simulacijska platforma	Wokwi	Simulacija rada i testiranje

4. Slika spojeva

Sljedeća slika prikazuje način povezivanja LED diode i tipkala:

📌 Napomena: Tipkalo je spojeno na pin 2 (INT0) i GND. Koristi se INPUT_PULLUP.

📷 (Dodaj sliku iz Wokwi projekta ili koristi automatski prikaz spojeva)

4. Opis rješenja

  Program je implementiran u Arduino okruženju koristeći biblioteku LowPower.h koja omogućuje jednostavno korištenje sleep modova.

📌 Periodično treptanje LED-ice

  LED dioda trepće 5 puta (svijetli i gasi se u razmaku od 1 sekunde) kako bi se simulirala aktivna faza rada sustava.

📌 Ulazak u Sleep mode

  Nakon treptanja, mikrokontroler ulazi u Power-down sleep mode, koji značajno smanjuje potrošnju energije.

cpp
Kopiraj
Uredi
LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
Sleep traje ukupno 16 sekundi (2 x 8 sekundi), osim ako se ne dogodi buđenje.

📌 Mehanizmi buđenja

  Pritisak tipkala – eksterni prekid (INT0) buđenje iz sna

  Watchdog timer – automatsko buđenje nakon vremenskog intervala

📌 Upravljanje izlaskom iz Sleep moda

Mikrokontroler odmah prelazi natrag u aktivnu fazu nakon buđenja. Postoji zaštita od neželjenog višestrukog buđenja (flag wakeUpFlag).

5. Zaključak
 
Ovaj laboratorijski rad uspješno demonstrira kako efikasno upravljati potrošnjom energije u ugradbenim sustavima koristeći sleep modove mikrokontrolera.

🔹 Sleep modovi omogućuju drastično smanjenje potrošnje u neaktivnim fazama.

🔹 Eksterni i interni prekidi omogućuju pouzdano buđenje sustava.

🔹 LowPower.h biblioteka znatno pojednostavljuje rad sa sleep funkcijama.

Moguće buduće nadogradnje:

✅ Dodavanje više senzora za buđenje iz sleep moda

✅ Dinamičko podešavanje trajanja sleep moda prema uvjetima

✅ Implementacija naprednijih sleep modova i analize potrošnje pomoću mjernog sklopa

🛠 Autor: Zvonimir Mlinarić
🎓 Predmet: Razvoj Ugradbenih sustava
🏫 Fakultet: Tehničko Veleučilište Zagreb

