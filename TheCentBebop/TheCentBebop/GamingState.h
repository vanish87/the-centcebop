// <author>Yuan Li</author>
// <email>yuan.li@nyu.edu</email>
// <date>2013-01-10</date>
// <summary>Main game loop, player can drive his spaceship</summary>
#pragma once

#include "DeferredRendering\PreDec.h"
#include "DeferredRendering\GameState.h"

#include "DeferredRendering\D3DModel.h"
#include "DeferredRendering\D3DSkyDome.h"

#include "Picking.h"
#include "Ship.h"

class Cannon;

class GamingState : public MocapGE::GameState
{
public:
	GamingState(void);
	~GamingState(void);

	virtual void Update();

	virtual void OnKeyDown( WPARAM key_para );

	virtual void OnKeyUp(WPARAM key_para);

	virtual void OnMouseDown( WPARAM mouse_para, int x, int y );

	virtual void OnMouseUp( WPARAM mouse_para, int x, int y );

	virtual void OnMouseMove( WPARAM mouse_para, int x, int y );

	Ship* GetShip(){ return ship_;};
	std::vector<Cannon*> GetParts();
private:
	MocapGE::D3DModel* ship_model,*cannon_1, *cannon_2;
	MocapGE::SceneObject* scene_ship_, *scene_cannon1_, *scene_cannon2_;

	MocapGE::D3DModel *pluto_, *jupiter_, *sun_;
	MocapGE::SceneObject *scene_pluto_, *scene_jupiter_, *scene_sun_;

	MocapGE::D3DModel *plane_;
	MocapGE::SceneObject *scene_plane_;

	//shy dome for universe background
	MocapGE::D3DSkyDome* sky_;

	//stores parts that could be attach to ship;
	std::vector<Cannon*> resemble_parts_;

	Ship* ship_;
	bool spacekey_down_, wkey_down_, skey_down_;

	MocapGE::Timer* timer_;

	bool first_person_;
	MocapGE::float3 cam_pos_;
	MocapGE::float3 cam_look_;
	float pitch_angle_;
	float speed_;

	float angle_;

};

