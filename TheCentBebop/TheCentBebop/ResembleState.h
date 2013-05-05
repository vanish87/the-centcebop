// <author>Yuan Li</author>
// <email>yuan.li@nyu.edu</email>
// <date>2013-01-10</date>
// <summary>Assembling Mode</summary>
#pragma once

#include "PreDec.h"
#include "GameState.h"
#include "Vector.h"

#include "D3DModel.h"

class Ship;
class Cannon;
class Picking;

class ResembleState : public MocapGE::GameState
{
public:
	typedef std::vector<Cannon*> PartList;

	ResembleState(Ship* ship, PartList parts);
	~ResembleState(void);
	
	virtual void Update();

	virtual void OnKeyDown( WPARAM key_para );

	virtual void OnKeyUp( WPARAM key_para );

	virtual void OnMouseDown( WPARAM mouse_para, int x, int y );

	virtual void OnMouseUp( WPARAM mouse_para, int x, int y );

	virtual void OnMouseMove( WPARAM mouse_para, int x, int y );
	void Attach( Ship* ship_, Cannon* picked_cannon, MocapGE::float3 picked_pos, MocapGE::float3 picked_normal);
	void Init(PartList parts);

private:
	Ship* ship_;

	MocapGE::float3 ship_dir_;
	MocapGE::float3 ship_pos_;
	MocapGE::float3 ship_up_ ;

	MocapGE::float3 cannon_pos_;

	bool first_flag_;
	bool left_ctr_down_;

	float cam_speed_deg_;

	bool mouse_down_;
	MocapGE::float2 pre_pos;

	std::vector<Picking*> picking_;
	Picking* ship_pick_;
	bool picked_;
	size_t picked_index_;
	MocapGE::D3DModel* picked_model_;
	PartList parts_;

};
