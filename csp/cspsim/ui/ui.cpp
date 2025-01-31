#include <string>
#include "ui.h"

struct ui_state ui_state;

ui_state::ui_state() {
	show_main = true;
	show_pause = true;
	show_settings = false;
	show_tutorials = false;
	show_missions = false;
	show_multiplayer = false;
	show_help = false;

	username[0] = '\0';
	sound_volume = 1.0f;
	strcpy(server_listen_ip, "0::0");
	strcpy(server_listen_port, "3160");

	window_margin = 50.0f;
}

void ui_center_window(const ImVec2 &size) {
	ImVec2 display_size = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(display_size.x / 2 - size.x / 2, display_size.y / 2 - size.y));
    ImGui::SetNextWindowSize(size);
}

void ui_center_window_with_margin(float margin) {
	ImVec2 display_size = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(margin, margin));
    ImGui::SetNextWindowSize(ImVec2(display_size.x - 2 * margin, display_size.y - 2 * margin));
}

void ui_render() {
	ui_theme_gray();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f));

	ui_screen_current();

	ImGui::PopStyleVar();
}
