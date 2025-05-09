/**
* Copyright [2013-2014] [OHsystem]
*
* We spent a lot of time writing this code, so show some respect:
* - Do not remove this copyright notice anywhere (bot, website etc.)
* - We do not provide support to those who removed copyright notice
*
* OHSystem is free software: You can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* You can contact the developers on: admin@ohsystem.net
* or join us directly here: http://ohsystem.net/forum/
*
* Visit us also on http://ohsystem.net/ and keep track always of the latest
* features and changes.
*
*
* This is modified from GHOST++: http://ghostplusplus.googlecode.com/
* Official GhostPP-Forum: http://ghostpp.com/
*/

#include "ghost.h"
#include "util.h"
#include "socket.h"
#include "commandpacket.h"
#include "bnlsprotocol.h"
#include "bnlsclient.h"

//
// CBNLSClient
//

CBNLSClient :: CBNLSClient( string nServer, uint16_t nPort, uint32_t nWardenCookie ) : m_WasConnected( false ), m_Server( nServer ), m_Port( nPort ), m_LastNullTime( 0 ), m_WardenCookie( nWardenCookie ), m_TotalWardenIn( 0 ), m_TotalWardenOut( 0 )
{
    m_Socket = new CTCPClient( );
    m_Protocol = new CBNLSProtocol( );
}

CBNLSClient :: ~CBNLSClient( )
{
    delete m_Socket;
    delete m_Protocol;

    while( !m_Packets.empty( ) )
    {
        delete m_Packets.front( );
        m_Packets.pop( );
    }
}

BYTEARRAY CBNLSClient :: GetWardenResponse( )
{
    BYTEARRAY WardenResponse;

    if( !m_WardenResponses.empty( ) )
    {
        WardenResponse = m_WardenResponses.front( );
        m_WardenResponses.pop( );
        ++m_TotalWardenOut;
    }

    return WardenResponse;
}

unsigned int CBNLSClient :: SetFD( void *fd, void *send_fd, int *nfds )
{
    if( !m_Socket->HasError( ) && m_Socket->GetConnected( ) )
    {
        m_Socket->SetFD( (fd_set *)fd, (fd_set *)send_fd, nfds );
        return 1;
    }

    return 0;
}

bool CBNLSClient :: Update( void *fd, void *send_fd )
{
    if( m_Socket->HasError( ) )
    {
        CONSOLE_Print( "[BNLSC: " + m_Server + ":" + UTIL_ToString( m_Port ) + ":C" + UTIL_ToString( m_WardenCookie ) + "] disconnected from BNLS server due to socket error" );
        return true;
    }

    if( !m_Socket->GetConnecting( ) && !m_Socket->GetConnected( ) && m_WasConnected )
    {
        CONSOLE_Print( "[BNLSC: " + m_Server + ":" + UTIL_ToString( m_Port ) + ":C" + UTIL_ToString( m_WardenCookie ) + "] disconnected from BNLS server" );
        return true;
    }

    if( m_Socket->GetConnected( ) )
    {
        m_Socket->DoRecv( (fd_set *)fd );
        ExtractPackets( );
        ProcessPackets( );

        if( GetTime( ) - m_LastNullTime >= 50 )
        {
            m_Socket->PutBytes( m_Protocol->SEND_BNLS_NULL( ) );
            m_LastNullTime = GetTime( );
        }

        while( !m_OutPackets.empty( ) )
        {
            m_Socket->PutBytes( m_OutPackets.front( ) );
            m_OutPackets.pop( );
        }

        m_Socket->DoSend( (fd_set *)send_fd );
        return false;
    }

    if( m_Socket->GetConnecting( ) && m_Socket->CheckConnect( ) )
    {
        CONSOLE_Print( "[BNLSC: " + m_Server + ":" + UTIL_ToString( m_Port ) + ":C" + UTIL_ToString( m_WardenCookie ) + "] connected" );
        m_WasConnected = true;
        m_LastNullTime = GetTime( );
        return false;
    }

    if( !m_Socket->GetConnecting( ) && !m_Socket->GetConnected( ) && !m_WasConnected )
    {
        CONSOLE_Print( "[BNLSC: " + m_Server + ":" + UTIL_ToString( m_Port ) + ":C" + UTIL_ToString( m_WardenCookie ) + "] connecting to server [" + m_Server + "] on port " + UTIL_ToString( m_Port ) );
        m_Socket->Connect( string( ), m_Server, m_Port );
        return false;
    }

    return false;
}

