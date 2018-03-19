// demo_rectangles.cpp created on 2018-03-19, part of XCI toolkit
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
#include <xci/graphics/Rectangles.h>

using namespace xci::graphics;

int main()
{
    Window window;
    window.create({800, 600}, "XCI rectangles demo");

    Rectangles rts(Color(0, 40, 40, 128), Color(180, 180, 0));
    rts.add_rectangle({-1, -0.5f, 2, 1}, 0.1);
    rts.add_rectangle({-0.5f, -0.8f, 1, 1.6f}, 0.05);

    window.display([&](View& view) {
        rts.draw(view, {0, 0});
    });
    return EXIT_SUCCESS;
}
