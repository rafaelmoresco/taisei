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
#include "youmu.h"
#include "renderer/api.h"

#define MYON (global.plr.slaves.first)

static Color myon_color(float f, float a) {
	// return rgba(0.8+0.2*f, 0.9-0.4*sqrt(f), 1.0-0.2*f*f, a);
	// return rgba(0.8+0.2*f, 0.9-0.4*sqrt(f), 1.0-0.4*f*f, a);
	return rgba(0.8+0.2*f, 0.9-0.4*sqrt(f), 1.0-0.35*f*f, a);
}

static int myon_particle_rule(Projectile *p, int t) {
	if(t < 0) {
		return ACTION_ACK;
	}

	p->pos += p->args[0];
	p->angle += 0.03 * (1 - 2 * (p->birthtime & 1));
	p->color = derive_color(approach_color(p->color, myon_color(0.5, 0), 0.04), CLRMASK_A, p->color);

	return ACTION_NONE;
}

static complex myon_tail_dir(void) {
	double angle = carg(MYON->args[0]);
	complex dir = cexp(I*(0.1 * sin(global.frames * 0.05) + angle));
	float f = abs(global.plr.focus) / 30.0;
	return f * f * dir;
}

static int myon_flare_particle_rule(Projectile *p, int t) {
	if(t < 0) {
		return ACTION_ACK;
	}

	// wiggle wiggle
	p->pos += 0.05 * (MYON->pos - p->pos) * cexp(I * sin((t - global.frames * 2) * 0.1) * M_PI/8);
	p->args[0] = 3 * myon_tail_dir();
	p->color = derive_color(p->color, CLRMASK_A, rgba(1, 1, 1, pow(1 - min(1, t / (double)p->timeout), 2)));

	return myon_particle_rule(p, t);
}

static void myon_draw_trail(Projectile *p, int t) {
	float fadein = clamp(t/10.0, p->args[2], 1);
	float s = min(1, 1 - t / (double)p->timeout);
	float a = color_component(p->color, CLR_A) * fadein;
	Color c = derive_color(p->color, CLRMASK_A, rgba(0, 0, 0, a * s * s));
	youmu_common_draw_proj(p, c, fadein * (2-s) * p->args[1]);
}

static void spawn_stardust(complex pos, Color clr, int timeout, complex v) {
	PARTICLE(
		.sprite = "stardust",
		.pos = pos+5*frand()*cexp(2.0*I*M_PI*frand()),
		.color = clr,
		.draw_rule = myon_draw_trail,
		.rule = myon_particle_rule,
		.timeout = timeout,
		.args = { v, 0.2 + 0.1 * frand(), 1 },
		.angle = M_PI*2*frand(),
		.blend = BLEND_ADD,
		.flags = PFLAG_NOREFLECT,
		.layer = LAYER_PARTICLE_LOW | 1,
	);
}

static void myon_spawn_trail(Enemy *e, int t) {
	float a = global.frames * 0.07;
	complex pos = e->pos + 3 * (cos(a) + I * sin(a));

	complex stardust_v = 3 * myon_tail_dir() * cexp(I*M_PI/16*sin(1.33*t));
	float f = abs(global.plr.focus) / 30.0;
	stardust_v = f * stardust_v + (1 - f) * -I;

	if(player_should_shoot(&global.plr, true)) {
		PARTICLE(
			.sprite = "smoke",
			.pos = pos+10*frand()*cexp(2.0*I*M_PI*frand()),
			.color = myon_color(f, (1 + f) * 0.05),
			.draw_rule = myon_draw_trail,
			.rule = myon_particle_rule,
			.timeout = 60,
			.args = { -I*0.0*cexp(I*M_PI/16*sin(t)), -0.2, 0 },
			.blend = BLEND_ADD,
			.flags = PFLAG_NOREFLECT,
			.angle = M_PI*2*frand(),
		);

		PARTICLE(
			.sprite = "flare",
			.pos = pos+5*frand()*cexp(2.0*I*M_PI*frand()),
			.color = rgba(1, 1, 1, 0.2),
			.draw_rule = Shrink,
			.rule = myon_particle_rule,
			.timeout = 10,
			.args = { cexp(I*M_PI*2*frand())*0.5, 0.2, 0 },
			.blend = BLEND_ADD,
			.flags = PFLAG_NOREFLECT,
			.angle = M_PI*2*frand(),
		);
	}

	PARTICLE(
		.sprite = "myon",
		.color = myon_color(f, 0.5),
		.pos = pos,
		.rule = myon_flare_particle_rule,
		.timeout = 40,
		.args = { f * stardust_v },
		.draw_rule = Shrink,
		.flags = PFLAG_NOREFLECT | PFLAG_REQUIREDPARTICLE,
		.angle = M_PI*2*frand(),
		.layer = LAYER_PARTICLE_LOW,
	);

	spawn_stardust(pos, rgba(1, 1, 1, 0.1), 60, stardust_v);
}

