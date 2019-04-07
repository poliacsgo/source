#include "../../../dependencies/common_includes.hpp"
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <intrin.h>
#include "../aimbot/lagcomp.hpp"
std::string clean_name(std::string name) {
	std::string wep = name;
	if (wep[0] == 'C') wep.erase(wep.begin());

	auto end_of_weapon = wep.find("Weapon");
	if (end_of_weapon != std::string::npos)
		wep.erase(wep.begin() + end_of_weapon, wep.begin() + end_of_weapon + 6);

	return wep;
}
#define tick2time( t )      ( interfaces::globals->interval_per_tick * ( t ) )
bool valid_tick(int tick) {
	float deltaTime = 0 - (tick2time(interfaces::globals->tick_count + 1) - tick2time(tick));
	return fabsf(deltaTime) <= 0.2f;
}

void c_visuals::run() {

	int screen_width, screen_height;
	interfaces::engine->get_screen_size(screen_width, screen_height);
	for (int i = 1; i <= interfaces::globals->max_clients; i++) {
		player_t* e = (player_t*)interfaces::entity_list->get_client_entity(i);
		player_t* local = (player_t*)interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player());

		if (e && e->is_valid(local, true, true, false) && c_config::get().visuals_enabled) {
			int x, y, w, h;

			if (local->team() == e->team())
				continue;

			player_info_t player_info;
			interfaces::engine->get_player_info(i, &player_info);

			vec3_t top, down, air, s[2];
			vec3_t adjust = vec3_t(0, 0, -18) * e->duck_amount();

			if (!(e->flags() & fl_onground) && (e->move_type() != movetype_ladder))
				air = vec3_t(0, 0, 10);
			else
				air = vec3_t(0, 0, 0);

			down = e->abs_origin() + air;

			if ((e->flags() & fl_ducking))
				top = down + vec3_t(0, 0, 58) + adjust;
			else
				top = down + vec3_t(0, 0, 72) + adjust;

			if (c_math::get().world_to_screen(top, s[1]) && c_math::get().world_to_screen(down, s[0])) {
				vec3_t delta = s[1] - s[0];

				h = static_cast<int>(fabs(delta.y));
				w = static_cast<int>(h / 2.0f);

				x = static_cast<int>(s[1].x - (w / 2));
				y = static_cast<int>(s[1].y);


				if (c_config::get().player_name) {
					RECT name_text_size = render::get().GetTextSize2(render::get().esp_font, player_info.name);
					render::get().draw_text(x + (w / 2) - (name_text_size.right / 2), y - 13, render::get().esp_font, player_info.name, false, color(180, 180, 180, 255));
				}

				if (c_config::get().player_box) {
					render::get().draw_outline(x, y, w, h, color(180, 180, 180));
					render::get().draw_outline(x - 1, y - 1, w + 2, h + 2, color(0, 0, 0));
					render::get().draw_outline(x + 1, y + 1, w - 2, h - 2, color(0, 0, 0));
				}
				if (c_config::get().player_weapon) {
					weapon_t* wep = (weapon_t*)interfaces::entity_list->get_client_entity_handle((uintptr_t)e->active_weapon_handle());
					if (!wep) return;
	
					char ammo[519];
					sprintf_s(ammo, "[%d / %d]", wep->is_knife() ? 0 : wep->clip1_count(), wep->is_knife() ? 0 : wep->primary_reserve_ammo_acount());

					render::get().draw_text((x + w) + 2, (y + h) - 12, render::get().esp_font, ammo, false, color(180, 180, 180));
				}

				if (c_config::get().player_health) {
					auto health = e->health();

					if (health > 100)
						health = 100;

					render::get().draw_filled_rect(x - 6, y, 4, h, color(0, 0, 0, 125));
					render::get().draw_filled_rect(x - 6, y, 4, (h / 100.f) * health, color(0, 185, 0));
					render::get().draw_outline(x - 6, y, 4, h, color(0, 0, 0, 255));

					render::get().draw_filled_rect(x - 7, y + (h / 100.f) * health, 6, 3, color(120, 120, 120));
					render::get().draw_text(x - 9, (y + 3) + (h / 100.f) * health, render::get().esp_font, std::to_string(health), false, color(180, 180, 180));

				}
				if (c_config::get().player_weapon) {
					player_t* wep = (player_t*)interfaces::entity_list->get_client_entity_handle((uintptr_t)e->active_weapon_handle());
					if (!wep) return;

					c_client_class* cclass = (c_client_class*)wep->client_class();
					if (cclass) {
						render::get().draw_text(x + (w / 2), (y + h) + 1, render::get().esp_font, clean_name(cclass->network_name), true, color(180, 180, 180));
					}
				}
			}
		}
	}
}
