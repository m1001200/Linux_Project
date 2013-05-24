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

const unsigned int MAX_LINE = 1024; // input Buffer Size
const int PORT = 6667;
const char *HOST = "irc.europa-irc.de"

using namespace std;

#ifdef WIN32
    SOCKET sockfd;
#else
    int sockfd;
#endif

void IrcDisconnect(){
#ifdef WIN32
    closesocket(sockfd); // Win. spezifisch: Sockets-Freigabe 
    WSACleanup();
#else
    close(sockfd);       // Unix. spezifisch: Sockets-Freigabe
#endif
}
void IrcConnect(){
#ifdef WIN32
    // Win. spezifisch: Sockets-Initialisierung
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,0),&wsa) != 0) exit(1);
#endif
    // Unix. spezifisch: Sockets-Initialisierung
    sockfd = socket(AF_INET, SOCK_STREAM,0);
    
    if (static_cast<int>(sockfd) < 0) {
        perror("socket()");
        IrcDisconnect();
        exit(1);
    }
    
    hostent *hp = gethostbyname(HOST);
    
    if (!hp) {
        cerr << "gethostbyname()" << endl;
        IrcDisconnect();
        exit(1);
    }
    
    sockaddr_in sin;
    memset((char*)&sin,0,sizeof(sin));
    sin.sin_family = AF_INET;
    memcpy((char*)&sin.sin_addr, hp->h_addr, hp->h_length);
    sin.sin_port = htons(PORT);
    memset(&(sin.sin_zero),0,8*sizeof(char));
    
    if (connect(sockfd,(sockaddr*)&sin,sizeof(sin)) == -1) {
        perror("connect()");
        IrcDisconnect();
        exit(1);
    }
}
void SendToUplink(const char *msg){
    send(sockfd, msg, strlen(msg),0);
}
void IrcIdentify(){
    SendToUplink("NICK_NAME Bot\r\n");
    SendToUplink("USER_NAME Bot 0 0 :Bot\r\n");
    SendToUplink("PRIVMSG NickServ IDENTIFY password\r\n");
    SendToUplink("JOIN #channel\r\n");
    SendToUplink("PRIVMSG #channel: Hallo!!!\r\n");
    
}
void PingParse(const string &buffer){
    size_t  pingPos = buffer.find("PING");
    if (pingPos != string::npos) {
        string pong("PONG" + buffer.substr(pingPos + 4) + "\r\n");
        cout << pong;
        SendToUplink(pong.c_str());
    }
}

