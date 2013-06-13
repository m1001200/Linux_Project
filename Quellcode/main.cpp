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
    sql_init();
    BotParam botParam[2];
    botParam[0]= sqlite_getlogdatabase(1);
    botParam[1]= sqlite_getlogdatabase(2);
    sql_close();
	while (countPID < 2) {
        
		PID[countPID] = fork();
		if (PID[countPID] < 0) {
			cerr << "Fehler beim fork()" << endl;
			exit(1);
		} else if(PID[countPID] == 0){
            cout << "BOT WIRD GESTARTET!!!" << endl;
			if(execlp ("./Bot", "./Bot", botParam[countPID].port.c_str(), botParam[countPID].server.c_str(), botParam[countPID].channel.c_str(), botParam[countPID].nick.c_str(), NULL))
            perror("Fehler beim execvp()");
			cout << "Das darf nicht ausgegeben werden!!!";
			exit(1);
		} else {
            countPID++;
        }
	}
	return 0;
}

#endif




