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
* or join us directly here: http://forum.ohsystem.net/
*
* Visit us also on http://ohsystem.net/ and keep track always of the latest
* features and changes.
*
*
* This is modified from GHOST++: http://ghostplusplus.googlecode.com/
*/

#include "ghost.h"
#include "util.h"
#include "socket.h"

#include <string.h>

#ifndef WIN32
int GetLastError( ) {
    return errno;
}
#endif

//
// CSocket
//

CSocket :: CSocket( ) :  m_Socket( INVALID_SOCKET ), m_HasError( false ), m_Error( 0 )
{
    memset( &m_SIN, 0, sizeof( m_SIN ) );
}

CSocket :: CSocket( SOCKET nSocket, struct sockaddr_in nSIN ) : m_Socket( nSocket ), m_SIN( nSIN ), m_HasError( false ), m_Error( 0 )
{

}

CSocket :: ~CSocket( )
{
    if( m_Socket != INVALID_SOCKET )
        closesocket( m_Socket );
}

BYTEARRAY CSocket :: GetPort( )
{
    return UTIL_CreateByteArray( m_SIN.sin_port, false );
}

BYTEARRAY CSocket :: GetIP( )
{
    return UTIL_CreateByteArray( (uint32_t)m_SIN.sin_addr.s_addr, false );
}

string CSocket :: GetIPString( )
{
    return inet_ntoa( m_SIN.sin_addr );
}

string CSocket :: GetErrorString( )
{
    if( !m_HasError )
        return "NO ERROR";

    switch( m_Error )
    {
    case EWOULDBLOCK:
        return "EWOULDBLOCK";
    case EINPROGRESS:
        return "EINPROGRESS";
    case EALREADY:
        return "EALREADY";
    case ENOTSOCK:
        return "ENOTSOCK";
    case EDESTADDRREQ:
        return "EDESTADDRREQ";
    case EMSGSIZE:
        return "EMSGSIZE";
    case EPROTOTYPE:
        return "EPROTOTYPE";
    case ENOPROTOOPT:
        return "ENOPROTOOPT";
    case EPROTONOSUPPORT:
        return "EPROTONOSUPPORT";
    case ESOCKTNOSUPPORT:
        return "ESOCKTNOSUPPORT";
    case EOPNOTSUPP:
        return "EOPNOTSUPP";
    case EPFNOSUPPORT:
        return "EPFNOSUPPORT";
    case EAFNOSUPPORT:
        return "EAFNOSUPPORT";
    case EADDRINUSE:
        return "EADDRINUSE";
    case EADDRNOTAVAIL:
        return "EADDRNOTAVAIL";
    case ENETDOWN:
        return "ENETDOWN";
    case ENETUNREACH:
        return "ENETUNREACH";
    case ENETRESET:
        return "ENETRESET";
    case ECONNABORTED:
        return "ECONNABORTED";
    case ECONNRESET:
        return "ECONNRESET";
    case ENOBUFS:
        return "ENOBUFS";
    case EISCONN:
        return "EISCONN";
    case ENOTCONN:
        return "ENOTCONN";
    case ESHUTDOWN:
        return "ESHUTDOWN";
    case ETOOMANYREFS:
        return "ETOOMANYREFS";
    case ETIMEDOUT:
        return "ETIMEDOUT";
    case ECONNREFUSED:
        return "ECONNREFUSED";
    case ELOOP:
        return "ELOOP";
    case ENAMETOOLONG:
        return "ENAMETOOLONG";
    case EHOSTDOWN:
        return "EHOSTDOWN";
    case EHOSTUNREACH:
        return "EHOSTUNREACH";
    case ENOTEMPTY:
        return "ENOTEMPTY";
    case EUSERS:
        return "EUSERS";
    case EDQUOT:
        return "EDQUOT";
    case ESTALE:
        return "ESTALE";
    case EREMOTE:
        return "EREMOTE";
    }

    return "UNKNOWN ERROR (" + UTIL_ToString( m_Error ) + ")";
}

void CSocket :: SetFD( fd_set *fd, fd_set *send_fd, int *nfds )
{
    if( m_Socket == INVALID_SOCKET )
        return;

    FD_SET( m_Socket, fd );
    FD_SET( m_Socket, send_fd );

#ifndef WIN32
    if( m_Socket > *nfds )
        *nfds = m_Socket;
#endif
}

