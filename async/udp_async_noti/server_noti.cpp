#include <bits/stdc++.h>
#include <winsock2.h>
#include <windows.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

class SinhVien {
private:
    string msv, name;
    string message;
    friend class boost::serialization::access;
public:
    SinhVien() {}
    SinhVien(string msv, string name) {
        this->msv = msv;
        this->name = name;
    }
    string getName()    { return this->name; }
    string getMsv()     { return this->msv; }
    string getMessage() { return this->message; }
    void setMessage(string message) { this->message = message; }
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & msv;
        ar & name;
        ar & message;
    }
};

vector<SinhVien> csdl = {
    {"SV01", "Tran Van A"},
    {"SV02", "Le Van B"},
    {"SV03", "Cham Van Chi"}
};

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    u_short port = 8389;

    sockaddr_in serverAddr;
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_port        = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");


    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Tao socket that bai\n";
        return 1;
    }

    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Bind that bai\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }


    cout << "Server dang cho du lieu gui den...\n";

    //Chỉ 1 event cho 1 socket duy nhất, đăng ký FD_READ
    WSAEVENT serverEvent = WSACreateEvent();
    WSAEventSelect(serverSocket, serverEvent, FD_READ);

    // Không cần vector clientSockets, eventHandles — chỉ 1 socket 1 event
    while (true) {
        //Chờ trên 1 event duy nhất
        DWORD index = WSAWaitForMultipleEvents(1, &serverEvent,
                                               FALSE, WSA_INFINITE, FALSE);

        WSANETWORKEVENTS networkEvents;
        WSAEnumNetworkEvents(serverSocket, serverEvent, &networkEvents);

        // Không có FD_ACCEPT, không có FD_CLOSE
        if (networkEvents.lNetworkEvents & FD_READ) {
            char rcvData[1024];
            SOCKADDR_IN clientAddr;
            int addrLen = sizeof(clientAddr);

            // cần lưu clientAddr để gửi lại
            int byteReceived = recvfrom(serverSocket, rcvData, sizeof(rcvData), 0,
                                        (SOCKADDR*)&clientAddr, &addrLen);

            if (byteReceived <= 0) {
                cout << "Receive failed\n";
            } else {
                stringstream ss;
                ss.write(rcvData, byteReceived);
                boost::archive::binary_iarchive bia(ss);
                SinhVien sv;
                bia >> sv;

                cout << sv.getMessage() << endl;
                cout << "Da nhan cua client: " << sv.getMsv() << " " << sv.getName() << endl;

                bool found = false;
                string msv = sv.getMsv();
                transform(msv.begin(), msv.end(), msv.begin(), ::toupper);

                for (SinhVien s : csdl) {
                    if (s.getMsv() == msv) { found = true; break; }
                }

                string msgToClient;
                if (!found) {
                    csdl.push_back({msv, sv.getName()});
                    msgToClient = "Da them sinh vien " + msv + " " + sv.getName() + " vao csdl!";
                } else {
                    msgToClient = "Da ton tai ma sinh vien trong csdl";
                }

                // gửi lại đúng địa chỉ client
                sendto(serverSocket, msgToClient.c_str(), msgToClient.size(), 0,
                       (SOCKADDR*)&clientAddr, addrLen);
            }
        }
    }

    closesocket(serverSocket);
    WSACloseEvent(serverEvent);
    WSACleanup();
    return 0;
}