static void myon_draw_proj_trail(Projectile *p, int t) {
	float time_progress = t / p->args[0];
	float s = 2 * time_progress;
	float a = color_component(p->color, CLR_A) * min(1, s) * (1 - time_progress);
	Color c = derive_color(p->color, CLRMASK_A, rgba(0, 0, 0, a));
	youmu_common_draw_proj(p, c, s * p->args[2]);
}

static int myon_proj(Projectile *p, int t) {
	if(t < 0) {
		return ACTION_ACK;
	}

	linear(p, t);

	//p->pos = global.plr.slaves->pos - global.plr.slaves->args[0] / cabs(global.plr.slaves->args[0]) * t * cabs(p->args[0]);
	//p->angle = carg(-global.plr.slaves->args[0]);

	// spawn_stardust(p->pos, multiply_colors(p->color, myon_color(abs(global.plr.focus) / 30.0, 0.1)), 20, p->args[0]*0.1);

	PARTICLE(
		.sprite = "boss_shadow",
		.pos = p->pos,
		.color = derive_color(p->color, CLRMASK_A, rgba(0, 0, 0, 0.075)),
		.draw_rule = myon_draw_proj_trail,
		.rule = linear,
		.timeout = 10,
		.args = { p->args[0]*0.8, 0.6, 0 },
		.flags = PFLAG_DRAWADD | PFLAG_NOREFLECT,
		.angle = p->angle,
	);

	return ACTION_NONE;
}

static void myon_proj_draw(Projectile *p, int t) {
	float a = 1 - pow(1 - min(1, t / 10.0), 2);
	Color c = multiply_colors(p->color, rgba(1, 1, 1, a));
	youmu_common_draw_proj(p, c, 1);
}

static Projectile* youmu_mirror_myon_proj(char *tex, complex pos, double speed, double angle, double aoffs, double upfactor, int dmg) {
	complex dir = cexp(I*(M_PI/2 + aoffs)) * upfactor + cexp(I * (angle + aoffs)) * (1 - upfactor);
	dir = dir / cabs(dir);

	// float f = ((global.plr.inputflags & INFLAG_FOCUS) == INFLAG_FOCUS);
	float f = smoothreclamp(abs(global.plr.focus) / 30.0, 0, 1, 0, 1);
	Color c, intermediate = rgb(1.0, 1.0, 1.0);

	if(f < 0.5) {
		c = mix_colors(
			intermediate,
			rgb(0.4, 0.6, 0.6),
			f * 2
		);
	} else {
		c = mix_colors(
			myon_color(f, 1),
			intermediate,
			(f - 0.5) * 2
		);
	}

	return PROJECTILE(
		.color = c, // mix_colors(myon_color(f, 1), rgb(0.6, 0.8, 0.7), f),
		.sprite = tex,
		.pos = pos,
		.rule = myon_proj,
		.args = { speed*dir },
		.draw_rule = myon_proj_draw,
		.type = PlrProj+dmg,
		.shader = "sprite_youmu_myon_shot",
	);
}

