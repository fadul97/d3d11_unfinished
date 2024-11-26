#include "renderer/d3d11/3d/scene_d3d11.h"

#if JPLATFORM_WINDOWS

#include "engine.h"
#include "logger.h"

joj::D3D11Scene::D3D11Scene()
{
}

joj::D3D11Scene::~D3D11Scene()
{
}

void joj::D3D11Scene::update(
	const f32 dt,
	const FreeCamera* camera,
	const JFloat4x4 proj_float4x4,
	D3D11ConstantBuffer& constant_buffer)
{
	// Set constants
	joj::JMatrix4x4 view = camera->get_view_mat();
	joj::JMatrix4x4 proj = DirectX::XMLoadFloat4x4(&proj_float4x4);

	for (const auto& obj : m_objects)
	{
		joj::JMatrix4x4 world = DirectX::XMLoadFloat4x4(&obj->get_world_float4x4());
		joj::JMatrix4x4 wvp = world * view * proj;

		// TODO: If scene is used, update code below => It doesn't work with the new CBPerObject struct
		JDEBUG("Update D3D11Scene::update method.");
		CBPerObject cbPerObject;
		XMStoreFloat4x4(&cbPerObject.world, XMMatrixTranspose(wvp));
		constant_buffer.update(joj::Engine::s_renderer->get_device_context(), cbPerObject);

		joj::Engine::s_renderer->get_device_context()->DrawIndexed(
			obj->get_index_count(),
			obj->get_index_location(),
			obj->get_vertex_location()
		);
	}
}

void joj::D3D11Scene::draw()
{
	for (const auto& obj : m_objects)
	{
		joj::Engine::s_renderer->get_device_context()->DrawIndexed(
			obj->get_index_count(),
			obj->get_index_location(),
			obj->get_vertex_location()
		);
	}
}

#endif // JPLATFORM_WINDOWS