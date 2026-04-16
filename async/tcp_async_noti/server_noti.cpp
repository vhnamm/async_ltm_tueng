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

    string getName() {
        return this->name;
    }

    string getMsv() {
        return this->msv;
    }

    void setMessage(string message){
        this->message = message;
    }

    string getMessage(){
        return this->message;
    }
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

vector<SOCKET> clientSockets;
vector<WSAEVENT> eventHandles;

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    u_short port = 8389;

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (listenSocket == INVALID_SOCKET) {
        cout << "Tao socket that bai\n";
        return 1;
    }

    if (bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Bind that bai\n";
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    listen(listenSocket, SOMAXCONN);

    cout << "Server listening...\n";

    WSAEVENT listenEvent = WSACreateEvent();

    WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE);

    eventHandles.push_back(listenEvent);

    while (true) {
        DWORD index = WSAWaitForMultipleEvents( eventHandles.size(), eventHandles.data(),
                                                FALSE, WSA_INFINITE, FALSE );


        int eventIndex = index - WSA_WAIT_EVENT_0;

        SOCKET currentSocket;

        if (eventIndex == 0) {
            currentSocket = listenSocket;
        } else {
            currentSocket = clientSockets[eventIndex - 1];
        }

        WSANETWORKEVENTS networkEvents;

        WSAEnumNetworkEvents(currentSocket, eventHandles[eventIndex], &networkEvents);

        if (networkEvents.lNetworkEvents & FD_ACCEPT) {
            SOCKADDR_IN clientAddr;
            int addrLen = sizeof(clientAddr);

            SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr,&addrLen);

            if (clientSocket == INVALID_SOCKET) {
                cout << "Accept failed\n";
            } else {
                WSAEVENT clientEvent = WSACreateEvent();

                WSAEventSelect(clientSocket, clientEvent, FD_READ | FD_CLOSE);

                clientSockets.push_back(clientSocket);
                eventHandles.push_back(clientEvent);

                cout << "Client connected\n";
            }
        }

        if (networkEvents.lNetworkEvents & FD_READ) {
            SOCKET currSocket = clientSockets[eventIndex - 1];

            string rcvData(1024, '\0');

            int byteReceived = recv(currSocket, rcvData.data(), rcvData.size(),0);

            if (byteReceived <= 0) {
                cout << "Receive failed\n";
            } else {
                stringstream ss;
                ss.write(rcvData.data(), byteReceived);

                boost::archive::binary_iarchive bia(ss);

                SinhVien sv;
                bia >> sv;

                cout << sv.getMessage() << endl;
                cout << "Da nhan cua client: "
                     << sv.getMsv()
                     << " "
                     << sv.getName()
                     << endl;

                bool found = false;
                string msv = sv.getMsv();

                transform(msv.begin(), msv.end(), msv.begin(), ::toupper);

                for (SinhVien s : csdl) {
                    if (s.getMsv() == msv) {
                        found = true;
                        break;
                    }
                }

                string msgToClient;

                if (!found) {
                    csdl.push_back({msv, sv.getName()});

                    msgToClient =
                        "Da them sinh vien " +
                        msv +
                        " " +
                        sv.getName() +
                        " vao csdl!";
                } else {
                    msgToClient = "Da ton tai ma sinh vien trong csdl";
                }

                send(currSocket, msgToClient.c_str(), msgToClient.size(), 0);
            }
        }

        if (networkEvents.lNetworkEvents & FD_CLOSE) {
            if (eventIndex == 0) {
                cout << "Listen socket closed\n";
                break;
            } else {
                cout << "Client disconnected\n";

                closesocket(clientSockets[eventIndex - 1]);
                WSACloseEvent(eventHandles[eventIndex]);

                clientSockets.erase(clientSockets.begin() + (eventIndex - 1));
                eventHandles.erase(eventHandles.begin() + eventIndex);
            }
        }
    }

    for (SOCKET s : clientSockets) {
        closesocket(s);
    }

    for (int i = 1; i < eventHandles.size(); i++) {
        WSACloseEvent(eventHandles[i]);
    }

    closesocket(listenSocket);
    WSACloseEvent(listenEvent);
    WSACleanup();

    return 0;
}