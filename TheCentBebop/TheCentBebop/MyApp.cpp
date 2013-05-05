#include "MyApp.h"
#include "DeferredRendering\Context.h"
#include "DeferredRendering\D3DModel.h"
#include "DeferredRendering\Timer.h"
using namespace MocapGE;

MyApp::MyApp(void): App("The CentBebop")
{
}


MyApp::~MyApp(void)
{
}

void MyApp::InitObjects()
{
	//set up lights
	for(int i = -30; i < 0 ; ++i)
	{
		point_light_ = new PointLight();
		point_light_->SetPos(float3(50 + i*30 ,10, -20));	
		point_light_->SetColor(float4((200 + i)/255.0f, (100 - i)/255.0f, (100 + i)/255.0f, 1.0f));
		point_light_->SetRange(50);
		point_light_->AddToScene();
	}

	for(int i = -30; i < 0 ; ++i)
	{
		point_light_ = new PointLight();
		point_light_->SetPos(float3(50 + i*30 ,10, 80));	
		point_light_->SetColor(float4((200 + i)/255.0f, (100 - i)/255.0f, (100 + i)/255.0f, 1.0f));
		point_light_->SetRange(50);
		point_light_->AddToScene();
	}

	spot_light_ = new SpotLight();
	spot_light_->SetPos(float3(50, 200, 0));
	spot_light_->SetDir(float3(0,10,0) - float3(50, 500, 0));
	spot_light_->SetInnerAngle(Math::PI / 6);
	spot_light_->SetOuterAngle(Math::PI / 4);
	spot_light_->AddToScene();

	float4x4 mat,trans;
	D3DModel *model = new D3DModel();
	model->LoadFile("../Media/sponza.dae");
	model->LoadShaderFile("FxFiles/DeferredLighting.cso");
	Math::Scale(mat, 0.5);
	model->SetModelMatrix(mat);
	ship_ = new SceneObject(model);
	ship_->AddToScene();

	model = new D3DModel();
	model->LoadFile("../Media/spacecraft_new.dae");
	model->LoadShaderFile("FxFiles/DeferredLighting.cso");
	Math::Scale(mat, 10);
	Math::Translate(trans,0,10,0);
	model->SetModelMatrix(trans * mat);
	ship_ = new SceneObject(model);
	ship_->AddToScene();

	timer_ = new Timer();
	timer_->Retart();

	first_person_ = false;
	pitch_angle_ = 0;
	speed_ = 2.5;
	Camera* camera = Context::Instance().AppInstance().GetCamera();
	cam_pos_ = float3(82.2,270.87,-67.49);
	cam_look_ = float3(81.78,270.16,-66.94);
	camera->SetView(cam_pos_, cam_look_, float3(0,1,0));

	sky_= new D3DSkyDome("../Media/universe.dds");
	sky_->LoadShaderFile("FxFiles/SkyDome.cso");
	SceneObject* sky_scene = new SceneObject(sky_);
	sky_scene->AddToScene();
}

void MyApp::ReleaseObjects()
{

}

void MyApp::Update()
{
	//std::cout<<timer_->Time()<<"\r";
	if(first_person_)
	{
		Camera* camera = Context::Instance().AppInstance().GetCamera();
		camera->SetView(cam_pos_, cam_look_, float3(0,1,0));

		float4x4 mat,trans, rotate;
		Math::Scale(mat, 30);
		Math::Translate(trans,0,10,0);
		Math::YRotation(rotate,Math::PI/2 *Math::Cos(timer_->Time()/1000.0f));
		//ship_->GetRenderElement()->SetModelMatrix(rotate * trans * mat);
	}
	//std::cout<<spot_light_->GetPos().x()<<"\r";
	spot_light_->SetDir(float3(0,-Math::Abs(Math::Sin(timer_->Time()/10000.0f)),Math::Cos(timer_->Time()/10000.0f)));
}

void MyApp::OnKeyDown( WPARAM key_para )
{
	float3 pos = point_light_->GetPos();	
	switch (key_para)
	{
	case 'F':
		first_person_ = true;
		break;
	case 'I':
		pos.x()++;
		break;
	case 'K':
		pos.x()--;
		break;
	case 'J':
		pos.z()++;
		break;
	case 'L':
		pos.z()--;
		break;

	default:
		break;
	}
	if(first_person_)
	{
		Camera* camera = Context::Instance().AppInstance().GetCamera();
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
}

void MyApp::OnMouseDown( WPARAM mouse_para, int x, int y )
{

}

void MyApp::OnMouseMove( WPARAM mouse_para, int x, int y )
{

}

void MyApp::OnMouseUp( WPARAM mouse_para, int x, int y )
{

}

int main()
{
	Context::Instance().LoadConfig("E:\\12Fall\\Independent Study\\IS-Deferred Rendering\\IS-Deferred Rendering\\Config.xml");
	ContextConfig cfg = Context::Instance().GetConfigure();
	Context::Instance().Configure(cfg);

	MyApp app;
	app.Initialize();
	app.Run();

	return 0;
}