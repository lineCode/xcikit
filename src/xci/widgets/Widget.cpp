// Widget.cpp created on 2018-04-23, part of XCI toolkit
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

#include "Widget.h"
#include <xci/util/log.h>
#include <xci/graphics/Window.h>

namespace xci {
namespace widgets {

using graphics::View;
using graphics::Key;
using graphics::Action;
using namespace util::log;


void Composite::add(WidgetPtr child)
{
    if (m_focus.expired())
        m_focus = child;
    m_child.push_back(std::move(child));
}


bool Composite::contains(const util::Vec2f& point)
{
    for (auto& child : m_child)
        if (child->contains(point))
            return true;
    return false;
}


void Composite::resize(View& view)
{
    for (auto& child : m_child)
        child->resize(view);
}


void Composite::draw(View& view, State state)
{
    for (auto& child : m_child) {
        state.focused = (m_focus.lock() == child);
        child->draw(view, state);
    }
}


void Composite::handle(View& view, const KeyEvent& ev)
{
    // Switch focus with Tab, Shift+Tab
    if (ev.action == Action::Press && ev.key == Key::Tab && !m_child.empty()) {
        if (m_focus.expired())
            m_focus = m_child.front();
        else {
            auto iter = std::find(m_child.begin(), m_child.end(), m_focus.lock());
            if (iter == m_child.end()) {
                // not found
                m_focus = m_child.front();
            } else {
                if (!ev.mod.shift) {
                    // tab
                    iter++;
                    if (iter == m_child.end())
                        iter = m_child.begin();
                } else {
                    // shift + tab
                    if (iter == m_child.begin())
                        iter = m_child.begin() + (m_child.size() - 1);
                    else
                        iter--;
                }
                m_focus = *iter;
            }
        }
        resize(view);
        view.refresh();
        return;
    }

    // Prpagate the event
    if (!m_focus.expired())
        m_focus.lock()->handle(view, ev);
}


void Composite::handle(View& view, const CharEvent& ev)
{
    if (!m_focus.expired())
        m_focus.lock()->handle(view, ev);
}


void Composite::handle(View& view, const MousePosEvent& ev)
{
    for (auto& child : m_child)
        child->handle(view, ev);
}


void Composite::handle(View& view, const MouseBtnEvent& ev)
{
    bool focus_changed = false;
    for (auto& child : m_child) {
        // Switch focus on click
        if (child->contains(ev.pos) && m_focus.lock() != child) {
            m_focus = child;
            focus_changed = true;
        }

        // Propagate the event
        child->handle(view, ev);
    }
    if (focus_changed) {
        resize(view);
        view.refresh();
    }
}


Bind::Bind(graphics::Window& window, Widget& root)
    : m_window(window)
{
    window.set_size_callback([&](View& v) { root.resize(v); });
    window.set_draw_callback([&](View& v) { root.draw(v, {}); });
    window.set_key_callback([&](View& v, const KeyEvent& e) { root.handle(v, e); });
    window.set_char_callback([&](View& v, const CharEvent& e) { root.handle(v, e); });
    window.set_mouse_position_callback([&](View& v, const MousePosEvent& e) { root.handle(v, e); });
    window.set_mouse_button_callback([&](View& v, const MouseBtnEvent& e) { root.handle(v, e); });
}


Bind::~Bind()
{
    m_window.set_size_callback(nullptr);
    m_window.set_draw_callback(nullptr);
    m_window.set_key_callback(nullptr);
    m_window.set_char_callback(nullptr);
    m_window.set_mouse_position_callback(nullptr);
    m_window.set_mouse_button_callback(nullptr);
}


}} // namespace xci::widgets
