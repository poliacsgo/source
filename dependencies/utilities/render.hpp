#pragma once
#include <string>
#include "singleton.hpp"
#include "../interfaces/interfaces.hpp"

enum font_flags {
	fontflag_none,
	fontflag_italic = 0x001,
	fontflag_underline = 0x002,
	fontflag_strikeout = 0x004,
	fontflag_symbol = 0x008,
	fontflag_antialias = 0x010,
	fontflag_gaussianblur = 0x020,
	fontflag_rotary = 0x040,
	fontflag_dropshadow = 0x080,
	fontflag_additive = 0x100,
	fontflag_outline = 0x200,
	fontflag_custom = 0x400,
	fontflag_bitmap = 0x800,
};

class render : public singleton<render> {
public: // fonts
	DWORD main_font;
	DWORD menu_font;
	DWORD tab_font;
	DWORD esp_font;
	DWORD ind_font;
	DWORD ind_font2;
	DWORD tit_font;

public: // basic renderer
	void setup_fonts( ) {
		static bool _o = false;
		if ( !_o ) {
			main_font = interfaces::surface->font_create( );
			menu_font = interfaces::surface->font_create();
			tab_font = interfaces::surface->font_create();
			esp_font = interfaces::surface->font_create();
			ind_font = interfaces::surface->font_create();
			ind_font2 = interfaces::surface->font_create();
			tit_font = interfaces::surface->font_create();

			interfaces::surface->set_font_glyph(main_font, "Helvetica Thin", 11, 550, 0, 0, font_flags::fontflag_antialias);
			interfaces::surface->set_font_glyph(menu_font, "Helvetica Thin", 14, 550, 0, 0, font_flags::fontflag_antialias);
			interfaces::surface->set_font_glyph(tab_font, "Helvetica Thin", 20, 550, 0, 0, font_flags::fontflag_antialias);

			interfaces::surface->set_font_glyph(tit_font, "Helvetica Thin", 25, 600, 0, 0, font_flags::fontflag_antialias);

			interfaces::surface->set_font_glyph(esp_font, "Roboto", 11, 500, 0, 0, font_flags::fontflag_dropshadow | font_flags::fontflag_antialias);
			interfaces::surface->set_font_glyph(ind_font, "Roboto", 30, 400, 0, 0, font_flags::fontflag_antialias);
			interfaces::surface->set_font_glyph(ind_font2, "Roboto", 35, 400, 0, 0, font_flags::fontflag_antialias);
			_o = true;
		}
	}
	void draw_line( int x1, int y1, int x2, int y2, color colour ) {
		interfaces::surface->set_drawing_color( colour.r, colour.g, colour.b, colour.a );
		interfaces::surface->draw_line( x1, y1, x2, y2 );
	}
	void draw_text( int x, int y, unsigned long font, const char* string, color colour ) {
		va_list va_alist;
		char buf[ 1024 ];
		va_start( va_alist, string );
		_vsnprintf( buf, sizeof( buf ), string, va_alist );
		va_end( va_alist );
		wchar_t wbuf[ 1024 ];
		MultiByteToWideChar( CP_UTF8, 0, buf, 256, wbuf, 256 );

		int width, height;
		interfaces::surface->get_text_size( font, wbuf, width, height );

		interfaces::surface->set_text_color( colour.r, colour.g, colour.b, colour.a );
		interfaces::surface->draw_text_font( font );
		interfaces::surface->draw_text_pos( x, y );
		interfaces::surface->draw_render_text( wbuf, wcslen( wbuf ) );
	}
	void draw_text( int x, int y, unsigned long font, std::string string, bool text_centered, color colour ) {
		std::wstring text = std::wstring( string.begin( ), string.end( ) );
		const wchar_t* converted_text = text.c_str( );

		int width, height;
		interfaces::surface->get_text_size( font, converted_text, width, height );

		interfaces::surface->set_text_color( colour.r, colour.g, colour.b, colour.a );
		interfaces::surface->draw_text_font( font );
		if ( text_centered )
			interfaces::surface->draw_text_pos( x - ( width / 2 ), y );
		else
			interfaces::surface->draw_text_pos( x, y );
		interfaces::surface->draw_render_text( converted_text, wcslen( converted_text ) );
	}
	void gradient(int x, int y, int w, int h, color c1, color c2)
	{
		render::get().draw_filled_rect(x, y, w, h, c1);

		BYTE first = c2.r;
		BYTE second = c2.g;
		BYTE third = c2.b;
		int a;
		if (GetKeyState(VK_INSERT) & 1)
			a = 255; else a = 0;

		for (int i = 0; i < h; i++)
		{
			float fi = i, fh = h;
			float a = fi / fh;
			DWORD ia = a * a;
			render::get().draw_filled_rect(x, y + i, w, 1, color(first, second, third, ia));
		}
	}
	__forceinline bool world_to_screen(vec3_t In, vec3_t& Out) {
		matrix3x4_t ViewMatrix = interfaces::engine->GetMatrix();
		Out.x = ViewMatrix.Matrix[0] * In.x + ViewMatrix.Matrix[1] * In.y + ViewMatrix.Matrix[2] * In.z + ViewMatrix.Matrix[3];
		Out.y = ViewMatrix.Matrix[4] * In.x + ViewMatrix.Matrix[5] * In.y + ViewMatrix.Matrix[6] * In.z + ViewMatrix.Matrix[7];
		Out.z = ViewMatrix.Matrix[12] * In.x + ViewMatrix.Matrix[13] * In.y + ViewMatrix.Matrix[14] * In.z + ViewMatrix.Matrix[15];

		if (Out.z < 0.01f) return false;
		float Inverse = 1.f / Out.z;

		Out.x *= Inverse;
		Out.y *= Inverse;

		int Width, Height;
		interfaces::engine->get_screen_size(Width, Height);
		auto X = Width / 2;
		auto Y = Height / 2;

		X += 0.5 * Out.x * Width + 0.5;
		Y -= 0.5 * Out.y * Height + 0.5;

		Out.x = X;
		Out.y = Y;
		return true;
	}
	void draw_filled_rect( int x, int y, int w, int h, color colour ) {
		interfaces::surface->set_drawing_color( colour.r, colour.g, colour.b, colour.a );
		interfaces::surface->draw_filled_rectangle( x, y, w, h );
	}
	void textured_polygon(int n, vertex_t* vertice, color col) {
		static int texture_id = interfaces::surface->create_new_texture_id(true);
		static unsigned char buf[4] = { 255, 255, 255, 255 };
		interfaces::surface->set_texture_rgba(texture_id, buf, 1, 1);
		interfaces::surface->set_drawing_color(col.r, col.g, col.b, col.a = 255);
		interfaces::surface->set_texture(texture_id);
		interfaces::surface->draw_polygon(n, vertice);
	}
	void filled_circle_v2(int x, int y, int w, int h, int radius, color col) {
		vertex_t round[64];
		for (int i = 0; i < 4; i++) {
			int _x = x + ((i < 2) ? (w - radius) : radius);
			int _y = y + ((i % 3) ? (h - radius) : radius);
			float a = 90.f * i;

			for (int j = 0; j < 16; j++) {
				float _a = DEG2RAD(a + j * 6.f);
				round[(i * 16) + j] = vertex_t(vec2_t(_x + radius * sin(_a), _y - radius * cos(_a)));
			}
		}
		interfaces::surface->set_drawing_color(col.r, col.g, col.b);
		interfaces::surface->draw_polygon(64, round);
	}
	void filled_circle(vec2_t position, float points, float radius, color color) {
		std::vector<vertex_t> vertices;
		float step = (float)M_PI * 2.0f / points;

		for (float a = 0; a < (M_PI * 2.0f); a += step)
			vertices.push_back(vertex_t(vec2_t(radius * cosf(a) + position.x, radius * sinf(a) + position.y)));

		textured_polygon((int)points, vertices.data(), color);
	}
	void draw_outline( int x, int y, int w, int h, color colour ) {
		interfaces::surface->set_drawing_color( colour.r, colour.g, colour.b, colour.a );
		interfaces::surface->draw_outlined_rect( x, y, w, h );
	}
	void draw_textured_polygon(int n, vertex_t* vertice, color col) {
		static int texture_id = interfaces::surface->create_new_texture_id(true);
		static unsigned char buf[4] = { 255, 255, 255, 255 };
		interfaces::surface->set_texture_rgba(texture_id, buf, 1, 1);
		interfaces::surface->set_drawing_color(col.r, col.g, col.b, col.a);
		interfaces::surface->set_texture(texture_id);
		interfaces::surface->draw_polygon(n, vertice);
	}

