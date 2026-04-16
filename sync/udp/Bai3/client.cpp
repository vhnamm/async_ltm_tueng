#include<bits/stdc++.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include<boost/serialization/vector.hpp>
using namespace std;

class Account{
    private:
        string username, password;
        bool isExist = false;
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int version){
            ar& username;
            ar& password;
            ar& isExist;
        }

    public:
        Account(){} 
        Account(string username, string password){
           this->username = username;
           this->password = password;
        }
        string getUsername(){return this->username;}
        string getPassword(){return this->password;}
        void setIsExist(bool ex){
            this->isExist = ex;
        }
        bool isExists(){return this->isExist;}
};

int main(){
    WSADATA wsa;
    u_short port = 8387;
    if(WSAStartup(MAKEWORD(2,2), &wsa) != 0){
        cout << "Loi khoi tao moi truong" << WSAGetLastError();
        return 1;
    }

    struct sockaddr_in server_addr;

    SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    string u, p;
    cout << "Nhap tai khoan:\n";
    cin>>u;
    cout << "Nhap mat khau:\n";
    cin>>p;

    // serialize
    Account a(u, p);
    stringstream ss;
    boost::archive::binary_oarchive boa(ss);
    boa << a;

    string accData = ss.str();

    sendto(client_socket, accData.data(), accData.size(), 0, (SOCKADDR*)&server_addr, sizeof(server_addr));

    // nhan
    string buffer(2048, '\0');
    int len = sizeof(server_addr);

    int received = recvfrom(client_socket, buffer.data(), buffer.size(), 0, (SOCKADDR*)&server_addr, &len);

    if (received <= 0) {
        cout << "Loi nhan du lieu\n";
        return 1;
    }

    Account data;

    stringstream ss2;
    ss2.write(buffer.data(), received);

    boost::archive::binary_iarchive bia(ss2);
    bia >> data;

    if(data.isExists()){
        cout << "Tai khoan da ton tai trong csdl\n";
    }else{
        cout << "Ko ton tai tai khoan nay\n";
    }

    WSACleanup();
    closesocket(client_socket);
    return 0;
}