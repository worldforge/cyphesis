/**
 Worldforge Next Generation MetaServer

 Copyright (C) 2011 Sean Ryan <sryan@evercrack.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 */

/**
 * Assumptions:
 *		1) Any errors, or misbehaviour of any kind will be ignored / dropped
 *
 * 	NMT_NULL
 * 		Description: empty packet
 * 		Type Size: 4 bytes
 * 		Value: 0
 * 		Payload Size: 0
 *
 * 	NMT_SERVERKEEPALIVE
 * 		Description: A game server keep-alive packet, part 1 of a 3-packet handshake
 * 		Type Size: 4 bytes
 * 		Value: 1 ( 00 00 00 01 )
 * 		Payload Size: 0
 *
 * 	NMT_HANDSHAKE
 * 		Description: The metaserver response to a SERVERKEEPALIVE or CLIENTKEEPALIVE, part 2 of 3-packet handshake
 * 		             Contains a random number.
 * 		Type Size: 4 bytes
 * 		Value: 3 ( 00 00 00 03 )
 * 		Payload Size: 4 bytes ( a random int )
 *
 * 	NMT_SERVERSHAKE
 * 		Description: The game server response to a HANDSHAKE, part 3 of 3-packet handshake.
 * 					 Triggers a new game server sesssion to be created, or updated.
 * 		Type Size: 4 bytes
 * 		Value: 4 ( 00 00 00 04 )
 * 		Payload Size: 4 bytes ( random int )
 *
 * 	NMT_CLIENTKEEPALIVE
 * 		Description: The game client keep-alive packet, part 1 of 3-packet handshake
 *		Type Size: 4 bytes
 *		Value: 2 ( 00 00 00 02 )
 *		Payload Size: 0
 *
 *	NMT_CLIENTSHAKE
 *		Description: The game client response to a HANDSHAKE, part 3 of a 3-packet handshake
 *		Type Size: 4 bytes
 *		Value: 5 ( 00 00 00 05 )
 * 		Payload Size: 4 bytes ( random int )
 *
 * 	NMT_LISTREQ
 * 		Description: A client request to give a list of servers
 * 		Type Size: 4 bytes
 * 		Value: 7 ( 00 00 00 07 )
 * 		Payload Size: 4 bytes ( int of the offset )
 *
 * 	NMT_LISTRESP
 * 		Description: The metaserver response with the values of IPs.
 * 		Type Size: 4 bytes
 * 		Value: 8 ( 00 00 00 08 )
 * 		Payload Size: Variable
 * 			4 bytes - int total servers sent
 * 			4 bytes - int packed servers sent
 * 			4 bytes - a 4 byte block representing EACH IP of the servers.  So if packed was set to 6,
 * 			          this block would be 24 bytes long.
 *
 *  NMT_DNSREQ
 *  	Description: Non-authenticated query in order find the queried server
 *  	Type Size: 4 bytes
 *  	Value: 32 ( 00 00 00 32 )
 *  	Payload Size: Variable
 *  	    4 bytes - int size of the string
 *  	    n bytes - 4 bytes per n
 *
 *		Note: response to this is going to be a dnsresp, almost identical to listresp.
 *
 *	NMT_DNSRESP
 *		Description: packed response to a dnsreq
 *		Type Size: 4 bytes
 *		Value: 33 ( 00 00 00 33 )
 *		Payload Size: Variable
 *			4 bytes - total server sent
 *			4 bytes - packed server sent
 *				4 bytes - length of string, one for each packed server, so 3 servers, has 12 (4*3)
 *				          int bytes
 *			n bytes - entire concatenated msg that will be sum of the size of all packed server
 *			          lengths
 *
 *		E.g: 33 05 05 06 04 07 07 04 foobarwaka1.2.3.4a.b.c.dtest
 *
 *		33 (4) - for packet type
 *		05 (4) - int total server count
 *		05 (4) - int packed server count
 *		06 (4) - length of resp 1
 *		04 (4) - length 0f resp 2
 *		07 (4) - length of resp 3
 *		07 (4) - length of resp 4
 *		04 (4) - length of resp 5
 *		foo(28) - message
 *				  (6) foobar - server 1
 *				  (4) waka   - server 2
 *				  (7) 1.2.3.4 - server 3
 *				  (7) a.b.c.d - server 4
 *				  (4) test    - server 5
 *		Total: 32 + 28 == 60 bytes ( plus about 12 bytes overhead )
 *
 *		Reasonably likely to fit 10 responses or so per packet, it's a bit heavier that the listresp
 *		because it's parsing IPs as strings ... so rather than 1.2.3.4 becoming 4 bytes, it's now
 *		7 ... and 123.123.123.123 is 15 bytes, instead of 4.
 *
 *Example Use Cases:
 *
 *	Use Case 1: Server Registration or Keep-alive.  Only servers with sessions can perform additional
 *	            requests, such as attribute registration/removal.
 *		1) Game Server sends a NMT_SERVERKEEPALIVE
 *		2) MS responds with a NMT_HANDSHAKE, contains a random number, creates a handshake
 *		3) Game Server responds with NMT_SERVERSHAKE, contains same random number AND handshake exists,
 *		   creates session ( if session exists, expiry time is adjusted )
 *
 *	Use Case 2: Game Server Shutdown / Quit
 *		1) Game Server sends a NMT_TERMINATE, if session exists, it is removed.
 *
 *	Use Case 3: Client Registration or Keep-alive.  Only clients with sessions can perform additional
 *				requests, such as LIST, FILTER, etc.
 *		1) Game Client sends a NMT_CLIENTKEEPALIVE
 *		2) MS responds with a NMT_HANDSHAKE, contains random number, creates a handshake
 *		3) Game Client responds with NMT_CLIENTSHAKE, contains same random number AND handshake exists,
 *		   creates a client session ( if session exists, expiry time is adjusted ).
 */
