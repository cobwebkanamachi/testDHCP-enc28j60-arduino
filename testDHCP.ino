//https://github.com/jcw/ethercard/blob/master/examples/testDHCP/testDHCP.ino
// Arduino demo sketch for testing the DHCP client code
//
// Original author: Andrew Lindsay
// Major rewrite and API overhaul by jcw, 2011-06-07
//
// Copyright: GPL V2
// See http://www.gnu.org/licenses/gpl.html

#include <EtherCard.h>

static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[700];
static uint32_t timer;

// called when a ping comes in (replies to it are automatic)
static void gotPinged (byte* ptr) {
  ether.printIp(">>> ping from: ", ptr);
}


void setup () {
  Serial.begin(57600);
  Serial.println(F("\n[testDHCP]"));

  Serial.print("MAC: ");
  for (byte i = 0; i < 6; ++i) {
    Serial.print(mymac[i], HEX);
    if (i < 5)
      Serial.print(':');
  }
  Serial.println();
  Serial.print( F("\nENC28J60 Revision ") );

  uint8_t rev = ether.begin(sizeof Ethernet::buffer, mymac);
  Serial.println( rev, DEC );
  if (rev == 0) {
    Serial.println(F("Failed to access Ethernet controller"));
  }
  
  Serial.println(F("Setting up DHCP"));
  if (!ether.dhcpSetup())
    Serial.println(F("DHCP failed"));
  
  ether.printIp("My IP: ", ether.myip);
  ether.printIp("Netmask: ", ether.netmask);
  ether.printIp("GW IP: ", ether.gwip);
  ether.printIp("DNS IP: ", ether.dnsip);
    // call this to report others pinging us
  ether.registerPingCallback(gotPinged);
    timer = -9999999; // start timing out right away

    
}

void loop () {
  word len = ether.packetReceive(); // go receive new packets
  word pos = ether.packetLoop(len); // respond to incoming pings
  
  // report whenever a reply to our outgoing ping comes back
  if (len > 0 && ether.packetLoopIcmpCheckReply(ether.hisip)) {
    Serial.print("  ");
    Serial.print((micros() - timer) * 0.001, 3);
    Serial.println(" ms");
  }
  
  // ping a remote server once every few seconds
  if (micros() - timer >= 5000000) {
    ether.hisip[0]=172;   //you should change this with your ping taget.
    ether.hisip[1]=0;     //you should change this with your ping taget.
    ether.hisip[2]=0;     //you should change this with your ping taget.
    ether.hisip[3]=1;     //you should change this with your ping taget.
    ether.printIp("to ping: ", ether.hisip);
    ether.printIp("Pinging: ", ether.hisip);
    timer = micros();
    ether.clientIcmpRequest(ether.hisip);
  }
}
