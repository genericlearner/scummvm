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
#include "common/path.h"
#include "common/textconsole.h"

#include "tetraedge/tetraedge.h"
#include "tetraedge/game/application.h"
#include "tetraedge/game/billboard.h"
#include "tetraedge/game/game.h"
#include "tetraedge/game/in_game_scene.h"
#include "tetraedge/game/character.h"
#include "tetraedge/game/characters_shadow.h"
#include "tetraedge/game/object3d.h"
#include "tetraedge/game/scene_lights_xml_parser.h"

#include "tetraedge/te/te_bezier_curve.h"
#include "tetraedge/te/te_camera.h"
#include "tetraedge/te/te_core.h"
#include "tetraedge/te/te_free_move_zone.h"
#include "tetraedge/te/te_light.h"
#include "tetraedge/te/te_renderer.h"
#include "tetraedge/te/te_lua_script.h"
#include "tetraedge/te/te_lua_thread.h"

namespace Tetraedge {

InGameScene::InGameScene() : _character(nullptr), _charactersShadow(nullptr),
_shadowLightNo(-1), _waitTime(-1.0f), _shadowColor(0, 0, 0, 0x80), _shadowFov(20.0f),
_shadowFarPlane(1000), _shadowNearPlane(1)
 {
}

void InGameScene::activateAnchorZone(const Common::String &name, bool val) {
	for (AnchorZone *zone : _anchorZones) {
		if (zone->_name == name)
			zone->_activated = val;
	}
}

void InGameScene::addAnchorZone(const Common::String &s1, const Common::String &name, float radius) {
	for (AnchorZone *zone : _anchorZones) {
		if (zone->_name == name) {
			zone->_radius = radius;
			return;
		}
	}
	warning("TODO: Finish InGameScene::addAnchorZone");
}

bool InGameScene::addMarker(const Common::String &markerName, const Common::String &imgPath, float x, float y, const Common::String &locType, const Common::String &markerVal) {
	const TeMarker *marker = findMarker(markerName);
	if (!marker) {
		Game *game = g_engine->getGame();
		TeSpriteLayout *markerSprite = new TeSpriteLayout();
		// Note: game checks paths here but seems to just use the original?
		markerSprite->setName(markerName);
		markerSprite->setAnchor(TeVector3f32(0.0f, 0.0f, 0.0f));
		markerSprite->load(imgPath);
		markerSprite->setSizeType(TeILayout::RELATIVE_TO_PARENT);
			markerSprite->setPositionType(TeILayout::RELATIVE_TO_PARENT);
		TeVector3f32 newSize;
		if (locType == "PERCENT") {
			Application *app = g_engine->getApplication();
			TeVector3f32 frontLayoutSize = app->_frontLayout.userSize();
			newSize.x() = frontLayoutSize.x() * (x / 100.0);
			newSize.y() = frontLayoutSize.y() * (y / 100.0);
		} else {
			newSize.x() = x / 800.0;
			newSize.y() = y / 600.0;
		}
		markerSprite->setSize(newSize);

		float screenWidth = (float)g_engine->getDefaultScreenWidth();
		float screenHeight = (float)g_engine->getDefaultScreenHeight();
		if (g_engine->getCore()->fileFlagSystemFlag("definition") == "SD") {
			markerSprite->setPosition(TeVector3f32(0.07, (4.0 / ((screenWidth / screenHeight) * 4.0)) * 0.04, 0.0));
		} else {
			markerSprite->setPosition(TeVector3f32(0.04, (4.0 / ((screenWidth / screenHeight) * 4.0)) * 0.04, 0.0));
		}
		markerSprite->setVisible(game->markersVisible());
		markerSprite->_tiledSurfacePtr->_frameAnim._loopCount = -1;
		markerSprite->play();

		TeMarker newMarker;
		newMarker._name = markerName;
		newMarker._val = markerVal;
		_markers.push_back(newMarker);
		TeLayout *bg = game->gui3().layout("background");
		if (bg)
			bg->addChild(markerSprite);
		_sprites.push_back(markerSprite);
	} else  {
		setImagePathMarker(markerName, imgPath);
	}
	return true;
}

/*static*/
float InGameScene::angularDistance(float a1, float a2) {
	float result;

	result = a2 - a1;
	if (result >= -3.141593 && result > 3.141593) {
		result = result + -6.283185;
	} else {
		result = result + 6.283185;
	}
	return result;
}

bool InGameScene::aroundAnchorZone(const AnchorZone *zone) {
	if (!zone->_activated)
		return false;
	const TeVector3f32 charpos = _character->_model->position();

	float xoff = charpos.x() - zone->_loc.x();
	float zoff = charpos.z() - zone->_loc.z();
    return sqrt(xoff * xoff + zoff * zoff) <= zone->_radius;
}

TeLayout *InGameScene::background() {
	return _bgGui.layout("background");
}

Billboard *InGameScene::billboard(const Common::String &name) {
	for (Billboard *billboard : _billboards) {
		if (billboard->model()->name() == name)
			return billboard;
	}
	return nullptr;
}

bool InGameScene::changeBackground(const Common::String &name) {
	if (Common::File::exists(name)) {
		_bgGui.spriteLayoutChecked("root")->load(name);
		return true;
	}
	return false;
}

Character *InGameScene::character(const Common::String &name) {
	if (_character->_model->name() == name)
		return _character;

	for (Character *c : _characters) {
		if (c->_model->name() == name)
			return c;
	}

	return nullptr;
}

void InGameScene::close() {
	reset();
	_loadedPath = "";
	TeScene::close();
	freeGeometry();
	if (_character && _character->_model && !findKate()) {
		models().push_back(_character->_model);
		models().push_back(_character->_shadowModel[0]);
		models().push_back(_character->_shadowModel[1]);
	}
	_objects.clear();
	for (TeFreeMoveZone *zone : _freeMoveZones)
		delete zone;
	_freeMoveZones.clear();
	_hitObjects.clear();
	for (TePickMesh2 *mesh : _pickMeshes)
		delete mesh;
	_pickMeshes.clear();
	_bezierCurves.clear();
	_dummies.clear();
	freeSceneObjects();
}

void InGameScene::convertPathToMesh(TeFreeMoveZone *zone) {
	TeIntrusivePtr<TeModel> model = new TeModel();
	model->_meshes.resize(1);
	model->setName("shadowReceiving");
	model->setPosition(zone->position());
	model->setRotation(zone->rotation());
	model->setScale(zone->scale());
	unsigned long nverticies = zone->verticies().size();
	model->_meshes[0].setConf(nverticies, nverticies, TeMesh::MeshMode_Triangles, 0, 0);
	for (unsigned int i = 0; i < nverticies; i++) {
		model->_meshes[0].setIndex(i, i);
		model->_meshes[0].setVertex(i, zone->verticies()[i]);
		model->_meshes[0].setNormal(i, TeVector3f32(0, 0, 1));
	}
	_zoneModels.push_back(model);
}

TeIntrusivePtr<TeBezierCurve> InGameScene::curve(const Common::String &curveName) {
	for (TeIntrusivePtr<TeBezierCurve> &c : _bezierCurves) {
		if (c->name() == curveName)
			return c;
	}
	return TeIntrusivePtr<TeBezierCurve>();
}

void InGameScene::deleteAllCallback() {
	warning("TODO: implement InGameScene::deleteAllCallback");
}

void InGameScene::deleteMarker(const Common::String &markerName) {
	if (!isMarker(markerName))
		return;

	for (unsigned int i = 0; i < _markers.size(); i++) {
		if (_markers[i]._name == markerName) {
			_markers.remove_at(i);
			break;
		}
	}

	Game *game = g_engine->getGame();
	TeLayout *bg = game->gui3().layout("background");
	if (!bg)
		return;
	for (Te3DObject2 *child : bg->childList()) {
		if (child->name() == markerName) {
			bg->removeChild(child);
			break;
		}
	}
}

void InGameScene::deserializeCam(Common::ReadStream &stream, TeIntrusivePtr<TeCamera> &cam) {
	cam->_projectionMatrixType = 2;
	cam->viewport(0, 0, _viewportSize.getX(), _viewportSize.getY());
	// load name/position/rotation/scale
	Te3DObject2::deserialize(stream, *cam);
	cam->_fov = stream.readFloatLE();
	cam->_somePerspectiveVal = stream.readFloatLE();
	cam->_orthNearVal = stream.readFloatLE();
	// Original loads the val then ignores it and sets 3000.
	stream.readFloatLE();
	cam->_orthFarVal = 3000.0;
}

void InGameScene::deserializeModel(Common::ReadStream &stream, TeIntrusivePtr<TeModel> &model, TePickMesh2 *pickmesh) {
	TeVector3f32 vec;
	TeVector2f32 vec2;
	TeQuaternion rot;
	TeColor col;
	TeMesh mesh;

	TeVector3f32::deserialize(stream, vec);
	model->setPosition(vec);
	pickmesh->setPosition(vec);
	TeQuaternion::deserialize(stream, rot);
	model->setRotation(rot);
	pickmesh->setRotation(rot);
	TeVector3f32::deserialize(stream, vec);
	model->setScale(vec);
	pickmesh->setScale(vec);
	uint32 indexcount = stream.readUint32LE();
	uint32 vertexcount = stream.readUint32LE();

	mesh.setConf(vertexcount, indexcount, TeMesh::MeshMode_Triangles, 0, 0);
	for (unsigned int i = 0; i < indexcount; i++)
		mesh.setIndex(i, stream.readUint32LE());
	for (unsigned int i = 0; i < vertexcount; i++) {
		TeVector3f32::deserialize(stream, vec);
		mesh.setVertex(i, vec);
	}
	for (unsigned int i = 0; i < vertexcount; i++) {
		TeVector3f32::deserialize(stream, vec);
		mesh.setNormal(i, vec);
	}
	for (unsigned int i = 0; i < vertexcount; i++) {
		TeVector2f32::deserialize(stream, vec2);
		mesh.setTextureUV(i, vec2);
	}
	for (unsigned int i = 0; i < vertexcount; i++) {
		col.deserialize(stream);
		mesh.setColor(i, col);
	}
	pickmesh->setNbTriangles(indexcount / 3);
	for (unsigned int i = 0; i < indexcount; i++) {
		vec = mesh.vertex(mesh.index(i));
		pickmesh->verticies().push_back(vec);
	}
	model->addMesh(mesh);
}

void InGameScene::draw() {
	TeScene::draw();

	if (currentCameraIndex() >= (int)cameras().size())
		return;

	currentCamera()->apply();
	TeLight::updateGlobal();
	for (unsigned int i = 0; i < _lights.size(); i++)
		_lights[i].update(i);

	TeCamera::restore();
}

void InGameScene::drawPath() {
	if (currentCameraIndex() >= (int)cameras().size())
		return;

	currentCamera()->apply();
	g_engine->getRenderer()->disableZBuffer();

	for (unsigned int i = 0; i < _freeMoveZones.size(); i++)
		_freeMoveZones[i]->draw();

	g_engine->getRenderer()->enableZBuffer();
}

InGameScene::Dummy InGameScene::dummy(const Common::String &name) {
	for (const Dummy &dummy : _dummies) {
		if (dummy._name == name)
			return dummy;
	}
	return Dummy();
}

bool InGameScene::findKate() {
	for (auto &m : models()) {
		if (m->name() == "Kate")
			return true;
	}
	return false;
}

const InGameScene::TeMarker *InGameScene::findMarker(const Common::String &name) {
	for (const TeMarker &marker : _markers) {
		if (marker._name == name)
			return &marker;
	}
	return nullptr;
}

const InGameScene::TeMarker *InGameScene::findMarkerByInt(const Common::String &val) {
	for (const TeMarker &marker : _markers) {
		if (marker._val == val)
			return &marker;
	}
	return nullptr;
}

InGameScene::SoundStep InGameScene::findSoundStep(const Common::String &name) {
	for (const auto &step : _soundSteps) {
		if (step._key == name)
			return step._value;
	}
	return SoundStep();
}

void InGameScene::freeGeometry() {
	_loadedPath.set("");
	for (TeFreeMoveZone *zone : _freeMoveZones)
		delete zone;
	_freeMoveZones.clear();
	_bezierCurves.clear();
	_dummies.clear();
	cameras().clear();
	_zoneModels.clear();
	if (_charactersShadow) {
		delete _charactersShadow;
		_charactersShadow = nullptr;
	}
}

void InGameScene::freeSceneObjects() {
	if (_character) {
		_character->setCharLookingAt(nullptr);
		_character->deleteAllCallback();
	}
	if (_characters.size() == 1) {
		_characters[0]->deleteAllCallback();
	}

	Game *game = g_engine->getGame();
	game->unloadCharacters();

	_characters.clear();

	for (Object3D *obj : _object3Ds) {
		obj->deleteLater();
	}
	_object3Ds.clear();

	for (Billboard *bb : _billboards) {
		bb->deleteLater();
	}
	_billboards.clear();

	for (TeSpriteLayout *sprite : _sprites) {
		sprite->deleteLater();
	}
	_sprites.clear();

	deleteAllCallback();
	_markers.clear();

	for (InGameScene::AnchorZone *zone : _anchorZones) {
		delete zone;
	}
	_anchorZones.clear();
}

float InGameScene::getHeadHorizontalRotation(Character *cter, const TeVector3f32 &vec) {
	//TeVector3f32 pos = cter->_model->position() - vec;
	error("TODO: Implement InGameScene::getHeadHorizontalRotation");
}

float InGameScene::getHeadVerticalRotation(Character *cter, const TeVector3f32 &vec) {
	//TeVector3f32 pos = cter->_model->position() - vec;
	error("TODO: Implement InGameScene::getHeadVerticalRotation");
}

Common::String InGameScene::imagePathMarker(const Common::String &name) {
	if (!isMarker(name))
		return Common::String();
	Game *game = g_engine->getGame();
	TeLayout *bg = game->gui3().layoutChecked("background");
	for (Te3DObject2 *child : bg->childList()) {
		TeSpriteLayout *spritelayout = dynamic_cast<TeSpriteLayout *>(child);
		if (spritelayout && spritelayout->name() == name) {
			return spritelayout->_tiledSurfacePtr->path().toString();
		}
	}
	return Common::String();
}

void InGameScene::initScroll() {
	_someScrollVector = TeVector2f32(0.5f, 0.0f);
}

bool InGameScene::isMarker(const Common::String &name) {
	for (const TeMarker &marker : _markers) {
		if (marker._name == name)
			return true;
	}
	return false;
}

bool InGameScene::isObjectBlocking(const Common::String &name) {
	for (const Common::String &b: _blockingObjects) {
		if (name == b)
			return true;
	}
	return false;
}

bool InGameScene::load(const Common::Path &path) {
	_actZones.clear();
	Common::File actzonefile;
	if (actzonefile.open(getActZoneFileName())) {
		if (Te3DObject2::loadAndCheckFourCC(actzonefile, "0TCA")) {
			uint32 count = actzonefile.readUint32LE();
			if (count > 1000000)
				error("Improbable number of actzones %d", count);
			_actZones.resize(count);
			for (unsigned int i = 0; i < _actZones.size(); i++) {
				_actZones[i].s1 = Te3DObject2::deserializeString(actzonefile);
				_actZones[i].s2 = Te3DObject2::deserializeString(actzonefile);
				for (int j = 0; j < 4; j++)
					TeVector2f32::deserialize(actzonefile, _actZones[i].points[j]);
				_actZones[i].flag1 = (actzonefile.readByte() != 0);
				_actZones[i].flag2 = true;
			}
		}
	}
	if (!_lights.empty()) {
		TeLight::disableAll();
		for (unsigned int i = 0; i < _lights.size(); i++) {
			_lights[i].disable(i);
		}
		_lights.clear();
	}
	_shadowLightNo = -1;

	const Common::Path lightspath = getLightsFileName();
	if (Common::File::exists(lightspath))
		loadLights(lightspath);

	if (!Common::File::exists(path))
		return false;

	TeScene::close();
	_loadedPath = path;
	Common::File scenefile;
	if (!scenefile.open(path))
		return false;

	uint32 ncameras = scenefile.readUint32LE();
	if (ncameras > 1024)
		error("Improbable number of cameras %d", ncameras);
	for (unsigned int i = 0; i < ncameras; i++) {
		TeIntrusivePtr<TeCamera> cam = new TeCamera();
		deserializeCam(scenefile, cam);
		cameras().push_back(cam);
	}

	uint32 nobjects = scenefile.readUint32LE();
	if (nobjects > 1024)
		error("Improbable number of objects %d", nobjects);
	for (unsigned int i = 0; i < nobjects; i++) {
		TeIntrusivePtr<TeModel> model = new TeModel();
		const Common::String modelname = Te3DObject2::deserializeString(scenefile);
		model->setName(modelname);
		const Common::String objname = Te3DObject2::deserializeString(scenefile);
		TePickMesh2 *pickmesh = new TePickMesh2();
		deserializeModel(scenefile, model, pickmesh);
		if (modelname.contains("Clic")) {
			_hitObjects.push_back(model);
			// TODO: double-check this, probably right?
			model->setVisible(false);
			model->setColor(TeColor(0, 0xff, 0, 0xff));
			models().push_back(model);
			pickmesh->setName(modelname);
		} else {
			delete pickmesh;
			if (modelname.substr(0, 2) != "ZB") {
				if (objname.empty()) {
					warning("[InGameScene::load] Unknown type of object named : %s", modelname.c_str());
				} else {
					InGameScene::Object obj;
					obj._name = objname;
					obj._model = model;
					_objects.push_back(obj);
					model->setVisible(false);
					models().push_back(model);
				}
			}
		}
	}

	uint32 nfreemovezones = scenefile.readUint32LE();
	if (nfreemovezones > 1024)
		error("Improbable number of free move zones %d", nfreemovezones);
	for (unsigned int i = 0; i < nfreemovezones; i++) {
		TeFreeMoveZone *zone = new TeFreeMoveZone();
		TeFreeMoveZone::deserialize(scenefile, *zone, &_blockers, &_rectBlockers, &_actZones);
		_freeMoveZones.push_back(zone);
		zone->setVisible(false);
	}

	uint32 ncurves = scenefile.readUint32LE();
	if (ncurves > 1024)
		error("Improbable number of curves %d", ncurves);
	for (unsigned int i = 0; i < ncurves; i++) {
		TeIntrusivePtr<TeBezierCurve> curve = new TeBezierCurve();
		TeBezierCurve::deserialize(scenefile, *curve);
		curve->setVisible(true);
		_bezierCurves.push_back(curve);
	}

	uint32 ndummies = scenefile.readUint32LE();
	if (ndummies > 1024)
		error("Improbable number of dummies %d", ndummies);
	for (unsigned int i = 0; i < ndummies; i++) {
		InGameScene::Dummy dummy;
		TeVector3f32 vec;
		TeQuaternion rot;
		dummy._name = Te3DObject2::deserializeString(scenefile);
		TeVector3f32::deserialize(scenefile, vec);
		dummy._position = vec;
		TeQuaternion::deserialize(scenefile, rot);
		dummy._rotation = rot;
		TeVector3f32::deserialize(scenefile, vec);
		dummy._scale = vec;
		_dummies.push_back(dummy);
	}

	for (TeFreeMoveZone *zone : _freeMoveZones) {
		convertPathToMesh(zone);
	}
	_charactersShadow = new CharactersShadow();
	_charactersShadow->create(this);
	onMainWindowSizeChanged();

	// FIXME: For some reason the game never re-adds kate's model to the list here..
	// how does it ever get drawn???
	if (!findKate()) {
		models().push_back(_character->_model);
	}

	return true;
}

bool InGameScene::loadCharacter(const Common::String &name) {
	Character *c = character(name);
	if (!c) {
		c = new Character();
		if (!c->loadModel(name, false)) {
			delete c;
			return false;
		}
		models().push_back(c->_model);
		models().push_back(c->_shadowModel[0]);
		models().push_back(c->_shadowModel[1]);
		_characters.push_back(c);
	}
	c->_model->setVisible(true);
	return true;
}

bool InGameScene::loadLights(const Common::Path &path) {
	SceneLightsXmlParser parser;

	parser.setLightArray(&_lights);

	if (!parser.loadFile(path.toString()))
		error("InGameScene::loadLights: Can't load %s", path.toString().c_str());
	if (!parser.parse())
		error("InGameScene::loadLights: Can't parse %s", path.toString().c_str());

	_shadowColor = parser.getShadowColor();
	_shadowLightNo = parser.getShadowLightNo();
	_shadowFarPlane = parser.getShadowFarPlane();
	_shadowNearPlane = parser.getShadowNearPlane();
	_shadowFov = parser.getShadowFov();

	return true;
}

void InGameScene::loadMarkers(const Common::Path &path) {
	_markerGui.load(path);
	TeLayout *bg = _bgGui.layoutChecked("background");
	TeSpriteLayout *root = Game::findSpriteLayoutByName(bg, "root");
	bg->setRatioMode(TeILayout::RATIO_MODE_NONE);
	root->addChild(bg);
}

bool InGameScene::loadObject(const Common::String &name) {
	Object3D *obj = object3D(name);
	if (!obj) {
		obj = new Object3D();
		if (!obj->loadModel(name)) {
			delete obj;
			return false;
		}
		models().push_back(obj->model());
		_object3Ds.push_back(obj);
	}
	obj->model()->setVisible(true);
	return true;
}

void InGameScene::loadObjectMaterials(const Common::String &name) {
	error("TODO: InGameScene::loadObjectMaterials");
}

void InGameScene::loadObjectMaterials(const Common::String &path, const Common::String &name) {
	error("TODO: InGameScene::loadObjectMaterials");
}

bool InGameScene::loadPlayerCharacter(const Common::String &name) {
	if (_character == nullptr) {
		_character = new Character();
		if (!_character->loadModel(name, true)) {
			_playerCharacterModel.release();
			return false;
		}

		_playerCharacterModel = _character->_model;

		if (!findKate()) {
			models().push_back(_character->_model);
			models().push_back(_character->_shadowModel[0]);
			models().push_back(_character->_shadowModel[1]);
		}
	}

	_character->_model->setVisible(true);
	return true;
}

static Common::Path _sceneFileNameBase() {
	Game *game = g_engine->getGame();
	Common::Path retval("scenes");
	retval.joinInPlace(game->currentZone());
	retval.joinInPlace(game->currentScene());
	return retval;
}

Common::Path InGameScene::getLightsFileName() const {
	return _sceneFileNameBase().joinInPlace("lights.xml");
}

Common::Path InGameScene::getActZoneFileName() const {
	return _sceneFileNameBase().joinInPlace("actions.bin");
}

Common::Path InGameScene::getBlockersFileName() const {
	return _sceneFileNameBase().joinInPlace("blockers.bin");
}

void InGameScene::loadBlockers() {
	_blockers.clear();
	_rectBlockers.clear();
	const Common::Path blockersPath = getBlockersFileName();
	if (!Common::File::exists(blockersPath))
		return;

	Common::File blockersfile;
	if (!blockersfile.open(blockersPath)) {
		warning("Couldn't open blockers file %s.", blockersPath.toString().c_str());
		return;
	}

	bool hasHeader = Te3DObject2::loadAndCheckFourCC(blockersfile, "BLK0");
	if (!hasHeader)
		blockersfile.seek(0);

	uint32 nblockers = blockersfile.readUint32LE();
	_blockers.resize(nblockers);
	for (unsigned int i = 0; i < nblockers; i++) {
		_blockers[i]._s = Te3DObject2::deserializeString(blockersfile);
		TeVector2f32::deserialize(blockersfile, _blockers[i]._pts[0]);
		TeVector2f32::deserialize(blockersfile, _blockers[i]._pts[1]);
		_blockers[i]._x = 1;
	}

	if (hasHeader) {
		uint32 nrectblockers = blockersfile.readUint32LE();
		_rectBlockers.resize(nrectblockers);
		for (unsigned int i = 0; i < nrectblockers; i++) {
			_rectBlockers[i]._s = Te3DObject2::deserializeString(blockersfile);
			for (unsigned int j = 0; j < 4l; j++) {
				TeVector2f32::deserialize(blockersfile, _rectBlockers[i]._pts[j]);
			}
			_rectBlockers[i]._x = 1;
		}
	}
}

void InGameScene::loadBackground(const Common::Path &path) {
	_bgGui.load(path);
	TeLayout *bg = _bgGui.layout("background");
	TeLayout *root = _bgGui.layout("root");
	bg->setRatioMode(TeILayout::RATIO_MODE_NONE);
	root->setRatioMode(TeILayout::RATIO_MODE_NONE);
	TeCamera *wincam = g_engine->getApplication()->mainWindowCamera();
	bg->disableAutoZ();
	bg->setZPosition(wincam->_orthNearVal);

	for (auto layoutEntry : _bgGui.spriteLayouts()) {
		AnimObject *animobj = new AnimObject();
		animobj->_name = layoutEntry._key;
		animobj->_layout = layoutEntry._value;
		animobj->_layout->_tiledSurfacePtr->_frameAnim.onFinished().add<AnimObject>(animobj, &AnimObject::onFinished);
		if (animobj->_name != "root")
			animobj->_layout->setVisible(false);
		_animObjects.push_back(animobj);
	}
}

bool InGameScene::loadBillboard(const Common::String &name) {
	Billboard *b = billboard(name);
	if (b)
		return true;
	b = new Billboard();
	if (b->load(name)) {
		_billboards.push_back(b);
		return true;
	} else {
		delete b;
		return false;
	}
}

void InGameScene::loadInteractions(const Common::Path &path) {
	_hitObjectGui.load(path);
	TeLayout *bgbackground = _bgGui.layoutChecked("background");
	Game *game = g_engine->getGame();
	TeSpriteLayout *root = game->findSpriteLayoutByName(bgbackground, "root");
	TeLayout *background = _hitObjectGui.layoutChecked("background");
	// TODO: For all TeButtonLayout childen of background, call
	// setDoubleValidationProtectionEnabled(false)
	// For now our button doesn't implement that.
	background->setRatioMode(TeILayout::RATIO_MODE_NONE);
	root->addChild(background);
}

void InGameScene::moveCharacterTo(const Common::String &charName, const Common::String &curveName, float curveOffset, float curveEnd) {
	Character *c = character(charName);
	if (c != nullptr && c != _character) {
		Game *game = g_engine->getGame();
		if (!game->_movePlayerCharacterDisabled) {
			// TODO: c->field_0x214 = c->characterSettings()._cutSceneCurveDemiPosition;
			TeIntrusivePtr<TeBezierCurve> crve = curve(curveName);
			c->placeOnCurve(crve);
			c->setCurveOffset(curveOffset);
			const Common::String walkStartAnim = c->walkAnim(Character::WalkPart_Start);
			if (walkStartAnim.empty()) {
				c->setAnimation(c->walkAnim(Character::WalkPart_Loop), true, false, false, -1, 9999);
			} else {
				c->setAnimation(c->walkAnim(Character::WalkPart_Start), false, false, false, -1, 9999);
			}
			c->walkTo(curveEnd, false);
			error("TODO: Finish InGameScene::moveCharacterTo");
		}
	}
}

void InGameScene::onMainWindowSizeChanged() {
	TeCamera *mainWinCam = g_engine->getApplication()->mainWindowCamera();
	_viewportSize = mainWinCam->viewportSize();
	Common::Array<TeIntrusivePtr<TeCamera>> &cams = cameras();
	for (unsigned int i = 0; i < cams.size(); i++) {
		cams[i]->viewport(0, 0, _viewportSize.getX(), _viewportSize.getY());
	}
}

Object3D *InGameScene::object3D(const Common::String &name) {
	for (Object3D *obj : _object3Ds) {
		if (obj->model()->name() == name)
			return obj;
	}
	return nullptr;
}

TeFreeMoveZone *InGameScene::pathZone(const Common::String &name) {
	for (TeFreeMoveZone *zone: _freeMoveZones) {
		if (zone->name() == name)
			return zone;
	}
	return nullptr;
}

void InGameScene::reset() {
	if (_character)
		_character->setFreeMoveZone(nullptr);
	freeSceneObjects();
	_bgGui.unload();
	unloadSpriteLayouts();
	_markerGui.unload();
	_hitObjectGui.unload();
}

TeLight *InGameScene::shadowLight() {
	if (_shadowLightNo == -1) {
		return nullptr;
	}
	return &_lights[_shadowLightNo];
}

void InGameScene::setImagePathMarker(const Common::String &markerName, const Common::String &path) {
	if (!isMarker(markerName))
		return;

	Game *game = g_engine->getGame();
	TeLayout *bg = game->gui3().layoutChecked("background");

	for (Te3DObject2 *child : bg->childList()) {
		if (child->name() == markerName) {
			TeSpriteLayout *sprite = dynamic_cast<TeSpriteLayout *>(child);
			if (sprite) {
				sprite->load(path);
				sprite->_tiledSurfacePtr->_frameAnim._loopCount = -1;
				sprite->play();
			}
		}
	}
}

void InGameScene::setPositionCharacter(const Common::String &charName, const Common::String &freeMoveZoneName, const TeVector3f32 &position) {
	Character *c = character(charName);
	if (!c) {
		warning("[SetCharacterPosition] Character not found %s", charName.c_str());
	} else if (c == _character && c->positionFlag()) {
		c->setFreeMoveZoneName(freeMoveZoneName);
		c->setPositionCharacter(position);
		c->setPositionFlag(false);
		c->setNeedsSomeUpdate(true);
	} else {
		c->stop();
		TeFreeMoveZone *zone = pathZone(freeMoveZoneName);
		if (!zone) {
			warning("[SetCharacterPosition] PathZone not found %s", freeMoveZoneName.c_str());
			for (TeFreeMoveZone *zone : _freeMoveZones)
				warning("zone: %s", zone->name().c_str());
			return;
		}
		TeIntrusivePtr<TeCamera> cam = currentCamera();
		zone->setCamera(cam, false);
		c->setFreeMoveZone(zone);
		SoundStep step = findSoundStep(freeMoveZoneName);
		c->setStepSound(step._stepSound1, step._stepSound2);
		bool correctFlag = true;
		const TeVector3f32 corrected = zone->correctCharacterPosition(position, &correctFlag, true);
		c->_model->setPosition(corrected);
		if (!correctFlag)
			error("[SetCharacterPosition] Warning : The character is not above the ground %s", charName.c_str());
	}
}

void InGameScene::setStep(const Common::String &scene, const Common::String &step1, const Common::String &step2) {
	SoundStep ss;
	ss._stepSound1 = step1;
	ss._stepSound2 = step2;
	_soundSteps[scene] = ss;
}

void InGameScene::setVisibleMarker(const Common::String &markerName, bool val) {
	if (!isMarker(markerName))
		return;

	error("TODO: Implement InGameScene::setVisibleMarker");
}

void InGameScene::unloadCharacter(const Common::String &name) {
	if (_character && _character->_model->name() == name) {
		_character->removeAnim();
		_character->deleteAnim();
		_character->deleteAllCallback();
		// TODO: deleteLater() something here..
		_character = nullptr;
	}
	for (unsigned int i = 0; i < _characters.size(); i++) {
		Character *c = _characters[i];
		if (c && c->_model->name() == name) {
			c->removeAnim();
			c->deleteAnim();
			c->deleteAllCallback();
			// TODO: deleteLater() something here..
			_characters.remove_at(i);
			break;
		}
	}
}

void InGameScene::unloadObject(const Common::String &name) {
	error("TODO: InGameScene::unloadObject");
}

void InGameScene::unloadSpriteLayouts() {
	for (auto *animobj : _animObjects) {
		delete animobj;
	}
	_animObjects.clear();
}

void InGameScene::update() {
	Game *game = g_engine->getGame();
	if (_bgGui.loaded()) {
		_bgGui.layoutChecked("background")->setZPosition(0.0f);
	}
	if (_character) {
		// TODO: Do some stuff for character here.
	}
	for (Character *c : _characters) {
		// TODO: Something with other characters.
	}
	// TODO: some other stuff with callbacks and spritelayouts here

	TeScene::update();

	float waitTime = _waitTimeTimer.timeFromLastTimeElapsed();
	if (_waitTime != -1.0 && waitTime > _waitTime) {
		bool resumed = false;
		for (unsigned int i = 0; i < game->yieldedCallbacks().size(); i++) {
			Game::YieldedCallback &yc = game->yieldedCallbacks()[i];
			if (yc._luaFnName == "OnWaitFinished") {
				TeLuaThread *thread = yc._luaThread;
				game->yieldedCallbacks().remove_at(i);
				thread->resume();
				resumed = true;
			}
		}
		if (!resumed)
			game->luaScript().execute("OnWaitFinished");
	}

	for (Object3D *obj : _object3Ds) {
		// TODO: something with object3ds
	}
}


bool InGameScene::AnimObject::onFinished() {
	error("TODO: Implement InGameScene::AnimObject::onFinished");
}

} // end namespace Tetraedge
