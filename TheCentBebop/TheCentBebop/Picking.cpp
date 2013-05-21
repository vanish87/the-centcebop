#include "Picking.h"
#include "DeferredRendering\Context.h"
#include "DeferredRendering\Ray.h"
#include "DeferredRendering\AABBox.h"

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
			VertexType* vertice_gpu = meshes_[i]->GetVertex();
			size_t vsize = meshes_[i]->GetVSize();
			float4x4 model_mat = meshes_[i]->GetModelMatrix();

			std::vector<VertexType*> new_mesh;
			vertice_cpu_.push_back(new_mesh);

			for(size_t j =0; j < vsize; j++)
			{			
				vertice_cpu_[i].push_back(new VertexType());
				//make point to model space
				vertice_cpu_[i][j]->position = vertice_gpu[j].position;
				vertice_cpu_[i][j]->normal = vertice_gpu[j].normal;

// 				float3 pos = vertice_gpu[j].position;
// 				std::cout<<pos.x()<<" "<<pos.y()<<" "<<pos.z()<<std::endl;
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
	//transform from screen space to view space
	float px = ((( 2.0f * screen_point.x()) / viewport->Width())  - 1.0f)/proj_matrix(0,0);
	float py = (((-2.0f * screen_point.y()) / viewport->Height()) + 1.0f)/proj_matrix(1,1);



	float4 pos = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float3 dir = float3(px, py, 1.0f);

	dir = Math::Normalize(dir);

	float4x4 inv_wv_mat = Math::Inverse(world_matrix * view_matrix);
	pos = Math::Transform(pos , inv_wv_mat);
	float4x4 inv_wv_mat_n = Math::InverTranspose(inv_wv_mat);
	dir = Math::TransformNormal(dir, inv_wv_mat_n);


	for(int i =0; i < aabbs_.size(); i++)
	{
		AABBox* aabb = aabbs_[i];
		std::vector<Mesh*> meshes_ = model_->GetMesh();
		float4x4 model_mat = meshes_[i]->GetModelMatrix();
		float4x4 inv_model_mat = Math::Inverse(model_mat);
		float4x4 inv_model_mat_n = Math::InverTranspose(inv_model_mat);

		pos = Math::Transform(pos , inv_model_mat);
		pos = pos / pos.w();
		dir = Math::TransformNormal(dir, inv_model_mat_n);
		dir = Math::Normalize(dir);
		//generate a picking ray in model space
		Ray ray = Ray(float3(pos.x(), pos.y(), pos.z()), dir);
		float t_min = std::numeric_limits<float>::max();
		float t = 0;
		float3 min_normal;
		//if ray intersected with aabb, then calculate the accurate point
		if(Math::IntersectRayAABB(&ray, aabb))
		{
			//PRINT("aabb picked");
			RenderLayout* rl = meshes_[i]->GetRenderLayout();
			MocapGE::uint32_t* ib = meshes_[i]->GetIndex();
			size_t i_size = rl->GetIndexCount();
			for(size_t j =0; j < i_size -3; j+=3)
	 		{
				if(Math::IntersectRayTriangle(&ray, vertice_cpu_[i][ib[j]]->position, vertice_cpu_[i][ib[j+1]]->position, vertice_cpu_[i][ib[j+2]]->position, t))
				{
					if(t < t_min)
					{
						//get the closest point
						t_min = t;
						//normal in model space with average normals in 3 vertex
						min_normal = (vertice_cpu_[i][ib[j]]->normal + vertice_cpu_[i][ib[j+1]]->normal+ vertice_cpu_[i][ib[j+2]]->normal) / 3;
					}
				}
			}
			if(t_min < std::numeric_limits<float>::max())
			{
				intersected_point = ray.Origin()+ ray.Direction() * t_min;
				float4x4 wm_mat = model_mat * world_matrix;
				float4x4 wm_mat_n = Math::InverTranspose(wm_mat);
				//make point to world space
				intersected_point = Math::Transform(intersected_point, wm_mat);
				min_normal = Math::Normalize(min_normal);

				intersected_normal = Math::TransformNormal(min_normal, wm_mat_n);
				return true;
			}
		}
		return false;
		
	}
	return false;
}
