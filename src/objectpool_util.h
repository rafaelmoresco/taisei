/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information.
 * ---
 * Copyright (c) 2011-2018, Lukas Weber <laochailan@web.de>.
 * Copyright (c) 2012-2018, Andrei Alexeyev <akari@alienslab.net>.
 */

#pragma once
#include "taisei.h"

#include "objectpool.h"

void objpool_release_list(ObjectPool *pool, List **dest);
void objpool_release_alist(ObjectPool *pool, ListAnchor *list);
bool objpool_is_full(ObjectPool *pool);
size_t objpool_object_contents_size(ObjectPool *pool);
void *objpool_object_contents(ObjectPool *pool, ObjectInterface *obj, size_t *out_size);

#define objpool_release_list(pool,dest) \
	objpool_release_list(pool, LIST_CAST(dest, **))

#define objpool_release_alist(pool,list) \
	objpool_release_alist(pool, LIST_ANCHOR_CAST(list, *))
