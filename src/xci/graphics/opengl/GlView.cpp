// GlView.cpp created on 2018-03-14, part of XCI toolkit
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

#include "GlView.h"
#include <xci/util/log.h>

// inline
#include <xci/graphics/View.inl>

namespace xci {
namespace graphics {

using namespace xci::util::log;


static GLuint compile_program(const char* vertex_source,
                              const char* fragment_source)
{
    // compile vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, nullptr);
    glCompileShader(vertex_shader);

    // check compilation result
    GLint result = GL_FALSE;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &length);
        std::string error_message(length + 1, '\0');
        glGetShaderInfoLog(vertex_shader, length, nullptr, &error_message[0]);
        log_error("vertex shader error: {}", error_message.c_str());
        return 0;
    }

    // compile fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, nullptr);
    glCompileShader(fragment_shader);

    // check compilation result
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &length);
        std::string error_message(length + 1, '\0');
        glGetShaderInfoLog(fragment_shader, length, nullptr, &error_message[0]);
        log_error("fragment shader error: {}", error_message.c_str());
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    // link program
    glLinkProgram(program);

    // check link status
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::string error_message(length + 1, '\0');
        glGetProgramInfoLog(program, length, nullptr, &error_message[0]);
        log_error("shader program error: {}", error_message.c_str());
        return 0;
    }

    // cleanup
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

#ifdef XCI_DEBUG_OPENGL
    // dump attributes
    GLint n, max_len;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &n);
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_len);
    for (GLuint i = 0; i < (GLuint)n; i++) {
        GLsizei length;
        GLint size;
        GLenum type;
        std::string name(max_len, '\0');
        glGetActiveAttrib(program, i, max_len, &length, &size, &type, &name[0]);
        log_debug("shader active attribute: {}", name.c_str());
    }

    // dump uniforms
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &n);
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_len);
    for (GLuint i = 0; i < (GLuint)n; i++) {
        GLsizei length;
        GLint size;
        GLenum type;
        std::string name(max_len, '\0');
        glGetActiveUniform(program, i, max_len, &length, &size, &type, &name[0]);
        log_debug("shader active uniform: {}", name.c_str());
    }
#endif

    return program;
}


GlView::GlView(Vec2u pixel_size)
    : m_pixel_size(pixel_size)
{
    resize(pixel_size);
}


GlView::~GlView()
{
    for (auto program : m_program) {
        glDeleteProgram(program);
    }
}


void GlView::resize(Vec2u pixel_size)
{
    // Decide between vert+/hor+ depending on screen orientation.
    if (pixel_size.x < pixel_size.y) {
        // preserve screen width
        float aspect = float(pixel_size.y) / float(pixel_size.x);
        m_size = {2.0f, 2.0f * aspect};
    } else {
        // preserve screen height
        float aspect = float(pixel_size.x) / float(pixel_size.y);
        m_size = {2.0f * aspect, 2.0f};
    }

    m_pixel_size = pixel_size;
}


GLuint
GlView::gl_program_from_string(GlView::ProgramId id,
                               const char* vertex_source,
                               const char* fragment_source)
{
    GLuint program = m_program[(size_t)id];
    if (!program) {
        program = compile_program(vertex_source, fragment_source);
        m_program[(size_t)id] = program;
    }
    return program;
}


}} // namespace xci::graphics
