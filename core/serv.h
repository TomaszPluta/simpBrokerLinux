// Definition of the ServerSocket class

#ifndef ServerSocket_class
#define ServerSocket_class

#include "Socket.h"
#include <string>

class ServerSocket : private Socket
{
 public:

  ServerSocket ( int port );
  ServerSocket (){};
  virtual ~ServerSocket();

  const ServerSocket& operator << ( const std::string& ) const;
  const ServerSocket& operator << ( const std::vector<char>& v ) const;
  const ServerSocket& operator >> ( std::string& ) const;
  const ServerSocket& operator >> ( std::vector<char>& vec ) const;

  void accept ( ServerSocket& );
  std::string get_cli_addr(void);
 private:

};


#endif
