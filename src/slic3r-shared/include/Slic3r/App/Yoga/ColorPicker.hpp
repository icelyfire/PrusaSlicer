#pragma once

#include "Slic3r/App/Yoga/Item.hpp"

namespace Slic3r::App::Yoga {

class ColorPicker : public Item
{
public:
    explicit ColorPicker(const std::string& name = {});

    struct Callbacks
    {
        std::function<void(const ImColor& color)> color_edited{nullptr};
        /// Called when a swatch of the palette is clicked (palette mode only).
        std::function<void(const ImColor& color)> color_picked{nullptr};
    };

    void render(const Vec2f& pos, const Vec2f& size) override;

    Callbacks& callbacks();

    void set_color(const ImColor& color);

    ImGuiColorEditFlags flags() const;

    void set_flags(ImGuiColorEditFlags flags);

    /// In palette mode a grid of basic colors is shown first, the full color wheel
    /// is only available behind the "Custom…" button.
    bool palette_mode() const;
    void set_palette_mode(bool enabled);

    /// Goes back from the color wheel to the palette (palette mode only).
    void show_palette();

protected:
    Vec2f get_item_size() override;

private:
    void render_palette();
    void render_wheel();
    bool wheel_visible() const;

private:
    Callbacks m_callbacks;

    bool m_palette_mode = false;
    bool m_show_wheel   = false;

    float m_data[4]             = {0, 0, 0, 1};
    ImGuiColorEditFlags m_flags = ImGuiColorEditFlags_NoAlpha
        | ImGuiColorEditFlags_NoLabel
        | ImGuiColorEditFlags_PickerHueWheel
        | ImGuiColorEditFlags_NoSidePreview
        | ImGuiColorEditFlags_NoSmallPreview
        | ImGuiColorEditFlags_DisplayRGB
        | ImGuiColorEditFlags_DisplayHex;
};

} // namespace Slic3r::App::Yoga
