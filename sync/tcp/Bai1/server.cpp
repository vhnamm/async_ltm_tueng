#include<bits/stdc++.h>
#include<winsock2.h>
#include<ws2tcpip.h>
using namespace std;

int main(){
    WSADATA wsa;
    u_short port = 83;
    if(WSAStartup(MAKEWORD(2,2), &wsa) != 0){
        cout << "Loi khoi tao moi truong" << WSAGetLastError();
        return 1;
    }

    struct sockaddr_in server_addr;
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int rs;
    rs = bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));

    if(rs == SOCKET_ERROR){
        cout << "Loi bind" << WSAGetLastError();
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    rs = listen(server_socket, SOMAXCONN);
    if(rs == SOCKET_ERROR){
        cout << "Lang nghe that bai\n";
        WSACleanup();
        closesocket(server_socket);
        return 1;
    }
    cout << "Server dang lang nghe va cho ket noi...\n";

    sockaddr_in client_addr;
    int len = sizeof(client_addr);
    SOCKET client_socket = accept(server_socket, (SOCKADDR*)&client_addr, &len);
    cout << "Client da ket noi!\n";

    int number[2];
    recv(client_socket, (char*)number, sizeof(number), 0);

    int sum = number[0] + number[1];
    cout << "Da nhan cua client 2 so: " << number[0] << " " << number[1] << endl;

    send(client_socket, (char*)&sum, sizeof(sum), 0);
    cout << "Da gui tong cho client\n";

    WSACleanup();
    closesocket(client_socket);
    closesocket(server_socket);
    return 0;
}