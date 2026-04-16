#include<bits/stdc++.h>
#include<winsock2.h>
#include<ws2tcpip.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include<boost/serialization/vector.hpp>
using namespace std;


class Account{
    private:
        string username, password;
        bool isExist = false;
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int version){
            ar& username;
            ar& password;
            ar& isExist;
        }


    public:
        Account(){} 
        Account(string username, string password){
           this->username = username;
           this->password = password;
        }
        string getUsername(){return this->username;}
        string getPassword(){return this->password;}
        void setIsExist(bool ex){
            this->isExist = ex;
        }
        bool isExists(){return this->isExist;}
};

vector<Account> users = {
    Account("admin", "123456"),
    Account("nam", "abc123"),
    Account("guest", "guest"),
    Account("test", "1111"),
    Account("student01", "pass01")
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

    Account a;

   string data(2048, '\0');

    int received = recv(client_socket, data.data(), data.size(), 0);

    if (received <= 0) {
        cout << "Loi nhan du lieu\n";
        return 1;
    }

    stringstream ss;
    ss.write(data.data(), received);
    boost::archive::binary_iarchive bia(ss);
    bia >> a;

    bool found = false;
    for(Account x : users){
        if(x.getUsername() == a.getUsername() && x.getPassword() == a.getPassword()){
            found = true;
            break;
        }
    }
    a.setIsExist(found);

    stringstream ss2;
    boost::archive::binary_oarchive boa(ss2);
    boa << a;
    string out = ss2.str();
    int dodai = out.length();

    send(client_socket, out.data(), dodai, 0);
    cout << "Da gui thong bao tai khoan toi client\n";
}