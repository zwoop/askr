/**
 * @file
 * @brief Implementation details for all the command line options management
 */

/*
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
#include <iomanip>
#include <string>
#include <memory>

#include "options.h"
#include "yaml.h"

namespace askr
{
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // Implementation details for class Options
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  // Convert the Options vector into the string of short-opts.
  std::string
  Options::get_short_options()
  {
    std::string res;

    for (auto const &opt : *this) {
      res += opt.short_opt();
      switch (opt.has_arg()) {
      case required_argument:
        res += ':';
        break;
      case optional_argument:
        res += "::";
        break;
      default:
        // This covers no_argument as well, since that gets done first
        break;
      }
    }

    return res;
  }

  // Convert the options vector to the equivalent C-style struct option (for getopt_long()).
  std::unique_ptr<askr::GetoptOption[]>
  Options::as_getopt()
  {
    size_t ix = 0;
    std::unique_ptr<askr::GetoptOption[]> getopts(new askr::GetoptOption[this->size() + 1]);

    Expects(getopts);
    for (auto const &opt : *this) {
      getopts[ix++] = {opt.long_opt().c_str(), opt.has_arg(), nullptr, opt.short_opt()};
    }
    getopts[ix] = {nullptr, 0, nullptr, 0};

    return getopts;
  }

  // Produce a nicely formatted help page, from the Options structure (which can be modified by the script)
  void
  Options::print_help()
  {
    bool got_help = false; // Needed to keep track when we printed the help, to separate script-local options

    std::cout << "askr v" << ASKR_VERSION << std::endl;
    std::cout << "Usage: askr [switches] [--] [programfile] [arguments]" << std::endl;
    for (auto const &opt : *this) {
      if (got_help) {
        std::cout << std::endl;
        std::cout << "Script specific options:" << std::endl;
        got_help = false;
      }
      std::cout << std::left << "\t-" << static_cast<char>(opt.short_opt()) << " | --" << std::setw(25) << opt.long_opt();
      std::cout << opt.description() << std::endl;
      if (opt.long_opt() == "help") {
        got_help = true;
      }
    }
  }

  // Parse the YAML section for options, return false if any YAML issues were encountered
  void
  Options::add_yaml(const YAML::Node &options)
  {
    for (auto const &item : options) {
      this->emplace_back(item.as<askr::Option>());
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // Implementation details for class OptionValues
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  // Insert a new option value, after parsing it etc. properly
  bool
  OptionValues::add(const std::string &key, const std::string &str)
  {
    options_[key] = {str}; // ToDo: Need the parsing here...

    return true;
  }

  // Getter implementation for retrieving an options value
  const std::vector<std::string> &
  OptionValues::get(const std::string &key) const
  {
    static const std::vector<std::string> EMPTY_;

    if (auto it = options_.find(key); it != options_.end()) {
      return it->second;
    }
    return EMPTY_;
  }

} // namespace askr

// These are the valid optoins keys for this section
static const std::vector<std::string> validOptionKeys = {"name", "description", "argument", "short", "long"};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Extensions to the YAML parser, specific to our Options
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace YAML
{
  bool
  convert<askr::Option>::decode(const Node &node, askr::Option &option)
  {
    // Basic validations first, this will throw as necessary
    askr::yaml::basic_validation(node, validOptionKeys);

    // Get the "name" key; required
    std::string name;

    if (node["name"]) {
      name = node["name"].as<std::string>();
    } else {
      throw YAML::ParserException(node.Mark(), "'name' key is required");
    }

    // Get the "description" key; required
    std::string description;

    if (node["description"]) {
      description = node["description"].as<std::string>();
    } else {
      throw YAML::ParserException(node["name"].Mark(), "'description' key is required");
    }

    // Get, and convert, the argument key; not required, defaults to no_argument
    int has_arg = no_argument;
    std::string argument;

    if (node["argument"]) {
      argument = node["argument"].as<std::string>();
      if (argument == "no" || argument == "none") {
        has_arg = no_argument;
      } else if (argument == "required") {
        has_arg = required_argument;
      } else if (argument == "optional") {
        has_arg = optional_argument;
      }
    }

    // Get, check and convert the short option; required
    std::string short_opt;

    if (node["short"]) {
      short_opt = node["short"].as<std::string>();
    }
    if (short_opt.size() != 2 || short_opt[0] != '-') {
      throw YAML::ParserException(node["short"].Mark(), "'short' value should start with a '-' followed by a single letter");
    }

    // Get the long option, and make sure it follows the format; not required
    std::string long_opt;

    if (node["long"]) {
      long_opt = node["long"].as<std::string>();
      if (long_opt.substr(0, 2) != "--") {
        throw YAML::ParserException(node["long"].Mark(), "'long' value must start with '--'");
      }
    }

    // We have to use the init function here, to avoid construction another object
    option.init(name, long_opt.substr(2), short_opt[1], description, has_arg);

    return true;
  }

} // namespace YAML
