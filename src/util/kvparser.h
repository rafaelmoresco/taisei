/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2018, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2018, Andrei Alexeyev <akari@alienslab.net>.
 */

#pragma once
#include "taisei.h"

#include <SDL.h>
#include "hashtable.h"

// The callback should return true on success and false on error. In the case of
// an error it is responsible for calling log_warn with a descriptive error
// message.
typedef bool (*KVCallback)(const char *key, const char *value, void *data);

typedef struct KVSpec {
	const char *name;

	char **out_str;
	int *out_int;
	double *out_double;
	float *out_float;
} KVSpec;

bool parse_keyvalue_stream_cb(SDL_RWops *strm, KVCallback callback, void *data);
bool parse_keyvalue_file_cb(const char *filename, KVCallback callback, void *data);
Hashtable* parse_keyvalue_stream(SDL_RWops *strm);
Hashtable* parse_keyvalue_file(const char *filename);
bool parse_keyvalue_stream_with_spec(SDL_RWops *strm, KVSpec *spec);
bool parse_keyvalue_file_with_spec(const char *filename, KVSpec *spec);
