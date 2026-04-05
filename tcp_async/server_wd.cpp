#include <bits/stdc++.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

class SinhVien
{
private:
    string msv, name;
    friend class boost::serialization::access;

public:
    SinhVien() {}
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

vector<SinhVien> csdl = {
    {"SV01", "Tran Van A"},
    {"SV02", "Le Van B"},
    {"SV03", "Cham Van Chi"}
};

LRESULT CALLBACK WindowProc(HWND hwd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == 1111)
    {
        SOCKET currSocket = (SOCKET)wParam;
        SOCKADDR_IN clientAddr;
        int addrLen = sizeof(clientAddr);

        int event = WSAGETSELECTEVENT(lParam);

        switch (event){
            case FD_ACCEPT: {
                cout << "Accept evnt = " << event << endl;
                SOCKET clientSocket = accept(wParam, (SOCKADDR *)&clientAddr, &addrLen);

                if (clientSocket == SOCKET_ERROR){
                    cout << "Chap nhan that bai\n";
                }
                else{
                    WSAAsyncSelect(clientSocket, hwd, 1111, FD_READ | FD_WRITE | FD_CLOSE);
                }
            break;
            }

            case FD_READ: {
                string rcvData(1024, '\0');

                int byteReceived = recv(currSocket, rcvData.data(), rcvData.size(), 0);

                if (byteReceived <= 0)
                {
                    cout << "Loi nhan du lieu\n";
                    return 0;
                }

                stringstream ss;
                ss.write(rcvData.data(), byteReceived);
                boost::archive::binary_iarchive bia(ss);

                SinhVien sv;

                bia >> sv;
                cout << "Da nhan cua client: " << sv.getMsv() << " " << sv.getName() << endl;
                bool found = false;
                string msv = sv.getMsv(); 

                transform(msv.begin(), msv.end(), msv.begin(), ::toupper);

                
                for(SinhVien s : csdl){
                    if(s.getMsv() == msv){
                        found = true;
                        break;
                    }
                }

                if(!found){ //neu chua ton tai trong csdl
                    csdl.push_back({msv, sv.getName()});
                    string msgToClient = "Da them sinh vien " + msv + " " + sv.getName() + " vao csdl!";
                    send(currSocket, msgToClient.data(), msgToClient.size(), 0);
                }else{
                    string msgToClient = "Da ton tai ma sinh vien trong csdl";
                    send(currSocket, msgToClient.data(), msgToClient.size(), 0);
                }
                break;
            }

            case FD_CLOSE:{
                cout<<"Client disconnected!"<<endl;
                closesocket(wParam);
                break;
            }
            default: {
                break;
            }
        }
    }
    return DefWindowProc(hwd, message, wParam, lParam);
}

HWND createMessageWindow(HINSTANCE hInstance)
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "AsyncSocketWindow";

    RegisterClass(&wc); // dang ky cua so voi he thong

    HWND a = CreateWindow("AsyncSocketWindow", "HiddenWindow", WS_OVERLAPPEDWINDOW, 0, 0, 0, 0,
                          NULL, NULL, hInstance, NULL); // tao cua so an

    return a;
}

int main()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HWND hiddenWin = createMessageWindow(hInstance);

    WSADATA wsaData;
    struct sockaddr_in serverAddr;
    u_short port = 8386;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (listenSocket == SOCKET_ERROR)
    {
        cout << "khoi tao socket lang nghe that bai\n";
        WSACleanup();
        return 1;
    }

    WSAAsyncSelect(listenSocket, hiddenWin, 1111, FD_ACCEPT | FD_CLOSE);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(port);

    bind(listenSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

    listen(listenSocket, 5);
    cout << "Server listening...\n";

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        DispatchMessage(&msg);
    }

    closesocket(listenSocket);
    DestroyWindow(hiddenWin);
    WSACleanup();

    return 0;
}