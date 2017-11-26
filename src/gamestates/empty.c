/*! \file empty.c
 *  \brief Empty gamestate.
 */
/*
 * Copyright (c) Sebastian Krzyszkowiak <dos@dosowisko.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../common.h"
#include <libsuperderpy.h>
#include <math.h>

struct Player;

struct WoolenCharacter {
	ALLEGRO_BITMAP* bitmap;
	double talkativeness; // 0 1
	double talkativeness_change; // -1 1
	double huginness; // 0 1
	double huginness_change; // -1 1
	double sensitiveness; // 0 1
	double sensitiveness_change; // -1 1
	double boringness; // 0 1
	double boringness_change; // -1 1

	double happiness; // -1 1
	double modificator; // 0.75 1.25
	long score;

	double position; // 0 1

	bool used, hugee;

	int talking;
	int to_talk;
	int hugging;
	int since_hugging;
	int since_being_talked_to;
	int since_talking;

	struct Player* player;
};

struct Player {
	bool active;
	double x, y;
	ALLEGRO_COLOR color;
	int mouse_id;
	double sensitivity;
	int motion;
	bool right;
	double position;

	struct WoolenCharacter *character, *hugee;
};

struct GamestateResources {
	// This struct is for every resource allocated and used by your gamestate.
	// It gets created on load and then gets passed around to all other function calls.
	//double x1, y1, x2, y2, x3, y3;
	struct Player players[4];

	int num_players;

	struct WoolenCharacter characters[5];

	ALLEGRO_BITMAP *test, *bg, *tree;
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

static double HappyMod(double val, double mod, double happiness) {
	return val + mod * happiness;
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data) {
	// Called 60 times per second (by default). Here you should do all your game logic.
	for (int i = 0; i < data->num_players; i++) {
		if (data->players[i].motion) {
			data->players[i].motion--;
		}
	}

	for (int i = 0; i < 5; i++) {
		if (data->characters[i].talking) {
			data->characters[i].since_talking = 0;
		}
		if (data->characters[i].hugging) {
			data->characters[i].since_hugging = 0;
		}

		double talkativeness = HappyMod(data->characters[i].talkativeness, data->characters[i].talkativeness_change, data->characters[i].happiness);
		double huginness = HappyMod(data->characters[i].huginness, data->characters[i].huginness_change, data->characters[i].happiness);
		double sensitiveness = HappyMod(data->characters[i].sensitiveness, data->characters[i].sensitiveness_change, data->characters[i].happiness);
		double boringness = HappyMod(data->characters[i].boringness, data->characters[i].boringness_change, data->characters[i].happiness);

		double happiness = 0;
		if (talkativeness >= 0.5) {
			happiness -= data->characters[i].since_being_talked_to * talkativeness / 60.0 / 100.0;
		}
		if (talkativeness < 0.5) {
			happiness += data->characters[i].since_being_talked_to * (1 - talkativeness) / 60.0 / 100.0;
		}

		if (huginness >= 0.5) {
			happiness -= data->characters[i].since_hugging * huginness / 60.0 / 100.0;
			if (data->characters[i].hugging) {
				happiness += data->characters[i].hugging * huginness / 60.0 / 50.0;
			}
		}
		if (huginness < 0.5) {
			happiness += data->characters[i].since_hugging * (1 - huginness) / 60.0 / 100.0;
		}

		if (sensitiveness >= 0.5) {
			happiness -= data->characters[i].hugging * sensitiveness / 60.0 / 200.0;
		}
		if (sensitiveness < 0.5) {
			happiness += data->characters[i].hugging * (1 - sensitiveness) / 60.0 / 200.0;
		}

		happiness -= boringness * (data->characters[i].since_hugging + data->characters[i].since_being_talked_to) / 60.0 / 300.0;

		data->characters[i].happiness += happiness * data->characters[i].modificator;

		if (data->characters[i].happiness > 1) {
			data->characters[i].happiness = 1;
		}
		if (data->characters[i].happiness < -1) {
			data->characters[i].happiness = -1;
		}
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.

	al_draw_bitmap(data->bg, 0, 0, 0);

	ALLEGRO_TRANSFORM orig_transform = *al_get_current_transform();
	for (int i = 0; i < data->num_players; i++) {
		ALLEGRO_TRANSFORM t;
		al_identity_transform(&t);
		al_translate_transform(&t, -240 / 2, -255 / 2 - 60);
		if (data->players[i].motion) {
			al_rotate_transform(&t, sin(floor(data->players[i].x / 100)) * ALLEGRO_PI / 20.0);
		}
		al_translate_transform(&t, 240 / 2, 255 / 2 - 60);
		al_translate_transform(&t, data->players[i].x, 800); //data->players[i].y);
		al_compose_transform(&t, &orig_transform);
		al_use_transform(&t);
		al_draw_tinted_bitmap(data->test, data->players[i].color, 0, 0, data->players[i].right ? ALLEGRO_FLIP_HORIZONTAL : 0);
		//al_draw_filled_rectangle(0, 0, 25, 25, data->players[i].color);
		al_use_transform(&orig_transform);
	}
	/*	al_draw_filled_rectangle(data->x1, data->y1, data->x1 + 10, data->y1 + 10, al_map_rgb(255, 0, 0));
	al_draw_filled_rectangle(data->x2, data->y2, data->x2 + 10, data->y2 + 10, al_map_rgb(0, 255, 0));
	al_draw_filled_rectangle(data->x3, data->y3, data->x3 + 10, data->y3 + 10, al_map_rgb(0, 0, 255));*/

	al_draw_bitmap(data->tree, 0, 0, 0);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}

	double sensitivity = 0.1;
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_1)) {
		data->players[0].sensitivity -= sensitivity;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_2)) {
		data->players[0].sensitivity += sensitivity;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_3)) {
		data->players[1].sensitivity -= sensitivity;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_4)) {
		data->players[1].sensitivity += sensitivity;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_5)) {
		data->players[2].sensitivity -= sensitivity;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_6)) {
		data->players[2].sensitivity += sensitivity;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_7)) {
		data->players[3].sensitivity -= sensitivity;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_8)) {
		data->players[3].sensitivity += sensitivity;
	}

	if (ev->type == MOT_EVENT_MOUSE_MOVE) {
		struct MoTMouseMoveEventData* d = (struct MoTMouseMoveEventData*)ev->user.data2;
		PrintConsole(game, "move x=%f, y=%f, id %d", d->x, d->y, ev->user.data1);
		for (int i = 0; i < data->num_players; i++) {
			if (data->players[i].active && data->players[i].mouse_id == ev->user.data1) {
				data->players[i].x += d->x * data->players[i].sensitivity * 0.25;
				data->players[i].y += d->y * data->players[i].sensitivity * 0.25;
				data->players[i].right = d->x > 0;
				data->players[i].motion = 5;
				break;
			}
		}
	}
	if (ev->type == MOT_EVENT_BUTTON_PRESS) {
		PrintConsole(game, "press button %d on id %d", ev->user.data2, ev->user.data1);
		bool found = false;
		for (int i = 0; i < data->num_players; i++) {
			if (data->players[i].active && data->players[i].mouse_id == ev->user.data1) {
				found = true;
				break;
			}
		}
		if (!found && data->num_players < 4) {
			data->players[data->num_players].active = true;
			data->players[data->num_players].x = 1920 / 2;
			data->players[data->num_players].y = 1080 / 2;
			data->players[data->num_players].mouse_id = ev->user.data1;
			data->players[data->num_players].sensitivity = 1.f;
			data->players[data->num_players].color = al_map_rgb_f(rand() / (double)RAND_MAX, rand() / (double)RAND_MAX, rand() / (double)RAND_MAX);

			int j = 0;
			while (rand() % 20 || data->characters[j].used) {
				j++;
				if (j >= 5) {
					j = 0;
				}
			}
			data->players[data->num_players].character = &data->characters[j];
			data->characters[j].player = &data->players[data->num_players];
			data->characters[j].used = true;
			PrintConsole(game, "PLAYER %d got CHARACTER %d", data->num_players, j);
			data->num_players++;
		}
	}
	if (ev->type == MOT_EVENT_BUTTON_RELEASE) {
		PrintConsole(game, "release button %d on id %d", ev->user.data2, ev->user.data1);
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	//
	// NOTE: Depending on engine configuration, this may be called from a separate thread.
	// Unless you're sure what you're doing, avoid using drawing calls and other things that
	// require main OpenGL context.

	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources));

	data->test = al_load_bitmap(GetDataFilePath(game, "zabka.png"));
	data->bg = al_load_bitmap(GetDataFilePath(game, "tlo.png"));
	data->tree = al_load_bitmap(GetDataFilePath(game, "drzewo.png"));

	progress(game); // report that we progressed with the loading, so the engine can move a progress bar
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	/*	data->x1 = 1920 / 2;
	data->y1 = 1080 / 2;
	data->x2 = data->x1;
	data->y2 = data->y1;
	data->x3 = data->x1;
	data->y3 = data->y1;
*/
	data->num_players = 0;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
}

void Gamestate_Pause(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets paused (so only Draw is being called, no Logic nor ProcessEvent)
	// Pause your timers and/or sounds here.
}

void Gamestate_Resume(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets resumed. Resume your timers and/or sounds here.
}

void Gamestate_Reload(struct Game* game, struct GamestateResources* data) {
	// Called when the display gets lost and not preserved bitmaps need to be recreated.
	// Unless you want to support mobile platforms, you should be able to ignore it.
}
