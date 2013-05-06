#include "GamingState.h"
#include "DeferredRendering\D3DSkyDome.h"
#include "DeferredRendering\Timer.h"

using namespace MocapGE;
GamingState::GamingState(void)
{
	ship_model = new D3DModel();
	ship_model->LoadFile("../Media/spacecraft_new.dae");
	ship_model->LoadShaderFile("FxFiles/DeferredLighting.cso");
	scene_ship_ = new SceneObject(ship_model);
	scene_ship_->AddToScene();

	ship_ = new Ship(ship_model);
	ship_->SetPos(float3(0, 0, 0));

	float3 ship_dir = ship_->GetDir();
	float3 ship_pos = ship_->GetPos();
	float3 ship_up = ship_->GetUp();

	Camera* camera = Context::Instance().AppInstance().GetCamera();

	//set init pos of camera, make it from back and upper position of ship
	float3 cam_pos = ship_pos - ship_dir * 50 + ship_up *10;
	float3 cam_at = ship_pos + ship_dir * 20;
	camera->SetView(cam_pos, cam_at, ship_up);
	//camera->SetView(float3(-5,0,0), float3(0,0,0), float3(0,1,0));
	float4x4 sacle_mat, trans_mat;
	Math::Identity(sacle_mat);
	Math::Identity(trans_mat);

	cannon_1 = new D3DModel();
	cannon_1->LoadFile("../Media/gun2.dae");
	cannon_1->LoadShaderFile("FxFiles/DeferredLighting.cso");
	Math::Scale(sacle_mat, 1);
	Math::Translate(trans_mat, 0, 5, 5);
	cannon_1->SetModelMatrix(sacle_mat * trans_mat);
	scene_cannon1_ = new SceneObject(cannon_1);
	scene_cannon1_->AddToScene();

	cannon_2 = new D3DModel();
	cannon_2->LoadFile("../Media/gun2.dae");
	cannon_2->LoadShaderFile("FxFiles/DeferredLighting.cso");
	scene_cannon2_ = new SceneObject(cannon_2);
    scene_cannon2_->AddToScene();


	Cannon* cannon1 = new Cannon(cannon_1, cannon_1);
	resemble_parts_.push_back(cannon1);
	Cannon* cannon2 = new Cannon(cannon_2, cannon_2);
	resemble_parts_.push_back(cannon2);

	sky_= new D3DSkyDome("../Media/universe.dds");
	sky_->LoadShaderFile("FxFiles/SkyDome.cso");
	SceneObject* sky_scene = new SceneObject(sky_);
	sky_scene->AddToScene();



	pluto_ = new D3DModel();
	pluto_->LoadFile("../Media/pluto.dae");
	pluto_->LoadShaderFile("FxFiles/DeferredLighting.cso");
	Math::Scale(sacle_mat, 10);
	Math::Translate(trans_mat, 400, 50, -50);
	pluto_->SetModelMatrix(sacle_mat* trans_mat);
	scene_pluto_ = new SceneObject(pluto_);
	scene_pluto_->AddToScene();

	jupiter_ = new D3DModel();
	jupiter_->LoadFile("../Media/jupiter.dae");
	jupiter_->LoadShaderFile("FxFiles/DeferredLighting.cso");
	Math::Translate(trans_mat, 400, 100, 0);
	Math::Scale(sacle_mat, 50);
	jupiter_->SetModelMatrix(sacle_mat* trans_mat);
	scene_jupiter_ = new SceneObject(jupiter_);
	scene_jupiter_->AddToScene();

	sun_ = new D3DModel();
	sun_->LoadFile("../Media/sun.dae");
	sun_->LoadShaderFile("FxFiles/DeferredLighting.cso");
	Math::Scale(sacle_mat, 1);
	Math::Translate(trans_mat, 2000, 50, 0);
	sun_->SetModelMatrix(sacle_mat* trans_mat);
	scene_sun_ = new SceneObject(sun_);
	scene_sun_->AddToScene();

	spacekey_down_ = false;
	wkey_down_ = false;
	skey_down_ = false;
	ship_pos = float3(0,0,0);

	timer_ = new Timer();
	timer_->Retart();

	first_person_ = false;
	pitch_angle_ = 0;
	speed_ = 2.5;
}


GamingState::~GamingState(void)
{
	delete ship_model;
	delete cannon_1;
	delete cannon_2;
	delete sky_;
}

