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
	point_light_ = new PointLight();
	point_light_->SetPos(float3(0,100, 0));	
	point_light_->SetRange(500);
	point_light_->AddToScene();
	//set up lights
// 	for(int i = -30; i < 0 ; ++i)
// 	{
// 		point_light_ = new PointLight();
// 		point_light_->SetPos(float3(50 + i*30 ,10, -20));	
// 		point_light_->SetColor(float4((200 + i)/255.0f, (100 - i)/255.0f, (100 + i)/255.0f, 1.0f));
// 		point_light_->SetRange(50);
// 		point_light_->AddToScene();
// 	}
// 
// 	for(int i = -30; i < 0 ; ++i)
// 	{
// 		point_light_ = new PointLight();
// 		point_light_->SetPos(float3(50 + i*30 ,10, 80));	
// 		point_light_->SetColor(float4((200 + i)/255.0f, (100 - i)/255.0f, (100 + i)/255.0f, 1.0f));
// 		point_light_->SetRange(50);
// 		point_light_->AddToScene();
// 	}



	float4x4 mat,trans;
	D3DModel *model = new D3DModel();
// 	model->LoadFile("../Media/sponza.dae");
// 	model->LoadShaderFile("FxFiles/DeferredLighting.cso");
// 	Math::Scale(mat, 0.5);
// 	model->SetModelMatrix(mat);
// 	ship_ = new SceneObject(model);
// 	ship_->AddToScene();

	model = new D3DModel();
// 	model->LoadFile("../Media/spacecraft_new.dae");
// 	model->LoadShaderFile("FxFiles/DeferredLighting.cso");
// 	Math::Scale(mat, 10);
// 	Math::Translate(trans,0,10,0);
// 	model->SetModelMatrix(trans * mat);
// 	ship_ = new SceneObject(model);
// 	ship_->AddToScene();

	timer_ = new Timer();
	timer_->Retart();
// 
// 
// 	Camera* camera = Context::Instance().AppInstance().GetCamera();
// 	cam_pos_ = float3(82.2,270.87,-67.49);
// 	cam_look_ = float3(81.78,270.16,-66.94);
// 	camera->SetView(cam_pos_, cam_look_, float3(0,1,0));
	resembling_ = nullptr;

	//set current state to gaming
	gaming_ = new GamingState();
	Context::Instance().GetStateManager().ChangeState(gaming_, SOP_PUSH);
}

void MyApp::ReleaseObjects()
{

}

void MyApp::Update()
{
	//std::cout<<timer_->Time()<<"\r";

	//std::cout<<spot_light_->GetPos().x()<<"\r";
	//spot_light_->SetDir(float3(0,-Math::Abs(Math::Sin(timer_->Time()/10000.0f)),Math::Cos(timer_->Time()/10000.0f)));
}

void MyApp::OnKeyDown( WPARAM key_para )
{
	switch (key_para) 
	{ 
		//entering assembling mode
		case 'R':
			{
				if(Context::Instance().GetStateManager().CurrentState() == gaming_)
				{
					//std::cout<<"Assemble"<<std::endl;
					PartList parts = gaming_->GetParts();
					if(resembling_ == nullptr)
						resembling_ = new ResembleState(gaming_->GetShip(), parts);
					resembling_->Init(parts);					
					Context::Instance().GetStateManager().ChangeState(resembling_, SOP_PUSH);
					break;
				}

				if(Context::Instance().GetStateManager().CurrentState() == resembling_)
				{
					//std::cout<<"Gaming"<<std::endl;
					Context::Instance().GetStateManager().ChangeState(resembling_, SOP_POP);
					break;
				}
				break;
			}
		default:
			break;
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