// Arkadij Doronin 24.05.2013
// ircBot

    #include <cstdlib>
    #include <iostream>
    #include <cstdio>
    #include <cstring>
    #include <string>

#ifdef WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
#endif