void CSocket :: Allocate( int type )
{
    m_Socket = socket( AF_INET, type, 0 );

    if( m_Socket == INVALID_SOCKET )
    {
        m_HasError = true;
        m_Error = GetLastError( );
        //CONSOLE_Print( "[SOCKET] error (socket) - " + GetErrorString( ) );
        return;
    }
}

void CSocket :: Reset( )
{
    if( m_Socket != INVALID_SOCKET )
        closesocket( m_Socket );

    m_Socket = INVALID_SOCKET;
    memset( &m_SIN, 0, sizeof( m_SIN ) );
    m_HasError = false;
    m_Error = 0;
}

string CSocket :: GetHostName( )
{
    if( m_CachedHostName.empty( ) )
    {
        char host[NI_MAXHOST], service[NI_MAXSERV];
#if !defined(WIN32)
        int s = getnameinfo( ( struct sockaddr * ) &m_SIN, sizeof( m_SIN ), host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV );
#endif
        string str( host );

        m_CachedHostName = str;

        if( m_CachedHostName.empty( ) )
            m_CachedHostName = "Unknown";

        return str;
    }
    else
        return m_CachedHostName;
}

//
// CTCPSocket
//

CTCPSocket :: CTCPSocket( ) : CSocket( ), m_Connected( false ), m_LastRecv( GetTime( ) ), m_LastSend( GetTime( ) )
{
    Allocate( SOCK_STREAM );

    // make socket non blocking

#ifdef WIN32
    int iMode = 1;
    ioctlsocket( m_Socket, FIONBIO, (u_long FAR *)&iMode );
#else
    fcntl( m_Socket, F_SETFL, fcntl( m_Socket, F_GETFL ) | O_NONBLOCK );
#endif
}

CTCPSocket :: CTCPSocket( SOCKET nSocket, struct sockaddr_in nSIN ) : CSocket( nSocket, nSIN )
{
    m_Connected = true;
    m_LastRecv = GetTime( );
    m_LastSend = GetTime( );

    // make socket non blocking

#ifdef WIN32
    int iMode = 1;
    ioctlsocket( m_Socket, FIONBIO, (u_long FAR *)&iMode );
#else
    fcntl( m_Socket, F_SETFL, fcntl( m_Socket, F_GETFL ) | O_NONBLOCK );
#endif
}

CTCPSocket :: ~CTCPSocket( )
{

}

void CTCPSocket :: Reset( )
{
    CSocket :: Reset( );

    Allocate( SOCK_STREAM );
    m_Connected = false;
    m_RecvBuffer.clear( );
    m_SendBuffer.clear( );
    m_LastRecv = GetTime( );
    m_LastSend = GetTime( );

    // make socket non blocking

#ifdef WIN32
    int iMode = 1;
    ioctlsocket( m_Socket, FIONBIO, (u_long FAR *)&iMode );
#else
    fcntl( m_Socket, F_SETFL, fcntl( m_Socket, F_GETFL ) | O_NONBLOCK );
#endif

    if( !m_LogFile.empty( ) )
    {
        ofstream Log;
        Log.open( m_LogFile.c_str( ), ios :: app );

        if( !Log.fail( ) )
        {
            Log << "----------RESET----------" << endl;
            Log.close( );
        }
    }
}

void CTCPSocket :: PutBytes( string bytes )
{
    m_SendBuffer += bytes;
}

void CTCPSocket :: PutBytes( BYTEARRAY bytes )
{
    m_SendBuffer += string( bytes.begin( ), bytes.end( ) );
}

void CTCPSocket :: DoRecvPlain( fd_set *fd ) {
    if( m_Socket == INVALID_SOCKET || m_HasError || !m_Connected )
        return;

    if( FD_ISSET( m_Socket, fd ) )
    {
        // data is waiting, receive it

        char buffer[20480];
        int c = recv( m_Socket, buffer, 20480, 0 );
        if(c!=0) {
          m_RecvBuffer += string( buffer, c );
          m_LastRecv = GetTime( );
        } else {
            // the other end closed the connection

            CONSOLE_Print( "[TCPSOCKET] closed by remote host" );
            m_Connected = false;
        }
    }
}

