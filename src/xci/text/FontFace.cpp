// FontFace.cpp - created on 2018-03-02, part of XCI toolkit
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

#include "FontFace.h"
#include <xci/util/log.h>

namespace xci {
namespace text {

using namespace util::log;


static inline float ft_to_float(FT_F26Dot6 ft_units) {
    return (float)(ft_units) / 64.f;
}


FontFace::~FontFace()
{
    if (face != nullptr) {
        auto err = FT_Done_Face(face);
        if (err) {
            log_error("FT_Done_FreeType: {}", err);
            return;
        }
    }
    if (stroker) {
        FT_Stroker_Done(stroker);
    }
}

// Helper to avoid repeating error handling etc.
template<typename F>
bool FontFace::load_face(F load_fn)
{
    if (face != nullptr) {
        log_error("FontFace: Reloading not supported! Create new instance instead.");
        return false;
    }
    auto err = load_fn();
    if (err == FT_Err_Unknown_File_Format) {
        log_error("FT_New_Face: Unknown file format");
        return false;
    }
    else if (err == FT_Err_Cannot_Open_Resource) {
        log_error("FT_New_Face: Cannot open resource");
        return false;
    }
    else if (err != 0) {
        log_error("Cannot open font (FT_New_Face: {})", err);
        return false;
    }

    // Our code points are in Unicode, make sure it's selected
    if (FT_Select_Charmap(face, FT_ENCODING_UNICODE) != 0) {
        log_error("FT_Select_Charmap: Error setting to Unicode: {}", err);
        FT_Done_Face(face);
        face = nullptr;
        return false;
    }

    return true;
}


bool FontFace::load_from_file(const char* file_path, int face_index)
{
    return load_face([this, file_path, face_index](){
        return FT_New_Face(library->raw_handle(), file_path, face_index, &face);
    });
}


bool FontFace::load_from_memory(const uint8_t *buffer, ssize_t size, int face_index)
{
    return load_face([this, buffer, size, face_index](){
        return FT_New_Memory_Face(library->raw_handle(), buffer, size, face_index, &face);
    });
}


bool FontFace::set_size(unsigned pixel_size)
{
    /*auto wh = float_to_ft(size);
    FT_Size_RequestRec size_req = {};
    size_req.type = FT_SIZE_REQUEST_TYPE_NOMINAL;
    size_req.width = wh;
    size_req.height = wh;
    auto err = FT_Request_Size(face, &size_req);*/
    auto err = FT_Set_Pixel_Sizes(face, pixel_size, pixel_size);
    if (err) {
        log_error("FT_Set_Char_Size: {}", err);
        return false;
    }

    return true;
}

bool FontFace::set_outline()
{
    if (stroker == nullptr) {
        // Create stroker
        auto err = FT_Stroker_New(library->raw_handle(), &stroker);
        if (err) {
            log_error("FT_Stroker_New: {}", err);
            return false;
        }
    }

    // TODO

    return true;
}

float FontFace::line_height() const
{
    return ft_to_float(face->size->metrics.height);
}

float FontFace::max_advance()
{
    // Measure letter 'M' instead of trusting max_advance
    uint glyph_index = get_glyph_index('M');
    if (!glyph_index) {
        return ft_to_float(face->size->metrics.max_advance);
    }
    auto glyph_slot = load_glyph(glyph_index);
    if (glyph_slot == nullptr) {
        return ft_to_float(face->size->metrics.max_advance);
    }
    return ft_to_float(glyph_slot->metrics.horiAdvance);
}

float FontFace::ascender() const
{
    return ft_to_float(face->size->metrics.ascender);
}

float FontFace::descender() const
{
    return ft_to_float(face->size->metrics.descender);
}

GlyphIndex FontFace::get_glyph_index(CodePoint code_point) const
{
    return FT_Get_Char_Index(face, code_point);
}

FT_Glyph_Metrics& FontFace::glyph_metrics()
{
    return face->glyph->metrics;
}

FT_Bitmap& FontFace::render_glyph()
{
    int err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    if (err) {
        log_error("FT_Render_Glyph error: {}", err);
    }
    return face->glyph->bitmap;
}

FT_GlyphSlot FontFace::load_glyph(GlyphIndex glyph_index)
{
    int err = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT | FT_LOAD_TARGET_LIGHT);
    if (err) {
        log_error("FT_Load_Glyph error: {}", err);
        return nullptr;
    }
    return face->glyph;
}


}} // namespace xci::text
