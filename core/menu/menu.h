#pragma once
#pragma once
#include <vector>
#include <memory>
#include <random>
#include <iostream>
#include <ShlObj_core.h>
#include <functional>
#include "../../dependencies/utilities/render.hpp"
#include "../../dependencies/common_includes.hpp"
#define menu_color color(c_config::get().menu_red, c_config::get().menu_green, c_config::get().menu_blue, fade_alpha)
class c_menu : public singleton<c_menu> {
public:
	bool menu_opened;
	vec2_t mouse_pos();
	bool mouse_in_params(int x, int y, int x2, int y2);
	void draw();
private:
	vec3_t _pos = vec3_t(500, 200, 0);

	int y_offset;
	int x_offset;
	void checkbox(std::string name, bool* item);
};