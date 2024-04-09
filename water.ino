#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

const int PGM_BUZZER = 2;
const int PGM_GREEN = 0;
const int PGM_RED = 1;
const int ADC_VBG = 0x8C;  // Incorrect in some versions of this library, so we set it here

const int ADC_LOW = A2;
const int ADC_HIGH = A3;

const int SIGNAL_1GREEN = 0;
const int SIGNAL_2GREEN = 1;
const int SIGNAL_1RED = 2;
const int SIGNAL_2RED = 3;

const int THRESH_DEBUG = 1020;
const int THRESH_WATER = 100;
/* 1100/V => 333 = 3.3V; 349 = 3.15V ; 367 = 3.0V  */
const int THRESH_VCC = 333;  // 3.3V.

void setup() {
  setup_watchdog(WDTO_4S);  // Periodic Heartbeat to awaken deep sleep()
  sleep_disable();
  pinMode(PGM_RED, OUTPUT);
  pinMode(PGM_GREEN, OUTPUT);
  pinMode(PGM_BUZZER, OUTPUT);
  pinMode(ADC_LOW, INPUT);
  pinMode(ADC_HIGH, INPUT);
}

void system_sleep() {
  ADCSRA &=~ _BV(ADEN);
  power_all_disable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  noInterrupts();
  sleep_enable();
  interrupts();
  sleep_mode();
  sleep_disable();
  power_all_enable();
  ADCSRA |= _BV(ADEN);
}

void setup_watchdog(int time) {
  byte timeout;

  timeout = (1<<WDCE)|(time&7)|((time&8)<<2);
  MCUSR &= ~(1 << WDRF);
  /* timed sequence */
  WDTCR |= (1 << WDCE) | (1 << WDE);
  WDTCR = timeout;
  WDTCR |= _BV(WDIE);
}

ISR(WDT_vect) {}

void morse_bit(int value) {
  int pin = value ? PGM_RED : PGM_GREEN;
  digitalWrite(pin, HIGH);
  delay(1000);
  digitalWrite(pin, LOW);
  delay(1000);
}

void morse_value(int value) {
  unsigned int idx = 1 << 9;
  int i;

  digitalWrite(PGM_BUZZER, HIGH);
  delay(50);
  digitalWrite(PGM_BUZZER, LOW);
  for (i = 0; i < 10; i++) {
    morse_bit(value & idx);
    idx >>= 1;
  }
}

int stable_read(int pin) {
  analogRead(pin);
  delay(2);
  return analogRead(pin);
}

int get_status() {
  int high = stable_read(ADC_HIGH);
  if (high > THRESH_DEBUG) {
    return 4;
  }
  if (high > THRESH_WATER) {
    return 3;
  }
  int low = stable_read(ADC_LOW);
  if (low > THRESH_DEBUG) {
    return 4;
  }
  if (low > THRESH_WATER) {
    return 2;
  }
  if (stable_read(ADC_VBG) > THRESH_VCC) {
    return 1;
  }
  return 0;
}

void send_signal(int value) {
  int alarm = (value & 1);
  int red = (value & 2);
  int pin = red ? PGM_RED : PGM_GREEN;
  int wait = red ? 50 : 20;

  if (alarm) {
    digitalWrite(PGM_BUZZER, HIGH);
    delay(50);
    digitalWrite(PGM_BUZZER, LOW);
    digitalWrite(pin, HIGH);
    delay(wait);
    digitalWrite(pin, LOW);
    delay(300);
  }
  digitalWrite(pin, HIGH);
  delay(wait);
  digitalWrite(pin, LOW);
  if (red) {
    system_sleep();  // 4s
  } else {
    for (int i = 0; i < 15; i++)
      system_sleep();  // 60s
  }
}

void send_debug() {
  morse_value(stable_read(ADC_HIGH));
  morse_value(stable_read(ADC_LOW));
  morse_value(stable_read(ADC_VBG));
  digitalWrite(PGM_BUZZER, HIGH);
  delay(50);
  digitalWrite(PGM_BUZZER, LOW);
  delay(50);
}

void loop() {
  int status = get_status();
  if (status > 3) {
    send_debug();
  } else {
    send_signal(status);
  }
}
