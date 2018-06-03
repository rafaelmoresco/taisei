/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2018, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2018, Andrei Alexeyev <akari@alienslab.net>.
 */

#pragma once
#include "taisei.h"

#include "plrmodes.h"

extern PlayerCharacter character_reimu;
extern PlayerMode plrmode_reimu_a;
extern PlayerMode plrmode_reimu_b;

void reimu_yinyang_visual(Enemy *e, int t, bool render);
