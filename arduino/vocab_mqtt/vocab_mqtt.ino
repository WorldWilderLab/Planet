/* Parts of this code are taken from Adafruit's differential example for their ADS1015 library */


#include <EthernetUdp.h>
#include <Dhcp.h>
#include <Ethernet.h>
#include <EthernetServer.h>
#include <util.h>
#include <EthernetClient.h>
#include <Dns.h>
#include <string.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <MedianFilter.h>
#include <Adafruit_ADS1015.h>
#include "eenet.h"


// version 
char version[] = "0.1 " __DATE__;


// refs & decs

void msetup(void);
void mloop(void);
void callback(char* topic, byte* payload, unsigned int length);

// change this to false if you want to use a manual ip address, enter this address in eenet.h!
const boolean USE_DHCP = true;


// sensor

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

#define INTEGRATION_TIME 100 // 5 cycles of 50Hz, 6 cycles of 60Hz


// filters

MedianFilter diff1(9,0);
MedianFilter diff2(9,0);


// MQTT

// where to publish / subscribe
// point this to your mqtt server. It could be a local machine, but for small numbers we can share a server.
char server[]      = "192.168.1.3"; // "mqtt.lan";

// this identifies your data. I currently have plants 1 to 3. Start yours at 10.
#define MQTTPREFIX   "/plant/3"


// input attenuator is 100k / (100k + (1M || (710k/2))
// where 100k is input resistor, 1M is bias to guard, 710k is differential input resistance

const float inputimpedance = 1.0/((1.0/1000000)+(1.0/710000));
const float attenuator = inputimpedance/(100000+inputimpedance);

  /* Be sure to update this value based on the input network, the IC and the gain settings! */
  //float multiplier = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */
const float multiplier = 0.015625F / attenuator; /* ADS1115  @ +/- 0.512 gain (16-bit results)  * 100K input, 1M to gnd  */

// Data types in variable storage

enum VARFLAGS { VSTRING = 1, VINT = 2, VFLOAT = 4, VCHANGED = 8 };

// server
EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);


// publishing list

struct publist {
       const char *topic;
       union { 
	 long d_long;
         const char * d_charp;
	 float d_float;
       } d, sent;
       unsigned long recent;
       unsigned int mintime;
       unsigned int maxtime;
       uint8_t flags;

} plist[] = { 
//  { "/light",   { 0 }, { 0 }, 0,500, 5000,  VFLOAT  },
  { "/probe/0",  { 0 }, { 0 }, 0,500, 5000,  VFLOAT  },
  { "/probe/1",  { 0 }, { 0 }, 0,500, 5000,  VFLOAT  },
//  { "/debug/loopt", { 0 }, { 0 },0,2000,25000, VINT    },
  { NULL }
};

enum PUBLISHIX { /*PLIGHT,*/ PPROBE0, PPROBE1, PLOOPT  };

#define VOLTJITTER   0.0002
#define LIGHTJITTER  5.0


float fabsf( float f )
{
  return (f < 0.0 ? -f : f);
}



void setup(void)
{
  Serial.begin(115200);
  //Serial.println("Hello!");
  
  //Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");
  //Serial.println("ADC Range: +/- 0.512V (1 bit = 0.015625mV/ADS1115)");
  
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  
  ads.begin();

  // connect to network
  msetup();
}

void vloop(void)
{
  int32_t results;
  uint32_t initial;
  int i;
  

  for ( results = 0, i = 0, initial = millis();  (millis()-initial) < INTEGRATION_TIME;  ) {
    results += ads.readADC_Differential_0_1();  
    ++i;
    //Serial.print(results/i); Serial.print(" after "); Serial.println(i);
  }
  plist[PPROBE0].d.d_float = ((float)results * multiplier )/ ((float)i*1000.0);
  
  //Serial.print("Differential 0/1: "); Serial.print(results/i); 
  //Serial.print(" after ");Serial.print(i); Serial.print(" samples "); 
  //Serial.print("("); Serial.print((float)results * multiplier / (float)i); Serial.print("mV)");
  //Serial.print("  ");

  for ( results = 0, i = 0, initial = millis();  (millis()-initial) < INTEGRATION_TIME;  ) {
    results += ads.readADC_Differential_2_3();  
    ++i;
    //Serial.print(results/i); Serial.print(" after "); Serial.println(i);
  }
  plist[PPROBE1].d.d_float = ((float)results * multiplier )/ ((float)i*1000.0);
    
  //Serial.print("Differential 2/3: "); Serial.print(results/i); 
  //Serial.print(" after ");Serial.print(i); Serial.print(" samples "); 
  //Serial.print("("); Serial.print(results * multiplier); Serial.print("mV)");
  //Serial.println("  ");
}



