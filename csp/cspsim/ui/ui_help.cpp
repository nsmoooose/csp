#include "ui.h"

static const char g_help_welcome[] = {
	"About CSP\n"
	"The Combat Simulator Project (CSP) is a community united by the common "
	"interest in combat simulator development. We are developing standards, "
	"technologies, code libraries, tools and artwork that can be used to build "
	"a wide variety of real-time combat simulators. Our ultimate goal is to "
	"create free, cross-platform, multiplayer simulations of large scale combat "
	"scenarios.\n"
	"\n"
	"Background\n"
	"During 1995-2000 quite a few commercial flight/combat simulator developers "
	"left the market. As developers turned elsewhere for profit, users were left "
	"with unsupported products and unrealized dreams. This depressive climate "
	"spawned a renewed interest in open-source simulator development in the "
	"simulator user community. While open-source simulator development had been "
	"shown to work, e.g. with the FlightGear (http://www.flightgear.org) "
	"simulator, no similar development effort existed for combat simulators. CSP "
	"was founded to be just that. CSP was inspired by a basic simulator "
	"framework demo originally written by Brandon Bohn. Brandon's ideas and code, "
	"acting as a concrete hands-on starting point, combined with Matt \"Boddman\" "
	"Boddicker's heroic efforts to provide a web-site and the necessary "
	"distributed development services, formed the foundation for what is now the "
	"Combat Simulator Project. CSP has evolved substantially since those early "
	"days. The project moved to SourceForge in late 2002, and development focused "
	"on refining and extending the basic flight simulation application. The first "
	"port to Linux was completed around that time, and key infrastructure elements "
	"were designed and implemented. The screenshots page tracks some of this "
	"development, although the focus is of course on visual advances. In early "
	"2004 the source code repository was moved from SourceForge CVS to a private "
	"subversion repository hosted at zerobar.net (https://www.zerobar.net/csp), "
	"where development continues at present.\n"
	"\n"
	"Interested?\n"
	"CSP is open for all to join. We invite everyone with interest in simulators "
	"and simulator development to contribute to this project. Even if not "
	"contributing directly as a developer, we hope you will visit the Wiki, check "
	"on our progress and join in our discussions."
};

static const char g_help_f16_first[] = {
	"If you don't have a joystick/throttle, you can use the keyboard to fly the "
	"plane. To takeoff in the default aircraft (F-16), increase the throttle to "
	"full afterburner, accelerate down the runway, and pitch the nose up at about "
	"150 kts. Here are a few hints that may help:\n"
	"\n"
	"The throttle setting is displayed in the white text overlay on the left side "
	"of the screen, along with lots of other diagnostic data. You can also look at "
	"the fuel flow indicator at the front right side of the cockpit, just below the "
	"display with orange/yellow text (DED). If the value is above 30000 or so you "
	"should have plenty of thrust for takeoff. If you are using the keyboard, you'll "
	"need to hold the 'increase throttle' key (=) for a few seconds. On German "
	"keyboards it is the '+' key.\n"
	"\n"
	"The airspeed is visible on the left side of the HUD, and also in the white "
	"text overlay (near the bottom left of the screen).\n"
	"\n"
	"To see the HUD more clearly, zoom the view by holding the left shift key at "
	"rotating the mouse wheel. Note that this may not be possible on laptops "
	"without external mice.\n"
	"\n"
	"If you are using the keyboard, you need to hold the down arrow to pitch the "
	"nose up to about 10 degrees when you pass 150 kts. You want the \"+\" sign in "
	"the top center of the HUD to align with the 10 degree pitch bar (horizontal "
	"line with \"10\" on either side). Release and press the down arrow as needed "
	"to maintain about 10 degrees pitch until you climb away from the runway.\n"
	"\n"
	"Press 'g' to raise your landing gear once you are up in the air.\n"
	"\n"
	"Landing is a bit more difficult, and landing gracefully without a "
	"joystick/throttle combination is particularly challenging. I'd recommend "
	"searching on the web for landing tutorials for Falcon4. The same instructions "
	"work well in CSP.\n"
	"\n"
	"Good luck!"
};


void ui_help_menu() {
	ImGui::OpenPopup("Help");
	ui_center_window_with_margin(ui_state.window_margin);
	if(ImGui::BeginPopupModal("Help", &ui_state.show_help, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse)) {
		static int selectedTab = 0; // Track the selected tab

		const char* tabs[] = {
			"Welcome",
			"F16 first steps",
			"Aircraft controls",
			"View controls",
			"Mouse",
			"Joystick",
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

		ImGui::BeginChild("Content", ImVec2(0, 0), true);
		switch (selectedTab) {
		case 0:
			ImGui::TextWrapped(g_help_welcome);
			break;
		case 1:
			ImGui::TextWrapped(g_help_f16_first);
			break;
		case 2:
			ImGui::Text("This is the content for Tab 3.");
			break;
		case 3:
			ImGui::Text("This is the content for Tab 4.");
			break;
		}
		ImGui::EndChild();

		ImGui::EndPopup();
	}
}
