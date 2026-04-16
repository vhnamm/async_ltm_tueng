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
    u_short port = 8386;
    if(WSAStartup(MAKEWORD(2,2), &wsa) != 0){
        cout << "Loi khoi tao moi truong" << WSAGetLastError();
        return 1;
    }

    struct sockaddr_in server_addr;

    SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    double a,b,c;
    cout << "Nhap canh thu 1:\n";
    cin>>a;
    cout << "Nhap canh thu 2:\n";
    cin>>b;
    cout << "Nhap canh thu 3:\n";
    cin >> c;

    TamGiac t(a, b, c);

    sendto(client_socket,(char*)&t,sizeof(t),0,
           (SOCKADDR*)&server_addr,sizeof(server_addr));

    TamGiac data;
    int len = sizeof(server_addr);

    recvfrom(client_socket,(char*)&data,sizeof(data),0,
             (SOCKADDR*)&server_addr,&len);

    if(data.getIsTri()){
        cout << "Day chinh la 1 tam giac do cau oi!\n";
    }else{
        cout << "3 canh do ko phai tam giac cau nhe\n";
    }

    WSACleanup();
    closesocket(client_socket);
    return 0;
}