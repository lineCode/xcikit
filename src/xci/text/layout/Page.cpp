// Page.cpp created on 2018-03-18, part of XCI toolkit
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

#include <xci/text/layout/Page.h>
#include <xci/graphics/View.h>
#include <xci/graphics/Sprites.h>
#include <xci/graphics/Shape.h>
#include <xci/core/string.h>
#include <xci/core/log.h>

#include <cassert>

namespace xci::text { class Style; }

namespace xci::text::layout {

using namespace xci::core::log;
using xci::graphics::View;
using xci::graphics::Color;
using xci::core::Rect_f;
using xci::core::Vec2f;
using xci::core::to_utf32;


Word::Word(Page& page, const std::string& string)
    : m_string(string), m_style(page.style())
{
    auto* font = m_style.font();
    if (!font) {
        assert(!"Font is not set!");
        return;
    }

    auto pxr = page.target_framebuffer_ratio();
    font->set_size(unsigned(m_style.size() / pxr.y));
    m_baseline = font->ascender() * pxr.y;
    const auto font_height = m_baseline - font->descender() * pxr.y;

    // Measure word (metrics are affected by string, font, size)
    core::Vec2f pen;
    m_bbox = {0, 0 - m_baseline, 0, font_height};
    for (CodePoint code_point : to_utf32(m_string)) {
        auto glyph = font->get_glyph(code_point);
        if (glyph == nullptr)
            continue;

        // Expand text bounds by glyph bounds
        core::Rect_f rect{pen.x ,
                          pen.y - m_baseline,
                          glyph->advance() * pxr.x,
                          font_height};

        m_bbox.extend(rect);
        pen.x += rect.w;
    }

    // Check line end
    if (page.width() > 0.0 && page.pen().x + pen.x > page.width()) {
        page.finish_line();
    }

    // Set position according to pen
    m_pos = page.pen();
    m_bbox.x += m_pos.x;
    m_bbox.y += m_pos.y;

    page.advance_pen(pen);
}


void Word::draw(graphics::View& target, const core::Vec2f& pos) const
{
    auto* font = m_style.font();
    if (!font) {
        assert(!"Font is not set!");
        return;
    }

    auto pxf = target.framebuffer_ratio();
    font->set_size(unsigned(m_style.size() / pxf.y));

    if (target.has_debug_flag(View::Debug::WordBBox)) {
        graphics::Shape bbox(Color(0, 150, 0), Color(50, 250, 50));
        bbox.add_rectangle(m_bbox, 1 * pxf.x);
        bbox.draw(target, pos);
    }

    bool show_bboxes = target.has_debug_flag(View::Debug::GlyphBBox);

    graphics::Sprites sprites(font->get_texture(), m_style.color());
    graphics::Shape bboxes(Color(150, 0, 0), Color(250, 50, 50));

    Vec2f pen;
    for (CodePoint code_point : to_utf32(m_string)) {
        auto glyph = font->get_glyph(code_point);
        if (glyph == nullptr)
            continue;

        Rect_f rect{pen.x + glyph->bearing().x * pxf.x,
                    pen.y - glyph->bearing().y * pxf.y,
                    glyph->size().x * pxf.x,
                    glyph->size().y * pxf.y};
        sprites.add_sprite(rect, glyph->tex_coords());
        if (show_bboxes)
            bboxes.add_rectangle(rect, 1 * pxf.x);

        pen.x += glyph->advance() * pxf.x;
    }

    auto p = pos + m_pos;
    if (show_bboxes)
        bboxes.draw(target, p);
    sprites.draw(target, p);

    if (target.has_debug_flag(View::Debug::WordBasePoint)) {
        auto pxr = target.screen_ratio();
        graphics::Shape basepoint(Color(150, 0, 255));
        basepoint.add_rectangle({-pxr.x, -pxr.y, 2 * pxr.x, 2 * pxr.y});
        basepoint.draw(target, p);
    }
}


const core::Rect_f& Line::bbox() const
{
    if (m_bbox_valid)
        return m_bbox;
    // Refresh
    bool first = true;
    for (auto& word : m_words) {
        if (first) {
            m_bbox = word->bbox();
            first = false;
        } else {
            m_bbox.extend(word->bbox());
        }
    }
    // Add padding
    if (m_padding != 0) {
        m_bbox.x -= m_padding;
        m_bbox.y -= m_padding;
        m_bbox.w += 2 * m_padding;
        m_bbox.h += 2 * m_padding;
    }
    m_bbox_valid = true;
    return m_bbox;
}


float Line::baseline() const
{
    if (m_words.empty())
        return 0;
    return m_words[0]->baseline();
}


void Span::add_word(Word& word)
{
    // Add word to current line
    auto& line = m_parts.back();
    line.add_word(word);
}


void Span::adjust_style(std::function<void(Style& word_style)> fn_adjust)
{
    for (Line& part : m_parts) {
        for (Word* word : part.words()) {
            fn_adjust(word->style());
        }
    }
}


Page::Page()
{
    m_lines.emplace_back();
}


core::Vec2f Page::target_framebuffer_ratio() const
{
    if (!m_target)
        return {1.0f/300, 1.0f/300};

    return m_target->framebuffer_ratio();
}


void Page::clear()
{
    m_pen = {0.f, 0.f};
    m_pen_offset = {0.f, 0.f};
    m_style.clear();
    m_width = 0.f;
    m_alignment = Alignment::Left;
    m_tab_stops.clear();
    m_lines.clear();
    m_lines.emplace_back();
    m_spans.clear();
    m_words.clear();
}


void Page::add_tab_stop(float x)
{
    m_tab_stops.push_back(x);
    std::sort(m_tab_stops.begin(), m_tab_stops.end());
}


void Page::finish_line()
{
    // Add new line
    if (!m_lines.back().is_empty())
         m_lines.emplace_back();
    // Add new part to open spans
    for (auto& span_pair : m_spans) {
        auto& span = span_pair.second;
        if (span.is_open()) {
            span.add_part();
        }
    }
    // Move pen
    m_pen.x = 0;
    advance_line();
}


void Page::advance_line(float lines)
{
    auto* font = m_style.font();
    auto pxr = target_framebuffer_ratio();
    font->set_size(unsigned(m_style.size() / pxr.y));
    float line_height = font->line_height() * pxr.y;
    m_pen.y += lines * line_height;
}


void Page::add_space(float spaces)
{
    m_pen.x += space_width() * spaces;
}


void Page::add_tab()
{
    // apply tab stops
    auto tab_stop = m_tab_stops.begin();
    float x = 0;
    while (x <= m_pen.x && tab_stop != m_tab_stops.end()) {
        x = *tab_stop++;
    }
    // apply generic tabs
    if (x <= m_pen.x) {
        float tab_size = 8 * space_width();
        while (x <= m_pen.x)
            x += tab_size;
    }
    // move to new position
    m_pen.x = x;
}


void Page::add_word(const std::string& string)
{
    m_words.emplace_back(*this, string);

    // Add word to current line
    auto& line = m_lines.back();
    line.add_word(m_words.back());

    // Add word to open spans
    for (auto& span_pair : m_spans) {
        auto& span = span_pair.second;
        if (span.is_open()) {
            span.add_word(m_words.back());
        }
    }
}


float Page::space_width()
{
    auto* font = m_style.font();
    auto pxr = target_framebuffer_ratio();
    font->set_size(unsigned(m_style.size() / pxr.y));
    auto glyph = font->get_glyph(' ');
    return glyph->advance() * pxr.x;
}


bool Page::begin_span(const std::string& name)
{
    auto result = m_spans.emplace(name, Span());
    if (!result.second) {
        log_error("Page: Span '{}' already exists!", name);
        return false;
    }
    return true;
}


bool Page::end_span(const std::string& name)
{
    auto iter = m_spans.find(name);
    if (iter == m_spans.end()) {
        log_error("Page: Span '{}' does not exists!", name);
        return false;
    }
    if (!iter->second.is_open()) {
        log_error("Page: Span '{}' is not open!", name);
        return false;
    }
    iter->second.close();
    return true;
}


Span* Page::get_span(const std::string& name)
{
    auto iter = m_spans.find(name);
    if (iter == m_spans.end())
        return nullptr;  // does not exist
    return &iter->second;
}


void Page::foreach_word(const std::function<void(const Word& word)>& cb) const
{
    if (!cb) return;
    for (auto& word : m_words) {
        cb(word);
    }
}


void Page::foreach_line(const std::function<void(const Line& line)>& cb) const
{
    if (!cb) return;
    for (auto& line : m_lines) {
        cb(line);
    }
}


void Page::foreach_span(const std::function<void(const Span& span)>& cb) const
{
    if (!cb) return;
    for (auto& pair : m_spans) {
        cb(pair.second);
    }
}


} // namespace xci::text::layout
