// TextInput.cpp created on 2018-06-02, part of XCI toolkit
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

#include "TextInput.h"
#include <xci/util/string.h>

namespace xci {
namespace widgets {

using namespace xci::graphics;
using namespace xci::text;
using namespace xci::util;


TextInput::TextInput(const std::string& string)
    : m_text(string),
      m_bg_rect(Color(10, 20, 40), Color(180, 180, 0))
{
    m_layout.set_default_font(&theme().font());
}


void TextInput::set_decoration_color(const graphics::Color& fill,
                                     const graphics::Color& border)
{
    m_bg_rect = Shape(fill, border);
}


void TextInput::update(View& view)
{
    m_layout.clear();
    m_layout.add_word(m_text);
    m_layout.typeset(view);
    m_bg_rect.clear();
    m_bg_rect.add_rectangle(bbox(), m_outline_thickness);
}


void TextInput::draw(View& view)
{
    auto rect = m_layout.bbox();
    m_bg_rect.draw(view, {0, 0});
    m_layout.draw(view, position() + Vec2f{m_padding - rect.x, m_padding - rect.y});
}


void TextInput::handle(View& view, const CharEvent& ev)
{
    m_text += to_utf8(ev.code_point);
    update(view);
    view.refresh();
}


void TextInput::handle(View& view, const MouseBtnEvent& ev)
{
    Widget::handle(view, ev);
}


util::Rect_f TextInput::bbox() const
{
    auto rect = m_layout.bbox();
    rect.enlarge(m_padding);
    rect.x = position().x;
    rect.y = position().y;
    return rect;
}


}} // namespace xci::widgets
