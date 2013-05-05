#include "Picking.h"
#include "Context.h"
#include "Ray.h"
#include "AABBox.h"

#include <limits>

using namespace MocapGE;
Picking::Picking(void)
	:model_(nullptr)
{
}

Picking::~Picking(void)
{
}

bool Picking::GetIntersection( D3DModel* model, Viewport* viewport, float2 screen_point, float3& intersected_point, float3& intersected_normal)
{
	if(model_ == nullptr)
	{
		model_ = model;
		std::vector<Mesh*> meshes_ = model_->GetMesh();
		for(size_t i =0; i < meshes_.size() ; i++)
		{
			float4x4 model_matrix = meshes_[i]->GetModelMatrix();
			float4x4 inv_model_mat = Math::InverTranspose(model_matrix);
			RenderLayout* rl = meshes_[i]->GetRenderLayout();
			VertexType* vertice_gpu = meshes_[i]->GetVertex();

			std::vector<VertexType*> new_mesh;
			vertice_cpu_.push_back(new_mesh);
			size_t vsize = rl->GetIndexCount();

			for(size_t j =0; j < vsize; j++)
			{			
				vertice_cpu_[i].push_back(new VertexType());
				//make point to model space
				vertice_cpu_[i][j]->position = Math::Transform(vertice_gpu[j].position, model_matrix);
				vertice_cpu_[i][j]->normal = Math::TransformNormal(vertice_gpu[j].normal, inv_model_mat);
			}
			AABBox* aabb = new AABBox(vertice_cpu_[i], vsize);
			aabbs_.push_back(aabb);
		}
	}
	//make all points in camera space
	Camera* camera = viewport->GetCamera();
	float4x4 proj_matrix = camera->GetProjMatrix();
	float4x4 view_matrix = camera->GetViewMatirx();

	float4x4 world_matrix = model->GetModelMatrix();
	float4x4 inv_world_mat = Math::InverTranspose(world_matrix);
	//transform from screen space to view space
	float px = ((( 2.0f * screen_point.x()) / viewport->Width())  - 1.0f) / proj_matrix(0, 0);
	float py = (((-2.0f * screen_point.y()) / viewport->Height()) + 1.0f) / proj_matrix(1, 1);

	float4 pos = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 dir = float4(px, py, 1.0f, 0.0f);

	float3 dir_ = Math::Normalize(float3(dir.x(), dir.y(), dir.z()));

	//generate a picking ray in view space
	Ray* ray = new Ray(float3(pos.x(), pos.y(), pos.z()), float3(dir_.x(), dir_.y(), dir_.z()));

	for(int i =0; i < aabbs_.size(); i++)
	{
		
		float4x4 wv_matrix =  world_matrix * view_matrix;
		std::vector<VertexType*> mesh_vertice;
		for(int j = 0; j< vertice_cpu_[i].size(); j++)
		{
			mesh_vertice.push_back(new VertexType());
			//make vertex to view space
			mesh_vertice[j]->position = Math::Transform(vertice_cpu_[i][j]->position, wv_matrix);
			//make normals to world space
			mesh_vertice[j]->normal = Math::TransformNormal(vertice_cpu_[i][j]->normal, inv_world_mat);
		}
		//recalculate the aabb
		AABBox* aabb = new AABBox(mesh_vertice, mesh_vertice.size());

		float t_min = std::numeric_limits<float>::max();
		float t = 0;
		float3 min_normal;
		//if ray intersected with aabb, then calculate the accurate point
		if(Math::IntersectRayAABB(ray, aabb))
		{
			for(size_t j =0; j < mesh_vertice.size() -3; j+=3)
	 		{
				if(Math::IntersectRayTriangle(ray, mesh_vertice[j]->position, mesh_vertice[j+1]->position, mesh_vertice[j+2]->position, t))
				{
					if(t < t_min)
					{
						//get the closest point
						t_min = t;
						//normal in world space with average normals in 3 vertex
						min_normal = (mesh_vertice[j]->normal + mesh_vertice[j+1]->normal+ mesh_vertice[j+2]->normal) / 3;
					}
				}
			}
			if(t_min < std::numeric_limits<float>::max())
			{
				intersected_point = ray->Origin()+ ray->Direction() * t_min;
				float4x4 inv_view_mat = Math::Inverse(view_matrix);
				//make point to world space
				intersected_point = Math::Transform(intersected_point, inv_view_mat);
				intersected_normal =Math::Normalize(min_normal);
				return true;
			}
		}
		return false;
		
	}
	return false;
}
