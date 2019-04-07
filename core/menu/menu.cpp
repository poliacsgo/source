#include "menu.h"
#include <random>
#include <conio.h>
#include <iostream>

vec2_t c_menu::mouse_pos()
{
	POINT mouse_position;
	GetCursorPos(&mouse_position);
	ScreenToClient(FindWindow(0, "Counter-Strike: Global Offensive"), &mouse_position);
	return { static_cast<float>(mouse_position.x), static_cast<float>(mouse_position.y) };
}
bool c_menu::mouse_in_params(int x, int y, int w, int h) {
	if (mouse_pos().x > x && mouse_pos().y > y && mouse_pos().x < w + x && mouse_pos().y < h + y)
		return true;
	return false;
}

void c_menu::draw()
{
	static bool _pressed = false;

	if (!_pressed && GetAsyncKeyState(VK_INSERT))
		_pressed = true;
	else if (_pressed && !GetAsyncKeyState(VK_INSERT))
	{
		_pressed = false;
		menu_opened = !menu_opened;

		interfaces::inputsystem->enable_input(!menu_opened);
	}
	static vec2_t _mouse_pos;

	static int _drag_x = 300;
	static int _drag_y = 300;
	static int _width = 250;
	static int _height = 440;

	int tab_fade = 0;
	static bool _dragging = false;
	bool _click = false;
	static bool _resizing = false;
	if (menu_opened) {

		if (GetAsyncKeyState(VK_LBUTTON))
			_click = true;

		vec2_t _mouse_pos = mouse_pos();

		if (_dragging && !_click)
			_dragging = false;

		if (_resizing && !_click)
			_resizing = false;

		if (_dragging&& _click) {
			_pos.x = _mouse_pos.x - _drag_x;
			_pos.y = _mouse_pos.y - _drag_y;
		}

		if ((mouse_in_params(_pos.x, _pos.y - 44, _width, 44)))
		{
			_dragging = true;
			_drag_x = _mouse_pos.x - _pos.x;
			_drag_y = _mouse_pos.y - _pos.y;
		}

		render::get().gradient(_pos.x, _pos.y - 40, _width, 40, color(44, 198, 252), color(9, 155, 235));
		render::get().draw_text(_pos.x + 20, _pos.y - 30, render::get().tab_font, "Friends ", color(220, 220, 220));
		render::get().draw_filled_rect(_pos.x, _pos.y, _width, _height + 10, color(220, 220, 220)); //main body

		render::get().draw_text(_pos.x + 15, _pos.y + _width + 168, render::get().tit_font, "Snapchat.CC", color(0, 0, 0));
		render::get().draw_text(_pos.x + 155, _pos.y + _width + 176, render::get().menu_font, "by polia#9328", color(40, 40, 40));

		render::get().draw_outline(_pos.x, _pos.y - 40, _width, _height + 50, color(0, 0, 0)); //main body

		y_offset = _pos.y + 30;
		x_offset = _pos.x + 11;

		checkbox("Active", &c_config::get().visuals_enabled);
		checkbox("Weapon", &c_config::get().player_weapon);
		checkbox("Health", &c_config::get().player_health);
		checkbox("Box", &c_config::get().player_box);
		checkbox("Name", &c_config::get().player_name);
		checkbox("Chams", &c_config::get().chams);
		checkbox("Chams XQZ", &c_config::get().cham_xqz);
		checkbox("Bunnyhop", &c_config::get().bunnyhop);
		checkbox("Fullbright", &c_config::get().fullbright);
		checkbox("Backtracking", &c_config::get().lagcomp);
	}
}