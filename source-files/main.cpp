#include <SFML/Graphics.hpp>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui-SFML.h"

#include "Master.h"
#include "Client.h"

using namespace std;
using namespace sf;

int main() {
    ios::sync_with_stdio(false);

    RenderWindow window(sf::VideoMode(1920, 1440), "CPP2PChat");
    ImGui::SFML::Init(window);
    window.resetGLStates();



    ImGuiIO* imgui_io = &ImGui::GetIO();
    imgui_io->FontGlobalScale = 2.f;
    imgui_io->IniFilename = NULL;

    ImVec2* win_size = &imgui_io->DisplaySize;



    char name_input[64] = "CallMeDaddy";
    char ip_input[64] = "192.168.0.175";
    char port_input[64] = "1201";

    vector <User*> rooms;
    int room_index = 0;
    int room_id = 0;



    bool create_join_win_open = false;
    bool ui_style_win_open = false;

    sf::Clock imgui_delta_clock;



    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        for (int i = 0; i < rooms.size(); i++) {
            rooms[i]->Update();
        }

        ImGui::SFML::Update(window, imgui_delta_clock.restart());

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Room")) {
                ImGui::MenuItem("Create / Join", NULL, &create_join_win_open);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Options")) {
                ImGui::MenuItem("UI", NULL, &ui_style_win_open);

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (create_join_win_open) {
            string title = "Create or Join   ";
            string frames = "|/-\\";

            title += frames[(int)(ImGui::GetTime() / 0.25f) & 3];
            title += "###CreateJoinWindow";

            ImGui::Begin(title.c_str(), &create_join_win_open, ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::InputText(" Name##NameInput", name_input, 64);

                ImGui::NewLine();

                ImGui::BeginTabBar("##CreateJoinTabs");

                    if (ImGui::BeginTabItem("Host")) {
                        ImGui::Text("Host Room");

                        ImGui::SameLine();

                        ImGui::TextDisabled("(?)");
                        if (ImGui::IsItemHovered()) {
                            ImGui::BeginTooltip();
                                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                                    ImGui::TextUnformatted("To make room non-local make sure you have port forwarding enabled for selected port");
                                ImGui::PopTextWrapPos();
                            ImGui::EndTooltip();
                        }

                        ImGui::NewLine();

                        ImGui::InputText(" Port", port_input, 64);

                        ImGui::NewLine();

                            if (ImGui::Button("Host")) {
                                rooms.push_back(new Master(stoi(port_input), name_input, room_id++));

                                create_join_win_open = false;
                            }

                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("Join")) {
                        ImGui::Text("Join Room");
                        ImGui::NewLine();
                        ImGui::InputText(" Ip Address", ip_input, 64);
                        ImGui::InputText(" Port", port_input, 64);
                        ImGui::NewLine();

                            if (ImGui::Button("Join")) {
                                rooms.push_back(new Client(ip_input, stoi(port_input), name_input, room_id++));

                                create_join_win_open = false;
                            }

                        ImGui::EndTabItem();
                    }

                ImGui::EndTabBar();
            ImGui::End();
        }

        if (ui_style_win_open) {
            ImGui::Begin("UI Style", &ui_style_win_open, ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::SliderFloat("UIScale", &imgui_io->FontGlobalScale, 0.5f, 3.f);
            ImGui::End();
        }

        float main_menu_bar_area = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

        ImGuiWindowFlags main_win_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImGui::SetNextWindowSize(ImVec2(win_size->x, win_size->y - main_menu_bar_area));
        ImGui::SetNextWindowPos(ImVec2(0, main_menu_bar_area));
        ImGui::Begin("Main", NULL, main_win_flags);

            ImGui::BeginChild("RoomList", ImVec2(win_size->x * 0.12f, 0), true);

                for (int i = 0; i < rooms.size(); i++) {
                    if (ImGui::Selectable((rooms[i]->name + "##" + to_string(i)).c_str(), room_index == i/*, ImGuiSelectableFlags_None, ImVec2(ImGui::GetWindowContentRegionWidth() * .8f, 0)*/))
                        room_index = i;

                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                        delete rooms[i];

                        rooms.erase(rooms.begin() + i);

                        if (i <= room_index) {
                            room_index--;
                        }

                        if (room_index < 0) room_index = 0;
                    }
                }

            ImGui::EndChild();

            ImGui::SameLine();

            if (rooms.size()) {
                User* c_user = rooms[room_index];

                c_user->Draw();
            }

        ImGui::End();

        //ImGui::ShowDemoWindow();

        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}