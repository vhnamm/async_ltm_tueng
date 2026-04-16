#include<bits/stdc++.h>
#include<winsock2.h>
#include<ws2tcpip.h>
using namespace std;

class TamGiac{
    private:
        double a, b, c;
        bool isTriangle;
    public:
        TamGiac(){} 
        TamGiac(double a, double b, double c){
            this->a = a;
            this->b = b;
            this->c = c;
        }
        void setIsTriangle(bool isTri){
            this->isTriangle = isTri;
        }
        double getA(){return this-> a;}
        double getB(){return this->b;}
        double getC(){return this->c;}
        bool getIsTri(){return this->isTriangle;}
};

int main(){
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    u_short port = 8386;

    struct sockaddr_in server_addr;

    SOCKET server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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

    cout << "Server dang cho du lieu...\n";

    sockaddr_in client_addr;
    int len = sizeof(client_addr);

    TamGiac t;

    recvfrom(server_socket,(char*)&t,sizeof(t),0,
             (SOCKADDR*)&client_addr,&len);

    if(t.getA() + t.getB() <= t.getC() ||
       t.getB() + t.getC() <= t.getA() ||
       t.getA() + t.getC() <= t.getB()){
        t.setIsTriangle(false);
    }else{
        t.setIsTriangle(true);
    }

    sendto(server_socket,(char*)&t,sizeof(t),0,
           (SOCKADDR*)&client_addr,len);

    cout << "Da gui ket qua cho client\n";

    closesocket(server_socket);
    WSACleanup();
}