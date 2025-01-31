#include "ui.h"

void (*ui_screen_current)();

void ui_screen_selector() {
	ImVec2 window_size = {250, 400};
	ImGui::SetNextWindowSize(window_size);
	ImGui::SetNextWindowPos(ImVec2(50, 50));
	ImGui::Begin(ui_i18n_get("csp"), &ui_state.show_main,
				 ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse);

	ImGui::Text(ui_i18n_get("main_menu"));

	ImVec2 button_size = { ImGui::GetContentRegionAvail().x, 0 };
	if (ImGui::Button("Main", button_size)) {
		ui_screen_current = ui_screen_main;
	}

	if(ImGui::Button("Pause", button_size)) {
		ui_state.show_pause = true;
		ui_screen_current = ui_screen_game;
	}

	if(ImGui::Button("Map", button_size)) {
	}

	ImGui::End();
}
