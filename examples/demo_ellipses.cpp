// demo_ellipses.cpp created on 2018-03-24, part of XCI toolkit
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

#include <xci/graphics/Window.h>
#include <xci/graphics/Ellipses.h>

using namespace xci::graphics;

int main()
{
    Window window;
    window.create({800, 600}, "XCI ellipses demo");

    // normally, the border scales with viewport size
    Ellipses rts(Color(0, 0, 40, 128), Color(180, 180, 0));
    rts.add_ellipse({-1, -0.6f, 2, 1.2f}, 0.05);
    rts.add_ellipse({-0.6f, -0.8f, 1.2f, 1.6f}, 0.02);

    // using View::screen_ratio, we can set constant border width, in screen pixels
    Ellipses rts_px(Color(40, 40, 0, 128), Color(255, 255, 0));

    window.display([&](View& view) {
        rts.draw(view, {0, 0});

        auto pxr = view.screen_ratio().x;
        rts_px.add_ellipse({0.0f, 0.0f, 0.5f, 0.5f}, 1 * pxr);
        rts_px.add_ellipse({0.1f, 0.1f, 0.5f, 0.5f}, 2 * pxr);
        rts_px.add_ellipse({0.2f, 0.2f, 0.5f, 0.5f}, 3 * pxr);
        rts_px.add_ellipse({0.3f, 0.3f, 0.5f, 0.5f}, 4 * pxr);
        rts_px.add_ellipse({0.4f, 0.4f, 0.5f, 0.5f}, 5 * pxr);
        rts_px.draw(view, {-0.45f, -0.45f});
        rts_px.clear_ellipses();
    });
    return EXIT_SUCCESS;
}