/*
 * Copyright (c) 2006, Technical University of Munich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */
#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)

#define ANNOUNCE_BOOT 1    //adds about 600 bytes to program size
#if ANNOUNCE_BOOT
#define PRINTA(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTA(...)
#endif

#define DEBUG 0
#if DEBUG
#define PRINTD(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTD(...)
#endif

#include <avr/pgmspace.h>
#include <avr/fuse.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <string.h>
#include <dev/watchdog.h>

#include "contiki.h"
#include "contiki-net.h"
#include "contiki-lib.h"
#include "lib/settings.h"
#include "dev/rs232.h"
#include "dev/serial-line.h"
#include "net/rime.h"
#include "rng.h"

//#include "loader/symbols-def.h"
//#include "loader/symtab.h"

#include "radio/rf230bb/rf230bb.h"
//#include "net/mac/frame802154.h"
//#include "net/mac/framer-802154.h"
//#include "net/sicslowpan.h"


#include <avr/signature.h>

/*-------------------------------------------------------------------------*/
static void
generate_new_eui64(uint8_t eui64[8]) {
  eui64[0] = 0x02;
  eui64[1] = rng_get_uint8();
  eui64[2] = rng_get_uint8();
  eui64[3] = 0xFF;
  eui64[4] = 0xFE;
  eui64[5] = rng_get_uint8();
  eui64[6] = rng_get_uint8();
  eui64[7] = rng_get_uint8();
}

/*-------------------------------------------------------------------------*/
static void
initialize(void)
{
  watchdog_init();
  watchdog_start();

  //rs232_init(RS232_PORT_0, USART_BAUD_115200,USART_PARITY_NONE | USART_STOP_BITS_1 | USART_DATA_BITS_8);
  //rs232_set_input(0,raven_lcd_serial_input);

  rs232_init(RS232_PORT_1, USART_BAUD_115200,USART_PARITY_NONE | USART_STOP_BITS_1 | USART_DATA_BITS_8);
  rs232_redirect_stdout(RS232_PORT_1);

  clock_init();

  if(MCUSR & (1<<PORF )) PRINTD("Power-on reset.\n");
  if(MCUSR & (1<<EXTRF)) PRINTD("External reset!\n");
  if(MCUSR & (1<<BORF )) PRINTD("Brownout reset!\n");
  if(MCUSR & (1<<WDRF )) PRINTD("Watchdog reset!\n");
  if(MCUSR & (1<<JTRF )) PRINTD("JTAG reset!\n");

  PRINTA("\n*******Booting %s*******\n",CONTIKI_VERSION_STRING);

  /* rtimers needed for radio cycling */
  rtimer_init();

  /* Initialize process subsystem */
  process_init();

  /* etimers must be started before ctimer_init */
  process_start(&etimer_process, NULL);
  ctimer_init();

  /* Start radio and radio receive process */
  NETSTACK_RADIO.init();

  /* Get a random seed for the 802.15.4 packet sequence number.
   * Some layers will ignore duplicates found in a history (e.g. Contikimac)
   * causing the initial packets to be ignored after a short-cycle restart.
   */
  random_init(rng_get_uint8());

  rimeaddr_t addr;

  size_t size = sizeof(addr);

  if(settings_get(SETTINGS_KEY_EUI64, 0, addr.u8, &size) != SETTINGS_STATUS_OK) {
    generate_new_eui64(addr.u8);
    settings_add(SETTINGS_KEY_EUI64, addr.u8, sizeof(addr));
    settings_set_uint16(SETTINGS_KEY_PAN_ID, 0xABCD);
    settings_set_uint16(SETTINGS_KEY_PAN_ADDR, 0xFFFF);
    settings_set_uint8(SETTINGS_KEY_CHANNEL, 26);
    settings_set_uint8(SETTINGS_KEY_TXPOWER, 0);
  }

  memcpy(&uip_lladdr.addr, &addr.u8, sizeof(rimeaddr_t));
  rimeaddr_set_node_addr(&addr);

  rf230_set_pan_addr(
    settings_get_uint16(SETTINGS_KEY_PAN_ID,0),
    settings_get_uint16(SETTINGS_KEY_PAN_ADDR,0),
    (uint8_t *)&addr.u8
  );
  rf230_set_channel(settings_get_uint8(SETTINGS_KEY_CHANNEL,0));
  rf230_set_txpower(settings_get_uint8(SETTINGS_KEY_TXPOWER,0));

  PRINTA("EUI-64 MAC: %x-%x-%x-%x-%x-%x-%x-%x\n",addr.u8[0],addr.u8[1],addr.u8[2],addr.u8[3],addr.u8[4],addr.u8[5],addr.u8[6],addr.u8[7]);

  /* Initialize stack protocols */
  queuebuf_init();
  NETSTACK_RDC.init();
  NETSTACK_MAC.init();
  NETSTACK_NETWORK.init();

#if ANNOUNCE_BOOT
  PRINTA("%s %s, channel %u , check rate %u Hz tx power %u\n",NETSTACK_MAC.name, NETSTACK_RDC.name, rf230_get_channel(),
    CLOCK_SECOND / (NETSTACK_RDC.channel_check_interval() == 0 ? 1:NETSTACK_RDC.channel_check_interval()),
    rf230_get_txpower());
#if UIP_CONF_IPV6_RPL
  PRINTA("RPL Enabled\n");
#endif
#if UIP_CONF_ROUTER
  PRINTA("Routing Enabled\n");
#endif
#endif /* ANNOUNCE_BOOT */

  process_start(&tcpip_process, NULL);

  /* Autostart other processes */
  autostart_start(autostart_processes);

  PRINTA("Ready\n");
}

/*-------------------------------------------------------------------------*/
int
main(void)
{
  initialize();

  while(1) {
    process_run();
    watchdog_periodic();
  }

  return 0;
}

/*---------------------------------------------------------------------------*/

void
log_message(char *m1, char *m2)
{
  PRINTF("%s%s\n", m1, m2);
}