void GamingState::Update()
{
	if(first_person_)
	{
		Camera* camera = Context::Instance().AppInstance().GetCamera();
		camera->SetView(cam_pos_, cam_look_, float3(0,1,0));

		float4x4 mat,trans, rotate;
		Math::Scale(mat, 30);
		Math::Translate(trans,0,10,0);
		Math::YRotation(rotate,Math::PI/2 *Math::Cos(timer_->Time()/1000.0f));
		//ship_->GetRenderElement()->SetModelMatrix(rotate * trans * mat);

		float4x4 trans_mat,sacle_mat;
		//	float3 pos = static_cast<SpotLight*>(Context::Instance().GetSceneManager().GetLights()[1])->GetPos();
		Math::Translate(trans_mat, 0 ,Math::Cos(timer_->Time()/10000.0f)*10, 0);
		Math::Scale(sacle_mat, 1);
		cannon_2->SetModelMatrix(sacle_mat * trans_mat);
	}
	else
	{
		//make spaceship return to even position
		if(!spacekey_down_)
			ship_->Deccelerating();
		if(!skey_down_)
			ship_->Stabling(false);
		if(!wkey_down_)
			ship_->Stabling(true);

		ship_->Update();
		float3 ship_dir = ship_->GetDir();
		float3 ship_pos = ship_->GetPos();
		float3 ship_up = ship_->GetUp();

		Camera* camera = Context::Instance().AppInstance().GetCamera();



		//update third person camera
		float3 cam_pos = ship_pos - ship_dir* 50  + ship_up *10;
		//std::cout<<cam_pos.x()<<" "<<cam_pos.y()<<" "<<cam_pos.z()<<"\r";
		float3 cam_at = ship_pos + ship_dir * 20;
		camera->SetView(cam_pos, cam_at, ship_up);



	}
	
}

void GamingState::OnKeyDown( WPARAM key_para )
{
	switch (key_para)
	{
	case 'F':
		first_person_ = !first_person_;
		break;
	default:
		break;
	}
	if(first_person_)
	{
		Camera* camera = Context::Instance().AppInstance().GetCamera();
		cam_look_ = camera->GetLookAt();
		cam_pos_ = camera->GetPos();
		float3 up_ = camera->GetUp();
		float4x4 mat;

		float3 dir =  cam_look_ - cam_pos_;
		float3 left = Math::Cross(dir,up_);
		switch (key_para)
		{
		case 'E':
			left = Math::Normalize(left);

			pitch_angle_ = -Math::PI/180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformNormal(dir, mat);
			dir = Math::Normalize(dir);
			cam_look_ = dir + cam_pos_;
			break;
		case 'C':
			left = Math::Normalize(left);

			pitch_angle_ = Math::PI/180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformNormal(dir, mat);
			dir = Math::Normalize(dir);
			cam_look_ = dir + cam_pos_;
			break;
		case 'X':
			left = float3(0,1,0);

			pitch_angle_ = - Math::PI/180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformNormal(dir, mat);
			dir = Math::Normalize(dir);
			cam_look_ = dir + cam_pos_;
			break;
		case 'Z':
			left = float3(0,1,0);

			pitch_angle_ = Math::PI/180;
			Math::Identity(mat);
			Math::RotationAxis(mat, left, pitch_angle_);

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::TransformNormal(dir, mat);
			dir = Math::Normalize(dir);
			cam_look_ = dir + cam_pos_;
			break;
		case 'W':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(dir);
			dir = dir*speed_;
			cam_look_ = cam_look_ + dir;
			cam_pos_ = cam_pos_ + dir;
			break;
		case 'S':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(dir);
			dir = dir*speed_;
			cam_look_ = cam_look_ - dir;
			cam_pos_ = cam_pos_ - dir;
			break;

		case 'A':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(left);
			dir = dir*speed_;
			cam_look_ = dir + cam_look_;
			cam_pos_ = cam_pos_ + dir;
			break;
		case 'D':

			//up_ = Math::TransformNormal(up_, mat);
			dir = Math::Normalize(left);
			dir = dir*speed_;
			cam_look_ = cam_look_ - dir;
			cam_pos_ = cam_pos_ - dir;
			break;
		case 'P':
			speed_ +=0.1;
			break;		
		case 'O':
			speed_ -=0.1;
			break;

		default:
			break;
		}
	}
	else
	{
		switch (key_para) 
		{ 
		case 'A':
			{
				ship_->TurnLeft();
				break;
			}
		case 'D':
			{
				ship_->TurnRight();
				break;
			}
		case 'S':
			{
				skey_down_ = true;
				ship_->HeadUp();
				break;
			}
		case 'W':
			{
				spacekey_down_ = true;
				ship_->HeadDown();
				break;
			}
		case 'U':
			{
				float4x4 rotation_mat;
				Math::YRotation(rotation_mat, angle_+=0.1);
				plane_->SetModelMatrix(rotation_mat);
				break;
			}
		case 'I':
			{
				float4x4 rotation_mat;
				Math::YRotation(rotation_mat, angle_-=0.1);
				plane_->SetModelMatrix(rotation_mat);
				break;
			}

		case VK_SPACE:
			{
				spacekey_down_ = true;
 				ship_->Accelerating();
				break;
			}
		}
	}
	
}

void GamingState::OnKeyUp(WPARAM key_para)
{
	switch (key_para) 
	{ 
	case 'S':
		{
			skey_down_ = false;
			break;
		}
	case 'W':
		{
			spacekey_down_ = false;
			break;
		}
	case VK_SPACE:
		{
			spacekey_down_ = false;
			break;
		}
	}
}

void GamingState::OnMouseDown( WPARAM mouse_para, int x, int y )
{	

}

void GamingState::OnMouseUp( WPARAM mouse_para, int x, int y )
{

}

void GamingState::OnMouseMove( WPARAM mouse_para, int x, int y )
{

}

std::vector<Cannon*> GamingState::GetParts()
{
	return resemble_parts_;
}
