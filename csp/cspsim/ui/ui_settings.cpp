#include "ui.h"

const char* l_languages[] = { "English", "Swedish", "German" };

void ui_settings_menu() {
	ImGui::OpenPopup("Settings");
	ui_center_window_with_margin(ui_state.window_margin);
	if(ImGui::BeginPopupModal("Settings", &ui_state.show_settings, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse)) {

		static int selectedTab = 0;

		const char* tabs[] = {
			ui_i18n_get("Profile"),
			ui_i18n_get("User interface"),
			ui_i18n_get("Controls"),
			ui_i18n_get("Display"),
			ui_i18n_get("Sound"),
		};
		const int tabCount = IM_ARRAYSIZE(tabs);

		ImGui::BeginChild("Tabs", ImVec2(150, 0), true);
		for (int i = 0; i < tabCount; i++) {
			if (ImGui::Selectable(tabs[i], selectedTab == i)) {
				selectedTab = i;
			}
		}
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginGroup();

		ImGui::BeginChild(ui_i18n_get("Content"), ImVec2(0, ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing()), true);
		switch (selectedTab) {
		case 0:
			ImGui::Text(ui_i18n_get("Username:"));
			ImGui::SetNextItemWidth(-1.0f);
			ImGui::InputText("##Username", ui_state.username, sizeof(ui_state.username));
			break;
		case 1:
			ImGui::Text(ui_i18n_get("Theme:"));
			ImGui::SetNextItemWidth(-1.0f);
			// TODO Listbox
			ImGui::Text(ui_i18n_get("Language:"));
			ImGui::SetNextItemWidth(-1.0f);
			// TODO Listbox

			static int selected_language = 0;
			if (ImGui::ListBox("##listbox", &selected_language, l_languages, IM_ARRAYSIZE(l_languages))) {
				switch(selected_language) {
				case 0: ui_i18n_set_language("en"); break;
				case 1: ui_i18n_set_language("sv"); break;
				case 2: ui_i18n_set_language("de"); break;
				}
			}

			break;
		case 2:
			break;
		case 3:
			ImGui::Text(ui_i18n_get("Fullscreen:"));
			ImGui::SetNextItemWidth(-1.0f);
			// TODO Listbox

			ImGui::Text(ui_i18n_get("Window size:"));
			ImGui::SetNextItemWidth(-1.0f);
			// TODO Listbox
			break;
		case 4:
			ImGui::Text(ui_i18n_get("Volume:"));
			ImGui::SetNextItemWidth(-1.0f);
			ImGui::SliderFloat("##Volume", &ui_state.sound_volume, 0.0f, 1.0f);
			break;
		}
		ImGui::EndChild();

		if(ImGui::Button(ui_i18n_get("Apply"))) {
			ui_state.show_settings = false;
		}
		ImGui::SameLine();
		if(ImGui::Button(ui_i18n_get("Cancel"))) {
			ui_state.show_settings = false;
		}

		ImGui::EndGroup();

		ImGui::EndPopup();
	}
}
