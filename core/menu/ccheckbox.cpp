#include "menu.h"
#include <ctime>

void c_menu::checkbox(std::string name, bool* item)
{
	int size = 10;
	static bool pressed = false;
	auto text_size = render::get().GetTextSize2(render::get().menu_font, name.c_str());

	if (c_menu::get().menu_opened) {
		if (!GetAsyncKeyState(VK_LBUTTON) && mouse_in_params(x_offset + 32, y_offset + 9, size + text_size.right, size))
		{
			if (pressed)
				*item = !*item;
			pressed = false;
		}

		if (GetAsyncKeyState(VK_LBUTTON) && mouse_in_params(x_offset + 32, y_offset + 9, size + text_size.right, size) && !pressed)
			pressed = true;

	}
	if (c_menu::get().menu_opened) {
		if (*item == true)
			render::get().draw_circle(x_offset + 37, y_offset + 12, 5, 100, color(180, 40, 40));
		else
			render::get().filled_circle_v2(x_offset + 32, y_offset + 7, size, size, 3, color(180, 40, 40));

		render::get().draw_circle(x_offset + 10, y_offset, 10, 200, color(30, 144, 255));

		render::get().draw_filled_rect(x_offset - 10, y_offset + 23, 250, 1, color(40, 40, 40, 50)); //line across

		render::get().draw_text(x_offset + 45, y_offset + 7, render::get().main_font, *item != true ? "Tap to load · 31s" : "Opened · 13h", false, color(0, 0, 0)); //text
		render::get().draw_text(x_offset + 32, y_offset - 10, render::get().menu_font, name.c_str(), false, color(0, 0, 0)); //text
	}
	y_offset += 40;
}