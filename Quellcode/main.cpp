// Arkadij Doronin 24.05.2013
// IRC-Bot

#ifndef main_cpp
#define main_cpp

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "SQLite.h"

using namespace std;

int main(){	
	pid_t PID[15];
	int countPID = 0;
    
    // sqlite_BD wird geöffnet
    sql_init();
    
    BotParam botParam[2];
    
    // Hier werden Parameter aus der sqlite_BD für jeden Bot geladen.
    
    // Bot_1-Parameter (port,server,channel,nick)
    botParam[0]= sqlite_getlogdatabase(1);
    // Bot_2-Parameter (port,server,channel,nick)
    botParam[1]= sqlite_getlogdatabase(2);
    
    // sqlite_BD wird geschlossen
    sql_close();
	while (countPID < 2) {
        
		PID[countPID] = fork();
		if (PID[countPID] < 0) {
			exit(1);
		} else if(PID[countPID] == 0){ // Übergabe an Bots
			if(execlp ("./Bot",
                       "./Bot",
                       botParam[countPID].port.c_str(),
                       botParam[countPID].server.c_str(),
                       botParam[countPID].channel.c_str(),
                       botParam[countPID].nick.c_str(), NULL));
			exit(1);
		} else {
            countPID++;
        }
	}
	return 0;
}

#endif




