// Arkadij Doronin 24.05.2013
// IRC-Bot
#include <iostream>
#include <cstdlib>
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

#ifdef WIN32
SOCKET sockfd;
#else
int sockfd;
#endif

using namespace std;

const unsigned int BUFF_SIZE = 1024;

void IrcConnect(const int port, const char* host);
void IrcDisconnect();
void SendToUplink(const char *msg);
void IrcIdentify();
void PingParse(const string & buffer);
void BotFunctions(const string & buffer);
void IrcParse(string buffer);


int main(int argc, char *argv[]){
    
	if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    IrcConnect(atoi(argv[1]),argv[2]); //"irc.europa-irc.de"
    cout << " >> CONECTION <<" << endl;
    IrcIdentify();
    cout << " >> IDENTIFY  <<" << endl;
    while (1) {
        char buffer[BUFF_SIZE + 1] = {0};
        if (recv(sockfd, buffer, BUFF_SIZE * sizeof(char), 0) < 0) {
            perror("recv()");
            IrcDisconnect();
            exit(1);
        }
        cout << buffer;
        IrcParse(buffer);
    }
    IrcDisconnect();
    return 0;
}

void IrcDisconnect(){
#ifdef WIN32
    closesocket(sockfd); // Win. spezifisch: Sockets-Freigabe
    WSACleanup();
#else
    close(sockfd);       // Unix. spezifisch: Sockets-Freigabe
#endif
}
void IrcConnect(const int port, const char* host){
    
#ifdef WIN32
    // Win. spezifisch: Sockets-Initialisierung
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,0),&wsa) != 0) exit(1);
#endif
    
    // Unix. spezifisch: Sockets-Initialisierung
    sockfd = socket(AF_INET, SOCK_STREAM,0);
    
    if (static_cast<int> (sockfd) < 0) {
        perror("socket()");
        IrcDisconnect();
        exit(1);
    }
    
    hostent *hp = gethostbyname(host);
    
    if (!hp) {
        cerr << "gethostbyname()" << endl;
        IrcDisconnect();
        exit(1);
    }
    
    sockaddr_in sin;
    memset((char*)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    memcpy((char*)&sin.sin_addr, hp->h_addr, hp->h_length);
    sin.sin_port = htons(port);
    memset(&(sin.sin_zero), 0, 8 * sizeof(char));
    
    if (connect(sockfd, (sockaddr*)&sin, sizeof(sin)) == -1) {
        perror("connect()");
        IrcDisconnect();
        exit(1);
    }
}
void SendToUplink(const char *msg){
    send(sockfd, msg, strlen(msg), 0);
}
void IrcIdentify(){
    SendToUplink("NICK ara222\r\n");                                   // NICK
    SendToUplink("USER ara222 0 0 :ara222\r\n");                          // Userdaten
    SendToUplink("PRIVMSG NickServ IDENTIFY password\r\n");         // Identifizieren
    SendToUplink("JOIN #europa-irc\r\n");                              // Betreten Channel
    SendToUplink("PRIVMSG #europa-irc :HALLO ARA!!!\r\n");                    // Nachricht Nr.1
    
}
void PingParse(const string &buffer){
    size_t  pingPos = buffer.find("PING");
    if (pingPos != string::npos) {
        string pong("PONG" + buffer.substr(pingPos + 4) + "\r\n");
        cout << pong;
        SendToUplink(pong.c_str());
    }
}
void BotFunctions(const string &buffer){
    size_t pos = 0;
    
    
    if ((pos = buffer.find("Botname: xxx")) != string::npos) {
        SendToUplink(("PRIVMSG ara111 \r\n"));// + buffer.substr(pingPos + 4) + "\r\n").c_str());
    } else if (buffer.find("exit") != string::npos){
        SendToUplink("PRIVMSG #europa-irc:Cya\r\n");
        IrcDisconnect();
        exit(0);
    } else if ((pos = buffer.find(":name ")) != string::npos){
        string name("NICK " + buffer.substr(pos + 6) + "\r\n");
        SendToUplink(name.c_str());
    }
    
}
void IrcParse(string buffer){
    if (buffer.find("\r\n") == buffer.length() - 2)
        buffer.erase(buffer.length() - 2);
    
    PingParse(buffer);
    BotFunctions(buffer);
}

