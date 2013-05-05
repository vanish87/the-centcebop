#include "ResembleState.h"

#include "Ship.h"
#include "Picking.h"
using namespace MocapGE;

//TODO :make it under some name space
const float ATTACH_DIS = 10.0f;

ResembleState::ResembleState(Ship* ship, PartList parts)
	:ship_(ship), parts_(parts)
{
	//set up init postion
	ship_pos_ = ship_->GetPos();
	cannon_pos_ = ship_pos_;
	parts_[0]->SetPos(cannon_pos_);
	parts_[1]->SetPos(cannon_pos_);
	for(size_t i =0; i< parts.size(); i ++)
	{
		picking_.push_back(new Picking());
	}
	ship_pick_ = new Picking();
}


ResembleState::~ResembleState(void)
{
}

void ResembleState::Update()
{
	float3 cam_tag_pos = ship_pos_ - ship_dir_ + ship_up_ * 3;
	Camera* camera = Context::Instance().AppInstance().GetCamera();
	float3 cam_pos = camera->GetPos();
	float3 cam_up = camera->GetUp();

	float3 dis = cam_tag_pos - cam_pos;
	if(first_flag_ && Math::Dot(dis, dis) > 0.001 )
	{
		//set camera from third person view to default assembling position
		dis = Math::Normalize(dis);
		camera->SetView(cam_pos + dis* 0.01, ship_pos_, cam_up);
	}
	else
	{
		first_flag_ = false;
	}
}

void ResembleState::OnKeyDown( WPARAM key_para )
{
	switch (key_para) 
	{ 
	case VK_CONTROL:
		{
			//allows player th rotate view while do assembling by pressing the LEFT CTRL
			//lock Cursor into the center of screen
			if(GetKeyState(VK_LCONTROL) & 0x8000)
			{
				left_ctr_down_ = true;
				Context::Instance().AppInstance().LockCursor(true);
			}
			break;
		}
	}
}

void ResembleState::OnKeyUp( WPARAM key_para )
{
	switch (key_para) 
	{ 
	case VK_CONTROL:
		{
			if(!(GetKeyState(VK_LCONTROL) & 0x8000))
			{
				left_ctr_down_ = false;
				Context::Instance().AppInstance().LockCursor(false);
			}
			break;
		}
	}
}

void ResembleState::OnMouseDown( WPARAM mouse_para, int x, int y )
{
	float2 screen_pos(x, y);
	float3 picked_pos;
	Viewport* viewport = Context::Instance().GetRenderFactory().GetRenderEngine().CurrentFrameBuffer()->GetViewport();
	mouse_down_= true;

	for(size_t i = 0; i < parts_.size(); i++)
	{
		//find out which part is picked
		picked_ = picking_[i]->GetIntersection(parts_[i]->GetModel(), viewport, screen_pos, picked_pos);
		if(picked_)
		{
			picked_model_ = parts_[i]->GetModel();
			picked_index_= i;
			break;
		}
	}

}

void ResembleState::OnMouseUp( WPARAM mouse_para, int x, int y )
{
	mouse_down_= false;
	picked_ = false;
	picked_index_ = -1;
}

