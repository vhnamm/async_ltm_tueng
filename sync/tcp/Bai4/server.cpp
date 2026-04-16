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

vector<SinhVien> v = {
    SinhVien("PT227", "Hoai Nam"),
    SinhVien("PT225", "Thi Nguyen"),
    SinhVien("PT234", "Van A"),
};

int main(){
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    struct sockaddr_in server_addr;
    u_short port = 8387;

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));

    listen(server_socket, SOMAXCONN);
    cout << "Server dang lang nghe...\n";

    struct sockaddr_in client_addr;
    int len = sizeof(client_addr);
    SOCKET client_socket = accept(server_socket, (SOCKADDR*)&client_addr, &len);

    cout << "Da ket noi voi client\n";

    string buffer(2048, '\0');
    int bytes = recv(client_socket, buffer.data(), buffer.size(), 0);
    stringstream ss;
    ss.write(buffer.data(), bytes);

    boost::archive::binary_iarchive bia(ss);
    SinhVien s;
    bia >> s;
    string ma2 = s.getMsv();
    transform(ma2.begin(), ma2.end(), ma2.begin(), ::toupper);

    bool found = false;
    for(SinhVien sv : v){
        string ma1 = sv.getMsv();
        
        transform(ma1.begin(), ma1.end(), ma1.begin(), ::toupper);
        
        if(ma1 == ma2){
            found = true;
            break;
        }
    }

    if(!found){
        s.setMsv(ma2);
        v.push_back(s);
        cout << "Da them moi sv vao csdl\n";
    }

    //gui
    s.setEx(found);
    stringstream ss2;
    boost::archive::binary_oarchive boa(ss2);
    boa << s;

    string sinhvien = ss2.str();
    int lenSv = sinhvien.length();
    send(client_socket, sinhvien.data(), lenSv, 0);
    cout << "Da gui ket qua cho client";
}