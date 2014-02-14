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
 * @(#)$$
 */

/**
 * \file
 *         Configuration for RZRAVEN USB stick "jackdaw"
 *
 * \author
 *         Simon Barner <barner@in.tum.de>
 *         David Kopf <dak664@embarqmail.com>
 */

#ifndef __CONTIKI_CONF_H__
#define __CONTIKI_CONF_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* ************************************************************************** */
// MARK: Basic Configuration
/* ************************************************************************** */

/* Platform name, type, and MCU clock rate */
#define PLATFORM_NAME  "RAVENUSB"
#define PLATFORM_TYPE  RAVENUSB_C

/* CPU Frequency, in Hz */
#ifndef F_CPU
#define F_CPU          8000000UL
#endif

#include <avr/eeprom.h>

#define EEPROM_CONF_SIZE		(E2END + 1)

#ifndef EEPROM_END_ADDR
#define EEPROM_END_ADDR         (EEPROM_CONF_SIZE - 1)
#endif

#ifndef SETTINGS_MAX_SIZE
/** The maximum amount EEPROM dedicated to settings. */
#define SETTINGS_MAX_SIZE	(127)  /**< Defaults to 127 bytes */
#endif

#ifndef SETTINGS_TOP_ADDR
/** The top address in EEPROM that settings should use. Inclusive. */
#define SETTINGS_TOP_ADDR	(settings_iter_t)(EEPROM_END_ADDR-4)
#endif

/* The AVR tick interrupt usually is done with an 8 bit counter around 128 Hz.
 * 125 Hz needs slightly more overhead during the interrupt, as does a 32 bit
 * clock_time_t.
 */

/* Clock ticks per second */
#ifndef CLOCK_CONF_SECOND
#define CLOCK_CONF_SECOND                   125
#endif

#if 1
/* 16 bit counter overflows every ~10 minutes */
typedef unsigned short clock_time_t;
#define CLOCK_LT(a,b)  ((signed short)((a)-(b)) < 0)
#define INFINITE_TIME 0xffff
#else
typedef unsigned long clock_time_t;
#define CLOCK_LT(a,b)  ((signed long)((a)-(b)) < 0)
#define INFINITE_TIME 0xffffffff
#endif

/* Turn on radio statistics */
#ifndef RADIOSTATS
#define RADIOSTATS	1
#endif

/* Enable the alternate LED scheme, which avoids burning out your retina with the blue LED. */
#ifndef JACKDAW_CONF_ALT_LED_SCHEME
#define JACKDAW_CONF_ALT_LED_SCHEME		1
#endif

/* Enable this if you want to be able to swap our the RDC in the netstack */
#ifndef JACKDAW_CONF_USE_CONFIGURABLE_RDC
#define JACKDAW_CONF_USE_CONFIGURABLE_RDC   0
#endif

/* Maximum tick interval is 0xffff/125 = 524 seconds */
#define RIME_CONF_BROADCAST_ANNOUNCEMENT_MAX_TIME CLOCK_CONF_SECOND * 524UL     /* Default uses 600UL */
#define COLLECT_CONF_BROADCAST_ANNOUNCEMENT_MAX_TIME CLOCK_CONF_SECOND * 524UL  /* Default uses 600UL */

/* Mac address, RF channel, PANID from EEPROM settings manager */
#ifndef CONTIKI_CONF_SETTINGS_MANAGER
#define CONTIKI_CONF_SETTINGS_MANAGER       1
#endif

/* Generate random MAC address on first startup */
#ifndef CONTIKI_CONF_RANDOM_MAC
#define CONTIKI_CONF_RANDOM_MAC             1
#endif

/* Simple stack monitor. Status is displayed from the USB menu with 'm' command */
#ifndef CONFIG_STACK_MONITOR
#define CONFIG_STACK_MONITOR                1
#endif

/* Default radio channel */
#ifndef CHANNEL_802_15_4
#define CHANNEL_802_15_4                    26
#endif

/* Use the radio clock skew method for the random number generator */
#define RNG_CONF_USE_RADIO_CLOCK            1

/* COM port to be used for SLIP connection. Not tested on Jackdaw. */
#define SLIP_PORT RS232_PORT_0

/* Use the ADC noise method for the random number generator */
//#define RNG_CONF_USE_ADC                    1

/* Pre-allocated memory for loadable modules heap space (in bytes)*/
/* Default is 4096. Currently used only when elfloader is present. Not tested on Jackdaw */
//#define MMEM_CONF_SIZE 256

/* Starting address for code received via the codeprop facility. Not tested on Jackdaw */
//#define EEPROMFS_ADDR_CODEPROP 0x8000

/* ************************************************************************** */
// MARK: - Platform-specific types and functions
/* ************************************************************************** */

/* These routines are not part of the contiki core but can be enabled in cpu/avr/clock.c */
void clock_delay_msec(uint16_t howlong);

void clock_adjust_ticks(clock_time_t howmany);

typedef unsigned long off_t;

typedef unsigned short uip_stats_t;

#ifndef EEPROM_CONF_END_ADDR
#include <avr/eeprom.h>
#define EEPROM_CONF_END_ADDR		E2END
#endif

/* ************************************************************************** */
// MARK: - USB Configuration
/* ************************************************************************** */

/* Set USB_CONF_MACINTOSH to prefer CDC-ECM+DEBUG enumeration for Mac/Linux
 * Leave undefined to prefer RNDIS+DEBUG enumeration for Windows/Linux
 * TODO:Serial port would enumerate in all cases and prevent falling through to
 * the supported network interface if USB_CONF_MACINTOSH is used with Windows
 * or vice versa. The Mac configuration is set up to still enumerate as RNDIS-ONLY
 * on Windows (without the serial port).
 * At present the Windows configuration will not enumerate on the Mac at all,
 * since it wants a custom descriptor for USB composite devices.
 */
#ifndef USB_CONF_MACINTOSH
#define USB_CONF_MACINTOSH      0
#endif

/* Set USB_CONF_SERIAL to enable the USB serial port that allows control of the
 * run-time configuration (COMx on Windows, ttyACMx on Linux, tty.usbmodemx on Mac)
 * Debug printfs will go to this port unless USB_CONF_RS232 is set.
 */
#ifndef USB_CONF_SERIAL
#define USB_CONF_SERIAL         1
#endif

/* Disable mass storage enumeration for more program space */
/* TODO: Mass storage is currently broken */
#ifndef USB_CONF_STORAGE
#define USB_CONF_STORAGE        0
#endif

/* ************************************************************************** */
// MARK: - USB Ethernet Hooks
/* ************************************************************************** */

#define USB_HOOK_UNENUMERATED()   status_leds_unenumerated()
#define USB_ETH_HOOK_READY()      status_leds_ready()
#define USB_ETH_HOOK_INACTIVE()   status_leds_inactive()

/* ************************************************************************** */
// MARK: - USB CDC-ACM (Pseudo UART) Hooks
/* ************************************************************************** */

#define USB_CDC_ACM_HOOK_RX(char)         status_leds_serial_rx()
#define USB_CDC_ACM_HOOK_TX_END(char)     status_leds_serial_tx()
#define USB_CDC_ACM_HOOK_CLS_CHANGED(s)   status_leds_serial_rx()
#define USB_CDC_ACM_HOOK_CONFIGURED()     status_leds_serial_rx()

/* ************************************************************************** */
// MARK: - Serial Port Settings
/* ************************************************************************** */

/* RS232 debugs have less effect on network timing and are less likely
 * to be dropped due to buffer overflow. Only tx is implemented at present.
 * The tx pad is the middle one behind the jackdaw leds.
 * RS232 output will work with or without enabling the USB serial port
 */
#define USB_CONF_RS232           1

/* ************************************************************************** */
// MARK: - RF230BB Hooks
/* ************************************************************************** */

#define RF230BB_HOOK_RADIO_OFF()	status_leds_radio_off()
#define RF230BB_HOOK_RADIO_ON()	status_leds_radio_on()

/* ************************************************************************** */
// MARK: - RF230BB Settings
/* ************************************************************************** */

/* AUTOACK receive mode gives better rssi measurements,
 * even if ACK is never requested
 */
#define RF230_CONF_AUTOACK        1

/* 1 + Number of auto retry attempts 0-15 (0 implies don't use
 * extended TX_ARET_ON mode with CCA)
 */
#define RF230_CONF_FRAME_RETRIES    2

/* CCA theshold energy -91 to -61 dBm (default -77). Set this
 * smaller than the expected minimum rssi to avoid packet collisions.
 * The Jackdaw menu 'm' command is helpful for determining the
 * smallest ever received rssi.
 */
#define RF230_CONF_CCA_THRES    -85

/* Number of CSMA attempts 0-7. 802.15.4 2003 standard max is 5. */
#define RF230_CONF_CSMA_RETRIES    5

/* Allow sneeze command from jackdaw menu. Useful for testing CCA
 * on other radios. During sneezing, any access to an RF230 register
 * will hang the MCU and cause a watchdog reset The host interface,
 * jackdaw menu and rf230_send routines are temporarily disabled to
 * prevent this. But some calls from an internal uip stack might get
 * through, e.g. from CCA or low power protocols as temporarily disabling
 * all the possible accesses would add considerable complication to the
 * radio driver!
 */
#define RF230_CONF_SNEEZER        1

/* Allow sneeze command from jackdaw menu */
#define RF230_CONF_SNEEZE         1

/* 211 bytes per queue buffer */
#define QUEUEBUF_CONF_NUM        8

/* 54 bytes per queue ref buffer */
#define QUEUEBUF_CONF_REF_NUM    2

#define MAC_CONF_CHANNEL_CHECK_RATE 8

/* ************************************************************************** */
// MARK: - UIP Settings
/* ************************************************************************** */

/* These mostly have no effect when the Jackdaw is a repeater
 * (CONTIKI_NO_NET=1 using fakeuip.c)
 */

/* -------------------------------------------------------------------------- */
// MARK:      General UIP Settings

#define UIP_CONF_LL_802154       1
#define UIP_CONF_LLH_LEN         14
#define UIP_CONF_BUFSIZE         UIP_LINK_MTU + UIP_LLH_LEN + 4

#if UIP_CONF_IPV6
/* -------------------------------------------------------------------------- */
// MARK:      IPv6/6LoWPAN

#define NETSTACK_CONF_NETWORK     sicslowpan_driver

#define RIMEADDR_CONF_SIZE       8

/* See uip-ds6.h */
#define UIP_CONF_DS6_NBR_NBU     2
#define UIP_CONF_DS6_DEFRT_NBU   2
#define UIP_CONF_DS6_PREFIX_NBU  3
#define UIP_CONF_MAX_ROUTES   2
#define UIP_CONF_DS6_ADDR_NBU    3
#define UIP_CONF_DS6_MADDR_NBU   0
#define UIP_CONF_DS6_AADDR_NBU   0

#define UIP_CONF_ICMP6           1

#define SICSLOWPAN_CONF_COMPRESSION SICSLOWPAN_COMPRESSION_HC06
#define SICSLOWPAN_CONF_CONVENTIONAL_MAC    1

/* Request 802.15.4 ACK on all packets sent by
 * sicslowpan.c(else autoretry). Broadcasts will be
 * duplicated by the retry count, since no one will ACK them!
 */
#define SICSLOWPAN_CONF_ACK_ALL   0

/* Allow 6loWPAN fragmentation (more efficient for large payloads
 * over a reliable channel)
 */
#define SICSLOWPAN_CONF_FRAG      1

/* Timeout for fragment reassembly. A reissued browser GET will also
 * cancel reassembly, typically in 2-3 seconds
 */
#define SICSLOWPAN_CONF_MAXAGE    3

/* 10 bytes per stateful address context - see sicslowpan.c.
 * Default is 1 context with prefix aaaa::/64. These must agree
 * with all the other nodes or there will be a failure to communicate!
 */
#ifndef SICSLOWPAN_CONF_MAX_ADDR_CONTEXTS
#define SICSLOWPAN_CONF_MAX_ADDR_CONTEXTS 1
#endif

#define SICSLOWPAN_CONF_ADDR_CONTEXT_0 {addr_contexts[0].prefix[0]=0xaa;addr_contexts[0].prefix[1]=0xaa;}
#define SICSLOWPAN_CONF_ADDR_CONTEXT_1 {addr_contexts[1].prefix[0]=0xbb;addr_contexts[1].prefix[1]=0xbb;}
#define SICSLOWPAN_CONF_ADDR_CONTEXT_2 {addr_contexts[2].prefix[0]=0x20;addr_contexts[2].prefix[1]=0x01;addr_contexts[2].prefix[2]=0x49;addr_contexts[2].prefix[3]=0x78,addr_contexts[2].prefix[4]=0x1d;addr_contexts[2].prefix[5]=0xb1;}

#else // #if UIP_CONF_IPV6
/* -------------------------------------------------------------------------- */
// MARK:      IPv4/Rime

/* Warning: IPv4 should build but is thoroughly untested */

#define NETSTACK_CONF_NETWORK     rime_driver

#define RIMEADDR_CONF_SIZE        2

#endif /* UIP_CONF_IPV6 */

/* ************************************************************************** */
// MARK: - NETSTACK Settings
/* ************************************************************************** */

#define NETSTACK_CONF_RDC         nullrdc_driver
#define NETSTACK_CONF_MAC         nullmac_driver
#define NETSTACK_CONF_FRAMER      framer_802154
#define NETSTACK_CONF_RADIO       rf230_driver

#define NETSTACK_CONF_RDC_CHANNEL_CHECK_RATE 8

#if 0   /* sicslowmac */
#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC         sicslowmac_driver
#endif

#if 0   /* ContikiMAC */
#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC         contikimac_driver
#endif

#if 0   /* CX-MAC */
#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC         cxmac_driver
#undef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU      5
#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES     5
#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU    5
#endif

#if JACKDAW_CONF_USE_CONFIGURABLE_RDC
/* EXPERIMENTAL --- Configurable radio cycling */
struct rdc_driver;
extern const struct rdc_driver *rdc_config_driver;
#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC         (*rdc_config_driver)
#endif

/* Jackdaw has USB power, can be always listening */
#define CONTIKIMAC_CONF_RADIO_ALWAYS_ON  1

#define CXMAC_CONF_ANNOUNCEMENTS  0

/* following gives 50% duty cycle for CXMAC RDC, undef for default 5% */
#define CXMAC_CONF_ON_TIME (RTIMER_ARCH_SECOND / 16)

/* ************************************************************************** */
// MARK: - RPL Settings
/* ************************************************************************** */

#if UIP_CONF_IPV6_RPL

/* Not completely working yet. Works on Ubuntu after $ifconfig usb0 -arp to
 * drop the neighbor solitications. Dropping the NS on other OSs is more
 * complicated, see the following link for more information:
 * <http://www.sics.se/~adam/wiki/index.php/Jackdaw_RNDIS_RPL_border_router>.
 */

/* RPL requires the uip stack. Change #CONTIKI_NO_NET=1 to
 * UIP_CONF_IPV6=1 in the examples makefile, or include the needed
 * source files in /plaftorm/avr-ravenusb/Makefile.avr-ravenusb
 */

/* For the present the buffer_length calcs in rpl-icmp6.c will need
 * adjustment by the length difference between 6lowpan (0) and
 * ethernet (14) link-layer headers:
 *    // buffer_length = uip_len - uip_l2_l3_icmp_hdr_len;
 *    buffer_length = uip_len - uip_l2_l3_icmp_hdr_len + UIP_LLH_LEN; //Add jackdaw ethernet header
 */

/* Define MAX_*X_POWER to reduce tx power and ignore weak rx packets for testing
 * a miniature multihop network. Leave undefined for full power and sensitivity.
 * tx=0 (3dbm, default) to 15 (-17.2dbm)
 * RF230_CONF_AUTOACK sets the extended mode using the energy-detect register with rx=0 (-91dBm) to 84 (-7dBm)
 *   else the rssi register is used having range 0 (91dBm) to 28 (-10dBm)
 *   For simplicity RF230_MIN_RX_POWER is based on the energy-detect value and divided by 3 when autoack is not set.
 * On the RF230 a reduced rx power threshold will not prevent autoack if enabled and requested.
 * These numbers applied to both Raven and Jackdaw give a maximum communication distance of about 15 cm
 * and a 10 meter range to a full-sensitivity RF230 sniffer.
#define RF230_MAX_TX_POWER 15
#define RF230_MIN_RX_POWER 30
 */

#ifndef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER             1
#endif

#ifndef RPL_BORDER_ROUTER
#define RPL_BORDER_ROUTER           1
#endif

#ifndef RPL_CONF_STATS
#define RPL_CONF_STATS              0
#endif

#define UIP_CONF_BUFFER_SIZE	 1300

#define UIP_CONF_ND6_SEND_RA        0
#define UIP_CONF_ND6_REACHABLE_TIME 600000
#define UIP_CONF_ND6_RETRANS_TIMER  10000

#if RPL_BORDER_ROUTER
#undef UIP_FALLBACK_INTERFACE
#define UIP_FALLBACK_INTERFACE      rpl_interface
#endif

/* Save all the RAM we can */
#define PROCESS_CONF_NO_PROCESS_NAMES 1
#undef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM           2
#undef QUEUEBUF_CONF_REF_NUM
#define QUEUEBUF_CONF_REF_NUM       1
#undef UIP_CONF_TCP_SPLIT
#define UIP_CONF_TCP_SPLIT          0
#undef UIP_CONF_STATISTICS
#define UIP_CONF_STATISTICS         0
#undef UIP_CONF_IPV6_QUEUE_PKT
#define UIP_CONF_IPV6_QUEUE_PKT     0
#define UIP_CONF_PINGADDRCONF       0
#define UIP_CONF_LOGGING            0
#undef UIP_CONF_MAX_CONNECTIONS
#define UIP_CONF_MAX_CONNECTIONS    2
#undef UIP_CONF_MAX_LISTENPORTS
#define UIP_CONF_MAX_LISTENPORTS    2
#define UIP_CONF_UDP_CONNS          6
#define UIP_CONF_ICMP_DEST_UNREACH  1
#define UIP_CONF_DHCP_LIGHT
#undef UIP_CONF_FWCACHE_SIZE
#define UIP_CONF_FWCACHE_SIZE       30
#define UIP_CONF_BROADCAST          1

/* Experimental option to pick up a prefix from host interface router advertisements */
/* Requires changes in uip6 and uip-nd6.c to pass link-local RA broadcasts */
/* If this is zero the prefix will be manually set in contiki-raven-main.c */
#ifndef UIP_CONF_ROUTER_RECEIVE_RA
#define UIP_CONF_ROUTER_RECEIVE_RA  0
#endif

#endif /* UIP_CONF_IPV6_RPL */

/* ************************************************************************** */
// MARK: - Other Settings
/* ************************************************************************** */

#define CCIF
#define CLIF

#endif /* __CONTIKI_CONF_H__ */
