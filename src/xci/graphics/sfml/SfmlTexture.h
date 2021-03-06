// SfmlTexture.h created on 2018-03-04, part of XCI toolkit

#ifndef XCI_GRAPHICS_SFML_TEXTURE_H
#define XCI_GRAPHICS_SFML_TEXTURE_H

#include <xci/graphics/Texture.h>

#include <SFML/Graphics/Texture.hpp>

namespace xci {
namespace graphics {


class SfmlTexture {
public:
    bool create(unsigned int width, unsigned int height);
    void update(const uint8_t* pixels, const Rect_u& region);

    Vec2u size() const;

    // access native handle
    const sf::Texture& sfml_texture() const { return m_texture; }

private:
    sf::Texture m_texture;
};


class Texture::Impl : public SfmlTexture {};


}} // namespace xci::graphics

#endif //XCI_GRAPHICS_SFML_TEXTURE_H
