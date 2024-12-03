#include "renderer/d3d11/animation_frame_d3d11.h"

#if JPLATFORM_WINDOWS

#include <logger.h>
#include <renderer/d3d11/DDSTextureLoader11.h>

joj::D3D11AnimationFrame::D3D11AnimationFrame()
    : m_sprite_sheet(nullptr), m_x0(0.0f), m_y0(0.0f),
    m_x1(0.0f), m_y1(0.0f)
{
}

joj::D3D11AnimationFrame::D3D11AnimationFrame(D3D11SpriteSheet& sprite_sheet, u32 x, u32 y)
    : m_sprite_sheet(&sprite_sheet)
{
    m_x0 = static_cast<f32>(x * sprite_sheet.get_cell_width()) / static_cast<f32>(sprite_sheet.get_width());
    m_y0 = static_cast<f32>(y * sprite_sheet.get_cell_height()) / static_cast<f32>(sprite_sheet.get_height());
    m_x1 = static_cast<f32>((x + 1) * sprite_sheet.get_cell_width()) / static_cast<f32>(sprite_sheet.get_width());
    m_y1 = static_cast<f32>((y + 1) * sprite_sheet.get_cell_height()) / static_cast<f32>(sprite_sheet.get_height());
}

joj::D3D11AnimationFrame::~D3D11AnimationFrame()
{
}



#endif // JPLATFORM_WINDOWS