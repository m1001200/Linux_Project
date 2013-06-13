#ifndef _ircBot_h
#define _ircBot_h

#include <iostream>


using namespace std;

string SearchUsername(string s);
void IrcConnect(const int port, const char* host);
void IrcDisconnect();
void SendToUplink(const char *msg);
void IrcIdentify(string channel, string nick);
void PingParse(const string & buffer);
void BotFunctions(const string & buffer);
void IrcParse(string buffer);

#endif
