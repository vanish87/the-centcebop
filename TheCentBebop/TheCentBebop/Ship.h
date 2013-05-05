// <author>Yuan Li</author>
// <email>yuan.li@nyu.edu</email>
// <date>2013-01-10</date>
// <summary>Spaceship</summary>
#pragma once

#include "PreDec.h"
#include "D3DModel.h"
#include "Math.h"

#include "Cannon.h"

class Ship
{
public:
	Ship(MocapGE::D3DModel* model);
	~Ship(void);

	void Update();
	MocapGE::float3 GetDir();
	float GetPHI(){return phi_;};
	float GetTheta(){return theta_;};
	MocapGE::float3 GetPos(){return pos_;};
	void SetPos(MocapGE::float3 pos){pos_ = pos;};
	MocapGE::float3 GetUp(){return up_;};
	void Accelerating();
	void Deccelerating();
	void TurnLeft();
	void TurnRight();
	void HeadUp();
	void HeadDown();
	void Stabling(bool dir);
	MocapGE::D3DModel* GetModel();

	void AddCannon(Cannon* cannon);
private:

	MocapGE::D3DModel* model_;
	std::vector<Cannon*> cannons_;

	float acceleration_;
	float speed_;
	int hp_;
	float phi_;//UD 
	float theta_;//LR theta =0 => towards +X-Axis
	MocapGE::float3 pos_;
	MocapGE::float3 up_;

	const float MAX_SPEED;
	const float TURNING_DEGREE;
};

