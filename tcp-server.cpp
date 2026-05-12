#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <thread>
#include <algorithm>

std::vector<int> clients;

int client_1;
int client_2;


void handle_client(int client_fd) {
    char buffer[1024];

    while (true) {
        ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0) {                  // remove client from list if they disconnect
            std::cout << "Client disconnected\n";
            close(client_fd);

            clients.erase(std::remove(clients.begin(), clients.end(), client_fd), clients.end());

            break;
        }

        buffer[bytes_received] = '\0';

        std::cout << "Message: " << buffer << "\n";

        // Forward to all OTHER clients
        for (int other_client : clients) {
            if (other_client != client_fd) {
                send(other_client, buffer, bytes_received, 0);
            }
        }
    }
}




int main() {
    int server_fd = socket(AF_INET,SOCK_STREAM, 0);


    //Listening Socket
    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;  // IPv4
    server_addr.sin_port = htons(9090);   // Port 9090
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // This machine

    bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr) );  // Attach socket to IP and Port 


    listen(server_fd, 5);       // 5 = how many connections can wait

    std::cout << "Listening on 127.0.0.1:9090\n";

    while(true) {

        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);   // accept() is blocking, waits for connection

        std::cout << "accept() returned: "
          << client_fd
          << "\n";

        char client_ip[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));   // conversion from network binary format

        std::cout << "Client connected from "
                << client_ip << ":"
                << ntohs(client_addr.sin_port)
                << "\n";

        clients.push_back(client_fd);           // push_back removes from end of list

        std::thread client_thread(handle_client, client_fd);   // separate thread to handle client connections

        client_thread.detach();
    }
    //return 0;
    close(server_fd);

}