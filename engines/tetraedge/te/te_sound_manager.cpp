/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common/file.h"

#include "audio/audiostream.h"
#include "audio/mixer.h"
#include "audio/decoders/wave.h"
#include "audio/decoders/vorbis.h"

#include "tetraedge/tetraedge.h"
#include "tetraedge/te/te_core.h"
#include "tetraedge/te/te_sound_manager.h"

namespace Tetraedge {

TeSoundManager::TeSoundManager() {
}


void TeSoundManager::playFreeSound(const Common::Path &path, float vol, const Common::String &channel) {
	TeCore *core = g_engine->getCore();
	Common::Path modPath = core->findFile(path);

	Common::File *streamfile = new Common::File();
	if (!streamfile->open(modPath)) {
		warning("TeSoundManager::playFreeSound: couldn't open %s", path.toString().c_str());
		delete streamfile;
		return;
	}

	Common::String fileName = path.getLastComponent().toString();

	Audio::AudioStream *stream;
	if (fileName.contains(".wav"))
		stream = Audio::makeWAVStream(streamfile, DisposeAfterUse::YES);
	else if (fileName.contains(".ogg"))
		stream = Audio::makeVorbisStream(streamfile, DisposeAfterUse::YES);
	else
		error("Unsupported audio file type %s", fileName.c_str());

	byte bvol = round(vol * 255.0);
	int channelId = channel.hash();
	Audio::Mixer *mixer = g_system->getMixer();
	if (!_handles.contains(channel))
		_handles.setVal(channel, Audio::SoundHandle());
	else
		mixer->stopHandle(_handles[channel]);
	mixer->playStream(Audio::Mixer::kMusicSoundType, &_handles[channel], stream, channelId, bvol);
}

void TeSoundManager::stopFreeSound(const Common::String &name) {
	if (!_handles.contains(name))
		return;
	Audio::Mixer *mixer = g_system->getMixer();
	mixer->stopHandle(_handles.getVal(name));
	_handles.erase(name);
}


} // end namespace Tetraedge
