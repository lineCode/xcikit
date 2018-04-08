// Renderer.cpp created on 2018-04-08, part of XCI toolkit
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

#include "Renderer.h"

#ifdef XCI_WITH_OPENGL
#include <xci/graphics/opengl/GlRenderer.h>
#endif

#include <cassert>

namespace xci {
namespace graphics {


Renderer& Renderer::default_renderer()
{
#ifdef XCI_WITH_OPENGL
    static GlRenderer renderer;
    return renderer;
#endif
    assert(!"No renderer available");
}


Renderer::~Renderer() = default;


}} // namespace xci::graphics