void CTCPSocket :: DoRecv( fd_set *fd )
{
    if( m_Socket == INVALID_SOCKET || m_HasError || !m_Connected )
        return;

    if( FD_ISSET( m_Socket, fd ) )
    {
        // data is waiting, receive it

        char buffer[1024];
        int c = recv( m_Socket, buffer, 1024, 0 );

        if( c > 0 )
        {
            // success! add the received data to the buffer

            if( !m_LogFile.empty( ) )
            {
                ofstream Log;
                Log.open( m_LogFile.c_str( ), ios :: app );

                if( !Log.fail( ) )
                {
                    Log << "					RECEIVE <<< " << UTIL_ByteArrayToHexString( UTIL_CreateByteArray( (unsigned char *)buffer, c ) ) << endl;
                    Log.close( );
                }
            }
            m_RecvBuffer += string( buffer, c );
            m_LastRecv = GetTime( );
        }
        else if( c == SOCKET_ERROR && GetLastError( ) != EWOULDBLOCK )
        {
            // receive error

            m_HasError = true;
            m_Error = GetLastError( );
            //CONSOLE_Print( "[TCPSOCKET] error (recv) - " + GetErrorString( ) );
            return;
        }
        else if( c == 0 )
        {
            // the other end closed the connection

            CONSOLE_Print( "[TCPSOCKET] closed by remote host" );
            m_Connected = false;
        }
    }
}

void CTCPSocket :: DoSend( fd_set *send_fd )
{
    if( m_Socket == INVALID_SOCKET || m_HasError || !m_Connected || m_SendBuffer.empty( ) )
        return;

    if( FD_ISSET( m_Socket, send_fd ) )
    {
        // socket is ready, send it

        int s = send( m_Socket, m_SendBuffer.c_str( ), (int)m_SendBuffer.size( ), MSG_NOSIGNAL );

        if( s > 0 )
        {
            // success! only some of the data may have been sent, remove it from the buffer

            if( !m_LogFile.empty( ) )
            {
                ofstream Log;
                Log.open( m_LogFile.c_str( ), ios :: app );

                if( !Log.fail( ) )
                {
                    Log << "SEND >>> " << UTIL_ByteArrayToHexString( BYTEARRAY( m_SendBuffer.begin( ), m_SendBuffer.begin( ) + s ) ) << endl;
                    Log.close( );
                }
            }
            m_SendBuffer = m_SendBuffer.substr( s );
            m_LastSend = GetTime( );
        }
        else if( s == SOCKET_ERROR && GetLastError( ) != EWOULDBLOCK )
        {
            // send error

            m_HasError = true;
            m_Error = GetLastError( );
            CONSOLE_Print( "[TCPSOCKET] error (send) - " + GetErrorString( ) );
            return;
        }
    }
}

void CTCPSocket :: DoSendPlain( fd_set *send_fd )
{
    if( m_Socket == INVALID_SOCKET || m_HasError || !m_Connected || m_SendBuffer.empty( ) )
        return;
    if( FD_ISSET( m_Socket, send_fd ) )
    {
        // socket is ready, send it

        int s = send( m_Socket, m_SendBuffer.c_str( ), (int)m_SendBuffer.size( ), MSG_NOSIGNAL );

        if( s > 0 )
        {
            // success! only some of the data may have been sent, remove it from the buffer

            if( !m_LogFile.empty( ) )
            {
                ofstream Log;
                Log.open( m_LogFile.c_str( ), ios :: app );

                if( !Log.fail( ) )
                {
                    Log << "SEND >>> " << m_SendBuffer.substr(0, s)  << endl;
                    Log.close( );
                }
            }
            m_SendBuffer = m_SendBuffer.substr( s );
            m_LastSend = GetTime( );
        }
        else if( s == SOCKET_ERROR && GetLastError( ) != EWOULDBLOCK )
        {
            // send error

            m_HasError = true;
            m_Error = GetLastError( );
            CONSOLE_Print( "[TCPSOCKET] error (send) - " + GetErrorString( ) );
            return;
        }
    }
}

void CTCPSocket :: Disconnect( )
{
    if( m_Socket != INVALID_SOCKET )
        shutdown( m_Socket, SHUT_RDWR );

    m_Connected = false;
}

