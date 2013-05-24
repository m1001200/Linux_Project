//  client.c
//  
//  Created by Arkadij Doronin on 20.05.13.

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

int main() {
    
    int socket_1;
    struct sockaddr_in server;
    char message[1024], resp[1024];
    
// Hier wird der Socket erzeugt.
    socket_1 = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_1 < 0) {
        perror("\nSocket konnte nicht erzeugt werden!\n");
        return 1;
    }
// Hier wir der 'struct' gefüllt
// (Protokollfamilie, local: inet_addr("127.0.0.1") und Port(1234)).
    
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(1234);
    
// Hier wird die Verbindung auffgebaut. 
    if (connect(socket_1, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) < 0) {
        perror("\nVerbindung konnte nicht aufgebaut werden!\n");
        return 1;
    }
    printf("\n+++++++++++++++ CLIENT ++++++++++++++++++++++++++\n");
    printf("\nVerbindung wurde erfolgreich hergestellt!\n");
    
    char username[256];
    printf("\nUSERNAME: "); cin.getline(username, 256);
    
// Hier wird der Username gesendet.
    if (write(socket_1, username, strlen(username)) < 0) {
        perror("\nFehler beim Username-Senden!\n");
        return 1;
    }
    
    while(1) {
        cout << username << " :: ";
        cin.getline(message, 1024);
        
// Hier werden Daten gesendet.
        if (write(socket_1, message, strlen(message)+1) < 0) {
            perror("\nFehler beim Daten-Senden!\n");
            return 1;
        }
// Hier werden Daten empfangen.
        
        /*
         if (read(socket_1, resp, 1024) < 0) {
         perror("Lesen fehlgeschlagen!");
         return 1;
         }
         printf("Reply: %s\n", response); */
    }
    close(socket_1);  // socket wird geschlossen.
    return 0;
}

