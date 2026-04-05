#include <bits/stdc++.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
using namespace std;

class SinhVien
{
private:
    string msv, name;
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

    template <class Archieve>
    void serialize(Archieve &ar, const unsigned int version)
    {
        ar & msv;
        ar & name;
    }
};

int main(int argc, char* argv[]){

    
    string clientName = argv[1];

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    u_short port = 8387;
    struct sockaddr_in serverAddr;

    // create socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        cout << "khoi tao socket that bai\n";
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int cnRs = connect(clientSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
    if (cnRs == SOCKET_ERROR)
    {
        cout << "ket noi toi server that bai\n";
        WSACleanup();
        return 1;
    }
    cout << "Ket noi toi svr thanh cong\n";

   while (true){
        string msv, name;
        int lanGui = 0;

        cout << "\nNhap ma sinh vien (nhap exit de thoat): ";
        cin >> msv;

        if (msv == "exit")
            break;

        cin.ignore();

        cout << "Nhap ten sinh vien: ";
        getline(cin, name);

        SinhVien sv(msv, name);

        stringstream ss;
        boost::archive::binary_oarchive boa(ss);
        boa << sv;

        string svData = ss.str();
        int len = svData.size();



     
        send(clientSocket, svData.c_str(), len, 0);

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
    return 0;
}