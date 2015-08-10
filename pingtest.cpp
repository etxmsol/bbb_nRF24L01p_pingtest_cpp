/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example RF Radio Ping Pair
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two different nodes,
 * connect the role_pin to ground on one.  The ping node sends the current time to the pong node,
 * which responds by sending the value back.  The ping node can then see how long the whole cycle
 * took.
 */

#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <RF24_Impl.h>
#include <compatibility.h>

// sets the role of this unit in hardware.  Connect to GND to be the 'pong' receiver
// Leave open to be the 'ping' transmitter
const int role_pin = 7;

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes
// in this system.  Doing so greatly simplifies testing.  The hardware itself specifies
// which node it is.
//
// This is done through the role_pin
//

// The various roles supported by this sketch
typedef enum { role_ping_out = 1, role_pong_back } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role;



int main(int argc, char** argv)
{
	RF24_Impl rf24;

    role = role_ping_out;

    printf("\n\rRF24/examples/pingpair/\n\r");
    printf("ROLE: %s\n\r",role_friendly_name[role]);

    rf24.begin();

    rf24.setRetries(15,15);

    rf24.setChannel(10);
	rf24.setPALevel(RF24_PA_MAX);
	rf24.setAutoAck(true);


	rf24.startListening();

	  //
	  // Dump the configuration of the rf unit for debugging
	  //

	rf24.printDetails();

	while(1)
	{
	  if (role == role_ping_out)
	  {
		// First, stop listening so we can talk.
		rf24.stopListening();

		// Take the time, and send it.  This will block until complete
		unsigned long time = __millis();
		printf("Now sending %lu...",time);
		bool ok = rf24.write( &time, sizeof(unsigned long) );
		printf("sizeof = %d\n", sizeof(unsigned long));
		if (ok)
		  printf("ok...");
		else
		  printf("failed.\n\r");

		// Now, continue listening
		rf24.startListening();

		// Wait here until we get a response, or timeout (250ms)
		unsigned long started_waiting_at = __millis();
		bool timeout = false;
		while ( ! rf24.available() && ! timeout ) {
		__msleep(5); //add a small delay to let radio.available to check payload
		  if (__millis() - started_waiting_at > 200 )
			timeout = true;
		}

		// Describe the results
		if ( timeout )
		{
		  printf("Failed, response timed out.\n\r");
		}
		else
		{
		  // Grab the response, compare, and send to debugging spew
		  unsigned long got_time;
		  rf24.read( &got_time, sizeof(unsigned long) );

		  // Spew it
		  printf("Got response %lu, round-trip delay: %lu\n\r",got_time,__millis()-got_time);
		}

		// Try again 1s later
		//    delay(1000);
		sleep(1);
	  }

	  //
	  // Pong back role.  Receive each packet, dump it out, and send it back
	  //

	  if ( role == role_pong_back )
	  {
		// if there is data ready
		if ( rf24.available() )
		{
		  // Dump the payloads until we've gotten everything
		  unsigned long got_time;
		  bool done = false;
		  while (!done)
		  {
			// Fetch the payload, and see if this was the last one.
			done = rf24.read( &got_time, sizeof(unsigned long) );

			// Spew it
			printf("Got payload %lu...",got_time);

		// Delay just a little bit to let the other unit
		// make the transition to receiver
			__msleep(20);
		  }

		  // First, stop listening so we can talk
		  rf24.stopListening();

		  // Send the final one back.
		  printf("Sent response.\n\r");
		  rf24.write( &got_time, sizeof(unsigned long) );

		  // Now, resume listening so we catch the next packets.
		  rf24.startListening();
		}
	  }
	}
	return 0;
}


// vim:cin:ai:sts=2 sw=2 ft=cpp
