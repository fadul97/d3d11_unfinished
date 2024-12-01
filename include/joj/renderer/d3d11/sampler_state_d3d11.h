#ifndef JOJ_D3D11_SAMPLER_STATE_H
#define JOJ_D3D11_SAMPLER_STATE_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "error_code.h"
#include <d3d11.h>

namespace joj
{
    class JAPI D3D11SamplerState
    {
    public:
        D3D11SamplerState();
        ~D3D11SamplerState();

        ErrorCode create_anisotropic_state(ID3D11Device* device);
        void bind_anisotropic_state(ID3D11DeviceContext* device_context, u32 start_slot, u32 num_samplers);
    private:
        ID3D11SamplerState* m_anisotropic;
    };
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_SAMPLER_STATE_H