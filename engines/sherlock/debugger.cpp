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
 *
 */

#include "sherlock/debugger.h"
#include "sherlock/sherlock.h"
#include "sherlock/music.h"
#include "sherlock/scalpel/3do/movie_decoder.h"
#include "sherlock/scalpel/scalpel_debugger.h"
#include "sherlock/tattoo/tattoo_debugger.h"
#include "audio/mixer.h"
#include "audio/decoders/aiff.h"
#include "audio/decoders/wave.h"

namespace Sherlock {

Debugger *Debugger::init(SherlockEngine *vm) {
	if (vm->getGameID() == GType_RoseTattoo)
		return new Tattoo::TattooDebugger(vm);
	else
		return new Scalpel::ScalpelDebugger(vm);
}

Debugger::Debugger(SherlockEngine *vm) : GUI::Debugger(), _vm(vm) {
	registerCmd("continue",	     WRAP_METHOD(Debugger, cmdExit));
	registerCmd("scene",         WRAP_METHOD(Debugger, cmdScene));
	registerCmd("song",          WRAP_METHOD(Debugger, cmdSong));
	registerCmd("dumpfile",      WRAP_METHOD(Debugger, cmdDumpFile));
}

void Debugger::postEnter() {
	if (!_3doPlayMovieFile.empty()) {
		Scalpel3DOMoviePlay(_3doPlayMovieFile.c_str(), Common::Point(0, 0));

		_3doPlayMovieFile.clear();
	}

	_vm->pauseEngine(false);
}

int Debugger::strToInt(const char *s) {
	if (!*s)
		// No string at all
		return 0;
	else if (toupper(s[strlen(s) - 1]) != 'H')
		// Standard decimal string
		return atoi(s);

	// Hexadecimal string
	uint tmp = 0;
	int read = sscanf(s, "%xh", &tmp);
	if (read < 1)
		error("strToInt failed on string \"%s\"", s);
	return (int)tmp;
}

bool Debugger::cmdScene(int argc, const char **argv) {
	if (argc != 2) {
		debugPrintf("Format: scene <room>\n");
		return true;
	} else {
		_vm->_scene->_goToScene = strToInt(argv[1]);
		return false;
	}
}

bool Debugger::cmdSong(int argc, const char **argv) {
	if (argc != 2) {
		debugPrintf("Format: song <room>\n");
		return true;
	}

	if (!_vm->_music->loadSong(strToInt(argv[1]))) {
		debugPrintf("Invalid song number.\n");
		return true;
	}
	return false;
}

bool Debugger::cmdDumpFile(int argc, const char **argv) {
	if (argc != 2) {
		debugPrintf("Format: dumpfile <resource name>\n");
		return true;
	}

	Common::SeekableReadStream *s = _vm->_res->load(argv[1]);
	if (!s) {
		debugPrintf("Invalid resource.\n");
		return true;
	}

	byte *buffer = new byte[s->size()];
	s->read(buffer, s->size());

	Common::DumpFile dumpFile;
	dumpFile.open(argv[1]);

	dumpFile.write(buffer, s->size());
	dumpFile.flush();
	dumpFile.close();

	delete[] buffer;

	debugPrintf("Resource %s has been dumped to disk.\n", argv[1]);

	return true;
}

} // End of namespace Sherlock
