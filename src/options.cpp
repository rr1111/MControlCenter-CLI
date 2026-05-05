/* Copyright (C) 2022  Dmitry Serov
 *
 * This file is part of MControlCenter.
 *
 * MControlCenter is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * MControlCenter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MControlCenter. If not, see <https://www.gnu.org/licenses/>.
 */
#include "options.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <climits>


Options::Options()
    :cli(false), cooler_boost(std::nullopt)
{}

void Options::print_help(std::string program_name)
{
    fprintf(stdout, R"(Description
Syntax: %s [options]

    -B, --coolerboost STATE             toggle fan cooler boost
    -M, --usermode MODE                 change user mode
    -L, --chargelimit PERCENT           set charge limit

    -b, --get-coolerboost               get current cooler boost STATE
    -m, --get-usermode                  get current user mode MODE
    -l, --get-chargelimit               get current charge limit PERCENT

    -h                                  show help

Arguments:
    STATE: can be 'ON', 'OFF' or 'TOGGLE'
    MODE: can be 'PERFORMANCE', 'BALANCED', 'SILENT', 'BATTERY', 'NEXT'
    PERCENT: can be any percentage (positive number)
)", program_name.c_str());
    exit(1);
}

void Options::process_args(int argc, char** argv)
{
    int option_index;
    while (true)
    {
        const auto opt = getopt_long(argc, argv, short_opts.data(), long_opts, &option_index);

        if (-1 == opt){
            break;
        }

        switch (opt)
        {
            case 0:
                cli = true;
                // long option without short equivalent
            break;

            case 'B':
                cli = true;

                if(std::string(optarg) =="ON"){
                    cooler_boost = std::optional<Options::State>{Options::State::ON};
                }
                else if(std::string(optarg) == "OFF")
                {
                    cooler_boost = std::optional<Options::State>{Options::State::OFF};
                }
                else if(std::string(optarg) == "TOGGLE")
                {
                    cooler_boost = std::optional<Options::State>{Options::State::TOGGLE};
                }
                else{
                    fprintf(stderr, "Wrong TOGGLE value for coolerboost option.\n");
                    print_help(argv[0]);
                }
            
            break;
            
            case 'M':
                cli = true;

                if(std::string(optarg) == "BALANCED"){
                    user_mode = std::optional<Options::Mode>{Options::Mode::BALANCED};
                }
                else if(std::string(optarg) == "PERFORMANCE")
                {
                    user_mode = std::optional<Options::Mode>{Options::Mode::PERFORMANCE};
                }
                else if(std::string(optarg) == "SILENT")
                {
                    user_mode = std::optional<Options::Mode>{Options::Mode::SILENT};
                }
                else if(std::string(optarg) == "BATTERY")
                {
                    user_mode = std::optional<Options::Mode>{Options::Mode::BATTERY};
                }
                else if(std::string(optarg) == "NEXT")
                {
                    user_mode = std::optional<Options::Mode>{Options::Mode::NEXT};
                }
                else{
                    fprintf(stderr, "Wrong MODE value for usermode option.\n");
                    print_help(argv[0]);
                }

            break;

            case 'L':
                cli = true;
                {
                    std::string s(optarg);
                    if (!s.empty() && std::all_of(s.begin(), s.end(),
                        [](unsigned char c){ return std::isdigit(c); }))
                    {
                        errno = 0;
                        char *end = nullptr;
                        unsigned long long val = std::strtoull(s.c_str(), &end, 10);
                        if (errno == 0 && *end == '\0' && val > 0 && val <= ULLONG_MAX) {
                            charge_limit = val;
                        } else {
                            fprintf(stderr, "Please enter a positive integer without sign.\n");
                            print_help(argv[0]);
                        }
                    } else {
                        fprintf(stderr, "Please enter a positive integer without sign.\n");
                        print_help(argv[0]);
                    }
                }

            break;

            case 'b':
                cli = true;
                get_cooler_boost = true;

            break;

            case 'm':
                cli = true;
                get_user_mode = true;

            break;

            case 'l':
                cli = true;
                get_charge_limit = true;

                break;

            case 'h': // -h or --help
            case '?': // Unrecognized option
            default:
                print_help(argv[0]);
            break;
        }
    }
}
