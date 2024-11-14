/**
 * @file error_code.h
 * @author Leonardo Fadul ( TODO: todo@domain.com)
 * @brief This file contains the error codes used throughout the engine,
 * and some helper functions for debug purposes.
 * @version 0.1
 * @date 2024-11-13
 * 
 * @copyright TODO: Copyright (c) 2024
 * 
 * REFERENCES: Travis Vroman (https://travisvroman.com/) - Kohi Game Engine (https://github.com/travisvroman/kohi)
 * 
 */

#ifndef JOJ_ERROR_CODE_H
#define JOJ_ERROR_CODE_H

#define JOJ_ENGINE_IMPLEMENTATION
#include "defines.h"

namespace joj
{
    /** @brief Represents the error codes for the engine. */
    enum class ErrorCode
    {
        /** @brief OK code, should be used when the operation was successful. */
        OK,

        /** @brief Generic error code, should be used when the operation failed. */
        FAILED,

        /** @brief Window Error code, should be used for invalid window handle. */
        ERR_WINDOW_HANDLE,

        /** @brief Window Error code, should be used for invalid window registration. */
        ERR_WINDOW_REGISTRATION,

        /** @brief Window Error code, should be used when window fails to adjust. */
        ERR_WINDOW_ADJUST,

        /** @brief Window Error code, should be used when window fails to move. */
        ERR_WINDOW_MOVE,

        /** @brief Window Error code, should be used when the device context fails to create. */
        ERR_WINDOW_DEVICE_CONTEXT,

        /** @brief Window Error code, should be used when the window fails to get its rect. */
        ERR_WINDOW_RECT,

        /** @brief Window Error code, should be used when the window fails to get its client rect. */
        ERR_WINDOW_CLIENT_RECT,

        /** @brief D3D11 Context Error code, should be used when creation of DXGIFactory2 fails. */
        ERR_CONTEXT_D3D11_DXGI_FACTORY2_CREATION,

        /** @brief D3D11 Context Error code, should be used when creation of D3D11Device fails. */
        ERR_CONTEXT_D3D11_DEVICE_CREATION,

        /** @brief D3D11 Context Error code, should be used when creation of Warp Adapter fails. */
        ERR_CONTEXT_D3D11_WARP_ADAPTER_CREATION,

        /** @brief D3D11 Context Error code, should be used when querying ID3D11Debug fails. */
        ERR_CONTEXT_D3D11_QUERY_INTERFACE_ID3D11_DEBUG,

        /** @brief D3D11 Context Error code, should be used when querying IDXGIDevice fails. */
        ERR_CONTEXT_D3D11_QUERY_INTERFACE_IDXGI_DEVICE,

        /** @brief D3D11 Context Error code, should be used when getting parent of IDXGIAdapter fails. */
        ERR_CONTEXT_D3D11_GET_PARENTOF_IDXGI_ADAPTER,

        /** @brief D3D11 Context Error code, should be used when getting parent of IDXGIFactory fails. */
        ERR_CONTEXT_D3D11_GET_PARENTOF_IDXGI_FACTORY,
    };

    /** @brief Get the index of the ErrorCode.
     *  @param err The error code to convert.
     *  @return The integer index of the error code.
     */
    i32 err_to_int(ErrorCode err);

    /** @brief Get the name of the ErrorCode.
     *  @param err The error code to convert.
     *  @return The string representation of the error code.
     */
    const char* err_to_str(ErrorCode err);
}

#endif // JOJ_ERROR_CODE_H