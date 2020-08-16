#include "Master.h"



Master::Master(unsigned short port, std::string name, unsigned short id) : User(port, name, id) {
    Connect();

    //InitPublicBot("testbot.lua");
}

Master::~Master() {
    if (port_listener.getLocalPort() != 0) port_listener.close();

    for (auto& [key, val] : clients) {
        delete val;
    }
}



void Master::Connect() {
    port_listener.setBlocking(false);

    if (port_listener.listen(port) != sf::Socket::Done) {
        Log(LOCAL_DEBUG_LOG, "Failed to listen to port");
    } else {
        Log(LOCAL_DEBUG_LOG, "Listening to port");
    }
}



void Master::Update() {
    CheckDisconnected();
    ListenConnectionRequests();
    ReceivePacket();
}



void Master::CheckDisconnected() {
    std::vector <std::string> should_erase;

    for (const auto& [key, val] : clients) {
        if (!IsConnected(*val->socket)) {
            should_erase.push_back(key);

            delete val;

            //Log("SOMEONE DISCONNECTED");
        }
    }

    for (const auto& i : should_erase) {
        clients.erase(i);
    }
}

void Master::ListenConnectionRequests() {
    _client_* client = new _client_(new sf::TcpSocket);

    if (port_listener.accept(*client->socket) == sf::Socket::Done) {
        clients[GetClientKey(client->socket)] = client;

        Log(LOCAL_DEBUG_LOG, "SOMEONE JOINED");
    }
}

void Master::ReceivePacket() {
    sf::Packet packet;

    for (const auto& [key, client] : clients) {
        if (client->socket->receive(packet) == sf::Socket::Done) {
            std::string s = UnpackMessage(packet);

            SendMessage(s);
        }
    }
}



void Master::SendMessage(std::string& s) {
    User::SendMessage(s);

    ProcessCommand(s);
}

void Master::SendPacket(sf::Packet& packet) {
    for (const auto& [key, client] : clients) {
        //if (IsConnected(*client->socket)) {
            client->socket->send(packet);
        //}
    }
}



std::string Master::GetClientKey(sf::TcpSocket* socket) {
    //if (!IsConnected(*socket)) return "";

    return socket->getRemoteAddress().toString() + ":" + std::to_string(socket->getRemotePort());
}



bool Master::IsMaster() {
    return true;
}



int Master::BotLocalPrint(lua_State* bot) {
    lua_getglobal(bot, "instance");
    Master* master = (Master*)lua_touserdata(bot, -1);

    std::string bot_msg = lua_tostring(bot, -2);
    std::string bot_name = lua_tostring(bot, -1);

    master->Log(bot_name, bot_msg);

    return 0;
}

int Master::BotPublicPrint(lua_State* bot) {
    lua_getglobal(bot, "instance");
    Master* master = (Master*)lua_touserdata(bot, -1);

    std::string bot_msg = lua_tostring(bot, -2);
    std::string bot_name = lua_tostring(bot, -1);

    master->SendMessage(bot_name);

    return 0;
}

void Master::InitPublicBot(std::string path) {
    lua_State* bot = luaL_newstate();
    luaL_openlibs(bot);

    if (ValidateLuaState(bot, luaL_dofile(bot, path.c_str()))) {
        lua_getglobal(bot, "GetName");

        if (lua_isfunction(bot, -1)) {
            if (ValidateLuaState(bot, lua_pcall(bot, 0, 1, 0))) {
                std::string bot_name = lua_tostring(bot, -1);

                if (!public_bots.contains(bot_name)) {
                    lua_pushlightuserdata(bot, this);
                    lua_setglobal(bot, "instance");

                    lua_register(bot, "Log", Master::BotLocalPrint);
                    lua_register(bot, "Send", Master::BotPublicPrint);

                    public_bots[bot_name] = bot;

                    lua_getglobal(bot, "OnJoined");

                    assert(lua_iscfunction(bot, -1));

                    if (lua_iscfunction(bot, -1)) {
                        if (ValidateLuaState(bot, lua_pcall(bot, 0, 0, 0))) {
                        } else {
                            Log(LOCAL_DEBUG_LOG, "Error executing OnJoined");
                        }
                    } else {
                        Log(LOCAL_DEBUG_LOG, "OnJoined is not a function");
                    }

                    Log(LOCAL_DEBUG_LOG, "Successfully imported bot " + bot_name);

                    return;
                } else {
                    Log(LOCAL_DEBUG_LOG, "Bot with same name " + bot_name + " already exists");
                }
            } else {
                Log(LOCAL_DEBUG_LOG, "Could not get bot name");
            }
        } else {
            Log(LOCAL_DEBUG_LOG, "GetName is not a function");
        }
    } else {
        Log(LOCAL_DEBUG_LOG, "Could not fined file: " + path);
    }

    lua_close(bot);

    delete bot;
}
