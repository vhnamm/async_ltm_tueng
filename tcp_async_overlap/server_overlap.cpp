#include <bits/stdc++.h>
#include <winsock2.h>
#include <windows.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

#define BUFFER_SIZE 4096
#define OP_READ  0
#define OP_WRITE 1

class SinhVien {
private:
    string msv, name;
    friend class boost::serialization::access;
public:
    SinhVien() {}
    SinhVien(string msv, string name) {
        this->msv = msv;
        this->name = name;
    }
    string getName() { return this->name; }
    string getMsv()  { return this->msv;  }
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & msv;
        ar & name;
    }
};

int main(){
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    u_short port = 8387;
    struct sockaddr_in serverAddr;

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
}