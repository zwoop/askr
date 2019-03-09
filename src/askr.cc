/**
 * @file
 * @brief The main entry point for the askr command line / runtime application
 *
 * This is intended to be use as a she-bang "script", e.g.
 *
 *     #!/usr/bin/env askr
 *
 * @section license License
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more contributor license agreements.  See the NOTICE
 * file distributed with this work for additional information regarding copyright ownership.  The ASF licenses this
 * file to you under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */
#include <iostream>
#include <fstream>
#include <exception>
#include <string>

#include <getopt.h>
#include <yaml-cpp/yaml.h>
#include <jemalloc/jemalloc.h>

#include "options.h"
#include "gsl/gsl"

namespace askr
{
namespace debug
{
    // Yes, this is a global, but turns out to be very useful to have debugging capabilities throughput the code
    std::bitset<16> gLevel;
} // namespace debug
} // namespace askr

/**
 * @brief main() is obviously the regular unix entry point.... Duh.
 *
 * @param argc  number of arguments
 * @param argv	the arguments

 * @return      exit value
 */
int
main(int argc, char **argv)
{
    // This is the standard set of getopt_long options that we always must support. We'll add to this one,
    // with the addition of script specific options. Note that these don't populate the name field, that is
    // only used (and needed) by plugins for identification.
    askr::Options askr_options = {{{"debug", 'D', "enable and set a debug level (bit-field)", required_argument},
                                   {"verbose", 'V', "enable verbose output and results ", no_argument},
                                   {"help", 'H', "show the help message (this)", no_argument}}};
    bool verbose_flag = false;
    int option_index = 0;

    if (GSL_LIKELY(argc >= 2)) {
        YAML::Node config;

        // Get the YAML config first, so we can extend the option parsing with specific script
        // options as necessary.
        try {
            config = YAML::LoadFile(argv[1]);
            Expects(config);
        } catch (std::exception &e) {
            std::cerr << "error in " << argv[1] << ": " << e.what() << std::endl;
            return 1;
        }
        if (!config.IsMap()) {
            std::cerr << "expected a map with options / reader / filter / output sections";
            return 1;
        }

        // Add the YAML command line options (if any)
        try {
            askr_options.add_yaml(config["options"]);
        } catch (std::exception &e) {
            std::cerr << "error in " << argv[1] << ": " << e.what() << std::endl;
            return 1;
        }

        // Now parse the command line options
        std::string short_opt = askr_options.get_short_options();
        std::unique_ptr<askr::GetoptOption[]> options = askr_options.as_getopt();

        while (true) {
            int c = getopt_long(argc, argv, short_opt.c_str(), options.get(), &option_index);

            if (c == -1) // Last option
                break;

            switch (c) {
                case 'D': {
                    std::string arg(optarg);
                    if (arg.size() > 2 && arg.substr(0, 2) == "0x") {
                        unsigned long long val;

                        std::istringstream(arg) >> std::hex >> val;
                        askr::debug::gLevel |= {val};
                    } else {
                        askr::debug::gLevel.set(std::stoi(optarg));
                    }
                } break;
                case 'V':
                    verbose_flag = true;
                    break;
                case 'H':
                    askr_options.print_help();
                    break;
                default:
                    // ToDo: Here we have to deal with all the "custom" options for the plugins used
                    break;
            }
        }
    } else {
        std::cerr << "Insufficient arguments";
    }
    // Dump jemalloc data, very verbose!
    if (askr::debug::Do(askr::debug::MEMORY)) {
        std::cerr << "jemalloc stats : " << std::endl;
        malloc_stats_print(NULL, NULL, NULL);
    }

    return 0;
}
