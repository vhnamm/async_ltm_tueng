#include <bits/stdc++.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#include<boost/archive/binary_iarchive.hpp>
#include<boost/archive/binary_oarchive.hpp>
using namespace std;
#define messageSignal 1111

class SinhVien{
    private:
        string name, msv;
        friend class boost::serialization::access;

    public:
    SinhVien(string msv, string name){
        this->msv = msv;
        this->name = name;
    }
    SinhVien(){}

    void setMsv(string msv){this->msv = msv;}
    string getMsv(){return this->msv;}
    void setName(string name){this->name = name;}
    string getName(){return this->name;}

    template<class Archieve>
    void serialize(Archieve &ar, const unsigned int version){
        ar& msv;
        ar& name;
    }

};

LRESULT CALLBACK WindowProc(HWND hwd, UINT message, WPARAM wParam, LPARAM lParam){
    //wParam: chứa Handle SOCKET -> typecasting sang SOCKET để xuer lí
    //lParam: chứa event dc packed: FD_ACCEPT,...
    SOCKET currSocket = (SOCKET)wParam;
    int event = WSAGETSELECTEVENT(lParam);
    struct sockaddr_in clientAddr;
    int len = sizeof(clientAddr);

    switch (event)
    {
    case FD_ACCEPT:{
        SOCKET clientSocket = accept(currSocket, (SOCKADDR*)&clientAddr, &len);

        if(clientSocket == SOCKET_ERROR){
            cout << "Chap nhan that bai\n";
            exit(1);
        }else{
            WSAAsyncSelect(clientSocket, hwd, messageSignal, FD_READ | FD_CLOSE | FD_WRITE);
        }
        break;
    }
        
    case FD_READ:{
        string buffer(1024, '\0');
        int byteRecv = recv(currSocket, buffer.data(), buffer.size(), 0);
        stringstream ss;
        ss.write(buffer.data(), byteRecv);

        boost::archive::binary_iarchive bia(ss);

        SinhVien s;
        bia >> s;

        cout << s.getMsv() << " " << s.getName() << endl;

        string reply = "Da nhan duoc du lieu tu client";
        send(currSocket, reply.data(), reply.size(), 0);
        break;
    }    
    
    case FD_CLOSE:{
        cout << "client disconected\n";
        closesocket(currSocket);
        break;
    }
    default:
        break;
    }
}

HWND createMessageWindow(HINSTANCE hInstance){
    WNDCLASS wdac = {0};
    wdac.lpfnWndProc = WindowProc;
    wdac.lpszClassName = "AsyncSocketWindow";
    wdac.hInstance = hInstance;

    RegisterClass(&wdac);

    HWND h = CreateWindow("AsyncSocketWindow","HiddenWindow", WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
    
    return h;

}

int main() {
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
    
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HWND h = createMessageWindow(hInstance);

    WSAAsyncSelect(listenSocket, h, messageSignal, FD_ACCEPT);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(port);

    bind(listenSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

    listen(listenSocket, SOMAXCONN);
    cout <<"Server listening...\n";

    MSG msg;
    while(GetMessage(&msg, NULL, 0 , 0)){
        DispatchMessage(&msg);
    }
    return 0;
}