void CTCPSocket :: SetNoDelay( bool noDelay )
{
    int OptVal = 0;

    if( noDelay )
        OptVal = 1;

    setsockopt( m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&OptVal, sizeof( int ) );
}

//
// CTCPClient
//

CTCPClient :: CTCPClient( ) : CTCPSocket( ), m_Connecting( false )
{

}

CTCPClient :: ~CTCPClient( )
{

}

void CTCPClient :: Reset( )
{
    CTCPSocket :: Reset( );
    m_Connecting = false;
}

void CTCPClient :: Disconnect( )
{
    CTCPSocket :: Disconnect( );
    m_Connecting = false;
}

void CTCPClient :: Connect( string localaddress, string address, uint16_t port )
{
    if( m_Socket == INVALID_SOCKET || m_HasError || m_Connecting || m_Connected )
        return;

    if( !localaddress.empty( ) )
    {
        struct sockaddr_in LocalSIN;
        memset( &LocalSIN, 0, sizeof( LocalSIN ) );
        LocalSIN.sin_family = AF_INET;

        if( ( LocalSIN.sin_addr.s_addr = inet_addr( localaddress.c_str( ) ) ) == INADDR_NONE )
            LocalSIN.sin_addr.s_addr = INADDR_ANY;

        LocalSIN.sin_port = htons( 0 );

        if( ::bind( m_Socket, (struct sockaddr *)&LocalSIN, sizeof( LocalSIN ) ) == SOCKET_ERROR )
        {
            m_HasError = true;
            m_Error = GetLastError( );
            //CONSOLE_Print( "[TCPCLIENT] error (bind) - " + GetErrorString( ) );
            return;
        }
    }

    // get IP address

    struct hostent *HostInfo;
    uint32_t HostAddress;
    HostInfo = gethostbyname( address.c_str( ) );

    if( !HostInfo )
    {
        m_HasError = true;
        // m_Error = h_error;
        //CONSOLE_Print( "[TCPCLIENT] error (gethostbyname)" );
        return;
    }

    memcpy( &HostAddress, HostInfo->h_addr, HostInfo->h_length );

    // connect

    m_SIN.sin_family = AF_INET;
    m_SIN.sin_addr.s_addr = HostAddress;
    m_SIN.sin_port = htons( port );

    if( connect( m_Socket, (struct sockaddr *)&m_SIN, sizeof( m_SIN ) ) == SOCKET_ERROR )
    {
        if( GetLastError( ) != EINPROGRESS && GetLastError( ) != EWOULDBLOCK )
        {
            // connect error

            m_HasError = true;
            m_Error = GetLastError( );
            //CONSOLE_Print( "[TCPCLIENT] error (connect) - " + GetErrorString( ) );
            return;
        }
    }

    m_Connecting = true;
}

bool CTCPClient :: CheckConnect( )
{
    if( m_Socket == INVALID_SOCKET || m_HasError || !m_Connecting )
        return false;

    fd_set fd;
    FD_ZERO( &fd );
    FD_SET( m_Socket, &fd );

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    // check if the socket is connected

#ifdef WIN32
    if( select( 1, NULL, &fd, NULL, &tv ) == SOCKET_ERROR )
#else
    if( select( m_Socket + 1, NULL, &fd, NULL, &tv ) == SOCKET_ERROR )
#endif
    {
        m_HasError = true;
        m_Error = GetLastError( );
        return false;
    }

    if( FD_ISSET( m_Socket, &fd ) )
    {
        m_Connecting = false;
        m_Connected = true;
        return true;
    }

    return false;
}

//
// CTCPServer
//

CTCPServer :: CTCPServer( ) : CTCPSocket( )
{
    // set the socket to reuse the address in case it hasn't been released yet

    int optval = 1;

#ifdef WIN32
    setsockopt( m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof( int ) );
#else
    setsockopt( m_Socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof( int ) );
#endif
}

CTCPServer :: ~CTCPServer( )
{

}

