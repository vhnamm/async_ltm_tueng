#include<bits/stdc++.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<boost/archive/binary_iarchive.hpp>
#include<boost/archive/binary_oarchive.hpp>

using namespace std;

class SinhVien
{
private:
    string msv, name;
    string message;
    friend class boost::serialization::access;

public:
    SinhVien(string msv, string name)
    {
        this->msv = msv;
        this->name = name;
    }

    string getName()
    {
        return this->name;
    }
    string getMsv() { return this->msv; }

    void setMessgae(string message){
        this -> message = message;
    }
    template <class Archieve>
    void serialize(Archieve &ar, const unsigned int version)
    {
        ar & msv;
        ar & name;
        ar & message;
    }
};

int main(int argc, char* argv[]){
    string clientName = argv[1];

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    struct sockaddr_in serverAddr;
    u_short port = 8389;

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int rs;
    rs = connect(clientSocket, (SOCKADDR*) &serverAddr, sizeof(serverAddr));
    if(rs == SOCKET_ERROR){
        cout << "Ket noi toi server qua cong 8389 that bai\n";
        WSACleanup();
        return 1;
    }
    
    cout << "Ket noi toi server thanh cong\n";

    while(true){
        string msv, name;
        cout << "Nhap ma sinh vien (nhap exit de thoat): ";
        cin >> msv;

        if(msv == "exit"){
            WSACleanup();
            closesocket(clientSocket);
            return 1;
        }

        cin.ignore();
        cout << "Nhap ten sinh vien: ";
        getline(cin, name);

        SinhVien sv = SinhVien(msv, name);
        sv.setMessgae("Client " + clientName + " da gui sinh vien cho server");

        stringstream ss;
        boost::archive::binary_oarchive boa(ss);
        boa << sv;

        string svData = ss.str();
        send(clientSocket, svData.data(), svData.size(), 0);
        
         cout << "Da gui sinh vien: "
            << sv.getMsv()
            << " - "
            << sv.getName() << endl;

        char buffer[1024];
        int received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (received > 0)
        {
            buffer[received] = '\0';
            cout << "Phan hoi tu server: " << buffer << endl;
        }

    }

}