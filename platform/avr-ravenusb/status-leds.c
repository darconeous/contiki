
#include "status-leds.h"

PROCESS(status_leds_process, "Status LEDs");

static uint8_t ledRX_timer, ledTX_timer, ledVCP_timer;

static enum {
  STATUS_LED_UNENUMERATED,
  STATUS_LED_INACTIVE,
  STATUS_LED_READY
} device_status_indicator;

PROCESS_THREAD(status_leds_process, ev, data_proc)
{
  static struct etimer et;

  static struct etimer et_indicator;

  PROCESS_BEGIN();

  Leds_init();

  etimer_set(&et, CLOCK_SECOND / 30);
  etimer_set(&et_indicator, CLOCK_SECOND / 8);

  while(1) {
    PROCESS_YIELD();

    if(etimer_expired(&et_indicator)) {
      if(device_status_indicator == STATUS_LED_READY) {
        LedSTAT_on();
      } else if(device_status_indicator == STATUS_LED_INACTIVE) {
        LedSTAT_toggle();
        etimer_set(&et_indicator, (CLOCK_SECOND / 8));
      } else {
        LedSTAT_toggle();
        etimer_set(&et_indicator, (CLOCK_SECOND / 3));
      }
    }

    if(etimer_expired(&et)) {
      if(0 != ledRX_timer) {
        ledRX_timer--;
        if(ledRX_timer & (1 << 1))
          LedRX_on();
        else
          LedRX_off();
      } else
        LedRX_off();

      if(0 != ledTX_timer) {
        ledTX_timer--;
        if(ledTX_timer & (1 << 1))
          LedTX_on();
        else
          LedTX_off();
      } else
        LedTX_off();

      if(0 != ledVCP_timer) {
        ledVCP_timer--;
        if(ledVCP_timer & (1 << 2))
          LedVCP_on();
        else
          LedVCP_off();
      } else
        LedVCP_off();

      if(ledRX_timer || ledTX_timer || ledVCP_timer)
        etimer_restart(&et);
    }
  }                             // while(1)

  PROCESS_END();
}

void
status_leds_radio_tx()
{
  if(!(ledRX_timer || ledTX_timer || ledVCP_timer))
    process_poll(&status_leds_process);
  ledTX_timer |= (1 << 2);
  if(((ledTX_timer - 1) & (1 << 1)))
    LedTX_on();
}

void
status_leds_radio_rx()
{
  if(!(ledRX_timer || ledTX_timer || ledVCP_timer))
    process_poll(&status_leds_process);
  ledRX_timer |= (1 << 2);
  if(((ledRX_timer - 1) & (1 << 1)))
    LedRX_on();
}

void
status_leds_serial_tx()
{
  if(!(ledRX_timer || ledTX_timer || ledVCP_timer))
    process_poll(&status_leds_process);
  ledVCP_timer |= (1 << 3);
  if(((ledVCP_timer - 1) & (1 << 2)))
    LedVCP_on();
}

void
status_leds_serial_rx()
{
  status_leds_serial_tx();
}

static bool radio_is_on;

void
status_leds_radio_on()
{
  if(!radio_is_on)
    status_leds_serial_tx();
  radio_is_on = true;
}

void
status_leds_radio_off()
{
  radio_is_on = false;
}

void
status_leds_unenumerated()
{
  device_status_indicator = STATUS_LED_UNENUMERATED;
}

void
status_leds_inactive()
{
  device_status_indicator = STATUS_LED_INACTIVE;
}

void
status_leds_ready()
{
  device_status_indicator = STATUS_LED_READY;
}

void
status_leds_will_sleep()
{
  if(STATUS_LED_READY == device_status_indicator)
    LedSTAT_off();
}

void
status_leds_did_wake()
{
  if(STATUS_LED_READY == device_status_indicator)
    LedSTAT_on();
}
