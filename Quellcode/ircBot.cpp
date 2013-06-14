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
string NICKNAME = "ara";

int main(int argc, char *argv[]){
    
	if (argc < 4) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    NICKNAME = argv[4];
    IrcConnect(atoi(argv[1]),argv[2]);  // server, port
    IrcIdentify(argv[3], argv[4]);      // channel, nick
    sql_init();
    while (1) {
        char buffer[BUFF_SIZE + 1] = {0};
        if (recv(sockfd, buffer, BUFF_SIZE * sizeof(char), 0) < 0) {
            IrcDisconnect();
            exit(1);
        }
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
        IrcDisconnect();
        exit(1);
    }
    hostent *hp = gethostbyname(host);
    if (!hp) {
        IrcDisconnect();
        exit(1);
    }
    // Socket Standartfunktionen
    sockaddr_in sin;
    memset((char*)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    memcpy((char*)&sin.sin_addr, hp->h_addr, hp->h_length);
    sin.sin_port = htons(port);
    memset(&(sin.sin_zero), 0, 8 * sizeof(char));
    
    if (connect(sockfd, (sockaddr*)&sin, sizeof(sin)) == -1) {
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
        SendToUplink(pong.c_str());  // wird an server gesendet
    }
}
void BotFunctions(const string &buffer){
    size_t pos = 0;
    
    if ((pos = buffer.find("Botname: xxx")) != string::npos) {
        SendToUplink(("PRIVMSG " + SearchUsername(buffer) + " :Was ist los? \r\n").c_str());
    // Bot schliessen
    } else if (buffer.find("exit") != string::npos){
    // Nachrich an user senden.
        SendToUplink(("PRIVMSG " +  SearchUsername(buffer) + " :Ciao, schöhen Tag noch!\r\n").c_str());
        IrcDisconnect();
        exit(0);
    // NICK - Name änderung
    } else if ((pos = buffer.find("name ")) != string::npos){
        string name("NICK " + buffer.substr(pos + 5) + "\r\n");
        SendToUplink(name.c_str());
    }
    if ((pos = buffer.find("lastseen ")) != string::npos) {
        string s = buffer.substr(pos+9);
        
        SendToUplink(("PRIVMSG " + SearchUsername(buffer) + " " + sql_lastseen(SearchUsername(s).c_str()) + "\r\n").c_str());
         return;
    }
    // Log wird gespeichert
    if ((buffer.find("start")) != string::npos){
		b_save = true;
        return;
	}
    // Log wird nicht mehr gespeichert
    if ((buffer.find("stop")) != string::npos){
		b_save = false;
        return;
	}
    // if message has => join <= then the bot join a channel given in params <=
	if((pos = buffer.find("join")) != string::npos){
		string tmp = "JOIN " + buffer.substr(pos + 5, buffer.length() -1) + "\r\n";
		SendToUplink(tmp.c_str());
		return;
	}
	// if message has => leave <= then the bot part a channel given in params <=
	if((pos = buffer.find("leave")) != string::npos){
		string tmp = "PART " + buffer.substr(pos + 5, buffer.length() -1) + "\r\n";
		SendToUplink(tmp.c_str());
		return;
	}
    // Log wird ausgegeben
    if ((buffer.find("print")) != string::npos){
        string s = sql_getchat();
        size_t anf_pos = 0;
        size_t end_pos = s.find("$");
        while (end_pos != s.npos) {
            SendToUplink(("PRIVMSG " +  SearchUsername(buffer) + " " + s.substr(anf_pos, end_pos) + "\r\n").c_str());
            s.erase(anf_pos, end_pos+1);
            end_pos = s.find("$");        
        }
        return;
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
	}
}
// Hier wird Endung abgeschnitten 
void IrcParse(string buffer){
    if (buffer.find("\r\n") == buffer.length() - 2)
        buffer.erase(buffer.length() - 2);
    if(buffer.find("NICK ") != buffer.npos){
        SendToUplink(("NICK " + NICKNAME + "\r\n").c_str());
    }
    PingParse(buffer);
    BotFunctions(buffer);
}
string SearchUsername(string s){
    string tmp = s.substr(s.find(":")+1,s.find("!")-1);
    return tmp;
}
#endif
