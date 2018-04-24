// demo_ui.cpp created on 2018-04-22, part of XCI toolkit
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

#include <xci/widgets/Button.h>
#include <xci/widgets/Checkbox.h>
#include <xci/widgets/FpsDisplay.h>
#include <xci/graphics/Window.h>
#include <xci/util/file.h>
#include <random>
#include <cstdlib>

using namespace xci::graphics;
using namespace xci::widgets;

int main()
{
    xci::util::chdir_to_share();

    Window& window = Window::default_window();
    window.create({800, 600}, "XCI UI demo");

    if (!Theme::load_default_theme())
        return EXIT_FAILURE;

    // Random color generator
    std::random_device rd;
    std::default_random_engine re(rd());
    std::uniform_int_distribution<int> runi(0, 255);
    auto random_color = [&](){ return Color(runi(re), runi(re), runi(re)); };

    Composite root;
    for (auto i : {1,2,3,4,5}) {
        auto button = std::make_shared<Button>(std::to_string(i) + ". click me!");
        button->set_position({-0.2f, -0.5f + i * 0.12f});
        button->set_font_size(0.07);
        button->on_click([button, &random_color](View& view) {
            button->set_text_color(random_color());
            button->update(view);
        });
        root.add(button);
    }

    for (auto i : {1,2,3,4,5}) {
        auto checkbox = std::make_shared<Checkbox>();
        checkbox->set_text("Checkbox " + std::to_string(i));
        checkbox->set_position({0.5f, -0.5f + i * 0.05f});
        root.add(checkbox);
    }

    auto fps_display = std::make_shared<FpsDisplay>();
    fps_display->set_position({-1.2f, -0.8f});
    root.add(fps_display);

    Bind bind(window, root);
    window.set_refresh_mode(RefreshMode::OnEvent);
    window.display();
    return EXIT_SUCCESS;
}
