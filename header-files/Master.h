#pragma once

#include <map>

#include "User.h"

class Master  : public User {
public:
    Master(unsigned short, std::string, unsigned short);
    ~Master() override;

private:
    void Connect() override;
    bool IsMaster() override;
    void CheckDisconnected() override;
    void ReceivePacket() override;
    void SendPacket(sf::Packet&) override;

    // Calls CheckDisconnected -> ListenConnectionRequests -> ReceivePacket
    void Update() override;

    // User::SendMessage -> ProcessCommand
    void SendMessage(std::string&) override;

    // Listen to port for incoming connection requests
    void ListenConnectionRequests();

    // Get client unique key
    std::string GetClientKey(sf::TcpSocket*);

    sf::TcpListener port_listener; // Listens to given port for incoming connection requests

    struct _client_ {
        explicit _client_(sf::TcpSocket* s = nullptr) {
            if (s) set_socket(s);
            else socket = s;
        }

        ~_client_() {
            if (!socket) return;
            if (socket->getRemoteAddress() != sf::IpAddress::None) socket->disconnect();

            delete socket;
        }

        void set_socket(sf::TcpSocket* s) {
            socket = s;
            socket->setBlocking(false);
        }

        sf::TcpSocket* socket;
        std::string name;
        ImVec4 color;
    };

    std::map <std::string, _client_*> clients; // Stores all clients with _client_ struct and [ip addr + ':' + remote port] as key



    std::map <std::string, lua_State*> public_bots; // todo store elsewise

    // Initialize local lua bot
    virtual void InitPublicBot(std::string);

    // Local Bot Log
    static int BotLocalPrint(lua_State*);

    // Public Bot Log
    static int BotPublicPrint(lua_State*);
};