// Button.cpp created on 2018-03-21, part of XCI toolkit
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

#include "Button.h"
#include <xci/text/Markup.h>

namespace xci {
namespace widgets {

using namespace xci::graphics;
using namespace xci::text;


Button::Button(const std::string &string)
    : m_bg_rect(Color(10, 20, 40), theme().color(ColorId::Default))
{
    set_focusable(true);
    m_layout.set_default_font(&theme().font());
    Markup markup(m_layout);
    markup.parse(string);
}


void Button::set_decoration_color(const graphics::Color& fill,
                                  const graphics::Color& border)
{
    m_bg_rect = Shape(fill, border);
}


void Button::set_text_color(const graphics::Color& color)
{
    m_layout.set_default_color(color);
}


void Button::resize(View& view)
{
    m_layout.typeset(view);
    auto rect = m_layout.bbox();
    rect.enlarge(m_padding);
    set_size(rect.size());
    set_baseline(-rect.y);

    rect.x = 0;
    rect.y = 0;
    m_bg_rect.clear();
    m_bg_rect.add_rectangle(rect, m_outline_thickness);
}


void Button::draw(View& view, State state)
{
    auto rect = m_layout.bbox();
    if (state.focused) {
        m_bg_rect.set_outline_color(theme().color(ColorId::Focus));
    } else if (last_hover() == LastHover::Inside) {
        m_bg_rect.set_outline_color(theme().color(ColorId::Hover));
    } else {
        m_bg_rect.set_outline_color(theme().color(ColorId::Default));
    }
    m_bg_rect.draw(view, position());
    m_layout.draw(view, position() + Vec2f{m_padding - rect.x, m_padding - rect.y});
}


bool Button::key_event(View& view, const KeyEvent& ev)
{
    if (ev.action == Action::Press && ev.key == Key::Enter) {
        do_click(view);
        return true;
    }
    return false;
}


void Button::mouse_pos_event(View& view, const MousePosEvent& ev)
{
    do_hover(view, contains(ev.pos - view.offset()));
}


bool Button::mouse_button_event(View& view, const MouseBtnEvent& ev)
{
    if (ev.action == Action::Press &&
        ev.button == MouseButton::Left &&
        contains(ev.pos - view.offset()))
    {
        do_click(view);
        return true;
    }
    return false;
}


}} // namespace xci::widgets
