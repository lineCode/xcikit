// unique.h created on 2018-03-30, part of XCI toolkit
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

#ifndef XCI_COMPAT_UNIQUE_H
#define XCI_COMPAT_UNIQUE_H

// Backported std::make_unique for C++11
#if !defined(__cpp_lib_make_unique) && __cplusplus == 201103L

namespace std {

template<class T, class... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace std

#endif

#endif //XCI_COMPAT_UNIQUE_H