void update(void )
{
  struct publist *pubp = plist;
  unsigned int now = millis();

  for (pubp = plist; pubp->topic != NULL; ++pubp) {
    if (((now - pubp->recent) > pubp->maxtime)
    || (((now - pubp->recent) > pubp->mintime) && (pubp->flags & VCHANGED))) {

       switch(pubp->flags & (VINT|VFLOAT|VSTRING)) {

       case VINT:
       	    if (client.publish2(MQTTPREFIX, pubp->topic, pubp->d.d_long)) {
	       pubp->sent.d_long = pubp->d.d_long;
    	       pubp->flags &= ~VCHANGED;
       	       pubp->recent = now;
    	    }
	    break;

      case VFLOAT:
      	   if (client.publish2(MQTTPREFIX, pubp->topic, pubp->d.d_float)) {
	      pubp->sent.d_float = pubp->d.d_float;
    	      pubp->flags &= ~VCHANGED;
       	      pubp->recent = now;
    	   }
	   break;

      case VSTRING:
      	   if (client.publish2(MQTTPREFIX, pubp->topic, pubp->d.d_charp)) {
	      pubp->sent.d_charp = pubp->d.d_charp;
    	      pubp->flags &= ~VCHANGED;
       	      pubp->recent = now;
    	   }
	   break;

      default:
	  pubp->flags &= ~VCHANGED;
       	  pubp->recent = now;
	  break;
      }
    }
  }

}

// main service loop

void loop()
{
  static unsigned long loopref;
  unsigned long looptime = 0;
  
  // report looptime

  if (loopref > 0) {
  
    looptime = millis() - loopref;
  }
  loopref = millis();
  
  mloop();
  vloop();
  update();



  // report light level - not available yet. Plan to use i2c light sensor

//  plist[PLIGHT].d.d_float = 0.0;
//  if (fabsf(plist[PLIGHT].sent.d_float - plist[PLIGHT].d.d_float) > LIGHTJITTER) {
//    plist[PLIGHT].flags |= VCHANGED;
//  }


  // report probe voltages

  for(int i = PPROBE0; i <= PPROBE1; ++i ) {
    plist[i].d.d_float = 0.0;
    if (plist[i].d.d_float >= 0) {
      if (fabsf(plist[i].sent.d_float - plist[i].d.d_float) > VOLTJITTER) {
        plist[i].flags |= VCHANGED;
      }
    }
  }

/*
  // report looptime

  plist[PLOOPT].d.d_long = looptime;
  if (abs(plist[PLOOPT].sent.d_long - plist[PLOOPT].d.d_long) > 2) {
    plist[PLOOPT].flags |= VCHANGED;
  }
*/
}
  

void subscribe()
{
  if (client.connect(MQTTPREFIX, MQTTPREFIX "/status", 0, 1, "OFFLINE")) {

    /*
    struct sublist *subp;
    for (subp = slist; subp->topic != NULL; ++subp) {

    	client.subscribe2(MQTTPREFIX, subp->topic);
    }
    */

    client.publish2(MQTTPREFIX, "/status", "CONNECTED", 1);
    client.publish2(MQTTPREFIX, "/version", version, 1);

    char ip[] = "000.000.000.000", *p = ip;
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
      // print the value of each byte of the IP address:
      itoa(Ethernet.localIP()[thisByte],p, 10);
      p += strlen(p);
      *p++ = '.';
    }
    *--p='\0';
    client.publish2(MQTTPREFIX, "/ip", ip, 1);
  }
}

void callback(char* topic, byte* payload, unsigned int length) 
{
}


void msetup()
{

/* 
   no eeprom setup yet

  // Pull in altered presets if available from AVR EEPROM (if signature bytes are set)
  if ((eeprom_read_byte(EEPROM_SIG_1) == EEPROM_SIG_1_VALUE) 
  &&  (eeprom_read_byte(EEPROM_SIG_2) == EEPROM_SIG_2_VALUE)) {

    uint8_t i=0;
    // trace("Using EEPROM settings\r\n");
    // copy eeprom to registerBuffer, starting at address 2
    for (;i<18; i++) ee.raw[i] = eeprom_read_byte(EEPROM_DATA+i); 
  }
  else {
    // trace("Using Default Settings\r\n");
    ;
  }

*/

  // print your local MAC address:

  //Serial.print("\n" MQTTPREFIX "  ");
  for (byte thisByte = 0; thisByte < 5; thisByte++) {
    // print the value of each byte of the MAC address:
    Serial.print(ee.mem.mac[thisByte], HEX);
    Serial.print(":"); 
  }
  Serial.print(ee.mem.mac[5], HEX);
  Serial.print("  ");
  if(USE_DHCP) {
    Ethernet.begin(ee.mem.mac);
  } else {
    Ethernet.begin(ee.mem.mac, ee.mem.ip);
  }

  // print your local IP address:

  for (byte thisByte = 0; thisByte < 3; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.print(Ethernet.localIP()[3], DEC);
  Serial.println();

  subscribe();
}

void mloop()
{
  /* poll the mqtt lib and reconnect if broken */

  if (!client.loop())
    subscribe();

  
  // This is useful only for boards with an ethernet bootloader
  // Not needed for standard arduino or arduino ethernet.
  // Check for a connection that indicates we should reset to the bootloader
  // so another sketch can be uploaded.
  // Note: This approach means that if the sketch hangs later in the loop then any connection
  //       attempt will not be detected so reset will not occur. So, don't hang. :)
  /*
  EthernetClient reset_client = reset_server.available(); // Unfortunately requires a byte sent.
  
  if (slist[SRESET].d.d_long > 0 || reset_client) {
    client.publish2(MQTTPREFIX, "/status", "RESETTING");
    client.disconnect();    
    reset_client.stop();

    wdt_disable();  
    wdt_enable(WDTO_2S);
    while (1)
	;
  }
  */
}


  


