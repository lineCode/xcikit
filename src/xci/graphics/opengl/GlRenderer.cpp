// GlRenderer.cpp created on 2018-04-07, part of XCI toolkit
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

#include "GlRenderer.h"
#include "GlTexture.h"
#include "GlShader.h"
#include "GlPrimitives.h"

namespace xci::graphics {


Renderer& Renderer::default_instance()
{
    static GlRenderer instance;
    return instance;
}


TexturePtr GlRenderer::create_texture()
{
    return std::make_shared<GlTexture>();
}


ShaderPtr GlRenderer::get_or_create_shader(ShaderId shader_id)
{
    if (shader_id != ShaderId::Custom) {
        auto& shader = m_shader[(size_t) shader_id];
        if (!shader) {
            shader = std::make_shared<GlShader>();
        }
        return shader;
    } else {
        return std::make_shared<GlShader>();
    }
}


PrimitivesPtr GlRenderer::create_primitives(VertexFormat format,
                                            PrimitiveType type)
{
    return std::make_shared<GlPrimitives>(format, type);
}


} // namespace xci::graphics
