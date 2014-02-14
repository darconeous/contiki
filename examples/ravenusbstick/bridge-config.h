
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define USB_CONF_MACINTOSH			1
#define USB_CONF_RS232                          1

#define CONTIKI_CONF_DEFAULT_HOSTNAME   "jackdaw"

#define NETSTACK_CONF_BRIDGE_MODE	1
#define JACKDAW_CONF_USE_USB_ETH	1
#define JACKDAW_CONF_USE_RADIO		1
#define RADIOALWAYSON               1
#define JACKDAW_CONF_USE_CONFIGURABLE_RDC 1
#define SETTINGS_CONF_INCLUDE_DEBUG_DUMP 0

#define NULLRDC_CONF_ADDRESS_FILTER	0

#ifndef UIP_CONF_IPV6_RPL
#define UIP_CONF_IPV6_RPL			0
#endif

#define RPL_HTTPD_SERVER            UIP_CONF_IPV6_RPL

#define CDC_ECM_USES_INTERRUPT_ENDPOINT	1

#if !defined(DEBUGFLOWSIZE) && DEBUG
#define DEBUGFLOWSIZE					100
#endif


#define SICSLOW_ETHERNET_CONF_UPDATE_USB_ETH_STATS 1

#define USB_ETH_CONF_PRODUCT_NAME "Jackdaw 6LoWPAN Adapter"
#define	USB_ETH_HOOK_IS_READY_FOR_INBOUND_PACKET()		rf230_is_ready_to_send()
#define USB_ETH_HOOK_HANDLE_INBOUND_PACKET(buffer,len)	do { uip_len = len ; mac_ethernetToLowpan(buffer); } while(0)
#define USB_ETH_HOOK_SET_PROMISCIOUS_MODE(value)	rf230_set_promiscuous_mode(value)
#define USB_ETH_HOOK_INIT()		mac_ethernetSetup()

#define RF230BB_HOOK_TX_PACKET(buffer,total_len) mac_log_802_15_4_tx(buffer,total_len)
#define RF230BB_HOOK_RX_PACKET(buffer,total_len) mac_log_802_15_4_rx(buffer,total_len)
#define	RF230BB_HOOK_IS_SEND_ENABLED()	mac_is_send_enabled()
extern bool mac_is_send_enabled(void);

extern void mac_log_802_15_4_tx(const uint8_t * buffer, size_t total_len);

extern void mac_log_802_15_4_rx(const uint8_t * buffer, size_t total_len);

#define UIP_CONF_LLH_LEN         14
#define UIP_CONF_BUFSIZE		UIP_LINK_MTU + UIP_LLH_LEN + 4  // +4 for vlan on macosx
#define UIP_CONF_MAX_CONNECTIONS 0
#define UIP_CONF_MAX_LISTENPORTS 0
#define UIP_CONF_IP_FORWARD      0
#define UIP_CONF_FWCACHE_SIZE    0
#define UIP_CONF_IPV6            1
#define UIP_CONF_IPV6_CHECKS     1
#define UIP_CONF_IPV6_QUEUE_PKT  0
#define UIP_CONF_IPV6_REASSEMBLY 0
#define UIP_CONF_NETIF_MAX_ADDRESSES  3
#define UIP_CONF_ND6_MAX_PREFIXES     3
#define UIP_CONF_ND6_MAX_NEIGHBORS    4
#define UIP_CONF_ND6_MAX_DEFROUTERS   2
#define UIP_CONF_UDP             0
#define UIP_CONF_TCP             0
#define UIP_CONF_TCP_SPLIT       0
#define UIP_CONF_STATISTICS      0

/* Optional, TCP needed to serve the RPL neighbor web page currently hard coded at bbbb::200 */
/* The RPL neighbors can also be viewed using the jackdaw menu */
/* A small MSS is adequate for the internal jackdaw webserver and RAM is very limited*/
#if RPL_HTTPD_SERVER
//#undef UIP_CONF_UDP
//#define UIP_CONF_UDP                1
#undef UIP_CONF_TCP
#define UIP_CONF_TCP                1
#define UIP_CONF_TCP_MSS           48
#define UIP_CONF_RECEIVE_WINDOW    48
#undef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU        5
#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU      5
#undef UIP_CONF_MAX_CONNECTIONS
#define UIP_CONF_MAX_CONNECTIONS    2
#endif
