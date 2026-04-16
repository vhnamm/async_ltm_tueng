#include<bits/stdc++.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include<boost/serialization/vector.hpp>
using namespace std;

class SinhVien{
    private:
        string msv, hoTen;
        bool isExist = false;
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int version){
            ar& msv;
            ar& hoTen;
            ar& isExist;
        }
    public:
        SinhVien(){}
        SinhVien(string msv, string hoTen){
            this->msv = msv;
            this->hoTen = hoTen;
        }
        string getMsv(){return this->msv;}
        string getHoTen(){return this->hoTen;}
        bool isExists(){return this->isExist;}
        void setEx(bool ex){
            this->isExist = ex;
        }
        void setMsv(string msv){
            this->msv = msv;
        }
};

int main(){
    WSADATA wsa;
    u_short port = 8387;
    if(WSAStartup(MAKEWORD(2,2), &wsa) != 0){
        cout << "Loi khoi tao moi truong" << WSAGetLastError();
        return 1;
    }

    struct sockaddr_in server_addr;

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int rs;
    rs = connect(client_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
    if(rs == SOCKET_ERROR){
        cout << "Ket noi toi server that bai\n";
        WSACleanup();
        closesocket(client_socket);
        return 1;
    }
    
    string msv, hoTen;
    cout << "Nhap ma sinh vien:\n";
    cin>>msv;
    cin.ignore();
    cout << "Nhap ten sinh vien:\n";
    getline(cin, hoTen);
    //gui
    SinhVien sv(msv, hoTen);
    stringstream ss;
    boost::archive::binary_oarchive boa(ss);

    boa << sv;   
    string svData = ss.str();
    int len = svData.length();
    send(client_socket, svData.data(), len, 0);
    
    //nhan
    SinhVien rcvData;
    
    string buffer(2048, '\0');

    int received = recv(client_socket, buffer.data(), buffer.size(), 0);

    if (received <= 0) {
        cout << "Loi nhan du lieu\n";
        return 1;
    }

    stringstream ss2;
    ss2.write(buffer.data(), received);
    boost::archive::binary_iarchive bia(ss2);
    bia >> rcvData;
    if(rcvData.isExists()){
        cout << "Ma sinh vien da ton tai, khong the them!\n";
    }else{
        cout << "Them thanh cong sinh vien vao csdl\n";
    }
    WSACleanup();
    closesocket(client_socket);
    return 0;
}