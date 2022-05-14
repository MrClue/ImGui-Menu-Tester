#pragma once

#include "includes.hpp"
#include "utils.hpp"

/* <REMINDER>
 * 
 * For consistency use "int" values for ImColor(), unless it must contain digits, then use "float".
 * 
**/

namespace colors {

	ImColor main_col = ImColor(41, 31, 57);
	ImColor main_col_less_alpha = ImColor(41, 31, 57, 225);

	ImColor secondary_col = ImColor(63, 52, 83);

	ImVec4 buttonActive = ImColor(41, 31, 57, 255);
	ImVec4 buttonInactive = ImColor(24, 17, 41, 255);
	ImVec4 buttonHovered = ImColor(30, 23, 47, 255);

	ImVec4 border_col = ImColor(74, 65, 95, 255);

	ImVec4 hidden = ImColor(0.f, 0.f, 0.f, 0.0001f);
	ImVec4 text_col = ImColor(255, 255, 255, 255);

	// cheat status
	ImVec4 status_undetected = ImColor(0, 215, 0, 255);
	ImVec4 statis_detected = ImColor(215, 0, 0, 255);
}

namespace theme {

	void MenuTheme();
	void MenuFonts();
}