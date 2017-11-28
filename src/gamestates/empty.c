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
	ALLEGRO_BITMAP *bitmap, *bitmaphug;
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

	int talks;
	int talking;
	int to_talk;
	int hugging;
	int since_hugging;
	int since_being_talked_to;
	int since_talking;
	int boredom_time;

	int offsetx, offsety, offsettulu;

	bool entrance;
	bool quit;

	struct Player* player;

	int burza;
	int burzacount;

	int dymek;
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
	int talking;
	bool hugging;

	double sum;
	int count;
	double avg;
	double sumtemp;
	int zerocount;

	ALLEGRO_BITMAP* screen;

	struct WoolenCharacter *character, *hugee;

	ALLEGRO_SAMPLE* sample;
	ALLEGRO_SAMPLE_INSTANCE* tup;

	ALLEGRO_SAMPLE* tul_sample;
	ALLEGRO_SAMPLE_INSTANCE* tul;
};

struct GamestateResources {
	// This struct is for every resource allocated and used by your gamestate.
	// It gets created on load and then gets passed around to all other function calls.
	//double x1, y1, x2, y2, x3, y3;
	struct Player players[4];
	long counter;
	long game_counter;

	bool started;
	bool finished;

	bool debug;
	bool background;

	int num_players;

	struct WoolenCharacter characters[5];

	ALLEGRO_BITMAP *test, *bg, *tree, *ramka, *dymek, *napis, *heart, *heart2, *dymek1, *dymek2, *dymek3;
	ALLEGRO_BITMAP* dymki[4];
	ALLEGRO_BITMAP *burza[6], *group;

	ALLEGRO_AUDIO_STREAM* music;

	ALLEGRO_SAMPLE* sample;
	ALLEGRO_SAMPLE_INSTANCE* win;
};

int HUGPOS = 1100;
int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

static double HappyMod(double val, double mod, double happiness) {
	return val;
	//	return val + mod * happiness;
}