static int youmu_mirror_myon(Enemy *e, int t) {
	if(t == EVENT_BIRTH)
		e->pos = e->pos0 + global.plr.pos;
	if(t < 0)
		return 1;

	myon_spawn_trail(e, t);

	Player *plr = &global.plr;
	float rad = cabs(e->pos0);

	double nfocus = plr->focus / 30.0;

	if(!(plr->inputflags & INFLAG_SHOT)) {
		nfocus = 0.0;
		e->pos0 = -rad * I;
	} else if(!(plr->inputflags & INFLAG_FOCUS)) {
		if((plr->inputflags & INFLAGS_MOVE)) {
			e->pos0 = rad * -plr->lastmovedir;
		} else {
			e->pos0 = e->pos - plr->pos;
			e->pos0 *= rad / cabs(e->pos0);
		}
	}

	complex target = plr->pos + e->pos0;
	complex v = cexp(I*carg(target - e->pos)) * min(10, 0.07 * max(0, cabs(target - e->pos) - VIEWPORT_W * 0.5 * nfocus));
	float s = sign(creal(e->pos) - creal(global.plr.pos));

	if(!s) {
		s = sign(sin(t/10.0));
	}

	float rot = clamp(0.005 * cabs(global.plr.pos - e->pos) - M_PI/6, 0, M_PI/8);
	v *= cexp(I*rot*s);
	e->pos += v;

	if(!(plr->inputflags & INFLAG_SHOT) || !(plr->inputflags & INFLAG_FOCUS)) {
		e->args[0] = plr->pos - e->pos;
	}

	e->args[1] += (e->args[0] - e->args[1]) * 0.1;

	if(player_should_shoot(&global.plr, true) && global.plr.deathtime >= -1) {
		int v1 = -10;
		int v2 = -10;

		double r1 = (psin(global.frames * 2.0) * 0.5 + 0.5) * 0.1;
		double r2 = (psin(global.frames * 1.2) * 0.5 + 0.5) * 0.1;

		double a = carg(e->args[0]);
		double f = smoothreclamp(0.5 + 0.5 * (1.0 - nfocus), 0, 1, 0, 1);
		double u = 0; // smoothreclamp(1 - nfocus, 0, 1, 0, 1);

		r1 *= f;
		r2 *= f;

		int p = plr->power / 100;
		int dmg_center = 180 - rint(160 * (1 - pow(1 - 0.25 * p, 2)));
		int dmg_side = 41 - 3 * p;

		if(plr->power >= 100 && !((global.frames+0) % 6)) {
			youmu_mirror_myon_proj("youmu",  e->pos, v2, a,  r1*1, u, dmg_side);
			youmu_mirror_myon_proj("youmu",  e->pos, v2, a, -r1*1, u, dmg_side);
		}

		if(plr->power >= 200 && !((global.frames+3) % 6)) {
			youmu_mirror_myon_proj("youmu", e->pos, v1, a,  r2*2, 0, dmg_side);
			youmu_mirror_myon_proj("youmu", e->pos, v1, a, -r2*2, 0, dmg_side);
		}

		if(plr->power >= 300 && !((global.frames+0) % 6)) {
			youmu_mirror_myon_proj("youmu",  e->pos, v2, a,  r1*3, 0, dmg_side);
			youmu_mirror_myon_proj("youmu",  e->pos, v2, a, -r1*3, 0, dmg_side);
		}

		if(plr->power >= 400 && !((global.frames+3) % 6)) {
			youmu_mirror_myon_proj("youmu", e->pos, v1, a,  r2*4, u, dmg_side);
			youmu_mirror_myon_proj("youmu", e->pos, v1, a, -r2*4, u, dmg_side);
		}

		if(!((global.frames+3) % 6)) {
			youmu_mirror_myon_proj("youmu", e->pos, v1, a, 0, 0, dmg_center);
		}
	}

	return 1;
}

static int youmu_mirror_self_proj(Projectile *p, int t) {
	if(t < 0) {
		return ACTION_ACK;
	}

	complex v0 = p->args[0];
	complex v1 = p->args[1];
	double f = creal(p->args[2]) ? clamp(t / p->args[2], 0, 1) : 1;
	complex v = v1*f + v0*(1-f);

	complex diff = p->pos0 + v * t - p->pos;
	p->pos += diff;
	p->angle = carg(diff ? diff : v);

	return 1;
}

static Projectile* youmu_mirror_self_shot(Player *plr, complex ofs, complex vel, int dmg, double turntime) {
	return PROJECTILE("youmu", plr->pos + ofs, 0,
		.type = PlrProj+dmg,
		.shader = "sprite_default",
		.draw_rule = myon_proj_draw,
		.rule = youmu_mirror_self_proj,
		.args = {
			vel*0.2*cexp(I*M_PI*0.5*sign(creal(ofs))), vel, turntime,
		},
	);
}

