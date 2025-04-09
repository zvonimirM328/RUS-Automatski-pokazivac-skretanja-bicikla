#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

volatile uint8_t wake_flag = 0;

void init_led() {
    DDRB |= (1 << PB5); // Pin 13 kao izlaz (LED)
}

void init_button_interrupt() {
    DDRD &= ~(1 << PD2);       // PD2 (INT0) kao ulaz
    PORTD |= (1 << PD2);       // Pull-up otpornik
    EICRA |= (1 << ISC01);     // Padajući brid (falling edge)
    EIMSK |= (1 << INT0);      // Omogući prekid INT0
}

void init_watchdog() {
    MCUSR &= ~(1 << WDRF);             // Očisti WDRF
    WDTCSR |= (1 << WDCE) | (1 << WDE); // Omogući promjenu
    WDTCSR = (1 << WDIE) | (1 << WDP3); // Prekid svakih ~4s
}

ISR(INT0_vect) {
    wake_flag = 1; // Postavi zastavicu buđenja
}

ISR(WDT_vect) {
    wake_flag = 1;
}

void enter_sleep() {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Power-down mode
    sleep_enable();
    sleep_bod_disable(); // Isključi BOD tokom sleepa (ako dostupno)
    sei();               // Omogući prekide
    sleep_cpu();         // Uđi u sleep
    sleep_disable();     // Nakon buđenja
}

int main(void) {
    cli(); // Onemogući prekide tokom inicijalizacije
    init_led();
    init_button_interrupt();
    init_watchdog();
    sei(); // Omogući globalne prekide

    while (1) {
        // LED treperi 5 sekundi (1s ON/OFF)
        for (uint8_t i = 0; i < 5; i++) {
            PORTB ^= (1 << PB5); // Toggle LED
            _delay_ms(500);
        }

        PORTB &= ~(1 << PB5); // Ugasi LED
        wake_flag = 0;

        enter_sleep(); // Uđi u sleep mode

        while (!wake_flag); // Čekaj buđenje
    }
}
