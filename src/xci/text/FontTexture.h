// FontTexture.h created on 2018-03-02, part of XCI toolkit
// Copyright 2018 Radek Brich
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef XCI_TEXT_FONTTEXTURE_H
#define XCI_TEXT_FONTTEXTURE_H

#include <rbp/MaxRectsBinPack.h>
#include <xci/graphics/Renderer.h>
#include <xci/core/geometry.h>

namespace xci {
namespace text {

using core::Vec2u;
using core::Rect_u;
using graphics::TexturePtr;
using graphics::Renderer;


// Places glyphs into a texture

class FontTexture {
public:
    // The size is fixed. If the size request cannot be satisfied by HW,
    // smaller size will be used (HW maximum texture size).
    explicit FontTexture(unsigned int size=512,
                         Renderer& renderer = Renderer::default_instance());

    // non-copyable
    FontTexture(const FontTexture&) = delete;
    FontTexture& operator =(const FontTexture&) = delete;

    /// Insert a glyph bitmap into texture, return texture coords
    /// \param size     IN size of glyph bitmap
    /// \param pixels   IN data of glyph bitmap
    /// \param coords   OUT new texture coordinates for the glyph
    /// \returns        false when there is no space
    bool add_glyph(Vec2u size, const uint8_t* pixels, Rect_u& coords);

    // Get the whole texture (cut the coords returned by `insert`
    // and you'll get your glyph picture).
    TexturePtr& get_texture() { return m_texture; }

    void clear();

private:
    Renderer& m_renderer;
    TexturePtr m_texture;
    rbp::MaxRectsBinPack m_binpack;
};

}} // namespace xci::text

#endif // XCI_TEXT_FONTTEXTURE_H
