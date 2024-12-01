#ifndef JOJ_D3D11_INPUT_LAYOUT_H
#define JOJ_D3D11_INPUT_LAYOUT_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include <d3d11.h>
#include <vector>
#include "error_code.h"
#include "shader_d3d11.h"

namespace joj
{
    class JAPI D3D11InputLayout
    {
    public:
        D3D11InputLayout();
        ~D3D11InputLayout();

        void describe_default_geometry_layout();

        ErrorCode create(ID3D11Device* device, D3D11Shader& shader);

        void bind(ID3D11DeviceContext* device_context);

    private:
        ID3D11InputLayout* m_layout;
        std::vector<D3D11_INPUT_ELEMENT_DESC> m_desc;
    };
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_INPUT_LAYOUT_H