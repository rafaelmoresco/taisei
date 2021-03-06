/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2018, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2018, Andrei Alexeyev <akari@alienslab.net>.
 */

#pragma once
#include "taisei.h"

#include <SDL_ttf.h>

#include "sprite.h"
#include "hashtable.h"

typedef enum {
	AL_Center,
	AL_Left,
	AL_Right
} Alignment;

enum {
	AL_Flag_NoAdjust = 0x10,
};

typedef struct Font Font;

void draw_text(Alignment align, float x, float y, const char *text, Font *font);
void draw_text_auto_wrapped(Alignment align, float x, float y, const char *text, int width, Font *font);
void render_text(const char *text, Font *font, Sprite *out_spr);

int stringwidth(char *s, Font *font);
int stringheight(char *s, Font *font);
int charwidth(char c, Font *font);

int font_line_spacing(Font *font);

void shorten_text_up_to_width(char *s, float width, Font *font);
void wrap_text(char *buf, size_t bufsize, const char *src, int width, Font *font);

void init_fonts(void);
void uninit_fonts(void);
void load_fonts(float quality);
void reload_fonts(float quality);
void free_fonts(void);
void update_font_cache(void);

extern struct Fonts {
	union {
		struct {
			Font *standard;
			Font *mainmenu;
			Font *small;
			Font *hud;
			Font *mono;
			Font *monosmall;
			Font *monotiny;
		};

		Font *first;
	};
} _fonts;
