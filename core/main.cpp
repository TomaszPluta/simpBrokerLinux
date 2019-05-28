#include "serv.h"
#include "SocketException.h"
#include <string>
#include <iostream>

int main ( int argc, int argv[] )
{












  std::cout << "running....\n";

  try
    {
      // Create the socket
      ServerSocket server ( 1886 );

      while ( true )
	{

	  ServerSocket new_sock;
	  server.accept ( new_sock );

	  try
	    {
	      while ( true )
		{
		  std::vector<char> data;
		  new_sock >> data;
		  new_sock << data;
		  for (auto i : data){
			  std::cout <<data[i];
		  }
		}
	    }
	  catch ( SocketException& ) {}

	}
    }
  catch ( SocketException& e )
    {
      std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
    }

  return 0;
}
