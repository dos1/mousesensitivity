/*! \file common.c
 *  \brief Common stuff that can be used by all gamestates.
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

#include "common.h"
#include <allegro5/allegro_x.h>
#include <libsuperderpy.h>

static void print_rawevent(struct Game* game, XIRawEvent* event) {
	/*	int i;
	double* val;
	//	double *raw_val;
	val = event->valuators.values;
	//raw_val = event->raw_values;
	for (i = 0; i < event->valuators.mask_len * 8; i++) {
		if (XIMaskIsSet(event->valuators.mask, i)) {
			double raw = *val++;
			if (event->sourceid == 13) {
				if (i == 0) {
					game->data->x1 += raw;
				}
				if (i == 1) {
					game->data->y1 += raw;
				}
			}
			if (event->sourceid == 14) {
				if (i == 0) {
					game->data->x2 += raw;
				}
				if (i == 1) {
					game->data->y2 += raw;
				}
			}
			if (event->sourceid == 17) {
				if (i == 0) {
					game->data->x3 += raw;
				}
				if (i == 1) {
					game->data->y3 += raw;
				}
			}
		}
	}*/
}

static void MoTMouseMoveEventDestructor(ALLEGRO_USER_EVENT* ev) {
	free(ev->data2);
}

bool GlobalEventHandler(struct Game* game, ALLEGRO_EVENT* ev) {
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_F)) {
		game->config.fullscreen = !game->config.fullscreen;
		if (game->config.fullscreen) {
			SetConfigOption(game, "SuperDerpy", "fullscreen", "1");
			al_hide_mouse_cursor(game->display);
		} else {
			SetConfigOption(game, "SuperDerpy", "fullscreen", "0");
			al_show_mouse_cursor(game->display);
		}
		al_set_display_flag(game->display, ALLEGRO_FULLSCREEN_WINDOW, game->config.fullscreen);
		al_set_display_flag(game->display, ALLEGRO_FRAMELESS, game->config.fullscreen);
		SetupViewport(game, game->viewport_config);
		PrintConsole(game, "Fullscreen toggled");
	}
	{
		while (XPending(game->data->display)) {
			//PrintConsole(game, "pending");

			XEvent e;
			XGenericEventCookie* cookie = &e.xcookie;
			XNextEvent(game->data->display, &e);

			int d = XGetEventData(game->data->display, cookie);
			//PrintConsole(game, "%d %d %d %d", cookie->type, cookie->extension, game->data->xi_opcode, cookie->evtype);
			if (d &&
			  cookie->type == GenericEvent &&
			  cookie->extension == game->data->xi_opcode) {
				//printf("EVENT type %d (%s)\n", cookie->evtype, type_to_name(cookie->evtype));
				switch (cookie->evtype) {
					case XI_RawButtonPress: {
						XIRawEvent* event = cookie->data;
						//						PrintConsole(game, "press button %d on id %d", event->detail, event->sourceid);
						ALLEGRO_EVENT ev;
						ev.user.type = MOT_EVENT_BUTTON_PRESS;
						ev.user.data1 = event->sourceid;
						ev.user.data2 = event->detail;
						al_emit_user_event(&(game->event_source), &ev, NULL);
						break;
					}
					case XI_RawButtonRelease: {
						XIRawEvent* event = cookie->data;
						//						PrintConsole(game, "release button %d on id %d", event->detail, event->sourceid);
						ALLEGRO_EVENT ev;
						ev.user.type = MOT_EVENT_BUTTON_RELEASE;
						ev.user.data1 = event->sourceid;
						ev.user.data2 = event->detail;
						al_emit_user_event(&(game->event_source), &ev, NULL);
						break;
					}
					case XI_RawMotion: {
						XIRawEvent* event = cookie->data;
						double* val;
						//	double *raw_val;
						val = event->valuators.values;
						//raw_val = event->raw_values;
						double x = 0, y = 0;
						if (XIMaskIsSet(event->valuators.mask, 0)) {
							x = val[0];
						}
						if (XIMaskIsSet(event->valuators.mask, 1)) {
							y = val[1];
						}
						//						PrintConsole(game, "move x=%f, y=%f, id %d", x, y, event->sourceid);
						ALLEGRO_EVENT ev;
						ev.user.type = MOT_EVENT_MOUSE_MOVE;
						ev.user.data1 = event->sourceid;
						struct MoTMouseMoveEventData* data = malloc(sizeof(struct MoTMouseMoveEventData));
						ev.user.data2 = data;
						data->x = x;
						data->y = y;

						al_emit_user_event(&(game->event_source), &ev, MoTMouseMoveEventDestructor);
						/*
						if (event->sourceid == 13) {
							game->data->x1 += x;
							game->data->y1 += y;
						} else if (event->sourceid == 14) {
							game->data->x2 += x;
							game->data->y2 += y;
						} else if (event->sourceid == 17) {
							game->data->x3 += x;
							game->data->y3 += y;
						}
*/
					} break;
					default:
						//	PrintConsole(game, "def");
						//print_deviceevent(cookie->data);
						break;
				}
			}

			XFreeEventData(game->data->display, cookie);
		}
	}
	return false;
}

