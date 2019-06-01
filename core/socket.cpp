// Implementation of the Socket class.


#include "Socket.h"
#include "string.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <vector>
#include "simp_broker.h"



Socket::Socket() :
m_sock ( -1 )
{

	memset ( &m_addr,
			0,
			sizeof ( m_addr ) );

}

Socket::~Socket()
{
	if ( is_valid() )
		::close ( m_sock );
}

bool Socket::create()
{
	m_sock = socket ( AF_INET,
			SOCK_STREAM,
			0 );

	if ( ! is_valid() )
		return false;


	// TIME_WAIT - argh
	int on = 1;
	if ( setsockopt ( m_sock, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &on, sizeof ( on ) ) == -1 )
		return false;


	return true;

}



bool Socket::bind ( const int port )
{

	if ( ! is_valid() )
	{
		return false;
	}



	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = INADDR_ANY;
	m_addr.sin_port = htons ( port );

	int bind_return = ::bind ( m_sock,
			( struct sockaddr * ) &m_addr,
			sizeof ( m_addr ) );


	if ( bind_return == -1 )
	{
		return false;
	}

	return true;
}


bool Socket::listen() const
{
	if ( ! is_valid() )
	{
		return false;
	}

	int listen_return = ::listen ( m_sock, MAXCONNECTIONS );


	if ( listen_return == -1 )
	{
		return false;
	}

	return true;
}


bool Socket::accept ( Socket& new_socket ) const
{
	int addr_length = sizeof ( m_addr );
	new_socket.m_sock = ::accept ( m_sock, ( sockaddr * ) &m_addr, ( socklen_t * ) &addr_length );
	if ( new_socket.m_sock <= 0 ){
		return false;
	}
	else{
		new_socket.m_addr = m_addr;
		//	  // deal with both IPv4 and IPv6:
		//	  int port;
		//	  char ipstr[INET6_ADDRSTRLEN];
		//	  sockaddr_storage * addr = (sockaddr_storage) m_addr;
		//	  if (m_addr.ss_family == AF_INET) {
		//	      struct sockaddr_in *s = (struct sockaddr_in *)&m_addr;
		//	      port = ntohs(s->sin_port);
		//	      inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
		//	  } else { // AF_INET6
		//	      struct sockaddr_in6 *s = (struct sockaddr_in6 *)&m_addr;
		//	      port = ntohs(s->sin6_port);
		//	      inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
		//	  }
		//
		//	  printf("Peer IP address: %s\n", ipstr);
		return true;
	}
}


bool Socket::send ( const std::string s ) const
{
	int status = ::send ( m_sock, s.c_str(), s.size(), MSG_NOSIGNAL );
	if ( status == -1 )
	{
		return false;
	}
	else
	{
		return true;
	}
}


bool Socket::send ( const std::vector<char> vec ) const
{

	uint8_t buff[MAXSEND];
	for (auto i : vec){
		buff[i] = vec[i];
	}

	int status = ::send ( m_sock, buff, vec.size(), MSG_NOSIGNAL );
	if ( status == -1 )
	{
		return false;
	}
	else
	{
		return true;
	}
}


int Socket::recv ( std::string& s ) const
{
	char buf [ MAXRECV + 1 ];

	s = "";

	memset ( buf, 0, MAXRECV + 1 );

	int status = ::recv ( m_sock, buf, MAXRECV, 0 );

	if ( status == -1 )
	{
		std::cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
		return 0;
	}
	else if ( status == 0 )
	{
		return 0;
	}
	else
	{
		s = buf;
		return status;
	}
}



int Socket::recv ( std::vector<char> & v ) const
{
	char buf [ MAXRECV + 1 ];
	memset ( buf, 0, MAXRECV + 1 );

	int status = ::recv ( m_sock, buf, MAXRECV, 0 );

	if ( status == -1 )
	{
		std::cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
		return 0;
	}
	else if ( status == 0 )
	{
		return 0;
	}
	else
	{
		v.clear();
		v.insert(v.end(), &buf[0], &buf[MAXRECV]);
		return status;
	}
}


bool Socket::connect ( const std::string host, const int port )
{
	if ( ! is_valid() ) return false;

	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons ( port );

	int status = inet_pton ( AF_INET, host.c_str(), &m_addr.sin_addr );

	if ( errno == EAFNOSUPPORT ) return false;

	status = ::connect ( m_sock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );

	if ( status == 0 )
		return true;
	else
		return false;
}

void Socket::set_non_blocking ( const bool b )
{

	int opts;

	opts = fcntl ( m_sock,
			F_GETFL );

	if ( opts < 0 )
	{
		return;
	}

	if ( b )
		opts = ( opts | O_NONBLOCK );
	else
		opts = ( opts & ~O_NONBLOCK );

	fcntl ( m_sock,
			F_SETFL,opts );

}



std::string Socket::get_addr(void){
	struct sockaddr_in *s = (struct sockaddr_in *)&m_addr;
	int port = ntohs(s->sin_port);
	char ipstr[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	std::string str(ipstr);
	//str<<port;
	return str;
}




