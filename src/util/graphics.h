/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2018, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2018, Andrei Alexeyev <akari@alienslab.net>.
 */

#pragma once
#include "taisei.h"

void set_ortho(float w, float h);
void colorfill(float r, float g, float b, float a);
void fade_out(float f);
void draw_stars(int x, int y, int numstars, int numfrags, int maxstars, int maxfrags, float alpha, float star_width);