static void youmu_mirror_shot(Player *plr) {
	if(!(global.frames % 4)) {
		play_sound("generic_shot");
	}

	int p = plr->power / 100;

	if(!(global.frames % 6)) {
		int dmg = 105 - 10 * p;
		youmu_mirror_self_shot(plr, +10 - I*20, -20.0*I, dmg, 0);
		youmu_mirror_self_shot(plr, -10 - I*20, -20.0*I, dmg, 0);
	}

	if(!((global.frames) % 6)) {
		for(int i = 0; i < p; ++i) {
			int dmg = 21;
			double spread = M_PI/64 * (1 + 0.5 * smoothreclamp(psin(global.frames/10.0), 0, 1, 0, 1));

			youmu_mirror_self_shot(plr, (+10 + I*10), -(20.0-i)*I*cexp(-I*(1+i)*spread), dmg, 20);
			youmu_mirror_self_shot(plr, (-10 + I*10), -(20.0-i)*I*cexp(+I*(1+i)*spread), dmg, 20);
		}
	}
}

static int youmu_split(Enemy *e, int t) {
	if(t < 0)
		return 1;

	if(global.frames - global.plr.recovery > 0) {
		return ACTION_DESTROY;
	}
	TIMER(&t);
	//FROM_TO(0, 220, 1) {
		tsrand_fill(5);
		double x = nfrand()*10;
		PARTICLE(
			.sprite = "petal",
			.pos = VIEWPORT_W/2+x+(x>0)*VIEWPORT_H*I,
			.rule = accelerated,
			.draw_rule = Petal,
			.color = rgba(0.1, 0.1, 0.5, min(60, t)),
			.args = {
				-10*I*x/fabs(x),
				-I*x/fabs(x)+anfrand(4)*0.1,
				afrand(1) + afrand(2)*I,
				afrand(3) + 360.0*I*afrand(0)
			},
			.flags = PFLAG_DRAWADD,
		);
	//}
	FROM_TO(0, 220, 1) {
		global.plr.pos = VIEWPORT_W/2.0 + (VIEWPORT_H-180)*I;
	}

	return 1;
}

static void youmu_mirror_shader(FBO *fbo) {
	ShaderProgram *shader = r_shader_get("youmua_bomb");

	double t = player_get_bomb_progress(&global.plr,0);
	r_shader_ptr(shader);
	r_uniform_float("tbomb", t);
	draw_fbo(fbo);
	r_shader_standard();

	colorfill(1,1,1,max(0,1-10*t));
}

static void youmu_mirror_bomb(Player *plr) {
	play_sound("bomb_youmu_b");
	create_enemy_p(&plr->slaves, 40.0*I, ENEMY_BOMB, NULL, youmu_split, 280,0,0,0);
}

static void youmu_mirror_init(Player *plr) {
	Enemy *myon = create_enemy_p(&plr->slaves, 40.0*I, ENEMY_IMMUNE, NULL, youmu_mirror_myon, 0, 0, 0, 0);
	myon->ent.draw_layer = LAYER_PLAYER_SLAVE;
}

static void youmu_mirror_preload(void) {
	const int flags = RESF_DEFAULT;

	preload_resources(RES_SPRITE, flags,
		"proj/youmu",
		"part/myon",
		"part/stardust",
	NULL);

	preload_resources(RES_TEXTURE, flags,
		"youmu_bombbg1",
	NULL);

	preload_resources(RES_SHADER_PROGRAM, flags,
		"youmua_bomb",
		"sprite_youmu_myon_shot",
	NULL);

	preload_resources(RES_SFX, flags | RESF_OPTIONAL,
		"bomb_youmu_b",
	NULL);
}

PlayerMode plrmode_youmu_a = {
	.name = "Mirror Sign",
	.character = &character_youmu,
	.shot_mode = PLR_SHOT_YOUMU_MIRROR,
	.procs = {
		.property = youmu_common_property,
		.bomb = youmu_mirror_bomb,
		.bomb_shader = youmu_mirror_shader,
		.bombbg = youmu_common_bombbg,
		.shot = youmu_mirror_shot,
		.init = youmu_mirror_init,
		.preload = youmu_mirror_preload,
	},
};
