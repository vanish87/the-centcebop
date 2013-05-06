// <author>Yuan Li</author>
// <email>yuan.li@nyu.edu</email>
// <date>2013-01-10</date>
// <summary>Picking test</summary>
#pragma once

#include <vector>
#include "DeferredRendering\PreDec.h"
#include "DeferredRendering\D3DModel.h"

class Picking
{
public:
	Picking(void);
	~Picking(void);

	//make all points in camera space
	//return true if model is picked, with intersected_point and intersected_normal of this point, return false otherwise
	bool GetIntersection( MocapGE::D3DModel* model, MocapGE::Viewport* viewport, MocapGE::float2 screen_point,
							MocapGE::float3& intersected_point, MocapGE::float3& intersected_normal = MocapGE::float3(0,0,0));

private:
	std::vector<std::vector<MocapGE::VertexType*>> vertice_cpu_;
	MocapGE::D3DModel* model_;
	std::vector<MocapGE::AABBox*> aabbs_;
};

