
#include <arpa/inet.h>
#include <zconf.h>
#include <string>
#include <fstream>
#include "ConnectionManager.h"

ConnectionManager::ConnectionManager(std::string ip, int port) {



    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");

    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");

    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
    }

    read(sock, buffer, 1024*10);
    usleep(500000);
    identify();


}

void ConnectionManager::readFromSocket() {
    valread = read(sock, buffer, 1024*10);
    buffer[valread] = '\0';
    remove("myFile.txt");
    std::ofstream jsonFile("myFile.txt", std::ofstream::trunc);
    jsonFile << buffer;
    jsonFile.close();

}


void ConnectionManager::identify() {
    std::string identityString = "{\"identity\": \"client\", \"command\": \"identifying\"}";
    send(sock, identityString.c_str(), identityString.size(), 0);
}