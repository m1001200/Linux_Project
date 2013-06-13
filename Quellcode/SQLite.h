#ifndef _SQLite_h
#define _SQLite_h
#include <iostream>

struct BotParam {
    std::string server, port, nick, channel;
};
std::string sql_getchat();
std::string sql_lastseen(const char name[]);
void sql_close();
void sql_init();
void createtable();
BotParam sqlite_getlogdatabase(int ID);
void sql_addchat(const char name[],
                 const char channel[],
                 const char chat[],
                 const char date[]);



#endif