void Gamestate_Logic(struct Game* game, struct GamestateResources* data) {
	// Called 60 times per second (by default). Here you should do all your game logic.
	data->counter++;
	bool domore = true;

	if (data->started && !data->finished) {
		al_set_audio_stream_playing(data->music, true);
	} else {
		al_set_audio_stream_playing(data->music, false);
	}

	for (int i = 0; i < data->num_players; i++) {
		if (data->players[i].motion) {
			data->players[i].motion--;
			al_play_sample_instance(data->players[i].tup);
		} else {
			al_stop_sample_instance(data->players[i].tup);
		}
	}

	double limit = 0.2 + (data->num_players * 0.1);
	bool end = true;
	for (int i = 0; i < 5; i++) {
		if (data->characters[i].happiness <= limit) {
			end = false;
			break;
		}
	}
	if (end) {
		if (data->started) {
			al_play_sample_instance(data->win);
		}
		data->started = false;
		data->finished = true;
		for (int i = 0; i < 4; i++) {
			al_stop_sample_instance(data->players[i].tup);
			data->players[i].active = false;
		}
		data->num_players = 0;
		for (int i = 0; i < 5; i++) {
			data->characters[i].hugee = false;
			data->characters[i].used = false;
		}
	}

	if (data->started) {
		if (data->game_counter % (60 * 22) == 60 * 20) {
			for (int i = 0; i < 5; i++) {
				data->characters[i].position = 0;
				data->characters[i].quit = true;
				data->characters[i].hugging = false;
			}
			for (int i = 0; i < 4; i++) {
				data->players[i].hugging = false;
			}
		}
		if (data->game_counter % (60 * 22) == 0) {
			for (int i = 0; i < 5; i++) {
				data->characters[i].hugee = false;
			}
			for (int i = 0; i < 5; i++) {
				data->characters[i].entrance = true;
				data->characters[i].position = (6 + i) * 120;
				data->characters[i].quit = false;

				int j = 0;
				int counter = 0;
				do {
					int coun = 0;
					while (rand() % 20 || data->characters[j].hugee) {
						coun++;
						if (coun > 100) {
							break;
						}

						j++;
						if (j >= 5) {
							j -= 5;
						}
					}
					counter++;
					if (counter > 100) {
						break;
					}

				} while ((&data->characters[j] == data->players[i].hugee) || (&data->characters[j] == data->players[i].character));

				data->characters[j].hugee = true;
				data->characters[j].player = &data->players[i];
				data->players[i].hugee = &data->characters[j];
			}
		}
		if (data->game_counter % (60 * 22) == 120) {
			for (int i = 0; i < 5; i++) {
				data->characters[i].position = 0;
				data->characters[i].entrance = false;
			}
		}

		data->game_counter++;
	}

	for (int i = 0; i < 5; i++) {
		if (data->characters[i].entrance) {
			data->characters[i].position -= 6 + i;
			domore = false;
		}
		if (data->characters[i].quit) {
			data->characters[i].position += 6 + i;
			domore = false;
		}
	}

	if (!domore) { return; }

	if (data->started) {
		for (int i = 0; i < data->num_players; i++) {
			if (data->players[i].sumtemp) {
				data->players[i].sum += data->players[i].sumtemp;
				data->players[i].count++;
				data->players[i].avg = data->players[i].sum / (double)data->players[i].count;
				data->players[i].sumtemp = 0;
				data->players[i].zerocount = 0;
			} else {
				data->players[i].zerocount++;
				if (data->players[i].zerocount > 120) {
					data->players[i].sum = 0;
					data->players[i].count = 0;
					data->players[i].avg = 0;
				}
			}

			if (data->players[i].x > HUGPOS + data->players[i].character->offsettulu) {
				data->players[i].hugee->hugging++;
				data->players[i].hugging = true;
			} else {
				data->players[i].hugee->hugging = 0;
				data->players[i].hugging = false;
			}

			if (data->players[i].talking) {
				data->players[i].hugee->since_being_talked_to = 0;
				data->players[i].hugee->boredom_time = 0;
				data->players[i].hugee->talks += 10;
				data->players[i].talking--;
				if ((data->players[i].hugee->to_talk == 0) && (data->players[i].hugee->talkativeness >= 0.5)) {
					data->players[i].hugee->to_talk = (rand() / (double)RAND_MAX) * 60 * 6 + 30;
				}
			} else {
				data->players[i].hugee->talks--;
				if (data->players[i].hugee->talks < 0) {
					data->players[i].hugee->talks = 0;
				}
				data->players[i].hugee->since_being_talked_to += 3;
				if (data->players[i].hugee->since_hugging) {
					data->players[i].hugee->boredom_time++;
				}
			}
		}

		for (int i = 0; i < 5; i++) {
			if (data->characters[i].talking) {
				data->characters[i].since_talking = 0;
				data->characters[i].talking--;
			} else {
				data->characters[i].since_talking += 3;
			}
			if (data->characters[i].hugging) {
				data->characters[i].since_hugging = 0;
				data->characters[i].boredom_time = 0;
			} else {
				data->characters[i].since_hugging += 3;
			}

			if (data->characters[i].to_talk) {
				data->characters[i].to_talk--;
				if (data->characters[i].to_talk == 0) {
					data->characters[i].talking = 60 * 2;
					data->characters[i].dymek = rand() % 4;
				}
			}

			double talkativeness = HappyMod(data->characters[i].talkativeness, data->characters[i].talkativeness_change, data->characters[i].happiness);
			double huginness = HappyMod(data->characters[i].huginness, data->characters[i].huginness_change, data->characters[i].happiness);
			double sensitiveness = HappyMod(data->characters[i].sensitiveness, data->characters[i].sensitiveness_change, data->characters[i].happiness);
			double boringness = HappyMod(data->characters[i].boringness, data->characters[i].boringness_change, data->characters[i].happiness);

			double happiness = 0;
			if (talkativeness >= 0.5) {
				happiness -= data->characters[i].since_being_talked_to * talkativeness / 60.0 / 50.0;
				happiness += data->characters[i].talks * talkativeness / 60.0 / 400.0;
				if (data->characters[i].to_talk == 0) {
					data->characters[i].to_talk = (rand() / (double)RAND_MAX) * 60 * 6 + 30;
				}
			}
			if (talkativeness < 0.5) {
				happiness -= data->characters[i].talks * (1 - talkativeness) / 60.0 / 200.0;
				happiness += data->characters[i].since_being_talked_to * (1 - talkativeness) / 60.0 / 500.0;
			}

			if (huginness >= 0.5) {
				happiness -= data->characters[i].since_hugging * huginness / 60.0 / 100.0;
				if (data->characters[i].hugging) {
					happiness += data->characters[i].hugging * huginness / 60.0 / 30.0;
				}
			}
			if (huginness < 0.5) {
				happiness += data->characters[i].since_hugging * (1 - huginness) * 1.5 / 60.0 / 100.0;
				if (data->characters[i].hugging) {
					happiness -= data->characters[i].hugging * (1 - huginness) / 60.0 / 10.0;
				}
			}

			if (data->characters[i].player) {
				if (sensitiveness >= 0.5) {
					happiness -= data->characters[i].player->avg * sensitiveness / 60.0 / 5000.0;
				}
				if (sensitiveness < 0.5) {
					happiness += (1 / data->characters[i].player->avg ? data->characters[i].player->avg : 1) * 20 * (1 - sensitiveness) / 60.0 / 5000.0;
				}
			}

			happiness -= boringness * pow(data->characters[i].boredom_time, 1.25) * 20 / 60.0 / 400000.0;

			if (happiness < 0) {
				//				happiness *= 0.75;
				if (!data->characters[i].player) {
					happiness *= 0.5;
				}
			}

			data->characters[i].happiness += (happiness * 0.02) * data->characters[i].modificator;

			if (data->characters[i].happiness > 1) {
				data->characters[i].happiness = 1;
			}
			if (data->characters[i].happiness < -1) {
				data->characters[i].happiness = -1;
			}
		}
	}
}
void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.

	//ALLEGRO_TRANSFORM orig_transform = *al_get_current_transform();

	for (int i = data->num_players - 1; i >= 0; i--) {
		al_set_target_bitmap(data->players[i].screen);
		al_clear_to_color(al_map_rgb(255, 255, 255));

		ALLEGRO_TRANSFORM t;
		al_identity_transform(&t);
		al_use_transform(&t);

		if (data->background) {
			al_draw_bitmap(data->bg, 0, 0, 0);
		}

		if (data->started) {
			al_draw_tinted_bitmap(data->heart, al_map_rgba(222, 222, 222, 222), 1400 + data->players[i].hugee->position, 260 + sin((data->counter + i * 60) / 24.0) * 20, 0);

			double happ = (data->players[i].hugee->happiness + 1) / 2.0;

			al_draw_bitmap_region(data->heart2, 0, (1 - happ / 1.05) * al_get_bitmap_height(data->heart2),
			  al_get_bitmap_width(data->heart2), happ / 1.05 * al_get_bitmap_height(data->heart2),
			  1400 + data->players[i].hugee->position, 260 + sin((data->counter + i * 60) / 24.0) * 20 + (1 - happ / 1.05) * al_get_bitmap_height(data->heart2), 0);

			al_identity_transform(&t);
			al_translate_transform(&t, -al_get_bitmap_width(data->players[i].hugee->bitmap) / 2, -al_get_bitmap_height(data->players[i].hugee->bitmap) * 0.75);
			//		if (data->players[i].motion) {
			al_rotate_transform(&t, sin(floor((1300 + data->players[i].hugee->position) / 100)) * ALLEGRO_PI / 20.0);
			//}
			al_translate_transform(&t, al_get_bitmap_width(data->players[i].hugee->bitmap) / 2, al_get_bitmap_height(data->players[i].hugee->bitmap) * 0.75);
			al_translate_transform(&t, 1300 + data->players[i].hugee->position, 600); //data->players[i].y);
			//		al_compose_transform(&t, &orig_transform);
			al_use_transform(&t);

			if (data->players[i].hugee->talking) {
				al_draw_bitmap(data->dymki[data->players[i].hugee->dymek], 0 + data->players[i].hugee->offsetx, -200 + data->players[i].hugee->offsety, ALLEGRO_FLIP_HORIZONTAL);
			}
			al_draw_tinted_bitmap(data->players[i].hugging ? data->players[i].hugee->bitmaphug : data->players[i].hugee->bitmap, al_map_rgb(255, 255, 255), data->players[i].hugee->offsetx, data->players[i].hugee->offsety + (data->players[i].hugging ? -20 : 0), 0);
		}
		al_identity_transform(&t);
		al_use_transform(&t);

		al_translate_transform(&t, -al_get_bitmap_width(data->players[i].character->bitmap) / 2, -al_get_bitmap_height(data->players[i].character->bitmap) * 0.75);
		if (data->players[i].motion) {
			al_rotate_transform(&t, sin(floor(data->players[i].x / 50)) * ALLEGRO_PI / 20.0);
		}
		al_translate_transform(&t, al_get_bitmap_width(data->players[i].character->bitmap) / 2, al_get_bitmap_height(data->players[i].character->bitmap) * 0.75);
		al_translate_transform(&t, data->players[i].x, 600); //data->players[i].y);
		//		al_compose_transform(&t, &orig_transform);
		al_use_transform(&t);
		if (data->players[i].talking) {
			al_draw_bitmap(data->dymki[data->players[i].character->dymek], 320 + data->players[i].character->offsetx, -190 + data->players[i].character->offsety, 0);
		}
		al_draw_tinted_bitmap(data->players[i].hugging ? data->players[i].character->bitmaphug : data->players[i].character->bitmap, data->players[i].color, data->players[i].character->offsetx, data->players[i].character->offsety + (data->players[i].hugging ? -20 : 0), ALLEGRO_FLIP_HORIZONTAL);

		if (data->players[i].character->happiness >= 1.0) {
			al_draw_scaled_rotated_bitmap(data->heart2, al_get_bitmap_width(data->heart2), al_get_bitmap_height(data->heart2),
			  data->players[i].character->offsetx, data->players[i].character->offsety + (data->players[i].hugging ? -20 : 0),
			  (sin((data->counter + i * 5) / 100.0) / 20.0 + 1) * 0.2, (sin((data->counter + i * 5) / 100.0) / 20.0 + 1) * 0.2, cos((data->game_counter + i * 2) / 6.9) * 0.02, 0);
		}
		//al_draw_filled_rectangle(0, 0, 25, 25, data->players[i].color);

		al_identity_transform(&t);
		al_use_transform(&t);

		if (data->debug) {
			al_draw_filled_rectangle(9, 9, 311, 111, al_map_rgb(0, 0, 0));
			al_draw_filled_rectangle(10, 10, 10 + 300 * data->players[i].hugee->talkativeness, 30, al_map_rgb(255, 0, 0));
			al_draw_filled_rectangle(10, 30, 10 + 300 * data->players[i].hugee->huginness, 50, al_map_rgb(0, 255, 0));
			al_draw_filled_rectangle(10, 50, 10 + 300 * data->players[i].hugee->sensitiveness, 70, al_map_rgb(0, 0, 255));
			al_draw_filled_rectangle(10, 70, 10 + 300 * data->players[i].hugee->boringness, 90, al_map_rgb(255, 0, 255));
			al_draw_filled_rectangle(10, 90, 10 + 300 * ((data->players[i].hugee->happiness / 2.0) + 0.5), 110, al_map_rgb(0, 255, 255));

			double tt = HappyMod(data->players[i].hugee->talkativeness, data->players[i].hugee->talkativeness_change, data->characters[i].happiness);
			double h = HappyMod(data->players[i].hugee->huginness, data->players[i].hugee->huginness_change, data->characters[i].happiness);
			double s = HappyMod(data->players[i].hugee->sensitiveness, data->players[i].hugee->sensitiveness_change, data->characters[i].happiness);
			double b = HappyMod(data->players[i].hugee->boringness, data->players[i].hugee->boringness_change, data->characters[i].happiness);
			al_draw_filled_rectangle(10, 10, 10 + 300 * tt, 30, al_map_rgba(128, 128, 128, 128));
			al_draw_filled_rectangle(10, 30, 10 + 300 * h, 50, al_map_rgba(128, 128, 128, 128));
			al_draw_filled_rectangle(10, 50, 10 + 300 * s, 70, al_map_rgba(128, 128, 128, 128));
			al_draw_filled_rectangle(10, 70, 10 + 300 * b, 90, al_map_rgba(128, 128, 128, 128));
			//al_draw_filled_rectangle(10, 90, 10 + 300 * ((data->players[i].hugee->happiness / 2.0) + 0.5), 110, al_map_rgba(0, 128, 128, 128));
		}

		if (data->background) {
			al_draw_bitmap(data->tree, 0, 0, 0);
		}
		al_draw_bitmap(data->ramka, 0, 0, 0);
	}

	al_set_target_backbuffer(game->display);
	al_clear_to_color(al_map_rgb(0, 0, 0));
	if (data->num_players == 0) {
		if (data->background) {
			al_draw_bitmap(data->bg, 0, 0, 0);
		}
		double s = sin(data->counter / 25.0) * 0.1 + 0.9;
		if (!data->finished) {
			al_draw_tinted_bitmap(data->napis, al_map_rgba_f(s, s, s, s), 0, s * 40, 0);
		}
		if (data->background) {
			al_draw_bitmap(data->tree, 0, 0, 0);
		}

		if (data->finished) {
			al_draw_rotated_bitmap(data->group, 1920 * 0.666, 1080 / 2, 1920 * 0.666, 1080 / 2, sin(data->counter / 100.0) * 0.06, 0);
		}
		al_draw_bitmap(data->ramka, 0, 0, 0);
	}
	if (data->num_players == 1) {
		al_draw_bitmap(data->players[0].screen, 0, 0, 0);
	}
	if (data->num_players == 2) {
		al_draw_scaled_bitmap(data->players[0].screen, 0, 0, 1920, 1080, 0, 1080 / 4, 1920 / 2, 1080 / 2, 0);
		al_draw_scaled_bitmap(data->players[1].screen, 0, 0, 1920, 1080, 1920 / 2, 1080 / 4, 1920 / 2, 1080 / 2, 0);
	}
	if (data->num_players > 2) {
		al_draw_scaled_bitmap(data->players[0].screen, 0, 0, 1920, 1080, 0, 0, 1920 / 2, 1080 / 2, 0);
		al_draw_scaled_bitmap(data->players[1].screen, 0, 0, 1920, 1080, 1920 / 2, 0, 1920 / 2, 1080 / 2, 0);
		if (data->num_players == 4) {
			al_draw_scaled_bitmap(data->players[2].screen, 0, 0, 1920, 1080, 0, 1080 / 2, 1920 / 2, 1080 / 2, 0);
			al_draw_scaled_bitmap(data->players[3].screen, 0, 0, 1920, 1080, 1920 / 2, 1080 / 2, 1920 / 2, 1080 / 2, 0);
		} else {
			al_draw_scaled_bitmap(data->players[2].screen, 0, 0, 1920, 1080, 1920 / 4, 1080 / 2, 1920 / 2, 1080 / 2, 0);
		}
	}
	//	al_use_transform(&orig_transform);
	/*	al_draw_filled_rectangle(data->x1, data->y1, data->x1 + 10, data->y1 + 10, al_map_rgb(255, 0, 0));
	al_draw_filled_rectangle(data->x2, data->y2, data->x2 + 10, data->y2 + 10, al_map_rgb(0, 255, 0));
	al_draw_filled_rectangle(data->x3, data->y3, data->x3 + 10, data->y3 + 10, al_map_rgb(0, 0, 255));*/
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
		if (!data->started) {
			data->started = true;
			data->characters[0].position = 6 * 120;
			data->characters[1].position = 7 * 120;
			data->characters[2].position = 8 * 120;
			data->characters[3].position = 9 * 120;
			data->characters[4].position = 10 * 120;
			data->game_counter = 0;

			for (int j = 0; j < 5; j++) {
				data->characters[j].happiness = 0.25;
				data->characters[j].boredom_time = 0;
				data->characters[j].since_being_talked_to = 0;
				data->characters[j].since_hugging = 0;
				data->characters[j].since_talking = 0;
				data->characters[j].talking = 0;
			}
		}
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_2)) {
		data->players[0].sensitivity -= sensitivity;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_3)) {
		data->players[0].sensitivity += sensitivity;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_4)) {
		data->players[1].sensitivity -= sensitivity;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_5)) {
		data->players[1].sensitivity += sensitivity;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_6)) {
		data->players[2].sensitivity -= sensitivity;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_7)) {
		data->players[2].sensitivity += sensitivity;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_8)) {
		data->players[3].sensitivity -= sensitivity;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_9)) {
		data->players[3].sensitivity += sensitivity;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_D)) {
		data->debug = !data->debug;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_B)) {
		data->background = !data->background;
	}

	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_W)) {
		data->started = false;
		data->finished = true;
		al_play_sample_instance(data->win);
		for (int i = 0; i < 4; i++) {
			data->players[i].active = false;
			al_stop_sample_instance(data->players[i].tup);
		}
		data->num_players = 0;
		for (int i = 0; i < 5; i++) {
			data->characters[i].hugee = false;
			data->characters[i].used = false;
		}
	}

	if (ev->type == MOT_EVENT_MOUSE_MOVE) {
		struct MoTMouseMoveEventData* d = (struct MoTMouseMoveEventData*)ev->user.data2;
		PrintConsole(game, "move x=%f, y=%f, id %d", d->x, d->y, ev->user.data1);
		for (int i = 0; i < data->num_players; i++) {
			if (data->players[i].active && data->players[i].mouse_id == ev->user.data1) {
				data->players[i].x += d->x * data->players[i].sensitivity * 0.25;
				data->players[i].y += d->y * data->players[i].sensitivity * 0.25;
				data->players[i].right = d->x > 0;
				data->players[i].motion = 10;
				data->players[i].sumtemp += fabs(data->players[i].x) / 100.0;

				if (data->players[i].x < 300 + data->players[i].character->offsettulu) {
					data->players[i].x = 300 + data->players[i].character->offsettulu;
				}
				if (data->players[i].x > HUGPOS + data->players[i].character->offsettulu + 10) {
					data->players[i].x = HUGPOS + data->players[i].character->offsettulu + 10;
				}
				break;
			}
		}
	}
	if (ev->type == MOT_EVENT_BUTTON_PRESS) {
		PrintConsole(game, "press button %d on id %d", ev->user.data2, ev->user.data1);
		if (ev->user.data2 != 1) {
			return;
		}
		bool found = false;
		int foundid = -1;
		for (int i = 0; i < data->num_players; i++) {
			if (data->players[i].active && data->players[i].mouse_id == ev->user.data1) {
				found = true;
				foundid = i;
				break;
			}
		}
		if (!found && data->num_players < 4 && !data->started) {
			data->players[data->num_players].active = true;
			data->players[data->num_players].x = 1920 / 2 - 200;
			//			data->players[data->num_players].y = 1080 / 2;
			data->players[data->num_players].mouse_id = ev->user.data1;
			data->players[data->num_players].sensitivity = 1.f;
			data->players[data->num_players].color = al_map_rgb(255, 255, 255); //al_map_rgb_f(rand() / (double)RAND_MAX, rand() / (double)RAND_MAX, rand() / (double)RAND_MAX);

			int j = 0;
			int counter = 0;
			while (rand() % 20 || data->characters[j].used) {
				j++;
				if (j >= 5) {
					j = 0;
				}
				counter++;
				if (counter > 100) {
					break;
				}
			}
			data->players[data->num_players].character = &data->characters[j];
			data->characters[j].player = &data->players[data->num_players];
			data->characters[j].used = true;
			PrintConsole(game, "PLAYER %d got CHARACTER %d", data->num_players, j);

			j = data->num_players;
			int coun = 0;
			do {
				int counter = 0;
				while (rand() % 20 || data->characters[j].hugee) {
					j++;
					if (j >= 5) {
						j -= 5;
					}
					counter++;
					if (counter > 100) {
						break;
					}
				}
				coun++;
				if (coun > 100) {
					break;
				}

			} while (&data->characters[j] == data->players[data->num_players].character);

			data->characters[j].hugee = true;
			data->characters[j].player = &data->players[data->num_players];
			data->players[data->num_players].hugee = &data->characters[j];

			data->num_players++;
		}
		if (found) {
			if (!data->started) { return; }
			PrintConsole(game, "found %d", foundid);
			if (data->players[foundid].talking == 0) {
				data->players[foundid].talking = 60 * 2;
				data->players[foundid].character->dymek = rand() % 4;
			}
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
	data->ramka = al_load_bitmap(GetDataFilePath(game, "ramka.png"));
	data->dymki[0] = al_load_bitmap(GetDataFilePath(game, "dymek4.png"));
	data->dymki[1] = al_load_bitmap(GetDataFilePath(game, "dymek3.png"));
	data->dymki[2] = al_load_bitmap(GetDataFilePath(game, "dymek2.png"));
	data->dymki[3] = al_load_bitmap(GetDataFilePath(game, "dymek1.png"));
	data->napis = al_load_bitmap(GetDataFilePath(game, "napis.png"));
	data->heart = al_load_bitmap(GetDataFilePath(game, "serce_puste.png"));
	data->heart2 = al_load_bitmap(GetDataFilePath(game, "serce_pelne.png"));

	data->burza[0] = al_load_bitmap(GetDataFilePath(game, "burza1.png"));
	data->burza[1] = al_load_bitmap(GetDataFilePath(game, "burza2.png"));
	data->burza[2] = al_load_bitmap(GetDataFilePath(game, "burza3.png"));
	data->burza[3] = al_load_bitmap(GetDataFilePath(game, "burza4.png"));
	data->burza[4] = al_load_bitmap(GetDataFilePath(game, "burza5.png"));
	data->burza[5] = al_load_bitmap(GetDataFilePath(game, "burza6.png"));
	data->group = al_load_bitmap(GetDataFilePath(game, "grouphug1.png"));

	data->characters[0].bitmap = al_load_bitmap(GetDataFilePath(game, "octopus.png"));
	data->characters[1].bitmap = al_load_bitmap(GetDataFilePath(game, "kret2a.png"));
	data->characters[2].bitmap = al_load_bitmap(GetDataFilePath(game, "myszka1.png"));
	data->characters[3].bitmap = al_load_bitmap(GetDataFilePath(game, "swinka1.png"));
	data->characters[4].bitmap = al_load_bitmap(GetDataFilePath(game, "zabka.png"));

	data->characters[0].bitmaphug = al_load_bitmap(GetDataFilePath(game, "octopus2.png"));
	data->characters[1].bitmaphug = al_load_bitmap(GetDataFilePath(game, "kret1a.png"));
	data->characters[2].bitmaphug = al_load_bitmap(GetDataFilePath(game, "myszka2.png"));
	data->characters[3].bitmaphug = al_load_bitmap(GetDataFilePath(game, "swinka2.png"));
	data->characters[4].bitmaphug = al_load_bitmap(GetDataFilePath(game, "zabka2.png"));

	data->characters[0].offsetx = 0;
	data->characters[0].offsety = 0;
	data->characters[1].offsetx = 130;
	data->characters[1].offsety = 90;
	data->characters[2].offsetx = 173;
	data->characters[2].offsety = 104;
	data->characters[3].offsetx = 112;
	data->characters[3].offsety = 48;
	data->characters[4].offsetx = 164;
	data->characters[4].offsety = 23;

	data->characters[0].entrance = true;
	data->characters[1].entrance = true;
	data->characters[2].entrance = true;
	data->characters[3].entrance = true;
	data->characters[4].entrance = true;

	data->characters[0].offsettulu = 80;
	data->characters[1].offsettulu = -30;
	data->characters[2].offsettulu = -180;
	data->characters[3].offsettulu = -40;
	data->characters[4].offsettulu = 0;

	data->players[0].screen = al_create_bitmap(1920, 1080);
	data->players[1].screen = al_create_bitmap(1920, 1080);
	data->players[2].screen = al_create_bitmap(1920, 1080);
	data->players[3].screen = al_create_bitmap(1920, 1080);

	data->sample = al_load_sample(GetDataFilePath(game, "winner.flac"));
	data->win = al_create_sample_instance(data->sample);
	al_attach_sample_instance_to_mixer(data->win, game->audio.fx);
	al_set_sample_instance_playmode(data->win, ALLEGRO_PLAYMODE_ONCE);

	data->music = al_load_audio_stream(GetDataFilePath(game, "music.flac"), 4, 1024);
	al_set_audio_stream_playing(data->music, false);
	al_set_audio_stream_playmode(data->music, ALLEGRO_PLAYMODE_LOOP);
	//al_set_audio_stream_gain(data->music, 1.5);
	al_attach_audio_stream_to_mixer(data->music, game->audio.music);

	data->players[0].sample = al_load_sample(GetDataFilePath(game, "tup.flac"));
	data->players[0].tup = al_create_sample_instance(data->players[0].sample);
	al_attach_sample_instance_to_mixer(data->players[0].tup, game->audio.fx);
	al_set_sample_instance_playmode(data->players[0].tup, ALLEGRO_PLAYMODE_LOOP);

	data->players[1].sample = al_load_sample(GetDataFilePath(game, "tup2.flac"));
	data->players[1].tup = al_create_sample_instance(data->players[1].sample);
	al_attach_sample_instance_to_mixer(data->players[1].tup, game->audio.fx);
	al_set_sample_instance_playmode(data->players[1].tup, ALLEGRO_PLAYMODE_LOOP);

	data->players[2].sample = al_load_sample(GetDataFilePath(game, "tup3.flac"));
	data->players[2].tup = al_create_sample_instance(data->players[2].sample);
	al_attach_sample_instance_to_mixer(data->players[2].tup, game->audio.fx);
	al_set_sample_instance_playmode(data->players[2].tup, ALLEGRO_PLAYMODE_LOOP);

	data->players[3].sample = al_load_sample(GetDataFilePath(game, "tup4.flac"));
	data->players[3].tup = al_create_sample_instance(data->players[3].sample);
	al_attach_sample_instance_to_mixer(data->players[3].tup, game->audio.fx);
	al_set_sample_instance_playmode(data->players[3].tup, ALLEGRO_PLAYMODE_LOOP);

	progress(game); // report that we progressed with the loading, so the engine can move a progress bar
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_audio_stream(data->music);
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
	al_set_mixer_gain(game->audio.mixer, 1.75);
	for (int j = 0; j < 5; j++) {
		data->characters[j].talkativeness = rand() / (double)RAND_MAX;
		data->characters[j].huginness = rand() / (double)RAND_MAX;
		data->characters[j].sensitiveness = rand() / (double)RAND_MAX;
		data->characters[j].boringness = rand() / (double)RAND_MAX;
		data->characters[j].talkativeness_change = (rand() / (double)RAND_MAX * 2 - 1) * 0.05;
		data->characters[j].huginness_change = (rand() / (double)RAND_MAX * 2 - 1) * 0.05;
		data->characters[j].sensitiveness_change = (rand() / (double)RAND_MAX * 2 - 1) * 0.05;
		data->characters[j].boringness_change = (rand() / (double)RAND_MAX * 2 - 1) * 0.05;
		data->characters[j].happiness = 0;
		data->characters[j].modificator = 1;
	}

	data->num_players = 0;

	data->debug = false;
	data->background = true;
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
