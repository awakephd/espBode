#ifndef ESPBODE_H
#define ESPBODE_H

/*  Comment this for DHCP. However, you'll
    need to obtain IP somehow. */

#define STATIC_IP

/* Static ip configuration */

#ifdef STATIC_IP
  #define ESP_IP              192,168,0,155       // Choose an IP address that is available on the network; for best results, reserve this address in the router
  #define ESP_MASK            255,255,255,0       // Typical mask for a local network
  #define ESP_GW              192,168,0,1         // Set the gateway as appropriate for the wifi network
#endif

/*  Select either AP or CLIENT mode:
    - AP - creates new network that oscilloscope can connect to
    - CLIENT - joins existing network */

//#define WIFI_MODE_AP
#define WIFI_MODE_CLIENT

/*  Character string used to simulate a Siglent AWG */

const char SIGLENT_ID[] = "IDN-SGLT-PRI SDG1062X";

/*  The ESP-01 requires a LOW output to turn on
    the built-in LED */

const int LED_ON = LOW;
const int LED_OFF = HIGH;

/*  The following enumeration is used by the loop_state variable to
    indicate the current state when processing the main loop. */

enum Loop_State {
  ls_passthrough,
  ls_ready_for_rpc,
  ls_wait_for_rpc,
  ls_ready_for_vxi,
  ls_wait_for_vxi
};

/*  The loop_state variable is created in espBode.ino */

extern Loop_State loop_state;

#endif