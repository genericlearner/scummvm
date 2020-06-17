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

#include "director/director.h"
#include "director/lingo/lingo.h"
#include "director/lingo/lingo-code.h"
#include "director/frame.h"
#include "director/score.h"
#include "director/sprite.h"

namespace Director {

struct EventHandlerType {
	LEvent handler;
	const char *name;
} static const eventHandlerDescs[] = {
	{ kEventPrepareMovie,		"prepareMovie" },
	{ kEventStartMovie,			"startMovie" },			//		D3
	{ kEventStepMovie,			"stepMovie" },			//		D3
	{ kEventStopMovie,			"stopMovie" },			//		D3

	{ kEventNew,				"newSprite" },
	{ kEventBeginSprite,		"beginSprite" },
	{ kEventEndSprite,			"endSprite" },

	{ kEventEnterFrame,			"enterFrame" },			//			D4
	{ kEventPrepareFrame,		"prepareFrame" },
	{ kEventIdle,				"idle" },				//		D3
	{ kEventStepFrame,			"stepFrame"},
	{ kEventExitFrame,			"exitFrame" },			//			D4

	{ kEventActivateWindow,		"activateWindow" },
	{ kEventDeactivateWindow,	"deactivateWindow" },
	{ kEventMoveWindow,			"moveWindow" },
	{ kEventResizeWindow,		"resizeWindow" },
	{ kEventOpenWindow,			"openWindow" },
	{ kEventCloseWindow,		"closeWindow" },
	{ kEventStart,				"start" },

	{ kEventKeyUp,				"keyUp" },				//			D4
	{ kEventKeyDown,			"keyDown" },			// D2 w		D4 (as when from D2)
	{ kEventMouseUp,			"mouseUp" },			// D2 w	D3
	{ kEventMouseDown,			"mouseDown" },			// D2 w	D3
	{ kEventRightMouseDown,		"rightMouseDown" },
	{ kEventRightMouseUp,		"rightMouseUp" },
	{ kEventMouseEnter,			"mouseEnter" },
	{ kEventMouseLeave,			"mouseLeave" },
	{ kEventMouseUpOutSide,		"mouseUpOutSide" },
	{ kEventMouseWithin,		"mouseWithin" },

	{ kEventTimeout,			"timeout" },			// D2 as when

	{ kEventStartUp,			"startUp" },

	{ kEventNone,				0 },
};

void Lingo::initEventHandlerTypes() {
	for (const EventHandlerType *t = &eventHandlerDescs[0]; t->handler != kEventNone; ++t) {
		_eventHandlerTypeIds[t->name] = t->handler;
		_eventHandlerTypes[t->handler] = t->name;
	}
	_eventHandlerTypes[kEventNone] = 0;
}

ScriptType Lingo::event2script(LEvent ev) {
	if (_vm->getVersion() < 4) {
		switch (ev) {
		//case kEventStartMovie: // We are precompiling it now
		//	return kMovieScript;
		case kEventEnterFrame:
			return kScoreScript;
		default:
			return kNoneScript;
		}
	}

	return kNoneScript;
}

int Lingo::getEventCount() {
	return _eventQueue.size();
}

void Lingo::setPrimaryEventHandler(LEvent event, const Common::String &code) {
	debugC(3, kDebugLingoExec, "setting primary event handler (%s)", _eventHandlerTypes[event]);
	_archives[kArchMain].primaryEventHandlers[event] = code;
	addCode(code.c_str(), kArchMain, kGlobalScript, event);
}

void Lingo::primaryEventHandler(LEvent event) {
	/* When an event occurs the message [...] is first sent to a
	 * primary event handler: [... if exists it is executed] and the
	 * event is passed on to other objects unless you explicitly stop
	 * the message by including the dontPassEventCommand in the script
	 * [D4 docs page 77]
	 */
	debugC(3, kDebugLingoExec, "calling primary event handler (%s)", _eventHandlerTypes[event]);
	switch (event) {
	case kEventMouseDown:
	case kEventMouseUp:
	case kEventKeyUp:
	case kEventKeyDown:
	case kEventTimeout:
		executeScript(kGlobalScript, event, 0);
		break;
	default:
		/* N.B.: No primary event handlers for events other than
		 * keyup, keydown, mouseup, mousedown, timeout
		 * [see: www.columbia.edu/itc/visualarts/r4110/s2001/handouts
		 * /03_03_Event_Hierarchy.pdf]
		 */
		warning("primaryEventHandler() on event other than mouseDown, mouseUp, keyUp, keyDown, timeout");
	}
}

void Lingo::registerInputEvent(LEvent event) {
	/* When the mouseDown or mouseUp occurs over a sprite, the message
	 * goes first to the sprite script, then to the script of the cast
	 * member, to the frame script and finally to the movie scripts.
	 *
	 * When the mouseDown or mouseUp doesn't occur over a sprite, the
	 * message goes to the frame script and then to the movie script.
	 *
	 * When more than one movie script [...]
	 * [D4 docs] */

	Score *score = _vm->getCurrentScore();
	Frame *currentFrame = score->_frames[score->getCurrentFrame()];
	assert(currentFrame != nullptr);
	uint16 spriteId = score->_currentMouseDownSpriteId;
	Sprite *sprite = score->getSpriteById(spriteId);

	primaryEventHandler(event);

	if (_dontPassEvent) {
		_dontPassEvent = false;

		return;
	}

	if (_vm->getVersion() > 3) {
		if (true) {
			// TODO: Check whether occurring over a sprite
			_eventQueue.push(LingoEvent(event, kScoreScript, sprite->_scriptId));
		}
		_eventQueue.push(LingoEvent(event, kCastScript, sprite->_castId));
		_eventQueue.push(LingoEvent(event, kScoreScript, currentFrame->_actionId));
		// TODO: Is the kFrameScript call above correct?
	} else if (event == kEventMouseDown || event == kEventMouseUp) {
		// If sprite is immediate, its script is run on mouseDown, otherwise on mouseUp
		bool queueEventNone = false;
		if (event == kEventMouseDown && sprite->_immediate) {
			queueEventNone = true;
		} else if (event == kEventMouseUp && !sprite->_immediate) {
			queueEventNone = true;
		}
	
		// Score (sprite) script overrides cast script
		if (sprite->_scriptId) {
			if (queueEventNone)
				_eventQueue.push(LingoEvent(kEventNone, kScoreScript, sprite->_scriptId, spriteId));
		} else {
			if (queueEventNone)
				_eventQueue.push(LingoEvent(kEventNone, kCastScript, sprite->_castId));
			_eventQueue.push(LingoEvent(event, kCastScript, sprite->_castId));
		}
	}

	runMovieScript(event);
}

void Lingo::runMovieScript(LEvent event) {
	/* If more than one movie script handles the same message, Lingo
	 * searches the movie scripts according to their order in the cast
	 * window [p.81 of D4 docs]
	 */

	if (_dontPassEvent)
		return;

	for (ScriptContextHash::iterator it = _archives[_archiveIndex].scriptContexts[kMovieScript].begin();
			it != _archives[_archiveIndex].scriptContexts[kMovieScript].end(); ++it) {
		if (_archives[_archiveIndex].eventHandlers.contains(ENTITY_INDEX(event, it->_key))) {
			_eventQueue.push(LingoEvent(event, kMovieScript, it->_key));
			break;
		}
	}
}

void Lingo::registerFrameEvent(LEvent event) {
	/* [in D4] the enterFrame, exitFrame, idle and timeout messages
	 * are sent to a frame script and then a movie script.	If the
	 * current frame has no frame script when the event occurs, the
	 * message goes to movie scripts.
	 * [p.81 of D4 docs]
	 */
	// TODO: Same for D2-3 or not?
	Score *score = _vm->getCurrentScore();

	if (event == kEventTimeout) {
		primaryEventHandler(event);
	}

	if (_dontPassEvent) {
		_dontPassEvent = false;

		return;
	}

	int entity;

	if (event == kEventPrepareFrame || event == kEventIdle) {
		entity = score->getCurrentFrame();
	} else {
		assert(score->_frames[score->getCurrentFrame()] != nullptr);
		entity = score->_frames[score->getCurrentFrame()]->_actionId;
	}
	_eventQueue.push(LingoEvent(event, kScoreScript, entity));

	runMovieScript(event);
}

void Lingo::registerGenericEvent(LEvent event) {
	// Movie Script
	if (event == kEventStart || event == kEventStartUp || event == kEventPrepareMovie ||
			event == kEventStartMovie || event == kEventStopMovie)
		; // we're OK
	else
		warning("STUB: processGenericEvent called for unprocessed event, additional logic probably needed");

	runMovieScript(event);
}

void Lingo::registerSpriteEvent(LEvent event) {
	Score *score = _vm->getCurrentScore();
	Frame *currentFrame = score->_frames[score->getCurrentFrame()];
	if (event == kEventBeginSprite) {
		// TODO: Check if this is also possibly a kSpriteScript?
		for (uint16 i = 0; i <= score->_numChannelsDisplayed; i++)
			if (currentFrame->_sprites[i]->_enabled)
				_eventQueue.push(LingoEvent(event, kCastScript, currentFrame->_sprites[i]->_scriptId));

	} else {
		warning("STUB: processSpriteEvent called for something else than kEventBeginSprite, additional logic probably needed");
	}

}

void Lingo::registerEvent(LEvent event) {
	switch (event) {
		case kEventKeyUp:
		case kEventKeyDown:
		case kEventMouseUp:
		case kEventMouseDown:
			registerInputEvent(event);
			break;

		case kEventIdle:
		case kEventEnterFrame:
		case kEventExitFrame:
		case kEventNone:
			registerFrameEvent(event);
			break;

		case kEventStart:
		case kEventStartUp:
		case kEventStartMovie:
		case kEventStopMovie:
		case kEventTimeout:
		case kEventPrepareMovie:
			registerGenericEvent(event);
			break;
		case kEventBeginSprite:
			registerSpriteEvent(event);
			break;

		default:
			warning("registerEvent: Unhandled event %s", _eventHandlerTypes[event]);
	}

	_dontPassEvent = false;
}

void Lingo::processEvent(LEvent event) {
	registerEvent(event);
	processEvents();
}

void Lingo::processEvents() {
	while (!_eventQueue.empty()) {
		LingoEvent el = _eventQueue.pop();

		if (_vm->getCurrentScore()->_stopPlay && el.event != kEventStopMovie)
			continue;

		processEvent(el.event, el.st, el.entityId, el.channelId);
	}
}

void Lingo::processEvent(LEvent event, ScriptType st, int entityId, int channelId) {
	if (entityId < 0)
		return;

	if (_dontPassEvent)
		return;

	_currentEntityId = entityId;
	_currentChannelId = channelId;

	if (!_eventHandlerTypes.contains(event))
		error("processEvent: Unknown event %d for entity %d", event, entityId);

	if (_archives[kArchMain].eventHandlers.contains(ENTITY_INDEX(event, entityId)) ||
			_archives[kArchShared].eventHandlers.contains(ENTITY_INDEX(event, entityId))) {
		// handler
		debugC(1, kDebugEvents, "Lingo::processEvent(%s, %s, %d): executing event handler", _eventHandlerTypes[event], scriptType2str(st), entityId);
		executeHandler(_eventHandlerTypes[event]);
	} else if (_vm->getVersion() < 4 && event == kEventNone && getScriptContext(_archiveIndex, st, entityId)) {
		// script (D3)
		debugC(1, kDebugEvents, "Lingo::processEvent(%s, %s, %d): executing event script", _eventHandlerTypes[event], scriptType2str(st), entityId);
		executeScript(st, entityId, 0);
	} else {
		debugC(9, kDebugEvents, "Lingo::processEvent(%s, %s, %d): no handler", _eventHandlerTypes[event], scriptType2str(st), entityId);
	}
}

} // End of namespace Director
