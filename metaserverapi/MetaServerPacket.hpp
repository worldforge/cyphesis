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

#ifndef METASERVERPACKET_HPP_
#define METASERVERPACKET_HPP_

/*
 * Local Includes
 */
#include "MetaServerProtocol.hpp"

/*
 * System Includes
 */
#include <iosfwd>
#include <cstring>      // memcpy
#include <netinet/in.h> // htonl
#include <array>
#include <string>

#define MAX_PACKET_BYTES 1024
#define MAX_UDP_OUT_BYTES 570

/**
 * The purpose of the MetaServerPacket is so simplify the process of
 * sending and receiving data from the metaserver.
 *
 * As defined specifically in the MetaServerProtocol header, the
 * different supported packet types are listed and detailed.
 *
 * There are 2 main use cases:
 *
 * 1: Sending a metaserver information
 *    In this case you need to define a MSP, populate the type, and
 *    then add data as required for that specific packet type.
 *
 *    Example: sending a KEEPALIVE
 *
 *    MetaServerPacket msp;
 *    msp.setPacketType(NMT_SERVERKEEPALIVE);
 *    byteStreamSender << msp.getBuffer().c_array()
 *
 *    NOTE: there are many other additional pieces of information that
 *    you CAN set, like address, and port, etc.  These constructs are
 *    available, because the metaserver makes use of them internally as
 *    it passes the data around ( and any process you make could too ),
 *    but there is no requirement to fill them (or to have them set) just
 *    for transportation.  The only mandatory element is the packet type (
 *    the size is mandatory too, but it is calculated from the buffer ).
 *
 * 2: Receiving Information
 *
 * 	  std::array<char,MAX_PACKET_BYTES> dataIn;
 * 	  ... assume that byte stream is read into, and that the transport
 * 	  mechanism provies the number of bytes received.
 *
 * 	  MetaServerPacket msp( dataIn, bytes_received );
 * 	  int handshake = 0;
 * 	  if ( msp.getPacketType() == NMT_HANDSHAKE )
 * 	     handshake = msp.getIntData(4);
 *
 * 	  NOTE: Data is stored either as Int [uint32_t specifically]
 * 	  ( ala getIntData/addIntData ) or a string.
 *
 * 	     i) msp.getPacketType() == msp.getIntData(0).  The first uint32_t is
 * 	     always reserved for the packet type.
 * 	     ii) All additional data is determined based on the protocol.  For
 * 	     example, the NMT_HANDSHAKE has a second int that represents the random
 * 	     number, and this represents the next uint32_t ( which is 4 bytes long )
 *
 *	   For a complete example, refer to the TestClient.cpp and TestServer.cpp
 *	   that cover both aspects.
 *
 */
class MetaServerPacket
{

public:
	MetaServerPacket();
	MetaServerPacket(const std::array<char,MAX_PACKET_BYTES>& pl, std::size_t bytes = 0 );
	~MetaServerPacket();

	const NetMsgType getPacketType() const { return m_packetType; }
	void setPacketType(const NetMsgType& nmt);

	const std::string getAddressStr() const { return m_AddressStr; }
	const uint32_t getAddressInt() const { return m_AddressInt; }
	const std::string getAddress() const { return m_AddressStr; }
	void setAddress(const std::string& address);

	void setSequence(const unsigned long long seq = 0) { m_Sequence = seq; }
	unsigned long long getSequence() const { return m_Sequence; }

	void setTimeOffset(const unsigned long long to = 0 ) { m_TimeOffset = to; }
	unsigned long long getTimeOffset() const { return m_TimeOffset; }

	unsigned int getPort() const { return m_Port; }
	void setPort(unsigned int p) { m_Port = p; }

	bool getOutBound() { return m_outBound; }
	void setOutBound(bool s) { m_outBound = s; }

	std::size_t getSize() const { return m_Bytes; }

	unsigned int addPacketData(uint32_t i);
	unsigned int addPacketData(const std::string& s);

	const std::string getPacketMessage(unsigned int offset) const;
	uint32_t getIntData(unsigned int offset) const;

	uint32_t	IpAsciiToNet(const char *buffer);
	std::string IpNetToAscii(uint32_t address);

	std::array<char,MAX_PACKET_BYTES> getBuffer() const { return m_packetPayload; }
	void setBuffer( std::array<char,MAX_PACKET_BYTES>& pl, std::size_t bytes = 0 )
	{
		m_packetPayload = pl;
		m_Bytes = bytes;
	}

	void parsePacketType();

private:


	char *pack_uint32(uint32_t data, char* buffer );
	char *unpack_uint32(uint32_t *dest, char* buffer ) const;
	char *pack_string(std::string str, char *buffer );
	char *unpack_string(std::string *dest, char* buffer, unsigned int length ) const;

	NetMsgType m_packetType;
	uint32_t m_AddressInt;
	std::string m_AddressStr;
	unsigned int m_Port;
	std::size_t m_Bytes;
	char * m_headPtr;
	char * m_writePtr;
	char * m_readPtr;
	std::array<char,MAX_PACKET_BYTES> m_packetPayload;
	bool m_needFree;
	bool m_outBound;
	unsigned long long m_Sequence;
	unsigned long long m_TimeOffset;

};

#endif /* METASERVERPACKET_HPP_ */