void ResembleState::OnMouseMove( WPARAM mouse_para, int x, int y )
{
	float2 screen_pos(x, y);
	Viewport* viewport = Context::Instance().GetRenderFactory().GetRenderEngine().CurrentFrameBuffer()->GetViewport();
	//rotate the view by pressing LEFT CTRL
	if(left_ctr_down_)
	{
		float2 delta = screen_pos - pre_pos;
		Camera* camera = Context::Instance().AppInstance().GetCamera();
		float3 cam_pos = camera->GetPos();
		float3 at = camera->GetLookAt();
		float3 up = camera->GetUp();

		float3 dir =  at - cam_pos;
		float3 inv_dir =  cam_pos - at;
		float3 new_dir;

		float4x4 rot_mat;
		Math::Identity(rot_mat);
		if(delta.x() > 1)
			Math::YRotation(rot_mat, -cam_speed_deg_);
		else
			if(delta.x() < 1)
				Math::YRotation(rot_mat, cam_speed_deg_);

		new_dir = Math::TransformNormal(inv_dir, rot_mat);

		float3 left = Math::Cross(dir,up);
		left = Math::Normalize(left);
		if(delta.y() > 1 )
			Math::RotationAxis(rot_mat, left, -cam_speed_deg_);
		else
			if(delta.y() < 1 )
				Math::RotationAxis(rot_mat, left, cam_speed_deg_);
		new_dir = Math::TransformNormal(new_dir, rot_mat);

		float3 trans_vec = new_dir - inv_dir;

		cam_pos = cam_pos + trans_vec;

		//make mouse move not too sensitive
		if(delta.x()!=0 && delta.y()!=0)
			camera->SetView(cam_pos, at, up);

		int2 center = Context::Instance().AppInstance().GetWindow().GetCenter();
		pre_pos = float2(center.x(), center.y());
		return;
	}
	else
	{
		//if mouse dragging
		if(mouse_down_)
		{
			float3 picked_pos;
			float3 picked_normal;
			if(picked_)
			{
				cannon_pos_ = parts_[picked_index_]->GetPos();
				ship_pos_ = ship_->GetPos();
				cannon_pos_ = ship_pos_ + cannon_pos_;
				float4x4 model_matrix = picked_model_->GetModelMatrix();
				float2 delta = screen_pos - pre_pos;
				Camera* camera = Context::Instance().AppInstance().GetCamera();
				float3 pos = camera->GetPos();
				float3 at = camera->GetLookAt();
				float3 dir =  at - pos;
				float3 up = camera->GetUp();
				float3 left = Math::Cross(dir,up);
				left = Math::Normalize(left);
				float3 right = float3(-left.x(), -left.y(), -left.z());
				float3 real_up = Math::Cross(dir, right);
				real_up = Math::Normalize(real_up);
				float3 down = float3(-real_up.x(), -real_up.y(), -real_up.z());
				if(delta.x() > 0)
					cannon_pos_ = cannon_pos_ + right / 50;
				else
					if(delta.x() < 0)
						cannon_pos_ = cannon_pos_ + left / 50;
				if(delta.y() > 0 )
					cannon_pos_ = cannon_pos_ + down/ 50;
				else
					if(delta.y() < 0 )
						cannon_pos_ = cannon_pos_ + real_up/ 50;

				//make cannon move on the plane that parallel to the view plane
				cannon_pos_ = cannon_pos_ + float3(delta.x()/320, delta.y()/200, 0) ;
				Math::Translate(model_matrix, cannon_pos_.x(), cannon_pos_.y(), cannon_pos_.z());
				picked_model_->SetModelMatrix(picked_model_->GetModelMatrix() *model_matrix);

				if (picked_index_ !=-1)
				{
					Cannon* cannon = parts_[picked_index_];
					cannon->SetPos(cannon_pos_ - ship_pos_);
					//if picking a cannon then test if mouse pointing to the ship
					//if YES, get the accurate point of ship and attach cannon on the surface of ship
					if(ship_pick_->GetIntersection(ship_->GetModel(), viewport ,screen_pos, picked_pos, picked_normal))
					{
						cannon->SetPos(picked_pos - ship_pos_);
						Attach(ship_, cannon, picked_pos, picked_normal);
					}
				}
			}

		}
	}
	
	pre_pos = float2(x,y);
}

void ResembleState::Attach( Ship* ship_, Cannon* picked_cannon ,float3 picked_pos, float3 picked_normal )
{
	float4x4 rot_matrix, world_matrix;
	float3 cannon_up = float3(0,1,0);
	float3 axis = Math::Cross(picked_normal, cannon_up);
	float theta = Math::ArcCos(Math::Dot(picked_normal, cannon_up));
	//get the rotation matrix so that we can make a fine attachment.
	Math::RotationAxis(rot_matrix, axis, theta);
	Math::Translate(world_matrix, picked_pos.x(), picked_pos.y(), picked_pos.z());
	D3DModel* cannon_model = picked_cannon->GetModel();
	cannon_model->SetModelMatrix(rot_matrix * world_matrix);

	ship_->AddCannon(picked_cannon);

}



void ResembleState::Init(PartList parts)
{
	//init with the parts that near the ship when entering Assembling Mode
	parts = parts;
	ship_dir_ = ship_->GetDir();
	ship_pos_ = ship_->GetPos();
	ship_up_  = ship_->GetUp();

	first_flag_ = true;
	left_ctr_down_ = false;

	cam_speed_deg_ = Math::PI /180;
	mouse_down_ = false;
	picked_ = false;
	picked_index_ = -1;
}
