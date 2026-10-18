#include "Slic3r/App/Yoga/ColorPicker.hpp"

#include "Slic3r/Biz/I18N/I18N.hpp"

#include <array>
#include <cmath>

namespace Slic3r::App::Yoga {

namespace {

struct PaletteColor
{
    int r;
    int g;
    int b;
};

constexpr int palette_columns = 6;

// Basic filament colors, in the order they appear in the grid (row by row).
constexpr std::array<PaletteColor, 24> palette_colors{{
    {0xFF, 0xFF, 0xFF}, {0xBF, 0xBF, 0xBF}, {0x80, 0x80, 0x80}, {0x40, 0x40, 0x40}, {0x00, 0x00, 0x00}, {0xF2, 0xE6, 0xCC},
    {0xE5, 0x39, 0x35}, {0x8E, 0x1B, 0x1B}, {0xFA, 0x68, 0x31}, {0xFB, 0x8C, 0x00}, {0xFD, 0xD8, 0x35}, {0xD4, 0xAF, 0x37},
    {0x9C, 0xCC, 0x65}, {0x43, 0xA0, 0x47}, {0x1B, 0x5E, 0x20}, {0x00, 0xAC, 0xC1}, {0x29, 0xB6, 0xF6}, {0x1E, 0x5F, 0xD8},
    {0x0D, 0x2A, 0x6B}, {0x8E, 0x24, 0xAA}, {0xD8, 0x1B, 0x60}, {0xF4, 0x8F, 0xB1}, {0x6D, 0x4C, 0x41}, {0xD7, 0xB9, 0x8E},
}};

constexpr float swatch_size    = 26.f;
constexpr float swatch_spacing = 6.f;

bool is_same_color(const float rgb[4], const PaletteColor& color)
{
    return std::lround(rgb[0] * 255.f) == color.r && std::lround(rgb[1] * 255.f) == color.g
        && std::lround(rgb[2] * 255.f) == color.b;
}

} // namespace

ColorPicker::Callbacks& ColorPicker::callbacks()
{
    return m_callbacks;
}

ColorPicker::ColorPicker(const std::string& name)
{
    set_object_name(name.empty() ? "ColorPicker" : name);
}

void ColorPicker::render(const Vec2f& pos, const Vec2f& size)
{
    render_item_begin(pos, size);

    ImGui::SetCursorScreenPos(to_im(pos));
    ImGui::BeginChild(
        "##picker_area",
        to_im(ColorPicker::get_item_size()),
        false,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
    );

    if (wheel_visible()) {
        render_wheel();
    } else {
        render_palette();
    }

    ImGui::EndChild();

    render_item_end(pos, size);
}

void ColorPicker::render_wheel()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::PushStyleVar(
        ImGuiStyleVar_ItemSpacing,
        ImVec2(style.ItemSpacing.x, 8.0f)
    ); // vertical gap between items
    ImGui::PushStyleVar(
        ImGuiStyleVar_ItemInnerSpacing,
        ImVec2(style.ItemInnerSpacing.x, 6.0f)
    ); // inner gaps some widgets use

    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::ColorPicker4(object_name().c_str(), m_data, m_flags)) {
        if (m_callbacks.color_edited) {
            m_callbacks.color_edited(ImColor(m_data[0], m_data[1], m_data[2], m_data[3]));
        }
    }

    ImGui::PopStyleVar(2);
}

void ColorPicker::render_palette()
{
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(swatch_spacing, swatch_spacing));

    for (int i = 0; i < int(palette_colors.size()); ++i) {
        const PaletteColor& color = palette_colors[i];
        if (i % palette_columns != 0) {
            ImGui::SameLine();
        }

        ImGui::PushID(i);
        const ImColor im_color(color.r, color.g, color.b);
        const bool clicked = ImGui::ColorButton(
            "##swatch",
            im_color.Value,
            ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoTooltip,
            ImVec2(swatch_size, swatch_size)
        );
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("#%02X%02X%02X", color.r, color.g, color.b);
        }
        if (is_same_color(m_data, color)) {
            ImGui::GetWindowDrawList()
                ->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32_WHITE, 3.f, 0, 2.f);
        }
        ImGui::PopID();

        if (clicked) {
            set_color(im_color);
            if (m_callbacks.color_picked) {
                m_callbacks.color_picked(im_color);
            }
        }
    }

    ImGui::Spacing();
    if (ImGui::Button(Biz::_u8L("Custom").c_str(), ImVec2(-FLT_MIN, 0.f))) {
        m_show_wheel = true;
        set_style_dirty();
    }

    ImGui::PopStyleVar();
}

void ColorPicker::set_color(const ImColor& color)
{
    m_data[0] = color.Value.x;
    m_data[1] = color.Value.y;
    m_data[2] = color.Value.z;
    m_data[3] = color.Value.w;
}

ImGuiColorEditFlags ColorPicker::flags() const
{
    return m_flags;
}

void ColorPicker::set_flags(ImGuiColorEditFlags flags)
{
    if (m_flags != flags) {
        m_flags = flags;
        set_style_dirty();
    }
}

bool ColorPicker::palette_mode() const
{
    return m_palette_mode;
}

void ColorPicker::set_palette_mode(bool enabled)
{
    if (m_palette_mode != enabled) {
        m_palette_mode = enabled;
        m_show_wheel   = false;
        set_style_dirty();
    }
}

void ColorPicker::show_palette()
{
    if (m_palette_mode && m_show_wheel) {
        m_show_wheel = false;
        set_style_dirty();
    }
}

bool ColorPicker::wheel_visible() const
{
    return !m_palette_mode || m_show_wheel;
}

Vec2f ColorPicker::get_item_size()
{
    if (wheel_visible()) {
        return {200, 240};
    }
    const int rows = (int(palette_colors.size()) + palette_columns - 1) / palette_columns;
    return {200, rows * (swatch_size + swatch_spacing) + 40};
}

} // namespace Slic3r::App::Yoga
