// demo_plugin.cpp created on 2018-04-19, part of XCI toolkit
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

#include <xci/core/SharedLibrary.h>
#include <xci/core/FileWatch.h>
#include <xci/core/log.h>
#include <functional>
#include <atomic>
#include <csignal>
#include <unistd.h>

using namespace xci::core;
using namespace xci::core::log;

static const char* filename = XCI_DEMO_PLUGINS "/libpluggable.so";
std::atomic_bool done {false};
std::atomic_bool reload {false};

static void sigterm(int)
{
    done = true;
}

int main()
{
    // Load library

    SharedLibrary lib;

    if (!lib.open(filename))
        return EXIT_FAILURE;

    auto fn = reinterpret_cast<const char*(*)()>(lib.resolve("sample_text"));
    if (!fn)
        return EXIT_FAILURE;

    // Setup hot reload

    FileWatchPtr watch = FileWatch::create();
    int wd = watch->add_watch(filename, [](FileWatch::Event ev) {
        if (ev == FileWatch::Event::Create || ev == FileWatch::Event::Modify)
            reload = true;
    });
    if (wd == -1)
        return EXIT_FAILURE;

    // Main loop

    signal(SIGTERM, sigterm);
    while (!done) {
        log_info("sample_text: {}", fn());
        sleep(1);

        if (reload) {
            lib.close();
            lib.open(filename);
            fn = reinterpret_cast<const char*(*)()>(lib.resolve("sample_text"));
            reload = false;
        }
    }

    return EXIT_SUCCESS;
}
