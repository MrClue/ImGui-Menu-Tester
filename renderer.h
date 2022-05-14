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

	ImVec4 main = ImColor(41, 31, 57, 255);
	ImVec4 main_less_alpha = ImColor(41, 31, 57, 225);

	ImVec4 secondary = ImColor(63, 52, 83, 255);

	ImVec4 button_active = ImColor(41, 31, 57, 255);
	ImVec4 button_inactive = ImColor(24, 17, 41, 255);
	ImVec4 button_hovered = ImColor(30, 23, 47, 255);

	ImVec4 border = ImColor(74, 65, 95, 255);

	ImVec4 hidden = ImColor(0.f, 0.f, 0.f, 0.0001f);
	ImVec4 text = ImColor(255, 255, 255, 255);

	// cheat status
	ImVec4 status_undetected = ImColor(0, 215, 0, 255);
	ImVec4 statis_detected = ImColor(215, 0, 0, 255);

}

namespace menu {
	
	ImVec2 main_menu_position;
	bool showBotBar = false;

	int tabs = 5;				// [aimbot, visuals, misc, skins, configs]
	int selectedTab = 0;		// switch-statement
	float tabSpacing = 2.f;		// spacing between tabs
	
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

	namespace sizes {
		ImVec2 main_menu = ImVec2(600.f, 500.f);		// main-menu
		ImVec2 click_to_show = ImVec2(600.f, 50.f);		// "click" button
		ImVec2 bottom_bar = ImVec2(600.f, 58.f);		// bottom-bar

	}

	namespace flags {
		auto main_menu = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
	}

}

namespace settings {
	// setting dummy vars
	bool dummy_bool = false;
	float dummy_float = 0.f;
	int dummy_int = 0;

	const char* aimBones[]{ "Head", "Chest", "Stomach" };
	int selectedBone = 0;
	int selected[3]{};
}