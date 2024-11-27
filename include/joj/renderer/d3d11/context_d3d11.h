#ifndef JOJ_D3D11_CONTEXT_H
#define JOJ_D3D11_CONTEXT_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

#if JPLATFORM_WINDOWS

#include "error_code.h"
#include "platform/win32/window_win32.h"
#include <dxgi1_6.h>
#include <d3d11.h>
#include <dxgidebug.h>

namespace joj
{
    class JAPI D3D11Context
    {
    public:
        D3D11Context();
        ~D3D11Context();

        ErrorCode create();
        void destroy();

        ID3D11Device* get_device() const;
        ID3D11DeviceContext* get_device_context() const;

        IDXGIFactory6* get_factory() const;
        ID3D11Debug* get_debug() const;

    private:
        ID3D11Device* m_device;
        ID3D11DeviceContext* m_device_context;
        IDXGIFactory6* m_factory;
        ID3D11Debug* m_debug;

#ifdef JOJ_DEBUG_MODE
        void log_hardware_info();
#endif
    };

    inline ID3D11Device* D3D11Context::get_device() const
    { return m_device; }

    inline ID3D11DeviceContext* D3D11Context::get_device_context() const
    { return m_device_context; }

    inline IDXGIFactory6* D3D11Context::get_factory() const
    { return m_factory; }

    inline ID3D11Debug* D3D11Context::get_debug() const
    { return m_debug; }
}

#endif // JPLATFORM_WINDOWS

#endif // JOJ_D3D11_CONTEXT_H