bool CTCPServer :: Listen( string address, uint16_t port )
{
    if( m_Socket == INVALID_SOCKET || m_HasError )
        return false;

    m_SIN.sin_family = AF_INET;

    if( !address.empty( ) )
    {
        if( ( m_SIN.sin_addr.s_addr = inet_addr( address.c_str( ) ) ) == INADDR_NONE )
            m_SIN.sin_addr.s_addr = INADDR_ANY;
    }
    else
        m_SIN.sin_addr.s_addr = INADDR_ANY;

    m_SIN.sin_port = htons( port );

    if( ::bind( m_Socket, (struct sockaddr *)&m_SIN, sizeof( m_SIN ) ) == SOCKET_ERROR )
    {
        m_HasError = true;
        m_Error = GetLastError( );
        //CONSOLE_Print( "[TCPSERVER] error (bind) - " + GetErrorString( ) );
        return false;
    }

    // listen, queue length 8

    if( listen( m_Socket, 8 ) == SOCKET_ERROR )
    {
        m_HasError = true;
        m_Error = GetLastError( );
        //CONSOLE_Print( "[TCPSERVER] error (listen) - " + GetErrorString( ) );
        return false;
    }

    return true;
}

CTCPSocket *CTCPServer :: Accept( fd_set *fd )
{
    if( m_Socket == INVALID_SOCKET || m_HasError )
        return NULL;

    if( FD_ISSET( m_Socket, fd ) )
    {
        // a connection is waiting, accept it

        struct sockaddr_in Addr;
        int AddrLen = sizeof( Addr );
        SOCKET NewSocket;

#ifdef WIN32
        if( ( NewSocket = accept( m_Socket, (struct sockaddr *)&Addr, &AddrLen ) ) == INVALID_SOCKET )
#else
        if( ( NewSocket = accept( m_Socket, (struct sockaddr *)&Addr, (socklen_t *)&AddrLen ) ) == INVALID_SOCKET )
#endif
        {
            // accept error, ignore it
        }
        else
        {
            // success! return the new socket

            return new CTCPSocket( NewSocket, Addr );
        }
    }

    return NULL;
}

//
// CUDPSocket
//

CUDPSocket :: CUDPSocket( ) : CSocket( )
{
    Allocate( SOCK_DGRAM );

    // enable broadcast support

    int OptVal = 1;
    setsockopt( m_Socket, SOL_SOCKET, SO_BROADCAST, (const char *)&OptVal, sizeof( int ) );

    // set default broadcast target
    m_BroadcastTarget.s_addr = INADDR_BROADCAST;
}

CUDPSocket :: ~CUDPSocket( )
{

}

bool CUDPSocket :: SendTo( struct sockaddr_in sin, BYTEARRAY message )
{
    if( m_Socket == INVALID_SOCKET || m_HasError )
        return false;

    string MessageString = string( message.begin( ), message.end( ) );

    if( sendto( m_Socket, MessageString.c_str( ), MessageString.size( ), 0, (struct sockaddr *)&sin, sizeof( sin ) ) == -1 )
        return false;

    return true;
}

bool CUDPSocket :: SendTo( string address, uint16_t port, BYTEARRAY message )
{
    if( m_Socket == INVALID_SOCKET || m_HasError )
        return false;

    // get IP address

    struct hostent *HostInfo;
    uint32_t HostAddress;
    HostInfo = gethostbyname( address.c_str( ) );

    if( !HostInfo )
    {
        m_HasError = true;
        // m_Error = h_error;
        //CONSOLE_Print( "[UDPSOCKET] error (gethostbyname)" );
        return false;
    }

    memcpy( &HostAddress, HostInfo->h_addr, HostInfo->h_length );
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = HostAddress;
    sin.sin_port = htons( port );

    return SendTo( sin, message );
}

bool CUDPSocket :: Broadcast( uint16_t port, BYTEARRAY message )
{
    if( m_Socket == INVALID_SOCKET || m_HasError )
        return false;

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = m_BroadcastTarget.s_addr;
    sin.sin_port = htons( port );

    string MessageString = string( message.begin( ), message.end( ) );

    if( sendto( m_Socket, MessageString.c_str( ), MessageString.size( ), 0, (struct sockaddr *)&sin, sizeof( sin ) ) == -1 )
    {
        //CONSOLE_Print( "[UDPSOCKET] failed to broadcast packet (port " + UTIL_ToString( port ) + ", size " + UTIL_ToString( MessageString.size( ) ) + " bytes)" );
        return false;
    }

    return true;
}

