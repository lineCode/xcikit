// file.cpp created on 2018-03-29, part of XCI toolkit
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

#include "file.h"
#include "FileWatch.h"
#include <fstream>

namespace xci {
namespace util {


std::string read_file(const std::string& filename)
{
    std::string content;
    std::ifstream f(filename);
    if (!f)
        return content;  // empty

    f.seekg(0, std::ios::end);
    content.resize((size_t) f.tellg());
    f.seekg(0, std::ios::beg);
    f.read(&content[0], content.size());
    if (!f)
        content.clear();

    return content;
}


int add_file_watch(const std::string& filename, std::function<void()> cb)
{
    return FileWatch::default_instance()
            .add_watch(filename, [cb](FileWatch::Event) { cb(); });
}


void remove_file_watch(int watch)
{
    FileWatch::default_instance().remove_watch(watch);
}


}}  // namespace xci::util
