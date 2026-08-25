/*
 * ATtiny13A — 12-LED Charlieplexed "LED Ladder" game  (CR2032 / low-power)
 * =========================================================================
 *
 * Hardware
 * --------
 *   MCU      : ATtiny13A (PDIP-8 / SOIC-8)
 *   Clock    : internal 1.2 MHz RC oscillator (default fuses). Keep it low —
 *              do NOT switch to 9.6 MHz on a coin cell (wastes current).
 *   Supply   : CR2032 coin cell (~3 V). Add a 47 uF bulk capacitor across
 *              VCC/GND near the chip — the coin cell has high internal
 *              resistance and the cap supplies the LED current pulses.
 *   LEDs     : 12 LEDs charlieplexed across PB0..PB3 (4 pins -> 12 LEDs).
 *              Use RED / GREEN / YELLOW LEDs (low forward voltage). Blue or
 *              white LEDs will barely light on 3 V.
 *   Button   : PB4 to GND (internal pull-up, active LOW). This button also
 *              WAKES the game from sleep.
 *
 *   Current limiting (CR2032): use ONE shared ~150 ohm resistor from VCC to
 *   the common LED supply node (only one LED is ever on at a time). This
 *   keeps peak LED current to a few mA, which a coin cell can handle.
 *
 * Power management
 * ----------------
 *   - ADC and analog comparator are turned off (power reduction register).
 *   - LEDs are driven with short pulses (low duty cycle) to save current.
 *   - After SHUTDOWN_MS with no button press the MCU enters POWER-DOWN sleep
 *     (~0.1 uA). It wakes on the next button press (pin-change interrupt).
 *   - No power switch is required. Add an optional VCC slide switch only if
 *     you want a true 0 uA "storage" off state.
 *
 * Game rules
 * ----------
 *   - A bright "climber" LED marches up the ladder (LED0 bottom -> LED11 top).
 *   - The top rung (LED11) is shown dimly as the GOAL.
 *   - Press the button to STOP the climber:
 *       * stop on the goal  -> WIN: celebration, then the game gets FASTER.
 *       * stop anywhere else-> MISS: blink, restart at the SAME speed.
 *   - Idle attract sweep while waiting for the first press.
 *   - No press for SHUTDOWN_MS -> deep sleep (press button to wake & play).
 *
 * Build / flash
 * -------------
 *   avr-gcc -mmcu=attiny13a -DF_CPU=1200000UL -Os -Wall -o main.elf main.c
 *   avr-objcopy -O ihex -R .eeprom main.elf main.hex
 *   avrdude -c usbasp -p attiny13a -U flash:w:main.hex:i
 *
 * Fuses (default 1.2 MHz, BOD disabled for low power):
 *   lfuse=0x6A  hfuse=0xFF   (factory default is fine)
 */

#ifndef F_CPU
#define F_CPU 1200000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

/* ------------------------------------------------------------------ */
/* Tunables                                                            */
/* ------------------------------------------------------------------ */
#define CP_MASK      0x0F        /* PB0..PB3 charlieplex pins          */
#define BTN_PIN      PB4         /* button on PB4                      */
#define BTN_PCINT    PCINT4      /* pin-change channel for PB4         */

#define GOAL_LED     11          /* top rung = goal                    */
#define SHUTDOWN_MS  30000UL     /* sleep after 30 s of no button press*/

/* LED on-times (us). Shorter = dimmer but lower average current.      */
#define MARKER_ON_US 700
#define GOAL_ON_US   450

/* Charlieplex table: index -> {anode, cathode}                        */
static const uint8_t led_anode[12]   = {0,0,0, 1,1,1, 2,2,2, 3,3,3};
static const uint8_t led_cathode[12] = {1,2,3, 0,2,3, 0,1,3, 0,1,2};

/* ------------------------------------------------------------------ */
/* State                                                               */
/* ------------------------------------------------------------------ */
static uint32_t inactive_ms = 0; /* time since last button press       */

/* ------------------------------------------------------------------ */
/* Charlieplex primitives                                             */
/* ------------------------------------------------------------------ */
static void leds_off(void)
{
    DDRB  &= (uint8_t)~CP_MASK;
    PORTB &= (uint8_t)~CP_MASK;
}

static void led_on(uint8_t i)
{
    uint8_t a = led_anode[i];
    uint8_t c = led_cathode[i];

    leds_off();
    PORTB |=  (uint8_t)(1 << a);
    DDRB  |=  (uint8_t)(1 << a);
    PORTB &= (uint8_t)~(1 << c);
    DDRB  |=  (uint8_t)(1 << c);
}

/* ------------------------------------------------------------------ */
/* Button                                                              */
/* ------------------------------------------------------------------ */
static uint8_t btn_pressed(void)
{
    return (PINB & (1 << BTN_PIN)) ? 0 : 1;
}

