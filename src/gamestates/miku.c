/*! \file empty.c
 *  \brief Empty gamestate.
 */
/*
 * Copyright (c) Sebastian Krzyszkowiak <dos@dosowisko.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "../common.h"
#include <libsuperderpy.h>

#define NEXT_GAMESTATE "empty"
#define SKIP_GAMESTATE NEXT_GAMESTATE

struct GamestateResources {
	ALLEGRO_BITMAP* bmp;
	int counter;
	ALLEGRO_AUDIO_STREAM* monkeys;
};

int Gamestate_ProgressCount = 1;

void Gamestate_Logic(struct Game* game, struct GamestateResources* data) {
	data->counter++;
	if (data->counter > 60 * 3) {
		SwitchCurrentGamestate(game, NEXT_GAMESTATE);
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	al_clear_to_color(al_map_rgb(255, 255, 255));
	al_draw_scaled_bitmap(data->bmp, 0, 0, al_get_bitmap_width(data->bmp), al_get_bitmap_height(data->bmp), 0, 0, game->viewport.width, game->viewport.height, 0);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	if (((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) || (ev->type == ALLEGRO_EVENT_TOUCH_END)) {
		SwitchCurrentGamestate(game, SKIP_GAMESTATE);
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	struct GamestateResources* data = malloc(sizeof(struct GamestateResources));
	data->bmp = al_load_bitmap(GetDataFilePath(game, "miku.png"));
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	al_destroy_bitmap(data->bmp);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	data->counter = 0;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {}

void Gamestate_Pause(struct Game* game, struct GamestateResources* data) {}

void Gamestate_Resume(struct Game* game, struct GamestateResources* data) {}

void Gamestate_Reload(struct Game* game, struct GamestateResources* data) {}