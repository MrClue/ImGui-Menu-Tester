#pragma once

#include "includes.hpp"
#include "utils.hpp"

/* <REMINDER>
 * 
 * For consistency use "int" values for ImColor(), 
 * unless it must contain digits, then use "float".
 * 
**/

namespace colors {
	
	ImVec4 light_purple = ImColor(41, 31, 56, 255);
	ImVec4 light_purple_reduced_alpha = ImColor(41, 31, 56, 200);

	ImVec4 dark_purple = ImColor(31, 21, 46, 255); // dark

	ImVec4 button_active = light_purple;
	ImVec4 button_inactive = ImColor(25, 17, 40, 255);
	ImVec4 button_hovered = ImColor(31, 21, 46, 200);

	ImVec4 border = ImColor(59, 49, 74, 255); //ImColor(74, 65, 95, 255);

	ImVec4 hidden = ImColor(0.f, 0.f, 0.f, 0.0001f);

	ImVec4 text = ImColor(255, 255, 255, 255);
	ImVec4 text_inactive = ImColor(255, 255, 255, 175);

	// cheat status
	ImVec4 status_undetected = ImColor(0, 215, 0, 255);
	ImVec4 statis_detected = ImColor(215, 0, 0, 255);
}

namespace menu {
	
	ImVec2 main_menu_position;

	bool showBotBar = false;
	bool isDetected = false;	// is cheat VAC detected?

	int tabs = 5;				// [aimbot, visuals, misc, skins, configs]
	int selectedTab = 0;		// switch-statement
	float tabSpacing = 0.f;		// spacing between tabs
	
	int preview_tabs = 3;
	int selected_prev_tab = 0;

	namespace sizes {
		ImVec2 main_menu = ImVec2(600.f, 500.f);		// main-menu
		ImVec2 click_to_show = ImVec2(600.f, 50.f);		// "click" button
		ImVec2 bottom_bar = ImVec2(600.f, 58.f);		// bottom-bar
	}

	namespace fonts {
		ImFont* defaultFont;
		ImFont* titleFont;
	}

	namespace flags {
		auto main_menu = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
		auto click_to_show = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
		auto bottom_bar = ImGuiWindowFlags_NoResize /* | ImGuiWindowFlags_NoCollapse */ | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;
	}

	namespace render {
		void RenderMenu();
	}

	namespace themes {
		void MenuTheme();
		void MenuFonts();
	}

	namespace navbar {
		void TabNavigation();
		void TabContent();
	}

	namespace widgets {
		void MainMenu();
		void ToggleBottomBar();
		void InitBottomBar();
	}

	namespace init_tab {
		void AimbotTab();
		void VisualsTab();
		void MiscTab();
		void SkinsTab();
		void ConfigsTab();
	}

}