void CUDPSocket :: SetBroadcastTarget( string subnet )
{
    if( subnet.empty( ) )
    {
        //CONSOLE_Print( "[UDPSOCKET] using default broadcast target" );
        m_BroadcastTarget.s_addr = INADDR_BROADCAST;
    }
    else
    {
        // this function does not check whether the given subnet is a valid subnet the user is on
        // convert string representation of ip/subnet to in_addr

        //CONSOLE_Print( "[UDPSOCKET] using broadcast target [" + subnet + "]" );
        m_BroadcastTarget.s_addr = inet_addr( subnet.c_str( ) );

        // if conversion fails, inet_addr( ) returns INADDR_NONE

        if( m_BroadcastTarget.s_addr == INADDR_NONE )
        {
            //CONSOLE_Print( "[UDPSOCKET] invalid broadcast target, using default broadcast target" );
            m_BroadcastTarget.s_addr = INADDR_BROADCAST;
        }
    }
}

void CUDPSocket :: SetDontRoute( bool dontRoute )
{
    int OptVal = 0;

    if( dontRoute )
        OptVal = 1;

    // don't route packets; make them ignore routes set by routing table and send them to the interface
    // belonging to the target address directly

    setsockopt( m_Socket, SOL_SOCKET, SO_DONTROUTE, (const char *)&OptVal, sizeof( int ) );
}

//
// CUDPServer
//

CUDPServer :: CUDPServer( ) : CUDPSocket( )
{
    // make socket non blocking

#ifdef WIN32
    int iMode = 1;
    ioctlsocket( m_Socket, FIONBIO, (u_long FAR *)&iMode );
#else
    fcntl( m_Socket, F_SETFL, fcntl( m_Socket, F_GETFL ) | O_NONBLOCK );
#endif

    // set the socket to reuse the address
    // with UDP sockets this allows more than one program to listen on the same port

    int optval = 1;

#ifdef WIN32
    setsockopt( m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof( int ) );
#else
    setsockopt( m_Socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof( int ) );
#endif
}

CUDPServer :: ~CUDPServer( )
{

}

bool CUDPServer :: Bind( struct sockaddr_in sin )
{
    if( m_Socket == INVALID_SOCKET || m_HasError )
        return false;

    m_SIN = sin;

    if( ::bind( m_Socket, (struct sockaddr *)&m_SIN, sizeof( m_SIN ) ) == SOCKET_ERROR )
    {
        m_HasError = true;
        m_Error = GetLastError( );
        //CONSOLE_Print( "[UDPSERVER] error (bind) - " + GetErrorString( ) );
        return false;
    }

    return true;
}

bool CUDPServer :: Bind( string address, uint16_t port )
{
    if( m_Socket == INVALID_SOCKET || m_HasError )
        return false;

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;

    if( !address.empty( ) )
    {
        if( ( sin.sin_addr.s_addr = inet_addr( address.c_str( ) ) ) == INADDR_NONE )
            sin.sin_addr.s_addr = INADDR_ANY;
    }
    else
        sin.sin_addr.s_addr = INADDR_ANY;

    sin.sin_port = htons( port );

    return Bind( sin );
}

void CUDPServer :: RecvFrom( fd_set *fd, struct sockaddr_in *sin, string *message )
{
    if( m_Socket == INVALID_SOCKET || m_HasError || !sin || !message )
        return;

    int AddrLen = sizeof( *sin );

    if( FD_ISSET( m_Socket, fd ) )
    {
        // data is waiting, receive it

        char buffer[1024];

#ifdef WIN32
        int c = recvfrom( m_Socket, buffer, 1024, 0, (struct sockaddr *)sin, &AddrLen );
#else
        int c = recvfrom( m_Socket, buffer, 1024, 0, (struct sockaddr *)sin, (socklen_t *)&AddrLen );
#endif

        if( c > 0 )
        {
            // success!

            *message = string( buffer, c );
        }
        else if( c == SOCKET_ERROR && GetLastError( ) != EWOULDBLOCK )
        {
            // receive error

            m_HasError = true;
            m_Error = GetLastError( );
            //CONSOLE_Print( "[UDPSERVER] error (recvfrom) - " + GetErrorString( ) );
        }
    }
}


#include <boost/python.hpp>

