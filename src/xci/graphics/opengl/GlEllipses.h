// GlEllipses.h created on 2018-03-24, part of XCI toolkit
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

#ifndef XCI_GRAPHICS_GL_ELLIPSES_H
#define XCI_GRAPHICS_GL_ELLIPSES_H

#include <xci/graphics/Ellipses.h>

#include <glad/glad.h>

#include <vector>

namespace xci {
namespace graphics {


class GlEllipses {
public:
    explicit GlEllipses(const Color& fill_color,
                          const Color& outline_color = Color::White());
    ~GlEllipses() { clear_gl_objects(); }

    void add_ellipse(const Rect_f& rect,
                     float outline_thickness = 0);
    void clear_ellipses();

    void draw(View& view, const Vec2f& pos);

private:
    void init_gl_objects();
    void clear_gl_objects();

private:
    Color m_fill_color;
    Color m_outline_color;

    struct Vertex {
        GLfloat x, y;         // vertex coords
        GLfloat iu, iv;       // inner edge of the border
        GLfloat ou, ov;       // outline edge of the border
    };
    std::vector<Vertex> m_vertex_data;
    std::vector<GLint> m_elem_first;  // first vertex of each element
    std::vector<GLsizei> m_elem_size;  // number of vertices of each element

    GLuint m_vertex_array = 0;  // aka VAO
    GLuint m_vertex_buffer = 0;  // aka VBO
    bool m_objects_ready = false;
};


class Ellipses::Impl : public GlEllipses {
public:
    explicit Impl(const Color& fill_color, const Color& outline_color)
            : GlEllipses(fill_color, outline_color) {}
};


}} // namespace xci::graphics

#endif // XCI_GRAPHICS_GL_ELLIPSES_H