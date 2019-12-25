/* ScummVM - Graphic Adventure Engine
*
* ScummVM is the legal property of its developers, whose names
* are too numerous to list here. Please refer to the COPYRIGHT
* file distributed with this source distribution.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "common/macresman.h"
#include "common/system.h"
#include "engines/util.h"

#include "graphics/fonts/macfont.h"
#include "graphics/macgui/macfontmanager.h"
#include "graphics/macgui/macwindowmanager.h"

#include "director/director.h"

namespace Director {

// Referred as extern
byte defaultPalette[768] = {
	0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x44, 0x44, 0x44,  //   0 (0x00)
	0x55, 0x55, 0x55, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0xaa, 0xaa, 0xaa,  //   4 (0x04)
	0xbb, 0xbb, 0xbb, 0xdd, 0xdd, 0xdd, 0xee, 0xee, 0xee, 0x00, 0x00, 0x11,  //   8 (0x08)
	0x00, 0x00, 0x22, 0x00, 0x00, 0x44, 0x00, 0x00, 0x55, 0x00, 0x00, 0x77,  //  12 (0x0c)
	0x00, 0x00, 0x88, 0x00, 0x00, 0xaa, 0x00, 0x00, 0xbb, 0x00, 0x00, 0xdd,  //  16 (0x10)
	0x00, 0x00, 0xee, 0x00, 0x11, 0x00, 0x00, 0x22, 0x00, 0x00, 0x44, 0x00,  //  20 (0x14)
	0x00, 0x55, 0x00, 0x00, 0x77, 0x00, 0x00, 0x88, 0x00, 0x00, 0xaa, 0x00,  //  24 (0x18)
	0x00, 0xbb, 0x00, 0x00, 0xdd, 0x00, 0x00, 0xee, 0x00, 0x11, 0x00, 0x00,  //  28 (0x1c)
	0x22, 0x00, 0x00, 0x44, 0x00, 0x00, 0x55, 0x00, 0x00, 0x77, 0x00, 0x00,  //  32 (0x20)
	0x88, 0x00, 0x00, 0xaa, 0x00, 0x00, 0xbb, 0x00, 0x00, 0xdd, 0x00, 0x00,  //  36 (0x24)
	0xee, 0x00, 0x00, 0x00, 0x00, 0x33, 0x00, 0x00, 0x66, 0x00, 0x00, 0x99,  //  40 (0x28)
	0x00, 0x00, 0xcc, 0x00, 0x00, 0xff, 0x00, 0x33, 0x00, 0x00, 0x33, 0x33,  //  44 (0x2c)
	0x00, 0x33, 0x66, 0x00, 0x33, 0x99, 0x00, 0x33, 0xcc, 0x00, 0x33, 0xff,  //  48 (0x30)
	0x00, 0x66, 0x00, 0x00, 0x66, 0x33, 0x00, 0x66, 0x66, 0x00, 0x66, 0x99,  //  52 (0x34)
	0x00, 0x66, 0xcc, 0x00, 0x66, 0xff, 0x00, 0x99, 0x00, 0x00, 0x99, 0x33,  //  56 (0x38)
	0x00, 0x99, 0x66, 0x00, 0x99, 0x99, 0x00, 0x99, 0xcc, 0x00, 0x99, 0xff,  //  60 (0x3c)
	0x00, 0xcc, 0x00, 0x00, 0xcc, 0x33, 0x00, 0xcc, 0x66, 0x00, 0xcc, 0x99,  //  64 (0x40)
	0x00, 0xcc, 0xcc, 0x00, 0xcc, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0x33,  //  68 (0x44)
	0x00, 0xff, 0x66, 0x00, 0xff, 0x99, 0x00, 0xff, 0xcc, 0x00, 0xff, 0xff,  //  72 (0x48)
	0x33, 0x00, 0x00, 0x33, 0x00, 0x33, 0x33, 0x00, 0x66, 0x33, 0x00, 0x99,  //  76 (0x4c)
	0x33, 0x00, 0xcc, 0x33, 0x00, 0xff, 0x33, 0x33, 0x00, 0x33, 0x33, 0x33,  //  80 (0x50)
	0x33, 0x33, 0x66, 0x33, 0x33, 0x99, 0x33, 0x33, 0xcc, 0x33, 0x33, 0xff,  //  84 (0x54)
	0x33, 0x66, 0x00, 0x33, 0x66, 0x33, 0x33, 0x66, 0x66, 0x33, 0x66, 0x99,  //  88 (0x58)
	0x33, 0x66, 0xcc, 0x33, 0x66, 0xff, 0x33, 0x99, 0x00, 0x33, 0x99, 0x33,  //  92 (0x5c)
	0x33, 0x99, 0x66, 0x33, 0x99, 0x99, 0x33, 0x99, 0xcc, 0x33, 0x99, 0xff,  //  96 (0x60)
	0x33, 0xcc, 0x00, 0x33, 0xcc, 0x33, 0x33, 0xcc, 0x66, 0x33, 0xcc, 0x99,  // 100 (0x64)
	0x33, 0xcc, 0xcc, 0x33, 0xcc, 0xff, 0x33, 0xff, 0x00, 0x33, 0xff, 0x33,  // 104 (0x68)
	0x33, 0xff, 0x66, 0x33, 0xff, 0x99, 0x33, 0xff, 0xcc, 0x33, 0xff, 0xff,  // 108 (0x6c)
	0x66, 0x00, 0x00, 0x66, 0x00, 0x33, 0x66, 0x00, 0x66, 0x66, 0x00, 0x99,  // 112 (0x70)
	0x66, 0x00, 0xcc, 0x66, 0x00, 0xff, 0x66, 0x33, 0x00, 0x66, 0x33, 0x33,  // 116 (0x74)
	0x66, 0x33, 0x66, 0x66, 0x33, 0x99, 0x66, 0x33, 0xcc, 0x66, 0x33, 0xff,  // 120 (0x78)
	0x66, 0x66, 0x00, 0x66, 0x66, 0x33, 0x66, 0x66, 0x66, 0x66, 0x66, 0x99,  // 124 (0x7c)
	0x66, 0x66, 0xcc, 0x66, 0x66, 0xff, 0x66, 0x99, 0x00, 0x66, 0x99, 0x33,  // 128 (0x80)
	0x66, 0x99, 0x66, 0x66, 0x99, 0x99, 0x66, 0x99, 0xcc, 0x66, 0x99, 0xff,  // 132 (0x84)
	0x66, 0xcc, 0x00, 0x66, 0xcc, 0x33, 0x66, 0xcc, 0x66, 0x66, 0xcc, 0x99,  // 136 (0x88)
	0x66, 0xcc, 0xcc, 0x66, 0xcc, 0xff, 0x66, 0xff, 0x00, 0x66, 0xff, 0x33,  // 140 (0x8c)
	0x66, 0xff, 0x66, 0x66, 0xff, 0x99, 0x66, 0xff, 0xcc, 0x66, 0xff, 0xff,  // 144 (0x90)
	0x99, 0x00, 0x00, 0x99, 0x00, 0x33, 0x99, 0x00, 0x66, 0x99, 0x00, 0x99,  // 148 (0x94)
	0x99, 0x00, 0xcc, 0x99, 0x00, 0xff, 0x99, 0x33, 0x00, 0x99, 0x33, 0x33,  // 152 (0x98)
	0x99, 0x33, 0x66, 0x99, 0x33, 0x99, 0x99, 0x33, 0xcc, 0x99, 0x33, 0xff,  // 156 (0x9c)
	0x99, 0x66, 0x00, 0x99, 0x66, 0x33, 0x99, 0x66, 0x66, 0x99, 0x66, 0x99,  // 160 (0xa0)
	0x99, 0x66, 0xcc, 0x99, 0x66, 0xff, 0x99, 0x99, 0x00, 0x99, 0x99, 0x33,  // 164 (0xa4)
	0x99, 0x99, 0x66, 0x99, 0x99, 0x99, 0x99, 0x99, 0xcc, 0x99, 0x99, 0xff,  // 168 (0xa8)
	0x99, 0xcc, 0x00, 0x99, 0xcc, 0x33, 0x99, 0xcc, 0x66, 0x99, 0xcc, 0x99,  // 172 (0xac)
	0x99, 0xcc, 0xcc, 0x99, 0xcc, 0xff, 0x99, 0xff, 0x00, 0x99, 0xff, 0x33,  // 176 (0xb0)
	0x99, 0xff, 0x66, 0x99, 0xff, 0x99, 0x99, 0xff, 0xcc, 0x99, 0xff, 0xff,  // 180 (0xb4)
	0xcc, 0x00, 0x00, 0xcc, 0x00, 0x33, 0xcc, 0x00, 0x66, 0xcc, 0x00, 0x99,  // 184 (0xb8)
	0xcc, 0x00, 0xcc, 0xcc, 0x00, 0xff, 0xcc, 0x33, 0x00, 0xcc, 0x33, 0x33,  // 188 (0xbc)
	0xcc, 0x33, 0x66, 0xcc, 0x33, 0x99, 0xcc, 0x33, 0xcc, 0xcc, 0x33, 0xff,  // 192 (0xc0)
	0xcc, 0x66, 0x00, 0xcc, 0x66, 0x33, 0xcc, 0x66, 0x66, 0xcc, 0x66, 0x99,  // 196 (0xc4)
	0xcc, 0x66, 0xcc, 0xcc, 0x66, 0xff, 0xcc, 0x99, 0x00, 0xcc, 0x99, 0x33,  // 200 (0xc8)
	0xcc, 0x99, 0x66, 0xcc, 0x99, 0x99, 0xcc, 0x99, 0xcc, 0xcc, 0x99, 0xff,  // 204 (0xcc)
	0xcc, 0xcc, 0x00, 0xcc, 0xcc, 0x33, 0xcc, 0xcc, 0x66, 0xcc, 0xcc, 0x99,  // 208 (0xd0)
	0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xff, 0xcc, 0xff, 0x00, 0xcc, 0xff, 0x33,  // 212 (0xd4)
	0xcc, 0xff, 0x66, 0xcc, 0xff, 0x99, 0xcc, 0xff, 0xcc, 0xcc, 0xff, 0xff,  // 216 (0xd8)
	0xff, 0x00, 0x00, 0xff, 0x00, 0x33, 0xff, 0x00, 0x66, 0xff, 0x00, 0x99,  // 220 (0xdc)
	0xff, 0x00, 0xcc, 0xff, 0x00, 0xff, 0xff, 0x33, 0x00, 0xff, 0x33, 0x33,  // 224 (0xe0)
	0xff, 0x33, 0x66, 0xff, 0x33, 0x99, 0xff, 0x33, 0xcc, 0xff, 0x33, 0xff,  // 228 (0xe4)
	0xff, 0x66, 0x00, 0xff, 0x66, 0x33, 0xff, 0x66, 0x66, 0xff, 0x66, 0x99,  // 232 (0xe8)
	0xff, 0x66, 0xcc, 0xff, 0x66, 0xff, 0xff, 0x99, 0x00, 0xff, 0x99, 0x33,  // 236 (0xec)
	0xff, 0x99, 0x66, 0xff, 0x99, 0x99, 0xff, 0x99, 0xcc, 0xff, 0x99, 0xff,  // 240 (0xf0)
	0xff, 0xcc, 0x00, 0xff, 0xcc, 0x33, 0xff, 0xcc, 0x66, 0xff, 0xcc, 0x99,  // 244 (0xf4)
	0xff, 0xcc, 0xcc, 0xff, 0xcc, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x33,  // 248 (0xf8)
	0xff, 0xff, 0x66, 0xff, 0xff, 0x99, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xff   // 252 (0xfc)
};


static byte director3Patterns[][8] = {
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
	{ 0xFF, 0xFF, 0x77, 0xFF, 0xFF, 0xFF, 0x77, 0xFF },
	{ 0x77, 0xFF, 0xDD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF },
	{ 0xFF, 0xDD, 0xFF, 0x55, 0xFF, 0xDD, 0xFF, 0x55 },
	{ 0xFF, 0xD5, 0xFF, 0x55, 0xFF, 0x5D, 0xFF, 0x55 },
	{ 0xFF, 0xAA, 0xFF, 0xAA, 0xFF, 0xAA, 0xFF, 0xAA },
	{ 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA },
	{ 0xAA, 0x44, 0xAA, 0x11, 0xAA, 0x44, 0xAA, 0x11 },
	{ 0xAA, 0x44, 0xAA, 0x00, 0xAA, 0x44, 0xAA, 0x00 },
	{ 0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00 },
	{ 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00, 0x88 },
	{ 0x88, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00 },
	{ 0x80, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00 },
	{ 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 },
	{ 0x21, 0x42, 0x84, 0x09, 0x12, 0x24, 0x48, 0x90 },
	{ 0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88 },
	{ 0xEE, 0xDD, 0xBB, 0x77, 0xEE, 0xDD, 0xBB, 0x77 },
	{ 0xF6, 0xED, 0xDB, 0xB7, 0x6F, 0xDE, 0xBD, 0x7B },
	{ 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F },
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF },
	{ 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF },
	{ 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF },
	{ 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF },
	{ 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00 },
	{ 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00 },
	{ 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00 },
	{ 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 },
	{ 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82 },
	{ 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88 },
	{ 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94, 0x94 },
	{ 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA },
	{ 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD, 0xAD },
	{ 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB },
	{ 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7, 0xB7 },
	{ 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF },
	{ 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x00 },
	{ 0x7F, 0x7F, 0x7F, 0x00, 0x7F, 0x7F, 0x7F, 0x00 },
	{ 0x77, 0x77, 0x77, 0x00, 0x77, 0x77, 0x77, 0x00 },
	{ 0x88, 0x88, 0x88, 0xFF, 0x88, 0x88, 0x88, 0xFF },
	{ 0x80, 0x80, 0x80, 0xFF, 0x80, 0x80, 0x80, 0xFF },
	{ 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFF },
	{ 0x01, 0x82, 0x44, 0x28, 0x10, 0x28, 0x44, 0x82 },
	{ 0x11, 0x82, 0x45, 0xAB, 0xD7, 0xAB, 0x45, 0x82 },
	{ 0xF7, 0x7F, 0xBE, 0x5D, 0x2A, 0x5D, 0xBE, 0x7F },
	{ 0xFE, 0x7D, 0xBB, 0xD7, 0xEF, 0xD7, 0xBB, 0x7D },
	{ 0xFE, 0x7F, 0xBF, 0xDF, 0xEF, 0xDF, 0xBF, 0x7F },
	{ 0xEE, 0x77, 0xBB, 0xDD, 0xEE, 0xDD, 0xBB, 0x77 },
	{ 0x11, 0x88, 0x44, 0x22, 0x11, 0x22, 0x44, 0x88 },
	{ 0x01, 0x80, 0x40, 0x20, 0x10, 0x20, 0x40, 0x80 },
	{ 0x22, 0x00, 0x01, 0x22, 0x54, 0x88, 0x01, 0x00 },
	{ 0xBF, 0xAF, 0xAB, 0xAA, 0xEA, 0xFA, 0xFE, 0xFF },
	{ 0xFF, 0xFF, 0xBE, 0x9C, 0xAA, 0xB6, 0xBE, 0xFF }
};

static byte director3QuickDrawPatterns[][8] = {
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
	{ 0xDD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF, 0x77, 0xFF },
	{ 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xDD, 0x77 },
	{ 0xEE, 0xDD, 0xBB, 0x77, 0xEE, 0xDD, 0xBB, 0x77 },
	{ 0xFF, 0xAA, 0xFF, 0xAA, 0xFF, 0xAA, 0xFF, 0xAA },
	{ 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 },
	{ 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55 },
	{ 0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88 },
	{ 0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00 },
	{ 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22 },
	{ 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00, 0x88 },
	{ 0x10, 0x20, 0x40, 0x80, 0x01, 0x02, 0x04, 0x08 },
	{ 0x80, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00 },
	{ 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88 },
	{ 0x58, 0xDF, 0x00, 0xDF, 0xDF, 0x58, 0x58, 0x58 },
	{ 0xB1, 0x36, 0x06, 0x60, 0x63, 0x1B, 0x18, 0x81 },
	{ 0x08, 0xFF, 0x01, 0x01, 0x01, 0xFF, 0x08, 0x08 },
	{ 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00 },
	{ 0x80, 0x80, 0x80, 0xFF, 0x80, 0x80, 0x80, 0x80 },
	{ 0x80, 0x10, 0x02, 0x40, 0x04, 0x20, 0x09, 0x00 },
	{ 0x80, 0x01, 0x82, 0x44, 0x38, 0x10, 0x20, 0x40 },
	{ 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00 },
	{ 0x22, 0xFF, 0x22, 0x22, 0x22, 0xFF, 0x22, 0x22 },
	{ 0x00, 0x08, 0x14, 0x2A, 0x55, 0x2A, 0x14, 0x08 },
	{ 0x81, 0xAA, 0x14, 0x08, 0x08, 0xAA, 0x41, 0x80 },
	{ 0x3E, 0x1D, 0x88, 0xD1, 0xE3, 0xC5, 0x88, 0x5C },
	{ 0xAA, 0x00, 0x80, 0x00, 0x88, 0x00, 0x80, 0x00 },
	{ 0x00, 0x11, 0x82, 0x44, 0x28, 0x11, 0x00, 0x55 },
	{ 0x7C, 0x10, 0x10, 0x28, 0xC7, 0x01, 0x01, 0x82 },
	{ 0xEE, 0x31, 0xF1, 0xF1, 0xEE, 0x13, 0x1F, 0x1F },
	{ 0x00, 0x40, 0x20, 0x10, 0x00, 0x01, 0x02, 0x04 },
	{ 0x00, 0x00, 0x40, 0xA0, 0x00, 0x04, 0x0A, 0x00 },
	{ 0x20, 0x60, 0x90, 0x09, 0x06, 0x81, 0x40, 0x20 },
	{ 0x00, 0x7F, 0x43, 0x5F, 0x5F, 0x5F, 0x7F, 0x7F },
	{ 0x01, 0x02, 0x45, 0xAA, 0xFF, 0x20, 0x40, 0x80 },
	{ 0x00, 0x44, 0x0A, 0x11, 0x11, 0x11, 0x51, 0x24 },
	{ 0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0 },
	{ 0xF8, 0xFC, 0xFA, 0xFC, 0xFA, 0x54, 0x2A, 0x00 },
	{ 0x42, 0xC3, 0x3C, 0x3C, 0x3C, 0x3C, 0xC3, 0x42 },
	{ 0x10, 0x38, 0x7C, 0xFE, 0x7D, 0x3A, 0x14, 0x08 },
	{ 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF },
	{ 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC },
	{ 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00 },
	{ 0xBB, 0xDD, 0xAE, 0x77, 0xEE, 0xDD, 0xAB, 0x77 },
	{ 0x80, 0x40, 0x40, 0x20, 0x20, 0x18, 0x06, 0x01 },
	{ 0x01, 0x82, 0x44, 0x28, 0x10, 0x28, 0x44, 0x82 },
	{ 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xFF, 0xFF },
	{ 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xFF, 0xFF, 0xFF },
	{ 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00 },
	{ 0xC3, 0x87, 0x0F, 0x1E, 0x3C, 0x78, 0xF0, 0xE1 },
	{ 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0 },
	{ 0xFF, 0xFF, 0xE7, 0xC3, 0x81, 0x18, 0x3C, 0x7E },
	{ 0x1F, 0x8F, 0xC7, 0xE3, 0xC7, 0x8F, 0x1F, 0x3E },
	{ 0xFF, 0x2A, 0xFF, 0xC8, 0xFF, 0x65, 0xFF, 0x9D }
};

void DirectorEngine::loadPatterns() {
	for (int i = 0; i < ARRAYSIZE(director3Patterns); i++)
		_director3Patterns.push_back(director3Patterns[i]);

	for (int i = 0; i < ARRAYSIZE(director3QuickDrawPatterns); i++)
		_director3QuickDrawPatterns.push_back(director3QuickDrawPatterns[i]);
}

Graphics::MacPatterns &DirectorEngine::getPatterns() {
	// TOOD: implement switch and other version patterns. (use getVersion());
	return _director3QuickDrawPatterns;
}

void DirectorEngine::setPalette(byte *palette, uint16 count) {
	_currentPalette = palette;
	_currentPaletteLength = count;

	_wm->passPalette(palette, count);
}

void DirectorEngine::testFontScaling() {
	int x = 10;
	int y = 10;
	int w = 640;
	int h = 480;

	initGraphics(w, h);
	_system->getPaletteManager()->setPalette(defaultPalette, 0, 256);

	Graphics::ManagedSurface surface;

	surface.create(w, h);
	surface.clear(255);

	Graphics::MacFont origFont(Graphics::kMacFontNewYork, 18);

	const Graphics::MacFONTFont *font1 = (const Graphics::MacFONTFont *)_wm->_fontMan->getFont(origFont);

	Graphics::MacFONTFont::testBlit(font1, &surface, 0, x, y + 200, 500);

	Graphics::MacFont bigFont(Graphics::kMacFontNewYork, 15);

	font1 = (const Graphics::MacFONTFont *)_wm->_fontMan->getFont(bigFont);

	Graphics::MacFONTFont::testBlit(font1, &surface, 0, x, y + 50 + 200, 500);

	const char *text = "d";

	for (int i = 9; i <= 20; i++) {
		Graphics::MacFont macFont(Graphics::kMacFontNewYork, i);

		const Graphics::Font *font = _wm->_fontMan->getFont(macFont);

		int width = font->getStringWidth(text);

		Common::Rect bbox = font->getBoundingBox(text, x, y, w);
		surface.frameRect(bbox, 15);

		font->drawString(&surface, text, x, y, width, 0);

		x += width + 1;
	}

	g_system->copyRectToScreen(surface.getPixels(), surface.pitch, 0, 0, w, h); // testing fonts

	Common::Event event;

	while (true) {
		if (g_system->getEventManager()->pollEvent(event))
			if (event.type == Common::EVENT_QUIT)
				break;

		g_system->updateScreen();
		g_system->delayMillis(10);
	}
}

void DirectorEngine::testFonts() {
	Common::String fontName("Helvetica");

	Common::MacResManager *fontFile = new Common::MacResManager();
	if (!fontFile->open(fontName))
		error("Could not open %s as a resource fork", fontName.c_str());

	Common::MacResIDArray fonds = fontFile->getResIDArray(MKTAG('F','O','N','D'));
	if (fonds.size() > 0) {
		for (Common::Array<uint16>::iterator iterator = fonds.begin(); iterator != fonds.end(); ++iterator) {
			Common::SeekableReadStream *stream = fontFile->getResource(MKTAG('F', 'O', 'N', 'D'), *iterator);
			Common::String name = fontFile->getResName(MKTAG('F', 'O', 'N', 'D'), *iterator);

			debug("Font: %s", name.c_str());

			Graphics::MacFontFamily font;
			font.load(*stream);
		}
	}

	delete fontFile;
}


}
