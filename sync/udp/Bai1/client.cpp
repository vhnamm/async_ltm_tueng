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

    SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int a, b;
    cout << "Nhap so thu 1:\n";
    cin >> a;
    cout << "Nhap so thu 2:\n";
    cin >> b;

    int nums[2] = {a, b};

    sendto(client_socket, (char*)&nums, sizeof(nums), 0,
           (SOCKADDR*)&server_addr, sizeof(server_addr));

    cout << "Da gui 2 so toi server\n";

    int tong;

    int len = sizeof(server_addr);
    recvfrom(client_socket, (char*)&tong, sizeof(tong), 0,
             (SOCKADDR*)&server_addr, &len);

    cout << "Da nhan lai cua server tong 2 so: " << tong << endl;

    WSACleanup();
    closesocket(client_socket);
    return 0;
}