	void draw_circle(int x, int y, int r, int s, color col) {
		float Step = M_PI * 2.0 / s;
		for (float a = 0; a < (M_PI*2.0); a += Step)
		{
			float x1 = r * cos(a) + x;
			float y1 = r * sin(a) + y;
			float x2 = r * cos(a + Step) + x;
			float y2 = r * sin(a + Step) + y;
			interfaces::surface->set_drawing_color(col.r, col.g, col.b, col.a);
			interfaces::surface->draw_line(x1, y1, x2, y2);
		}
	}

	void get_text_size( unsigned long font, const char* string, int w, int h ) {
		va_list va_alist;
		char buf[ 1024 ];
		va_start( va_alist, string );
		_vsnprintf( buf, sizeof( buf ), string, va_alist );
		va_end( va_alist );
		wchar_t out[ 1024 ];
		MultiByteToWideChar( CP_UTF8, 0, buf, 256, out, 256 );

		interfaces::surface->get_text_size( font, out, w, h );
	}
	void get_text_size( unsigned long font, std::string string, int w, int h ) {
		std::wstring text = std::wstring( string.begin( ), string.end( ) );
		const wchar_t* out = text.c_str( );

		interfaces::surface->get_text_size( font, out, w, h );
	}
	RECT GetTextSize2(DWORD font, const char* text)
	{
		size_t origsize = strlen(text) + 1;
		const size_t newsize = 100;
		size_t convertedChars = 0;
		wchar_t wcstring[newsize];
		mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);

		RECT rect; int x, y;
		interfaces::surface->get_text_size(font, wcstring, x, y);
		rect.left = x; rect.bottom = y;
		rect.right = x;
		return rect;
	}
	vec2_t get_screen_size(vec2_t area)
	{
		static int old_w, old_h;
		interfaces::engine->get_screen_size((int&)area.x, (int&) area.y);

		if ((int&) area.x != old_w || (int&) area.y != old_h)
		{
			old_w = (int&) area.x;
			old_h = (int&) area.y;
			return vec2_t(old_w, old_h);
		}
		return area;
	}
};