// Implementation of the ServerSocket class

#include "serv.h"
#include "SocketException.h"


ServerSocket::ServerSocket ( int port )
{
  if ( ! Socket::create() )
    {
      throw SocketException ( "Could not create server socket." );
    }

  if ( ! Socket::bind ( port ) )
    {
      throw SocketException ( "Could not bind to port." );
    }

  if ( ! Socket::listen() )
    {
      throw SocketException ( "Could not listen to socket." );
    }

}

ServerSocket::~ServerSocket()
{
}


const ServerSocket& ServerSocket::operator << ( const std::string& s ) const
{
  if ( ! Socket::send ( s ) )
    {
      throw SocketException ( "Could not write to socket." );
    }

  return *this;

}


const ServerSocket& ServerSocket::operator << ( const std::vector<char>& v ) const
{
  if ( ! Socket::send ( v ) )
    {
      throw SocketException ( "Could not write to socket." );
    }

  return *this;

 }



const ServerSocket& ServerSocket::operator >> ( std::string& s ) const
{
  if ( ! Socket::recv ( s ) )
    {
      throw SocketException ( "Could not read from socket." );
    }

  return *this;
}



const ServerSocket& ServerSocket::operator >> ( std::vector<char>& vec ) const
{
  if ( ! Socket::recv ( vec ) )
    {
      throw SocketException ( "Could not read from socket." );
    }

  return *this;
}




void ServerSocket::accept ( ServerSocket& sock )
{
  if ( ! Socket::accept ( sock ) )
    {
      throw SocketException ( "Could not accept socket." );
    }
}




std::string ServerSocket::get_cli_addr(void){
	return Socket::get_addr();
}









