#include "Ship.h"
using namespace MocapGE;

Ship::Ship(MocapGE::D3DModel* model)
	:model_(model), acceleration_(0.1), speed_(0), hp_(100), phi_(0), theta_(0), pos_(float3(0.0f, 0.0f, 0.0f)), up_(float3(0, 1, 0))
	,MAX_SPEED(1), TURNING_DEGREE(0.01)
{
}


Ship::~Ship(void)
{
}

MocapGE::float3 Ship::GetDir()
{
	float dx = Math::Cos(theta_);
	float dy = Math::Sin(phi_);
	float dz = Math::Sin(theta_);

	return float3(dx, dy, dz);
}

void Ship::Update()
{
	//update ship and cannons that attached to it
	float3 ship_dir = GetDir();
	pos_ = pos_ + ship_dir * speed_;
	float4x4 ship_mat, yrotation_mat, xrotation_mat, translate_mat;


	float4x4 scale_mat;
	Math::Scale(scale_mat, 0.3);
	Math::Identity(ship_mat);
	Math::Identity(yrotation_mat);
	Math::Identity(xrotation_mat);
	Math::Identity(translate_mat);
	Math::YRotation(yrotation_mat, -theta_);
	Math::ZRotation(xrotation_mat, phi_);
	Math::Translate(translate_mat, pos_.x(), pos_.y(), pos_.z());

	ship_mat =  scale_mat*xrotation_mat * yrotation_mat  * translate_mat;
	model_->SetModelMatrix(ship_mat);
	for(size_t i =0; i< cannons_.size(); i++)
	{
		cannons_[i]->Update(ship_mat);
	}
}

void Ship::Accelerating()
{
	if(speed_ < MAX_SPEED)
		speed_ +=acceleration_;
}

void Ship::Deccelerating()
{
	if(speed_ > 0)
		speed_ -= acceleration_*0.3;
	else
		speed_ = 0;
}

void Ship::TurnLeft()
{
	theta_ += TURNING_DEGREE;
}

void Ship::TurnRight()
{
	theta_ -= TURNING_DEGREE;
}

void Ship::HeadUp()
{
	if(phi_ < Math::PI/2)
		phi_+= TURNING_DEGREE;
}

void Ship::HeadDown()
{
	if(phi_ > -Math::PI/4)
		phi_-= TURNING_DEGREE;
}

void Ship::Stabling( bool dir )
{
	if(dir)
	{
		if(phi_ > 0.0f) phi_-=TURNING_DEGREE* 0.01;
	}
	else
	{
		if(phi_ < 0.0f) phi_ += TURNING_DEGREE* 0.01;
	}
}

D3DModel* Ship::GetModel()
{
	return model_;
}

void Ship::AddCannon( Cannon* cannon )
{
	//TODO : use <set> for better performence
	bool exsited = false;
	for(size_t i =0; i < cannons_.size(); i++)
	{
		if(cannons_[i] == cannon)
		{
			exsited = true;
			break;
		}
	}
	if(!exsited)
		cannons_.push_back(cannon);
}