void CSocket :: RegisterPythonClass( )
{
	using namespace boost::python;

	class_<CSocket>("socket", no_init)
		.def_readonly("socket", &CSocket::m_Socket)
		.def_readonly("hasError", &CSocket::m_HasError)
		.def_readonly("error", &CSocket::m_Error)

		.def("getPort", &CSocket::GetPort)
		.def("getIP", &CSocket::GetIP)
		.def("getIPString", &CSocket::GetIPString)
		.def("hasError", &CSocket::HasError)
		.def("getError", &CSocket::GetError)
		.def("getErrorString", &CSocket::GetErrorString)

		.def("allocate", &CSocket::Allocate)
		.def("reset", &CSocket::Reset)
	;
}

void CTCPSocket :: RegisterPythonClass( )
{
	void (CTCPSocket::*PutBytes1)(string)		= &CTCPSocket::PutBytes;
	void (CTCPSocket::*PutBytes2)(BYTEARRAY)	= &CTCPSocket::PutBytes;

	using namespace boost::python;

	class_< CTCPSocket, bases<CSocket> >("TCPSocket", no_init)
		.def_readonly("connected", &CTCPSocket::m_Connected)
		.def_readonly("logFile", &CTCPSocket::m_LogFile)
		.def_readonly("recvBuffer", &CTCPSocket::m_RecvBuffer)
		.def_readonly("sendBuffer", &CTCPSocket::m_SendBuffer)
		.def_readonly("lastRecv", &CTCPSocket::m_LastRecv)
		.def_readonly("lastSend", &CTCPSocket::m_LastSend)

		.def("getConnected", &CTCPSocket::GetConnected)
		.def("getBytes", &CTCPSocket::GetBytes, return_internal_reference<>())
		.def("putBytes", PutBytes1)
		.def("putBytes", PutBytes2)
		.def("clearRecvBuffer", &CTCPSocket::ClearRecvBuffer)
		.def("clearSendBuffer", &CTCPSocket::ClearSendBuffer)
		.def("getLastRecv", &CTCPSocket::GetLastRecv)
		.def("getLastSend", &CTCPSocket::GetLastSend)

		.def("disconnect", &CTCPSocket::Disconnect)
		.def("setNoDelay", &CTCPSocket::SetNoDelay)
		.def("setLogFile", &CTCPSocket::SetLogFile)
	;
}

void CTCPClient :: RegisterPythonClass( )
{
	using namespace boost::python;

	class_< CTCPClient, bases<CTCPSocket> >("TCPClient", no_init)
		.def_readonly("connecting", &CTCPClient::m_Connecting)

		.def("getConnected", &CTCPClient::GetConnected)
		.def("connect", &CTCPClient::Connect)
		.def("checkConnect", &CTCPClient::CheckConnect)
	;
}
void CTCPServer :: RegisterPythonClass( )
{
	using namespace boost::python;

	class_< CTCPServer, bases<CTCPSocket> >("TCPServer", no_init)
		.def("listen", &CTCPServer::Listen)
		.def("accept", &CTCPServer::Accept, return_internal_reference<>())
	;
}

void CUDPSocket :: RegisterPythonClass( )
{
	bool (CUDPSocket::*SendTo1)(struct sockaddr_in, BYTEARRAY)	= &CUDPSocket::SendTo;
	bool (CUDPSocket::*SendTo2)(string, uint16_t, BYTEARRAY)	= &CUDPSocket::SendTo;

	using namespace boost::python;

	class_< CUDPSocket, bases<CSocket> >("UDPSocket", no_init)
		.def_readonly("broadcastTarget", &CUDPSocket::m_BroadcastTarget)

		.def("sendTo", SendTo1)
		.def("sendTo", SendTo2)

		.def("broadcast", &CUDPSocket::Broadcast)
		.def("setBroadcastTarget", &CUDPSocket::SetBroadcastTarget)
		.def("setDontRoute", &CUDPSocket::SetDontRoute)
	;
}
void CUDPServer :: RegisterPythonClass( )
{
	bool (CUDPServer::*Bind1)(struct sockaddr_in)	= &CUDPServer::Bind;
	bool (CUDPServer::*Bind2)(string, uint16_t)		= &CUDPServer::Bind;

	using namespace boost::python;

	class_< CUDPServer, bases<CUDPSocket> >("UDPServer", no_init)
		.def("bind", Bind1)
		.def("bind", Bind2)
	;
}
