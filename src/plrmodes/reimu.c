/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2018, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2018, Andrei Alexeyev <akari@alienslab.net>.
 */

#include "taisei.h"

#include "global.h"
#include "plrmodes.h"
#include "reimu.h"

PlayerCharacter character_reimu = {
    .id = PLR_CHAR_REIMU,
    .lower_name = "reimu",
    .proper_name = "Reimu",
    .full_name = "Hakurei Reimu",
    .title = "Shrine Maiden",
    .dialog_sprite_name = "dialog/reimu",
    .player_sprite_name = "reimu",
    .ending = {
        .good = good_ending_marisa,
        .bad = bad_ending_marisa,
    },
};
