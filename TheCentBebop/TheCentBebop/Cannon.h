// <author>Yuan Li</author>
// <email>yuan.li@nyu.edu</email>
// <date>2013-01-10</date>
// <summary>Cannon design</summary>
#pragma once
#include "PreDec.h"
#include "D3DModel.h"

class Cannon
{
public:
	Cannon(MocapGE::D3DModel* model, MocapGE::D3DModel* bullet);
	~Cannon(void);

	void Update(MocapGE::float4x4 parent);
	void Fire();
	MocapGE::float3 GetPos();
	void SetPos( MocapGE::float3 pos );

	MocapGE::D3DModel* GetModel() { return model_;};
private:
	MocapGE::D3DModel* model_;
	MocapGE::D3DModel* bullet_;
	
	bool fired;
	bool hit_;
	float bullet_speed_;

	int hp_;
	MocapGE::float3 dir_;
	//relative pos to ship
	MocapGE::float3 pos_;

};

