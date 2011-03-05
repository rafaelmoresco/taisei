/*
 * This software is licensed under the terms of the MIT-License
 * See COPYING for further information. 
 * ---
 * Copyright (C) 2011, Lukas Weber <laochailan@web.de>
 */

#include "stage.h"

#include <SDL/SDL.h>
#include "global.h"
#include "font.h"

SDL_Event event;

void stage_start() {
	init_player(&global.plr, Youmu);
}

void stage_input() {	
	while(SDL_PollEvent(&event)) {
		if(event.type == SDL_KEYDOWN) {
			switch(event.key.keysym.sym) {					
				case SDLK_LSHIFT:
					global.plr.focus = 1;
					break;
				case SDLK_y:
					global.plr.fire = True;
					break;
				case SDLK_ESCAPE:
					exit(1);
					break;
				default:
					break;
			}
		} else if(event.type == SDL_KEYUP) {
			switch(event.key.keysym.sym) {
				case SDLK_LSHIFT:
					global.plr.focus = -30; // that's for the transparency timer
					break;
				case SDLK_y:
					global.plr.fire = False;
					break;
				default:
					break;
			}
		} else if(event.type == SDL_QUIT) {
			exit(1);
		}
	}
	
	float speed = 0.01*VIEWPORT_W/((global.plr.focus > 0)+1);	
	
	Uint8 *keys = SDL_GetKeyState(NULL);
	
	global.plr.moving = False;
	
	if(keys[SDLK_LEFT] && !keys[SDLK_RIGHT]) {
		global.plr.moving = True;
		global.plr.dir = 1;
	} else if(keys[SDLK_RIGHT] && !keys[SDLK_LEFT]) {
		global.plr.moving = True;
		global.plr.dir = 0;
	}	
	
	if(keys[SDLK_LEFT] && global.plr.x - global.plr.ani.w/2 - speed > 0)
		global.plr.x -= speed;		
	if(keys[SDLK_RIGHT] && global.plr.x + global.plr.ani.w/2 + speed < VIEWPORT_W)
		global.plr.x += speed;
	if(keys[SDLK_UP] && global.plr.y - global.plr.ani.h/2 - speed > 0)
		global.plr.y -= speed;
	if(keys[SDLK_DOWN] && global.plr.y + global.plr.ani.h/2 + speed < VIEWPORT_H)
		global.plr.y += speed;

}

void stage_draw() {
	glPushMatrix();
	glTranslatef(VIEWPORT_X,VIEWPORT_Y,0);
	
	player_draw(&global.plr);

	draw_projectiles();

	Fairy *f = global.fairies;
	while(f != NULL) {
		draw_fairy(f);		
		f = f->next;
	}	
	
	glPopMatrix();
	
	char buf[16];
	sprintf(buf, "Power: %.2f", global.plr.power);
	
	draw_texture(SCREEN_W/2, SCREEN_H/2, &global.textures.hud);
	draw_text(buf, SCREEN_W-200, 200, _fonts.biolinum);
	SDL_GL_SwapBuffers();
}

void stage_logic() {
	player_logic(&global.plr);
	
	process_fairies();
	process_projectiles();
	
	global.frames++;
	
	if(SDL_GetTicks() > global.time+1000) {
		fprintf(stderr, "FPS: %d\n", global.fps);
		global.fps = 0;
		global.time = SDL_GetTicks();
	} else {
		global.fps++;
	}
}

void stage_end() {
	free_projectiles();
	free_fairies();
	global.frames = 0;
}
		