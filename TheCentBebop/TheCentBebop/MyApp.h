#ifndef MYAPP_H_
#define MYAPP_H_

#pragma once


#include "DeferredRendering\App.h"
#include "DeferredRendering\PreDec.h"
#include "DeferredRendering\D3DSkyDome.h"
#include "GamingState.h"
#include "ResembleState.h"

class MyApp : public MocapGE:: App
{
public:
	MyApp(void);
	~MyApp(void);

	void InitObjects();
	void ReleaseObjects();
	void Update();

	virtual void OnKeyDown( WPARAM key_para );

	virtual void OnMouseDown( WPARAM mouse_para, int x, int y );
	virtual void OnMouseMove( WPARAM mouse_para, int x, int y );
	virtual void OnMouseUp( WPARAM mouse_para, int x, int y );


private:
	typedef std::vector<Cannon*> PartList;

	MocapGE::PointLight* point_light_;
	MocapGE::SpotLight* spot_light_;

	MocapGE::SceneObject* ship_;

	MocapGE::Timer* timer_;

	//shy dome for universe background
	MocapGE::D3DSkyDome* sky_;





	GamingState* gaming_;
	ResembleState* resembling_;
};

#endif
