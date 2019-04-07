#pragma once //backup
#include "../../dependencies/common_includes.hpp"
#include "../../dependencies/interfaces/textures.hpp"
#include <algorithm>
#include "../menu/menu.h"
#include "../features/visuals/visuals.hpp"
#include "../features/visuals/chams.hpp"
#include "../features/aimbot/lagcomp.hpp"
#include "../features/misc/bunnyhop.hpp"
#include <Psapi.h>
#define square( x ) ( x * x )
#include <cmath>

inline float FastSqrt(float x) {
	unsigned int i = *(unsigned int*)&x;
	i += 127 << 23;
	i >>= 1;
	return *(float*)&i;
}

std::unique_ptr<vmt_hook> hooks::clientmode_hook;
std::unique_ptr<vmt_hook> hooks::panel_hook;
std::unique_ptr<vmt_hook> hooks::renderview_hook;
std::unique_ptr<vmt_hook> hooks::surface_hook;

WNDPROC hooks::wndproc_original = NULL;
void hooks::initialize( ) {
	clientmode_hook = std::make_unique<vmt_hook>( );
	panel_hook = std::make_unique<vmt_hook>( );
	renderview_hook = std::make_unique<vmt_hook>( );
	surface_hook = std::make_unique<vmt_hook>();

	clientmode_hook->setup( interfaces::clientmode );
	clientmode_hook->hook_index( 24, reinterpret_cast< void* >( create_move ) );

	panel_hook->setup( interfaces::panel );
	panel_hook->hook_index( 41, reinterpret_cast< void* >( paint_traverse ) );

	renderview_hook->setup( interfaces::render_view );
	renderview_hook->hook_index( 9, reinterpret_cast< void* >( scene_end ) );

	surface_hook->setup(interfaces::surface);
	surface_hook->hook_index(67, reinterpret_cast< void* >( lock_cursor ) );

	wndproc_original = ( WNDPROC ) SetWindowLongPtrA( FindWindow( "Valve001", NULL ), GWL_WNDPROC, ( LONG ) wndproc );

	interfaces::console->get_convar( "mat_queue_mode" )->set_value( 0 );
	interfaces::console->get_convar( "viewmodel_fov" )->callbacks.SetSize( 0 );
	interfaces::console->get_convar( "viewmodel_offset_x")->callbacks.SetSize( 0 );
	interfaces::console->get_convar( "viewmodel_offset_z")->callbacks.SetSize( 0 );
	interfaces::console->get_convar( "mat_postprocess_enable" )->set_value( 0 );
	interfaces::console->get_convar( "crosshair" )->set_value( 1 );
	render::get( ).setup_fonts( );
}

void hooks::shutdown( ) {
	panel_hook->release( );
	renderview_hook->release( );
	surface_hook->release();
	clientmode_hook->release();

	SetWindowLongPtrA( FindWindow( "Valve001", NULL ), GWL_WNDPROC, ( LONG ) wndproc_original );
}

bool __stdcall hooks::create_move(float frame_time, c_usercmd* user_cmd) {
	static auto original_fn = reinterpret_cast<create_move_fn>(clientmode_hook->get_original(24))(interfaces::clientmode, frame_time, user_cmd);
	if (!user_cmd || !user_cmd->command_number)
		return original_fn;

	if (!interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()))
		return original_fn;

	player_t * local = (player_t*)interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player());
	if (!local) return false;
	if (interfaces::engine->is_connected() && interfaces::engine->is_in_game()) {

		if (c_config::get().bunnyhop) {
			bhop::get().bunnyhop(user_cmd);
			bhop::get().autostrafe(user_cmd);
		}

		if (c_config::get().lagcomp)
			lagcomp::get().handle(local, user_cmd);

		if (c_config::get().fullbright)
			interfaces::console->get_convar("mat_fullbright")->set_value(1);
		else
			interfaces::console->get_convar("mat_fullbright")->set_value(0);

		utilities::math::fix_movement(user_cmd);
		utilities::math::normalize_view(user_cmd->viewangles);
	}
	return false;
}
void __stdcall hooks::paint_traverse( unsigned int panel, bool force_repaint, bool allow_force ) {
	std::string panel_name = interfaces::panel->get_panel_name( panel );

	reinterpret_cast< paint_traverse_fn >( panel_hook->get_original( 41 ) )( interfaces::panel, panel, force_repaint, allow_force );

	static unsigned int _panel = 0;
	static bool once = false;

	if ( !once ) {
		PCHAR panel_char = ( PCHAR ) interfaces::panel->get_panel_name( panel );
		if ( strstr( panel_char, "MatSystemTopPanel" ) ) {
			_panel = panel;
			once = true;
		}
	}
	else if ( _panel == panel ) {
		c_menu::get().draw();

		if (interfaces::engine->is_in_game() && interfaces::engine->is_connected())
			c_visuals::get().run();
	}
}
void __stdcall hooks::scene_end( ) {
	for (int i = 0; i <= 64; i++) {
		player_t* e = (player_t*)interfaces::entity_list->get_client_entity(i);
		if (!e) continue;
		player_t* local = (player_t*)interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player());
		if (!local) continue;
		if (e->dormant()) continue;
		if (!e->health()) continue;
		if (local->team() == e->team()) continue;

		static i_material* texture = chams::get().create_material(false, true, false);
		static i_material* texture_xqz = chams::get().create_material(true, true, false);

		float color[3] = { 0.f };
		color[0] = 255 / 255.f;
		color[1] = 255 / 255.f;
		color[2] = 255 / 255.f;
		if (c_config::get().chams && c_config::get().visuals_enabled) {
			if (c_config::get().cham_xqz)
				interfaces::model_render->override_material(texture_xqz);
			else
				interfaces::model_render->override_material(texture);
		
			interfaces::render_view->color_modulation(color);
			e->draw_model(0x1, 255);
			interfaces::model_render->override_material(nullptr);
		}
	}

	reinterpret_cast< scene_end_fn >(renderview_hook->get_original(9))(interfaces::render_view);
}
LRESULT __stdcall hooks::wndproc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam ) {
	return CallWindowProcA( wndproc_original, hwnd, message, wparam, lparam );
}

void __stdcall hooks::lock_cursor() {
	reinterpret_cast<lock_cursor_fn>(surface_hook->get_original(67))(interfaces::surface);

	if (c_menu::get().menu_opened) {
		interfaces::surface->unlock_cursor();
		return;
	}
}
