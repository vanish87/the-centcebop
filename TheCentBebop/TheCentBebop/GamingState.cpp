#include "GamingState.h"
#include "D3DSkyDome.h"
#include "Timer.h"

using namespace MocapGE;
GamingState::GamingState(void)
{
	ship_model = new D3DModel();
	ship_model->LoadFile("..\\Media\\plane_ssdo.dae");
	//ship_model->LoadPomTexture("..\\Media\\pom.png");
	ship_model->LoadShaderFile("..\\FxFiles\\DeferredLighting.fxo");
	scene_ship_ = new SceneObject(ship_model);
	scene_ship_->AddToScene();

	ship_ = new Ship(ship_model);
	ship_->SetPos(float3(0, 0, 0));

	float3 ship_dir = ship_->GetDir();
	float3 ship_pos = ship_->GetPos();
	float3 ship_up = ship_->GetUp();

	Camera* camera = Context::Instance().AppInstance().GetCamera();

	//set init pos of camera, make it from back and upper position of ship
	float3 cam_pos = ship_pos - ship_dir * 3 + ship_up;
	float3 cam_at = ship_pos + ship_dir * 2;
	//camera->SetView(cam_pos, cam_at, ship_up);
	camera->SetView(float3(-5,0,0), float3(0,0,0), float3(0,1,0));

	cannon_1 = new D3DModel();
	cannon_1->LoadFile("..\\Media\\gun2.dae");
	cannon_1->LoadShaderFile("..\\FxFiles\\DeferredLighting.fxo");
	scene_cannon1_ = new SceneObject(cannon_1);
	//scene_cannon1_->AddToScene();

	cannon_2 = new D3DModel();
	cannon_2->LoadFile("..\\Media\\gun2.dae");
	cannon_2->LoadShaderFile("..\\FxFiles\\DeferredLighting.fxo");
	scene_cannon2_ = new SceneObject(cannon_2);
	//scene_cannon2_->AddToScene();


	Cannon* cannon1 = new Cannon(cannon_1, cannon_1);
	resemble_parts_.push_back(cannon1);
	Cannon* cannon2 = new Cannon(cannon_2, cannon_2);
	resemble_parts_.push_back(cannon2);

	sky_= new D3DSkyDome("..\\Media\\universe.dds");
	sky_->LoadShaderFile("..\\FxFiles\\SkyDome.fxo");
	SceneObject* sky_scene = new SceneObject(sky_);
	//sky_scene->AddToScene();

	float4x4 sacle_mat, trans_mat;
	Math::Identity(sacle_mat);
	Math::Identity(trans_mat);

	pluto_ = new D3DModel();
	pluto_->LoadFile("..\\Media\\pluto.dae");
	pluto_->LoadShaderFile("..\\FxFiles\\DeferredLighting.fxo");
	Math::Scale(sacle_mat, 10);
	pluto_->SetModelMatrix(sacle_mat* trans_mat);
	scene_pluto_ = new SceneObject(pluto_);
	//scene_pluto_->AddToScene();

	jupiter_ = new D3DModel();
	jupiter_->LoadFile("..\\Media\\jupiter.dae");
	jupiter_->LoadShaderFile("..\\FxFiles\\DeferredLighting.fxo");
	Math::Translate(trans_mat, 400, 50, 0);
	Math::Scale(sacle_mat, 50);
	jupiter_->SetModelMatrix(sacle_mat* trans_mat);
	scene_jupiter_ = new SceneObject(jupiter_);
	//scene_jupiter_->AddToScene();

	sun_ = new D3DModel();
	sun_->LoadFile("..\\Media\\sun.dae");
	sun_->LoadShaderFile("..\\FxFiles\\DeferredLighting.fxo");
	Math::Scale(sacle_mat, 100);
	Math::Translate(trans_mat, 2000, 50, 0);
	sun_->SetModelMatrix(sacle_mat* trans_mat);
	scene_sun_ = new SceneObject(sun_);
	//scene_sun_->AddToScene();

	spacekey_down_ = false;
	wkey_down_ = false;
	skey_down_ = false;
	ship_pos = float3(0,0,0);

	plane_ = new D3DModel();
	//plane_->LoadFile("..\\Media\\test.dae");
	//plane_->LoadPomTexture("..\\Media\\pom.png");
	plane_->LoadShaderFile("..\\FxFiles\\DeferredLighting.fxo");
	Math::Scale(sacle_mat, 5);
	Math::Translate(trans_mat, 0, 0, 0);
	plane_->SetModelMatrix(sacle_mat* trans_mat);
	scene_plane_ = new SceneObject(plane_);
	//scene_plane_->AddToScene();
	angle_ = 0;

	timer_ = new Timer();
	timer_->Retart();
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
	//make spaceship return to even position
	if(!spacekey_down_)
		ship_->Deccelerating();
 	if(!skey_down_)
 		ship_->Stabling(false);
 	if(!wkey_down_)
 		ship_->Stabling(true);

	//ship_->Update();
	float3 ship_dir = ship_->GetDir();
	float3 ship_pos = ship_->GetPos();
	float3 ship_up = ship_->GetUp();

	Camera* camera = Context::Instance().AppInstance().GetCamera();



	//update third person camera
	float3 cam_pos = ship_pos - ship_dir  + ship_up;
	//std::cout<<cam_pos.x()<<" "<<cam_pos.y()<<" "<<cam_pos.z()<<"\r";
	float3 cam_at = ship_pos;
	//camera->SetView(cam_pos, cam_at, ship_up);

	float4x4 trans_mat,sacle_mat;
//	float3 pos = static_cast<SpotLight*>(Context::Instance().GetSceneManager().GetLights()[1])->GetPos();
	Math::Translate(trans_mat, 400 ,Math::Cos(timer_->Time()/5000)*50, 0);
	Math::Scale(sacle_mat, 50);
	jupiter_->SetModelMatrix(sacle_mat*trans_mat);
}

void GamingState::OnKeyDown( WPARAM key_para )
{
	switch (key_para) 
	{ 
	case 'A':
		{
			//ship_->TurnLeft();
			break;
		}
	case 'D':
		{
			//ship_->TurnRight();
			break;
		}
	case 'S':
		{
			//skey_down_ = true;
			//ship_->HeadUp();
			break;
		}
	case 'W':
		{
			//spacekey_down_ = true;
			//ship_->HeadDown();
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
