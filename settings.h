#pragma once

namespace settings {

	bool isMenuOpen = false;

	namespace aimbot {
		bool enable_aimbot = false;
		float fov = 0.0f;
		float speed = 0.0f;

		const char* bones[]{ "Head", "Neck", "Chest", "Stomach" };
		int selected_array[4]{};
		int selected_bone = 0;

		namespace misc {
			int weapon_type = 0;
			bool target_in_air = false;
			bool target_in_smoke = false;
			bool auto_pistol = false;
		}
	}

	namespace visuals {
		namespace esp {
			bool enable_esp = false;
			int esp_type = 0; // only visible, visible + behind walls
			bool name_esp = false;
			bool health_esp = false;
			bool armor_esp = false;
			bool weapon_esp = false;
			inline bool esp_flags[7] = { false, false, false, false, false, false, false }; // todo
			bool box_esp = false;
			float box_esp_visible_color[4] = { 255 / 255.f, 255 / 255.f , 255 / 255.f, 1.0f };
			float box_esp_hidden_color[4] = { 255 / 255.f, 255 / 255.f , 255 / 255.f, 1.0f };
		}
		
		namespace chams {
			bool enable_chams = false;
			int chams_type = 0; // only visible, visible + behind walls
			float chams_visible_color[4] = { 255 / 255.f, 255 / 255.f , 255 / 255.f, 1.0f };
			float chams_hidden_color[4] = { 255 / 255.f, 255 / 255.f , 255 / 255.f, 1.0f };
		}
		
		namespace glow {
			bool enable_glow = false;
			int glow_type = 0; // visible, visible + behind walls
			float glow_visible_color[4] = { 255 / 255.f, 255 / 255.f , 255 / 255.f, 1.0f };
			float glow_hidden_color[4] = { 255 / 255.f, 255 / 255.f , 255 / 255.f, 1.0f };
		}
		
	}

	namespace misc {
		bool enable_bhop = false;
		bool enable_recoil_crosshair = false;
		bool modify_flashbang_effects = false;
		float flashbang_alpha = 1.0f;
	}

	namespace skins {
		bool enable_skinchanger = false;
		int selected_weapon = 0;
	}


}