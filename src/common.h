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

#define LIBSUPERDERPY_DATA_TYPE struct CommonResources
#include <libsuperderpy.h>

#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XInput2.h>

struct CommonResources {
	// Fill in with common data accessible from all gamestates.
	Display* display;
	int xi_opcode;

	// 13, 16, 17
};

typedef enum {
	MOT_EVENT_BUTTON_PRESS = 512,
	MOT_EVENT_BUTTON_RELEASE,
	MOT_EVENT_MOUSE_MOVE
} MOT_EVENT_TYPE;

struct MoTMouseMoveEventData {
	double x;
	double y;
};

struct CommonResources* CreateGameData(struct Game* game);
void DestroyGameData(struct Game* game);
bool GlobalEventHandler(struct Game* game, ALLEGRO_EVENT* ev);
