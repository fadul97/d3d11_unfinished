#ifndef JOJ_D3D11_TEXTURE_MANAGER_H
#define JOJ_D3D11_TEXTURE_MANAGER_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "error_code.h"
#include <d3d11.h>
#include <map>
#include <string>

namespace joj
{
    class JAPI D3D11TextureManager
    {
    public:
        D3D11TextureManager();
        ~D3D11TextureManager();

        ErrorCode init(ID3D11Device* device);
        ID3D11ShaderResourceView* create_texture(std::wstring filename);

    private:
        D3D11TextureManager(const D3D11TextureManager& rhs);
        D3D11TextureManager& operator=(const D3D11TextureManager& rhs);

        ID3D11Device* m_device;
        std::map<std::wstring, ID3D11ShaderResourceView*> m_texture_SRV;
    };
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_TEXTURE_MANAGER_H