#include "ui.h"

void ui_theme_gray() {
	ImGuiStyle& style = ImGui::GetStyle();

	const auto medium = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	const auto lighter = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);

	// Set a gray gradient background for the window
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // Darker gray for window background
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);  // Slightly lighter gray for the title bar
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // Active window title

	// Set button gradient colors
	style.Colors[ImGuiCol_Button] = medium;
	style.Colors[ImGuiCol_ButtonHovered] = lighter;
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);  // Even lighter gray when active

	// Set the frame background (for input fields, combo boxes, etc.)
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);  // Dark gray for frames
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f); // Lighter gray for hover
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);  // Active state

	// Set text color to white for contrast against the gray background
	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White text

	// Set scrollbar color
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);  // Dark gray for scrollbar background
	style.Colors[ImGuiCol_ScrollbarGrab] = medium;
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = lighter;
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);  // Active gray scrollbar color

	// Set the border color for windows, buttons, and other widgets
	style.Colors[ImGuiCol_Border] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f); // Slightly dark gray border

	// Set popup background to a darker gray
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f); // Darker gray for popups

	// Set the selection color in lists, text inputs, etc.
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); // Light gray for selected items

	// Selectable
    style.Colors[ImGuiCol_Header] = medium;
    style.Colors[ImGuiCol_HeaderHovered] = lighter;
    style.Colors[ImGuiCol_HeaderActive] = medium;
}
