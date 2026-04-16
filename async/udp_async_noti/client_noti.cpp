#include<bits/stdc++.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<boost/archive/binary_iarchive.hpp>
#include<boost/archive/binary_oarchive.hpp>

using namespace std;

class SinhVien {
private:
    string msv, name;
    string message;
    friend class boost::serialization::access;
public:
    SinhVien(string msv, string name) {
        this->msv = msv;
        this->name = name;
    }
    string getName() { return this->name; }
    string getMsv()  { return this->msv; }
    void setMessgae(string message) { this->message = message; }
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & msv;
        ar & name;
        ar & message;
    }
};

int main(int argc, char* argv[]) {
    string clientName = argv[1];

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    struct sockaddr_in serverAddr;
    u_short port = 8389;

  
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_port        = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");



    cout << "Ket noi toi server thanh cong\n";

    while (true) {
        string msv, name;
        cout << "Nhap ma sinh vien (nhap exit de thoat): ";
        cin >> msv;

        if (msv == "exit") {
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
        int addrLen = sizeof(serverAddr);

        
        sendto(clientSocket, svData.data(), svData.size(), 0,
               (SOCKADDR*)&serverAddr, addrLen);

        cout << "Da gui sinh vien: " << sv.getMsv() << " - " << sv.getName() << endl;

        char buffer[1024];
        int received = recvfrom(clientSocket, buffer, sizeof(buffer), 0,
                                (SOCKADDR*)&serverAddr, &addrLen);

        if (received > 0) {
            buffer[received] = '\0';
            cout << "Phan hoi tu server: " << buffer << endl;
        }
    }
}