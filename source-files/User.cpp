#include "User.h"



const unsigned int User::CHARACTER_INPUT_LIMIT = 1248;
const std::string User::LOCAL_DEBUG_LOG = "LOCAL DEBUG LOG";



User::User(unsigned short port, std::string& name, unsigned short id) : port(port), name(name), id(id) {
    input_buffer = new char[CHARACTER_INPUT_LIMIT];
    memset(input_buffer, 0, sizeof *input_buffer);
}

User::~User() {
    delete input_buffer;
}



bool User::IsMaster() {
    return false;
}

bool User::IsConnected(sf::TcpSocket& socket) {
    return socket.getRemoteAddress() != sf::IpAddress::None;
}



void User::FormatMessage(std::string& s) {
    if (s[0] != '/') s = "/msg [name]:[" + name + "] " + '[' + s + ']';
}

void User::ProcessCommand(std::string& s) {
    Command cmd(s);

    std::string cmd_str = cmd.GetCommand();

    if (cmd_str == "msg") {
        std::string n = cmd.GetValue("name");
        std::string m = cmd.GetContent(0);

        PushMessage(n, m);
    }

    if (cmd_str == "sys") {
        std::system(cmd.GetContent(0).c_str());
    }
}

bool User::ValidateMessage(std::string& s) {
    return true;
}



sf::Packet User::PackMessage(std::string& s) {
    sf::Packet packet;

    packet << s;

    return packet;
}

void User::SendMessage(std::string& s) {
    FormatMessage(s);

    if (!ValidateMessage(s)) return;

    sf::Packet packet = PackMessage(s);
    SendPacket(packet);
}



void User::PushMessage(std::string& u, std::string& s) {
    chat_history.push_back("[" + u + "] " + s);
}

std::string User::UnpackMessage(sf::Packet& packet) {
    std::string s;

    packet >> s;

    return s;
}


void User::Log(std::string name, std::string msg) {
    PushMessage(name, msg);
}



void User::Draw() {
    ImGui::BeginGroup();

        ImGui::BeginChild("RoomView");

            const float text_area_height = ImGui::GetFrameHeightWithSpacing() * 4.f;
            const float text_area_reserve = ImGui::GetStyle().ItemSpacing.y + text_area_height;

            ImGui::BeginChild("ScrollingRegion", ImVec2(0, -text_area_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

            for (int i = 0; i < chat_history.size(); i++) {
                const char* item = chat_history[i].c_str();
                //ImVec4 color = color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                //ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(item);
                //ImGui::PopStyleColor();
            }

            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);

            ImGui::PopStyleVar();
            ImGui::EndChild();

            /*if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this)) {
                char* s = InputBuf;
                Strtrim(s);
                if (s[0])
                    ExecCommand(s);
                strcpy(s, "");
                reclaim_focus = true;
            }*/

            bool reclaim_focus = false;
            ImGuiInputTextFlags input_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine;// | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
            if (ImGui::InputTextMultiline("##Input", input_buffer, CHARACTER_INPUT_LIMIT, ImVec2(ImGui::GetWindowContentRegionWidth(), text_area_height), input_flags)) {
                if (!((std::string)input_buffer).empty()) {
                    std::string input = input_buffer;
                    SendMessage(input);
                    memset(input_buffer, 0, sizeof *input_buffer);
                }

                reclaim_focus = true;
            }

            ImGui::SetItemDefaultFocus();
            if (reclaim_focus)
                ImGui::SetKeyboardFocusHere(-1);

        ImGui::EndChild();

    ImGui::EndGroup();
}


/*
bool User::ValidateLuaState(lua_State* bot, int v) {
    if (v != LUA_OK) {
        std::cout << lua_tostring(bot, -1) << std::endl;

        return false;
    }

    return true;
}
*/