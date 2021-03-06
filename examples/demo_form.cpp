// demo_form.cpp created on 2018-06-23, part of XCI toolkit
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
#include <xci/widgets/FpsDisplay.h>
#include <xci/widgets/Form.h>
#include <xci/graphics/Window.h>
#include <xci/widgets/Label.h>
#include <xci/core/Vfs.h>
#include <xci/core/format.h>
#include <xci/config.h>
#include <random>
#include <cstdlib>

using namespace xci::text;
using namespace xci::graphics;
using namespace xci::widgets;
using namespace xci::core;


class MousePosInfo: public Widget {
public:
    MousePosInfo() : m_text("Mouse: ", Theme::default_theme().font()) {
        m_text.set_color(Color(255, 150, 50));
    }

    void resize(View& view) override { m_text.resize(view); }
    void draw(View& view, State state) override { m_text.draw(view, position()); }

    void mouse_pos_event(View& view, const MousePosEvent& ev) override {
        m_text.set_fixed_string("Mouse: " +
                                format("({}, {})", ev.pos.x, ev.pos.y));
        resize(view);
        view.refresh();
    }

private:
    Text m_text;
};


int main()
{
    Vfs::default_instance().mount(XCI_SHARE_DIR);

    Window& window = Window::default_window();
    window.create({800, 600}, "XCI form demo");

    if (!Theme::load_default_theme())
        return EXIT_FAILURE;

    Composite root;

    // Form #1
    auto form1 = std::make_shared<Form>();
    form1->set_position({-1.0f, -0.5f});
    root.add(form1);

    std::string input_text = "2018-06-23";
    form1->add_input("date", input_text);

    bool checkbox1 = false;
    form1->add_input("checkbox1", checkbox1);

    bool checkbox2 = true;
    form1->add_input("checkbox2", checkbox2);

    auto button = std::make_shared<Button>("submit");
    form1->add_hint(Form::Hint::NextColumn);
    form1->add(button);

    // Form #1 output
    auto output_text = std::make_shared<Label>();
    output_text->set_position({0.2f, -0.5f});
    output_text->text().set_color(Color(180, 100, 140));
    button->on_click([output_text, &input_text, &checkbox1, &checkbox2]
                     (View& view) {
        auto text = format("Submitted:\n\n"
                           "input_text = {}\n\n"
                           "checkbox1 = {}\n\n"
                           "checkbox2 = {}\n\n",
                           input_text, checkbox1, checkbox2);
        output_text->text().set_string(text);
        output_text->resize(view);
    });
    root.add(output_text);

    // Form #2
    auto form2 = std::make_shared<Form>();
    form2->set_position({-1.0f, 0.2f});
    root.add(form2);

    std::string name = "Player1";
    form2->add_input("name", name);

    bool hardcore = false;
    form2->add_input("hardcore", hardcore);

    // Mouse pos
    auto mouse_pos_info = std::make_shared<MousePosInfo>();
    mouse_pos_info->set_position({-1.2f, 0.9f});
    root.add(mouse_pos_info);

    // FPS
    auto fps_display = std::make_shared<FpsDisplay>();
    fps_display->set_position({-1.2f, -0.8f});
    root.add(fps_display);

    window.set_refresh_mode(RefreshMode::OnDemand);
    //window.set_debug_flags(View::DebugFlags(View::Debug::LineBaseLine));

    window.set_key_callback([&root](View& v, const KeyEvent& e) {
        if (e.action == Action::Press && e.key == Key::D) {
            root.dump(std::cout);
            std::cout << std::endl;
        }
    });

    Bind bind(window, root);
    window.display();
    return EXIT_SUCCESS;
}