struct CommonResources* CreateGameData(struct Game* game) {
	struct CommonResources* data = calloc(1, sizeof(struct CommonResources));

	//Window win = al_get_x_window_id(game->display);
	data->display = XOpenDisplay(NULL);

	int e, error;
	XQueryExtension(data->display, "XInputExtension", &(data->xi_opcode), &e, &error);
	Window win = DefaultRootWindow(data->display);
	//	XSelectInput(data->display, win, ExposureMask);
	//exit(1);
	XIEventMask mask[2];
	XIEventMask* m;
	m = &mask[0];
	m->deviceid = XIAllDevices;
	m->mask_len = XIMaskLen(XI_LASTEVENT);
	m->mask = calloc(m->mask_len, sizeof(char));
	XISetMask(m->mask, XI_ButtonPress);
	XISetMask(m->mask, XI_ButtonRelease);
	XISetMask(m->mask, XI_KeyPress);
	XISetMask(m->mask, XI_KeyRelease);
	XISetMask(m->mask, XI_Motion);
	XISetMask(m->mask, XI_DeviceChanged);
	XISetMask(m->mask, XI_Enter);
	XISetMask(m->mask, XI_Leave);
	XISetMask(m->mask, XI_FocusIn);
	XISetMask(m->mask, XI_FocusOut);
#ifdef HAVE_XI22
	XISetMask(m->mask, XI_TouchBegin);
	XISetMask(m->mask, XI_TouchUpdate);
	XISetMask(m->mask, XI_TouchEnd);
#endif
	if (m->deviceid == XIAllDevices) {
		XISetMask(m->mask, XI_HierarchyChanged);
	}
	XISetMask(m->mask, XI_PropertyEvent);

	m = &mask[1];
	m->deviceid = XIAllMasterDevices;
	m->mask_len = XIMaskLen(XI_LASTEVENT);
	m->mask = calloc(m->mask_len, sizeof(char));
	XISetMask(m->mask, XI_RawKeyPress);
	XISetMask(m->mask, XI_RawKeyRelease);
	XISetMask(m->mask, XI_RawButtonPress);
	XISetMask(m->mask, XI_RawButtonRelease);
	XISetMask(m->mask, XI_RawMotion);
#ifdef HAVE_XI22
	XISetMask(m->mask, XI_RawTouchBegin);
	XISetMask(m->mask, XI_RawTouchUpdate);
	XISetMask(m->mask, XI_RawTouchEnd);
#endif

	XISelectEvents(data->display, win, &mask[0], 2);
	//XISelectEvents(data->display, DefaultRootWindow(data->display), &mask[1], 1);
	//XMapWindow(data->display, win);
	XSync(data->display, False);

	free(mask[0].mask);
	free(mask[1].mask);

	//XEvent event;
	//XMaskEvent(data->display, ExposureMask, &event);
	//XSelectInput(data->display, win, 0);
	/*
	data->x1 = 1920 / 2;
	data->y1 = 1080 / 2;
	data->x2 = data->x1;
	data->y2 = data->y1;
	data->x3 = data->x1;
	data->y3 = data->y1;
*/
	return data;
}

void DestroyGameData(struct Game* game) {
	free(game->data);
}
