/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information. 
 * ---
 * Copyright (C) 2011, Lukas Weber <laochailan@web.de>
 * Code by Juergen Kieslich
 */

#include "poweritem.h"
#include "global.h"
#include "list.h"

void create_poweritem(complex pos, complex v, Type type) {
	Poweritem *p = create_element((void **)&global.poweritems, sizeof(Poweritem));
	p->pos = pos;
	p->pos0 = pos;
	p->v = v;
	p->birthtime = global.frames;
	p->auto_collect = 0;
	p->type = type;
}


void delete_poweritem(Poweritem *poweritem) {
	delete_element((void **)&global.poweritems, poweritem);
}

void draw_poweritems() {
	Poweritem *p;
	Texture *tex = NULL;
	for(p = global.poweritems; p; p = p->next){
	  switch(p->type){
	  case Power:
	    tex = get_tex("items/power");
	    break;
	  case Point:
	    tex = get_tex("items/point");
	    break;
	  case Life:
	    tex = get_tex("items/life");
	    break;
	  case Bomb:
	    tex = get_tex("items/bomb");
	    break;
	  default:
	    break;
	}
		draw_texture_p(creal(p->pos), cimag(p->pos), tex);
	}
}

void delete_poweritems() {
	delete_all_elements((void **)&global.poweritems, delete_element);
}

void move_poweritem(Poweritem *p) {
	int t = global.frames - p->birthtime;
	complex lim = I*2;
	
	if(p->auto_collect)	
		p->pos -= 7*cexp(I*carg(p->pos - global.plr.pos));
	else
		p->pos = p->pos0 + log(t/5.0 + 1)*5*(p->v + lim) + lim*t;
}

void process_poweritems() {
	Poweritem *poweritem = global.poweritems, *del = NULL;
    int v;
	while(poweritem != NULL) {
		if(cimag(global.plr.pos) < POINT_OF_COLLECT || cabs(global.plr.pos - poweritem->pos) < 19 + global.plr.focus
			|| global.frames - global.plr.recovery < 0)
			poweritem->auto_collect = 1;
		
		move_poweritem(poweritem);
		
		v = collision(poweritem);
		if(v == 1) {
			switch(poweritem->type) {
			case Power:
				global.plr.power += 0.1;
				break;
			case Point:
				global.points += 100;
				break;
			case Life:
				global.plr.lifes++;
				break;
			case Bomb:
				global.plr.bombs++;
				break;
			}
			play_sound("item_generic");
		}
		if(v == 1 || creal(poweritem->pos) < -9 || creal(poweritem->pos) > VIEWPORT_W + 9
			|| cimag(poweritem->pos) > VIEWPORT_H + 8 ) {
			del = poweritem;
			poweritem = poweritem->next;
			delete_poweritem(del);
		} else {
			poweritem = poweritem->next;
		}
	}
}

int collision(Poweritem *p) {
	if(cabs(global.plr.pos - p->pos) < 5)
		return 1;
	
	return 0;
}
