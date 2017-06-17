#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>



#define PORT 8888


class ConnectionManager {
public:
    ConnectionManager(std::string ip, int port);
    void readFromSocket();
    void actFromJSONFile();
    void identify();
    int sock = 0, valread;
    char buffer[1024*10] = {0};


private:

    struct sockaddr_in address;

    struct sockaddr_in serv_addr;
    //char *hello = "Hello from client";


};
#endif // CONNECTIONMANAGER_H