#include "ui.h"

static void ui_pause_toolbar() {
    const ImVec2 window_pos = ImVec2(ImGui::GetIO().DisplaySize.x - 300.0f, 0.0f); // Adjust the x-offset
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300.0f, 50.0f));

    if (ImGui::Begin("##", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
		auto button_size = ImVec2(80.0f, 30.0f);
		ImVec2 content_region = ImGui::GetContentRegionAvail();
        float total_width = 3 * button_size.x + 2 * ImGui::GetStyle().ItemSpacing.x;
        float offset_x = (content_region.x - total_width) / 2.0f;
		float offset_y = (content_region.y - button_size.y) / 2.0f;

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offset_y);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset_x);

        if (ImGui::Button(ui_i18n_get("help"), button_size)) {
			ui_state.show_help = true;
        }
        ImGui::SameLine();
        if (ImGui::Button(ui_i18n_get("options"), button_size)) {
			ui_state.show_settings = true;
        }
        ImGui::SameLine();
        if (ImGui::Button(ui_i18n_get("quit"), button_size)) {
			ui_state.show_main = false;
        }
    }
    ImGui::End();
}


static void ui_pause() {
	ImVec2 window_size = {400, 100};
	ui_center_window(window_size);
	ImGui::Begin(ui_i18n_get("csp"), &ui_state.show_pause,
				 ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse);

    ImVec2 buttonSize(100.0f, 30.0f);
    float buttonSpacing = ImGui::GetStyle().ItemSpacing.x;
    float totalWidth = (buttonSize.x * 2) + buttonSpacing;
    ImVec2 contentRegion = ImGui::GetContentRegionAvail();
    float offsetX = (contentRegion.x - totalWidth) / 2.0f;
    float offsetY = (contentRegion.y - buttonSize.y) / 2.0f;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);

    if (ImGui::Button("Resume", buttonSize)) {
		ui_state.show_pause = false;
    }

    ImGui::SameLine();
    if (ImGui::Button("Main menu", buttonSize)) {
		ui_screen_current = ui_screen_main;
    }

	ImGui::End();
}

void ui_screen_game() {
	if(ui_state.show_pause) {
		ui_pause_toolbar();
		ui_pause();
	}

	if(ui_state.show_settings) {
		ui_settings_menu();
	}
	if(ui_state.show_help) {
		ui_help_menu();
	}
}