static void btn_init(void)
{
    DDRB  &= (uint8_t)~(1 << BTN_PIN);
    PORTB |=  (uint8_t)(1 << BTN_PIN);   /* pull-up                    */
}

static void btn_wait_release(void)
{
    while (btn_pressed()) _delay_ms(5);
    _delay_ms(20);
}

/* ------------------------------------------------------------------ */
/* Pin-change interrupt (used only to wake from sleep)                */
/* ------------------------------------------------------------------ */
ISR(PCINT0_vect)
{
    /* nothing to do — the edge itself wakes the CPU */
}

/* ------------------------------------------------------------------ */
/* Deep sleep: power-down until the button is pressed                 */
/* ------------------------------------------------------------------ */
static void enter_sleep(void)
{
    leds_off();
    wdt_disable();

    /* wake source: pin-change on the button (PB4 / PCINT4) */
    PCMSK |= (uint8_t)(1 << BTN_PCINT);
    GIMSK |= (uint8_t)(1 << PCIE);

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    cli();
    sleep_enable();
    sei();
    sleep_cpu();                 /* <-- sleeps here, ~0.1 uA           */
    /* woke up */
    sleep_disable();
    cli();
    GIMSK &= (uint8_t)~(1 << PCIE);
    PCMSK &= (uint8_t)~(1 << BTN_PCINT);
    sei();

    inactive_ms = 0;
    _delay_ms(50);               /* let the button settle              */
}

/* ------------------------------------------------------------------ */
/* Display + timing helper (also tracks inactivity)                   */
/* ------------------------------------------------------------------ */
static uint8_t display_wait(uint16_t ms, uint8_t marker, uint8_t goal, uint8_t show_goal)
{
    uint16_t elapsed = 0;
    uint8_t  frame   = 0;

    while (elapsed < ms) {
        led_on(marker);
        _delay_us(MARKER_ON_US);

        if (show_goal && goal != marker && (frame & 1)) {
            led_on(goal);
            _delay_us(GOAL_ON_US);
        }

        leds_off();
        elapsed += 2;
        frame++;

        if (btn_pressed()) {
            _delay_ms(20);
            if (btn_pressed()) {
                inactive_ms = 0;   /* activity!                        */
                return 1;
            }
        }
    }
    inactive_ms += ms;           /* no press during this window        */
    return 0;
}

/* ------------------------------------------------------------------ */
/* Effects                                                             */
/* ------------------------------------------------------------------ */
static void celebrate(void)
{
    for (uint8_t t = 0; t < 3; t++)
        for (uint8_t i = 0; i < 12; i++) {
            led_on(i);
            _delay_ms(12);
        }
    leds_off();
}

static void blink_at(uint8_t pos)
{
    for (uint8_t b = 0; b < 6; b++) {
        led_on(pos);
        _delay_ms(80);
        leds_off();
        _delay_ms(80);
    }
}

static void idle_attract(void)
{
    uint8_t i = 0;
    while (!btn_pressed()) {
        led_on(i);
        _delay_ms(50);
        leds_off();
        inactive_ms += 50;
        if (inactive_ms >= SHUTDOWN_MS) return;   /* caller will sleep  */
        if (++i > 11) i = 0;
    }
    _delay_ms(20);
    btn_wait_release();
    inactive_ms = 0;
}

/* ------------------------------------------------------------------ */
/* Low-power peripheral init                                           */
/* ------------------------------------------------------------------ */
static void power_init(void)
{
    PRR  |= (uint8_t)(1 << PRADC);  /* disable ADC clock               */
    ACSR |= (uint8_t)(1 << ACD);    /* disable analog comparator       */
    wdt_disable();
}

/* ------------------------------------------------------------------ */
/* Main                                                                */
/* ------------------------------------------------------------------ */
int main(void)
{
    power_init();
    btn_init();
    leds_off();
    sei();

    uint16_t step_ms = 120;
    const uint16_t MIN_STEP = 25;
    const uint16_t STEP_DEC = 12;

    for (;;) {
        inactive_ms = 0;
        idle_attract();

        /* If we timed out while idle, sleep until the button is pressed */
        if (inactive_ms >= SHUTDOWN_MS) {
            enter_sleep();
            continue;            /* wake -> start over at attract       */
        }

        uint8_t marker = 0;

        for (;;) {
            if (inactive_ms >= SHUTDOWN_MS) {
                enter_sleep();
                break;           /* wake -> restart via outer loop      */
            }

            uint8_t pressed = display_wait(step_ms, marker, GOAL_LED, 1);

            if (pressed) {
                if (marker == GOAL_LED) {
                    celebrate();
                    if (step_ms > MIN_STEP) {
                        step_ms -= STEP_DEC;
                        if (step_ms < MIN_STEP) step_ms = MIN_STEP;
                    }
                } else {
                    blink_at(marker);
                }
                btn_wait_release();
                marker = 0;
            } else {
                if (++marker > 11) marker = 0;
            }
        }
    }

    return 0;
}