void CBNLSClient :: ExtractPackets( )
{
    string *RecvBuffer = m_Socket->GetBytes( );
    BYTEARRAY Bytes = UTIL_CreateByteArray( (unsigned char *)RecvBuffer->c_str( ), RecvBuffer->size( ) );

    while( Bytes.size( ) >= 3 )
    {
        uint16_t Length = UTIL_ByteArrayToUInt16( Bytes, false );

        if( Length >= 3 )
        {
            if( Bytes.size( ) >= Length )
            {
                m_Packets.push( new CCommandPacket( 0, Bytes[2], BYTEARRAY( Bytes.begin( ), Bytes.begin( ) + Length ) ) );
                *RecvBuffer = RecvBuffer->substr( Length );
                Bytes = BYTEARRAY( Bytes.begin( ) + Length, Bytes.end( ) );
            }
            else
                return;
        }
        else
        {
            CONSOLE_Print( "[BNLSC: " + m_Server + ":" + UTIL_ToString( m_Port ) + ":C" + UTIL_ToString( m_WardenCookie ) + "] error - received invalid packet from BNLS server (bad length), disconnecting" );
            m_Socket->Disconnect( );
            return;
        }
    }
}

void CBNLSClient :: ProcessPackets( )
{
    while( !m_Packets.empty( ) )
    {
        CCommandPacket *Packet = m_Packets.front( );
        m_Packets.pop( );

        if( Packet->GetID( ) == CBNLSProtocol :: BNLS_WARDEN )
        {
            BYTEARRAY WardenResponse = m_Protocol->RECEIVE_BNLS_WARDEN( Packet->GetData( ) );

            if( !WardenResponse.empty( ) )
                m_WardenResponses.push( WardenResponse );
        }

        delete Packet;
    }
}

void CBNLSClient :: QueueWardenSeed( uint32_t seed )
{
    m_OutPackets.push( m_Protocol->SEND_BNLS_WARDEN_SEED( m_WardenCookie, seed ) );
}

void CBNLSClient :: QueueWardenRaw( BYTEARRAY wardenRaw )
{
    m_OutPackets.push( m_Protocol->SEND_BNLS_WARDEN_RAW( m_WardenCookie, wardenRaw ) );
    ++m_TotalWardenIn;
}

#include <boost/python.hpp>

void CBNLSClient :: RegisterPythonClass( )
{
	using namespace boost::python;
	
	class_<CBNLSClient>("BNLSClient", no_init)
		.def("getWardenResponse", &CBNLSClient::GetWardenResponse)
		.def("getTotalWardenIn", &CBNLSClient::GetTotalWardenIn)
		.def("getTotalWardenOut", &CBNLSClient::GetTotalWardenOut)
		.def("queueWardenSeed", &CBNLSClient::QueueWardenSeed)
		.def("queueWardenRaw", &CBNLSClient::QueueWardenRaw)
		.def_readonly("socket", &CBNLSClient::m_Socket)
		.def_readonly("protocol", &CBNLSClient::m_Protocol)
		.def_readonly("packets", &CBNLSClient::m_Packets)
		.def_readonly("wasConnected", &CBNLSClient::m_WasConnected)
		.def_readonly("server", &CBNLSClient::m_Server)
		.def_readonly("port", &CBNLSClient::m_Port)
		.def_readonly("lastNullTime", &CBNLSClient::m_LastNullTime)
		.def_readonly("wardenCookie", &CBNLSClient::m_WardenCookie)
		.def_readonly("outPackets", &CBNLSClient::m_OutPackets)
		.def_readonly("wardenResponse", &CBNLSClient::m_WardenResponses)
		.def_readonly("totalWardenIn", &CBNLSClient::m_TotalWardenIn)
		.def_readonly("totalWardenOut", &CBNLSClient::m_TotalWardenOut)
	;
}
