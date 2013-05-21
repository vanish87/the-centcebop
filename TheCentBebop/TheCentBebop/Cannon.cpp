#include "Cannon.h"
using namespace MocapGE;

Cannon::Cannon(D3DModel* model, SceneObject* bullet)
	:model_(model), bullet_(bullet), fired(false), hit_(false), bullet_speed_(0.1), hp_(100), dir_(float3(0,0,-1)), theta_(Math::PI), pos_(float3(0,0,0))
{
	bullet_->SetVisiable(false);
	bullet_state_ = WAIT;
}


Cannon::~Cannon(void)
{
	delete model_;
	delete bullet_;
}

void Cannon::Update(float4x4 parent )
{
	float4x4 cannon_mat, yrot_mat, xrot_mat;
	Math::Identity(cannon_mat);
	float theta = Math::ArcTan(Math::Sqrt(dir_.x()*dir_.x()+ dir_.y()*dir_.y())/ dir_.z());
	float phi = Math::ArcTan(dir_.y() / dir_.x());
	Math::YRotation(yrot_mat, theta_);
	Math::XRotation(xrot_mat, phi);
	Math::Translate(cannon_mat, pos_.x(), pos_.y(), pos_.z());

	float4x4 scale_mat;
	Math::Scale(scale_mat, 0.1);

	cannon_mat = yrot_mat * scale_mat * local_rot_ * cannon_mat * parent;
	model_->SetModelMatrix(cannon_mat);
	bullet_->GetRenderElement()->SetModelMatrix(cannon_mat);

	//std::cout<<bullet_state_<<"\r";
	switch (bullet_state_)
	{
	case Cannon::FIRE:
		bullet_->SetVisiable(true);
		bullet_pos_ = bullet_pos_ + bullet_dir_ * bullet_speed_;
		Math::Translate(cannon_mat, bullet_pos_.x(), bullet_pos_.y(), bullet_pos_.z());
		cannon_mat = yrot_mat * scale_mat * local_rot_ * cannon_mat * parent;
		bullet_->GetRenderElement()->SetModelMatrix(cannon_mat);
		if(Math::Dot(bullet_pos_,bullet_pos_) > 500)
		{
			bullet_state_ = WAIT;
		}
		break;
	case Cannon::WAIT:
		bullet_->SetVisiable(false);
		bullet_pos_ = pos_;
		bullet_dir_ = dir_;
		break;
	default:
		break;
	}
}

float3 Cannon::GetPos()
{
	return pos_;
}

void Cannon::SetPos(float3 pos )
{
	pos_ = pos;
	//std::cout<<pos_.x()<<"c "<<pos_.y()<<" "<<pos_.z()<<"\r";
}

void Cannon::SetRotation( float3 axis, float angle )
{
	float4x4 tran_mat;
	Math::RotationAxis(local_rot_, axis, angle);
}

void Cannon::Left()
{
	theta_ -= 0.1;
}

void Cannon::Right()
{
	theta_ += 0.1;
}

void Cannon::SetTheta( float theta )
{
	theta_ = theta;
	dir_.z() = Math::Sin(Math::PI - theta);
	dir_.x() = Math::Cos(Math::PI - theta);
}

void Cannon::Fire()
{
	if(bullet_state_ != FIRE)
	{
		bullet_dir_ = dir_;
		bullet_state_ = FIRE;
	}
}


