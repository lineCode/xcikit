// Window.h created on 2018-03-04, part of XCI toolkit

#ifndef XCI_GRAPHICS_WINDOW_H
#define XCI_GRAPHICS_WINDOW_H

#include <xci/graphics/View.h>

#include <xci/util/geometry.h>
using xci::util::Vec2u;
using xci::util::Vec2f;

#include <string>

namespace xci {
namespace graphics {


class WindowImpl;

class Window: public View {
public:
    Window();
    ~Window() override;

    void create(const Vec2u& size, const std::string& title);
    void display();

    void draw(const Sprite& sprite, const Vec2f& pos) override;

private:
    WindowImpl* m_impl;
};


}} // namespace xci::graphics

#endif // XCI_GRAPHICS_WINDOW_H