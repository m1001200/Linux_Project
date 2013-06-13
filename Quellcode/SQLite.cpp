// Arkadij Doronin 24.05.2013
// IRC-Bot

#ifndef SQLite_cpp
#define SQLite_cpp

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <sqlite3.h>
#include "SQLite.h"

using namespace std;


#define SQLITEFILE "../SQLite/login.sqlite3"
sqlite3 *sqlitedb = NULL; // Pointer zur SQLite

void sql_init(){
    sqlite3_open(SQLITEFILE, &sqlitedb);
    createtable();
}
void sql_addchat(const char name[],const char channel[],const char chat[], const char date[]){
	char tmp[1200];
	sprintf(tmp,"INSERT INTO chat (nick,channel, chat, date) VALUES ('%s', '%s', '%s', '%s');", name, channel, chat, date);
	sqlite3_exec(sqlitedb, tmp, NULL, NULL, NULL);
}
void createtable(){
    sqlite3_exec(sqlitedb, "CREATE TABLE chat (id integer primary key, nick text, channel text, chat text, date text);", NULL, NULL, NULL);
}
BotParam sqlite_getlogdatabase(int ID){
	sqlite3_stmt *vm;
	sqlite3_prepare(sqlitedb, "SELECT * FROM loginglist", -1, &vm, NULL);
	BotParam botParam;
    int index = 1;
	while (sqlite3_step(vm) != SQLITE_DONE) {
        if(ID == index){
            botParam.server = (char*)sqlite3_column_text(vm, 1);
            stringstream ss;
            ss << sqlite3_column_int(vm, 2);
            botParam.port = ss.str();
            botParam.nick = (char*)sqlite3_column_text(vm, 3);
            botParam.channel = (char*)sqlite3_column_text(vm, 4);
        }
    index++;
    }
	sqlite3_finalize(vm);
	
	return botParam;
}
string sql_lastseen(const char name[]){
	
	sqlite3_stmt *vm;
	
	stringstream s;
	s << "SELECT * FROM chat WHERE nick ='" << name << "' ORDER BY id DESC LIMIT 1";
	sqlite3_prepare(sqlitedb, s.str().c_str(), -1, &vm, NULL);
	
	stringstream ss;
	
	while (sqlite3_step(vm) != SQLITE_DONE)
	{
		ss	<< (char*)sqlite3_column_text(vm, 1) << " "
        << (char*)sqlite3_column_text(vm, 4) << std::endl;
	}
	
	sqlite3_finalize(vm);
	
	return ss.str();
}
string sql_getchat(){
	sqlite3_stmt *vm;
	if(sqlite3_prepare(sqlitedb, "SELECT * FROM chat", -1, &vm, NULL) != 0)
		createtable();
	
	stringstream ss;
    
	while (sqlite3_step(vm) != SQLITE_DONE)
	{
		ss	<< (char*)sqlite3_column_text(vm, 1) << " "
            << (char*)sqlite3_column_text(vm, 3) << " "
            << (char*)sqlite3_column_text(vm, 4) << "$";
	}
	
	sqlite3_finalize(vm);
	
	return ss.str();
}
void sql_close(){
    sqlite3_close(sqlitedb);
}

#endif





