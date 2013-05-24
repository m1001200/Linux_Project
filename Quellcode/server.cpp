//  server.cpp
//  
//  Created by Arkadij Doronin on 20.05.13.


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <iostream>

using namespace std;

#define PORT 1234
#define BUF_SIZE 1024

int main() {
    int socket_1, socket_2, received, child_pid;
    struct sockaddr_in server;
    char buf[BUF_SIZE];
    char username[256];
    int username_length;

// Hier wird der 'socket_1' erzeugt
    socket_1 = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_1 < 0) {
        perror("\nSocket konnte nicht erzeugt werden!\n");
        exit(1);
    }
// Hier wir der 'struct' gefuellt (Protokollfamilie, Interface und Port(1234))
    server.sin_family       = AF_INET;      
    server.sin_addr.s_addr  = htonl(INADDR_ANY);   
    server.sin_port         = htons(PORT);      
// Hier wird der 'socket_1' konfiguriert
    if (bind(socket_1, (struct sockaddr*) &server, sizeof(struct sockaddr_in))) {
        perror("\nSocket konnte nicht konfiguriert werden!\n");
        exit(1);
    }
// Socket wird in den listen-Zustand übergeben 
    listen(socket_1, 5);
    printf("\nSocket wurde vorbereitet!\n");
    
    while(1) {
        printf("\nEs wird auf eine Client-Verbindung gewartet\n");
// Hier wird eine Verbindung akzeptiert, sonst wird bloockiert.
        socket_2 = accept(socket_1, 0, 0);
        printf("\n++++++++++++++++ SERVER +++++++++++++++++++++++++\n\n");
        if (socket_2 < 0) {
            perror("Fehler beim accept() Aufruf !");
            continue;
        } else {
            if (username_length = read(socket_2, username, 256)) {
                if (username_length < 0)
                    perror("\nFehler beim read() Aufruf !\n");
                else {
                    username[username_length] = '\0';           // usernamensende
                    write(1, username, username_length);        // usernamensausgabe
                    write(1, " ist angemeldet.\n", 32);         // standartausgabe
                    //send(sock2, "yes", 3, 0);
                }
            }
        }
// Hier wird ein Kinderprozess erzeugt.
        if((child_pid = fork()) == 0) {
          
            while (received = read(socket_2, buf, BUF_SIZE)) {
                write(1, username, username_length);
                write(1, " schreibt :: ", 13);
                buf[received] = '\0';
                write(1, buf, received);
                write(1, "\n", 1);
            }
            printf("\nEnde der Kommunikation\n");
            close(socket_2);
        } else if(child_pid > 0) {
            // Elternprozess
        } else {
            perror("\nKindererzeugung fork() hat fehlgeschlagen!\n");
            close(socket_2);    // socket_2 wird geschlossen.
            }
        }
    close(socket_1);    // socket_1 wird geschlossen.
    return 0;
}
