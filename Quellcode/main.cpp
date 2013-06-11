// Arkadij Doronin 24.05.2013
// IRC-Bot

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <sqlite3.h>
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

using namespace std;

#ifdef WIN32
    SOCKET sockfd;
#else
    int sockfd;
#endif

#define SQLITEFILE "../SQLite/login.sqlite3"
sqlite3 *sqlitedb = NULL; // Pointer zur SQLite

struct BotParam {
	string server, port, nick, channel;
} botParam;

int main(){	
	pid_t PID[15];
	int countPID = 0;
	int chatNum = 1;
    
	if(sqlite3_open(SQLITEFILE, &sqlitedb)){
		cout << "Fehler beim sqlite3_open()";
		exit(1);
	}
    void createtable();
	if(!sqlite_getlogdatabase())
		cout << "Fehler beim sqlite_getdatabase" << endl;
    sqlite_getchatdatabase();
	while (chatNum > 0 && countPID < 10) {
        
		PID[countPID] = fork();
		if (PID[countPID] < 0) {
			cerr << "Fehler beim foke()" << endl;
			chatNum = 0;
		} else if(PID[countPID] == 0){
            cout << "BOT WIRD GESTARTET!!!" << endl;
			if(execlp ("./Bot", "./Bot", botParam.port.c_str(), botParam.server.c_str(), NULL))
            perror("Fehler beim execvp()");
			cout << "Das darf nicht ausgegeben werden!!!";
			chatNum = 0;
		} else {
			countPID++;
			while (chatNum > 0) {
				sleep(1);
				cout << "Wollen Sie weiteren Chat starten ( Y / N )?" << endl;
				string answer;
				cin >> answer;
				if(answer == "Y" || answer == "y"){
					chatNum = 1;
				} else if(answer == "N" || answer == "n"){
					chatNum = 0;
				} else {
					cout << "Die Eingabe ist unklar, bitte versuchen Sie noch ein mal! " << endl;
				}
			}
		}
	}
	sqlite3_close(sqlitedb);
	
	for (; countPID > 0; --countPID){
		cout << countPID << endl;
		while(0 < waitpid(PID[countPID],NULL,0));
	}
	return 0;
}






