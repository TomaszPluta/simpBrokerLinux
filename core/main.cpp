#include "serv.h"
#include "SocketException.h"
#include <string>
#include <iostream>
#include "simp_broker.h"



int main ( int argc, char * argv[] )
{








	SimpBroker simpBroker;



  std::cout << "running....\n";

  try
    {
      ServerSocket server ( 1886 );

      while ( true )
	{

	  ServerSocket new_sock;
	  server.accept ( new_sock );

	  try
	    {
	      while ( true )
		{
		  std::vector<char> rxFrame;
		  new_sock >> rxFrame;
		  std::cout<<new_sock.get_cli_addr();
		  conn_ack_t * ack  = simpBroker.OnReceivedFrame(rxFrame.data(), new_sock.get_cli_addr());
		  std::vector<char> rsp;
		  rsp.push_back(ack->control_type);
		  rsp.push_back(ack->remainin_len);
		  rsp.push_back(1);
		  rsp.push_back(0);

		  new_sock<<rsp;

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
