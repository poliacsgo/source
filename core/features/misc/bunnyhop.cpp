#include "bunnyhop.hpp"
bool bhop::key_state(unsigned int key) {
	std::copy(keys, keys + 255, old_keys);
	for (int x = 0; x < 255; x++)
		keys[x] = (GetAsyncKeyState(x));

	return keys[key];
}

void bhop::bunnyhop(c_usercmd* user_cmd) {
	player_t* local = (player_t*)interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player());
	if (user_cmd->buttons & in_jump) {
		if (!(local->flags() & fl_onground))
			user_cmd->buttons &= ~in_jump;
	}
}

void bhop::autostrafe(c_usercmd* user_cmd) {
	player_t* local = (player_t*)interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player());
	bool key_pressed = true;
	if (key_state(0x41) || key_state(0x57) || key_state(0x53) || key_state(0x44)) key_pressed = false;

	if ((GetAsyncKeyState(VK_SPACE) && !(local->flags() & fl_onground)) && key_pressed) {
		if (user_cmd->mousedx > 1 || user_cmd->mousedx < -1) {
			user_cmd->sidemove = user_cmd->mousedx < 0.f ? -450.f : 450.f;
		}
		else {
			user_cmd->forwardmove = (1800.f * 4.f) / local->velocity().length();
			user_cmd->sidemove = (user_cmd->command_number % 2) == 0 ? -450.f : 450.f;
			if (user_cmd->forwardmove > 450.f)
				user_cmd->forwardmove = 450.f;
		}
	}
}