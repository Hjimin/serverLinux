/* C runtime includes */
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <conio.h>

/* lwIP core includes */
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/tcp.h"

/* lwIP netif includes */
#include "netif/etharp.h"
void my_server_init(void);
void my_client_init(void);

/* NO_SYS == 1: we need information about the timer intervals: */
#include "lwip/ip_frag.h"
#include "lwip/igmp.h"

/* ethernet layer include */
#include "pktif.h"

/* include the port-dependent configuration */
#include "lwipcfg_msvc.h"

/* port-defined functions used for timer execution, defined in sys_arch.c */
void sys_init_timing();
u32_t sys_now();

/* globales variables for netifs */
/* THE ethernet interface */
struct netif netif;

/* special functions used for NO_SYS==1 only */
typedef struct _timers_infos
{
   int timer;
   int timer_interval;
   void (*timer_func)(void);
} timers_infos;

static timers_infos timers_table[] =
{
   { 0, TCP_FAST_INTERVAL,       tcp_fasttmr},
   { 0, TCP_SLOW_INTERVAL,       tcp_slowtmr},
   { 0, ARP_TMR_INTERVAL,        etharp_tmr},
   { 0, IGMP_TMR_INTERVAL,       igmp_tmr},
   { 0, IP_TMR_INTERVAL,         ip_reass_tmr},
};

/* get the current time and see if any timer has expired */
static void timers_update()
{
   /* static variables for timer execution, initialized to zero! */
	static int last_time;

	int cur_time, time_diff, idxtimer;


         cur_time = sys_now();
         time_diff = cur_time - last_time;
      
         /* the '> 0' is an easy wrap-around check: the big gap at
          * the wraparound step is simply ignored... */
         if (time_diff > 0)
         {
         	last_time = cur_time;
  
  		for( idxtimer=0; idxtimer<(sizeof(timers_table)/sizeof(timers_infos)); idxtimer++)
		{
			timers_table[idxtimer].timer += time_diff;
  			if (timers_table[idxtimer].timer > timers_table[idxtimer].timer_interval){
                 		timers_table[idxtimer].timer_func();
  				timers_table[idxtimer].timer -= timers_table[idxtimer].timer_interval;
           		}
       		}
   	 }
}

void status_callback(struct netif *netif)
{
	if (netif_is_up(netif)){ 
		printf("status_callback==UP, local interface IP is %s\n", inet_ntoa(*(struct in_addr*)&(netif->ip_addr)));
   	} else {
		printf("status_callback==DOWN\n");
   	}
}

void link_callback(struct netif *netif)
{
       if (netif_is_link_up(netif))
       {
           printf("link_callback==UP\n");
       }
       else
       {
           printf("link_callback==DOWN\n");
       }
}

/* This function initializes all network interfaces */
static void msvc_netif_init()
{
   struct ip_addr ipaddr, netmask, gw;

   LWIP_PORT_INIT_GW(&gw);
   LWIP_PORT_INIT_IPADDR(&ipaddr);
   LWIP_PORT_INIT_NETMASK(&netmask);

printf("Starting lwIP, local interface IP is %s\n", inet_ntoa(*(struct in_addr*)&ipaddr));


netif_set_default(netif_add(&netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, ethernet_input));


   netif_set_status_callback(&netif, status_callback);

   netif_set_link_callback(&netif, link_callback);

   netif_set_up(&netif);
}

void main_loop()
{
   /* initialize lwIP stack, network interfaces and application */
   sys_init_timing();
   lwip_init();

/* init network interfaces */

   msvc_netif_init();

   /* init application */

//my_server_init(); /* My Server */



   /* MAIN LOOP for driver update (and timers if NO_SYS) */
   while (!_kbhit())
   {
       /* handle timers (already done in tcpip.c when NO_SYS=0) */
       timers_update();

       /* check for packets and link status*/
       ethernetif_poll(&netif);

       /* check for loopback packets on all netifs */
       netif_poll_all();
   }

   /* release the pcap library... */
   ethernetif_shutdown(&netif);
}

int main(void)
{
   /* no stdio-buffering, please! */
   setvbuf(stdout, NULL,_IONBF, 0);

   main_loop();

   return 0;
}


