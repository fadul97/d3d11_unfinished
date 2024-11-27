#ifndef MESH_VIEW_DEMO_H
#define MESH_VIEW_DEMO_H

// ---------------------------------------------------------------------------------

#include "joj/application/app.h"
#include "joj/renderer/d3d11/3d/basic_model_d3d11.h"
#include "joj/systems/camera/free_camera.h"
#include "joj/systems/light/light.h"
#include <renderer/d3d11/shader_d3d11.h>
#include <renderer/d3d11/constant_buffer_d3d11.h>

// ---------------------------------------------------------------------------------

class MeshViewDemo : public joj::App
{
public:
	void init();
	void update(const f32 dt);
	void draw();
	void shutdown();

	void build_shaders();
	void build_vertex_layout();
	void build_model();
	void build_constant_buffers();

private:
	joj::D3D11TextureManager mTexMgr;
	joj::D3D11BasicModel* mBaseModel;

	joj::JFloat3 mOriginalLightDir[3];
	joj::DirectionalLight mDirLights[3];

	joj::FreeCamera m_cam;
	b8 firstPerson = true;
	b8 hideCursor = false;
	i32 centerX = 400;
	i32 centerY = 300;
	b8 fast = false;
	f32 speed = 50.0f;
	b8 is_wireframe = false;

	joj::JFloat4x4 mView = joj::float4x4_identity();
	joj::JFloat4x4 mProj = joj::float4x4_identity();

	std::vector<D3D11_INPUT_ELEMENT_DESC> m_input_desc;
	ID3D11InputLayout* m_input_layout = nullptr;

	joj::D3D11Shader m_shader;

	joj::D3D11ConstantBuffer m_object_cb;
	joj::D3D11ConstantBuffer m_frame_cb;
};

#endif // MESH_VIEW_DEMO_H