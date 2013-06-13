// Arkadij Doronin 24.05.2013
// IRC-Bot
#ifndef ircBot_cpp
#define ircBot_cpp

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include "ircBot.h"
#include "SQLite.h"

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
bool b_save = false;

int main(int argc, char *argv[]){
    
	if (argc < 4) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    
    IrcConnect(atoi(argv[1]),argv[2]); //"irc.europa-irc.de"
    cout << " >> CONECTION <<" << endl;
    IrcIdentify(argv[3], argv[4]);
    cout << " >> IDENTIFY  <<" << endl;
    sql_init();
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
        cerr << hp << " gethostbyname() " << host << endl;
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
        exit(1);
    }
}
void SendToUplink(const char *msg){
    send(sockfd, msg, strlen(msg), 0);
}
void IrcIdentify(string channel, string nick){
    // Identifikations-Funktion
    SendToUplink(("NICK " + nick + "\r\n").c_str());                    // NICK
    SendToUplink(("USER " + nick + " 0 0 :" + nick + "\r\n").c_str());  // Userdaten
    SendToUplink("PRIVMSG NickServ IDENTIFY password\r\n");             // Identifizieren
    SendToUplink(("JOIN #" + channel + "\r\n").c_str());                // Betreten Channel
    SendToUplink(("PRIVMSG #" + channel + " :HALLO!!!\r\n").c_str());   // Nachricht Nr.1
    
}
void PingParse(const string &buffer){
    // PingPongParser-Funktion
    size_t  pingPos = buffer.find("PING");
    if (pingPos != string::npos) {
        string pong("PONG" + buffer.substr(pingPos + 4) + "\r\n");
        cout << pong;
        SendToUplink(pong.c_str());  // wird an server gesendet
    }
}
void BotFunctions(const string &buffer){
    size_t pos = 0;
    
    if ((pos = buffer.find("Botname: xxx")) != string::npos) {
        SendToUplink(("PRIVMSG " + SearchUsername(buffer) + " :Was ist los? \r\n").c_str());
// Bot schliessen
    } else if (buffer.find("exit") != string::npos){
// Nachrich an channel senden.
        SendToUplink("PRIVMSG #channel :CIAO, schöhen Tag noch!\r\n"); // <-- Hier mus noch was gemacht werden!!!
// Nachrich an user senden.
        SendToUplink(("PRIVMSG " +  SearchUsername(buffer) + " :CIAO, schöhen Tag noch!\r\n").c_str());
        IrcDisconnect();
        exit(0);
// NICK - Name änderung
    } else if ((pos = buffer.find("name ")) != string::npos){
        string name("NICK " + buffer.substr(pos + 5) + "\r\n");
        SendToUplink(name.c_str());
    }
    if ((buffer.find("save")) != string::npos){
		b_save = true;
        SendToUplink(("PRIVMSG " +  SearchUsername(buffer) + " :Information wird gespeichert!!!\r\n").c_str());
	}
    if ((buffer.find("stopsave")) != string::npos){
		b_save = false;
        SendToUplink(("PRIVMSG " +  SearchUsername(buffer) + " :Information wird nicht mehr gespeichert!!!\r\n").c_str());
	}
    if ((buffer.find("print")) != string::npos){
        //SendToUplink(("PRIVMSG " +  SearchUsername(buffer) + " :Information wird gespeichert!!!\r\n").c_str());
        sqlite_getchatdatabase();
	}

    if (b_save) {
		time_t now = time(0);
		
		time(&now);
		tm* localtm = localtime(&now);
		
		// Aktuelles Datum und Zeit speichern 
		stringstream t;
		t << localtm->tm_year+1900 << "-" << localtm->tm_mon << "-" << localtm->tm_mday << " "
        << localtm->tm_hour	<< ":" << localtm->tm_min << ":" << localtm->tm_sec;
		
		// Name speichern
		string name(buffer.substr(buffer.find(":") + 1, buffer.find("!") - 1));
        // Message speichern
		string message(buffer.substr(buffer.find(" :") + 2, buffer.length() - 1));
        // Channelname speichern
		string channel	(buffer.substr(buffer.find("!") + 1, buffer.find("@") - 1));
		// Datenbank-Eintrag
		sql_addchat(name.c_str(), channel.c_str(), message.c_str(), t.str().c_str());
        cout << endl << " Information wurde erfolgreich gespeichert!" << endl;
	}
}
void IrcParse(string buffer){
    if (buffer.find("\r\n") == buffer.length() - 2)
        buffer.erase(buffer.length() - 2);
    
    PingParse(buffer);
    BotFunctions(buffer);
}
string SearchUsername(string s){
    string tmp = s.substr(s.find(":")+1,s.find("!")-1);
    return tmp;
}
#endif