#ifndef METASERVERPROTOCOL_HPP_
#define METASERVERPROTOCOL_HPP_

#include <cstdint> // uint32_t

typedef uint32_t NetMsgType;

/*
 * Original metaserver protocol
 */
const NetMsgType NMT_NULL = 0;
const NetMsgType NMT_SERVERKEEPALIVE = 1;
const NetMsgType NMT_CLIENTKEEPALIVE = 2;
const NetMsgType NMT_HANDSHAKE = 3;
const NetMsgType NMT_SERVERSHAKE = 4;
const NetMsgType NMT_CLIENTSHAKE = 5;
const NetMsgType NMT_TERMINATE = 6;
const NetMsgType NMT_LISTREQ = 7;
const NetMsgType NMT_LISTRESP = 8;
const NetMsgType NMT_PROTO_ERANGE = 9;
const NetMsgType NMT_LAST = 10;

/**
 *  Enhancements of the metaserver-ng
 */
const NetMsgType NMT_SERVERATTR = 11;
const NetMsgType NMT_CLIENTATTR = 12;
const NetMsgType NMT_CLIENTFILTER = 13;
const NetMsgType NMT_ATTRRESP = 14;
const NetMsgType NMT_SERVERCLEAR = 15;
const NetMsgType NMT_CLIENTCLEAR = 16;

/*
 * Monitor Controls
 *
 * Intended as a read-only peek at the internals
 * of the metaserver
 *
 * 0 VERSION
 * 1 APIVERSION
 * 2 PACKETS
 * 3 STARTED
 * 4 SERVERS
 * 5 CLIENTS
 * 6 MAXSERVERS
 *
 */
const NetMsgType NMT_MONITOR = 21;

/*
 * Administrator Controls
 *
 * Intended to provide the ability to affect the
 * data inside the metaserver
 *
 * @TODO thing about whether we want just yes/no type responses from
 * admin packets
 */
const NetMsgType NMT_ADMIN = 25;

/*
 * DNS Hooks
 */
const NetMsgType NMT_DNSREQ = 32;
const NetMsgType NMT_DNSRESP = 33;

/*
 * DNS Query Types
 *
 * Leaves options for future expansion by type.
 *
 * Future considerations to:
 *
 * TXT
 * AXFR
 */
const NetMsgType DNS_TYPE_ALL = 0;
const NetMsgType DNS_TYPE_A = 1;
const NetMsgType DNS_TYPE_PTR = 2;


/*
 * Convenience Printing
 */
const char DNS_PRETTY[][30] = {
		"DNS_TYPE_ALL",
		"DNS_TYPE_A",
		"DNS_TYPE_PTR"
};

/**
 * Convenience Printing
 */
const char NMT_PRETTY[][30] = {
		"NMT_NULL",
		"NMT_SERVERKEEPALIVE",
		"NMT_CLIENTKEEPALIVE",
		"NMT_HANDSHAKE",
		"NMT_SERVERSHAKE",
		"NMT_CLIENTSHAKE",
		"NMT_TERMINATE",
		"NMT_LISTREQ",
		"NMT_LISTRESP",
		"NMT_PROTO_ERANGE",
		"NMT_LAST",
		"NMT_SERVERATTR",
		"NMT_CLIENTATTR",
		"NMT_CLIENTFILTER",
		"NMT_ATTRRESP",
		"NMT_SERVERCLEAR",
		"NMT_CLIENTCLEAR",
		"RESERVED17",
		"RESERVED18",
		"RESERVED19",
		"RESERVED20",
		"RESERVED21",
		"RESERVED22",
		"RESERVED23",
		"RESERVED24",
		"RESERVED25",
		"RESERVED26",
		"RESERVED27",
		"RESERVED28",
		"RESERVED29",
		"RESERVED30",
		"RESERVED31",
		"NMT_DNSREQ",
		"NMT_DNSRESP"
};

#endif /* METASERVERPROTOCOL_HPP_ */
