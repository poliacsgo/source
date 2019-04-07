#pragma once
#include "../../dependencies/utilities/singleton.hpp"
class c_config : public singleton< c_config > {
private:

public:
	bool visuals_enabled = false; 
	bool player_name = false; 
	bool player_box = false; 
	bool player_health = false; 
	bool player_weapon = false; 
	bool flash_indicator = false;
	bool cham_xqz = false; 
	bool chams = false; 
	bool fullbright = false; 
	bool bunnyhop = false; 
	bool lagcomp = false; 
};