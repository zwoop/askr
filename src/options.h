/**
 * @file
 * @brief Include file for our custom options processing / management
 *
 * This is all based on top of the standard getopt_long() APIs and structure, however, we need to be able to
 * customize the structs in some reasonable way. This is important for askr scripts to be able to extend with
 * new command line options custom for just the script.
 *
 * This is not a public API.
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
#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

#include <yaml-cpp/yaml.h>
#include <getopt.h>

#include "askr/askr.h"
#include "gsl/gsl"

namespace askr
{
/**
 * @brief Convenience class name for the getopt_long() struct.
 *
 */
using GetoptOption = struct option;

/**
 * @class Option
 * @brief This structure holds one, extended command line option definition.
 *
 * This augments the regular option struct with the description field.
 */
class Option
{
public:
    /**
     * @brief Construct a new Option object, without any parameters.
     *
     * This is not a usable configuration at this point, you must call the askr::Option::init() method to set it
     * up after this.
     */
    Option()
    {
        if (askr::debug::Do(askr::debug::CPP)) {
            std::cerr << "Calling naked CTOR" << std::endl;
        }
    }

    /**
     * @brief Construct a new Option object, this is for the default configuartions (static options)
     *
     * @param long_opt       The long option string (e.g. "help")
     * @param short_opt      The short option character (e.g. 'H')
     * @param description    A brief description of the option
     * @param has_arg        If the option has no, required or optional arguments
     */
    Option(const char *long_opt, char short_opt, const char *description, int has_arg)
        : long_opt_(long_opt), short_opt_(short_opt), description_(description), has_arg_(has_arg)
    {
        if (askr::debug::Do(askr::debug::CPP)) {
            std::cerr << "Option:Option(): calling C-style CTOR" << std::endl;
        }
    }

    /**
     * @brief Construct a new Option object, this is used for creating custom options (plugins) via YAML
     *
     * @param name           The descriptive name of the option, used for YAML identification
     * @param long_opt       The long option string (e.g. "help")
     * @param short_opt      The short option character (e.g. 'H')
     * @param description    A brief description of the option
     * @param has_arg        If the option has no, required or optional arguments
     */
    Option(const std::string &name, const std::string &long_opt, char short_opt, const std::string &description, int has_arg)
        : name_(name), long_opt_(long_opt), short_opt_(short_opt), description_(description), has_arg_(has_arg)
    {
        if (askr::debug::Do(askr::debug::CPP)) {
            std::cerr << "Option::Option(): calling C++-style CTOR" << std::endl;
        }
    }

    /**
     * @brief Initialize the Option with all the required field values.
     *
     * This is useful for example when you create an object using the default CTOR, which does not properly
     * initialize the member variables.
     */
    void
    init(const std::string &name, const std::string &long_opt, char short_opt, const std::string &description, int has_arg)
    {
        if (askr::debug::Do(askr::debug::CPP)) {
            std::cout << "Option::init(): calling init function" << std::endl;
        }

        name_ = name;
        long_opt_ = long_opt;
        short_opt_ = short_opt;
        description_ = description;
        has_arg_ = has_arg;
    }

    /**
     * @brief Simple getter.
     *
     * @return  The name of the option, as a std::string
     */
    const std::string &
    name() const
    {
        return name_;
    }

    /**
     * @brief Simple getter.
     *
     * @return  The description of the option, as a std::string
     */
    const std::string &
    description() const
    {
        return description_;
    }

    /**
     * @brief Simple getter.
     *
     * @return  The has_arg value of the option
     */
    int
    has_arg() const
    {
        return has_arg_;
    }

    /**
     * @brief Simple getter.
     *
     * @return  The short-opt character of the option (without dash)
     */
    char
    short_opt() const
    {
        return short_opt_;
    }

    /**
     * @brief Simple getter.
     *
     * @return  The long-opt string of the option (without dashes)
     */
    const std::string &
    long_opt() const
    {
        return long_opt_;
    }

private:
    std::string name_;        /**< An (optional) descriptive name of the option, used by plugins */
    std::string long_opt_;    /**< Long option string, e.g. 'help' for --help. */
    char short_opt_ = '\0';   /**< Short option character, e.g. 'H' for -H. */
    std::string description_; /**< A short description of the option. */
    int has_arg_ = 0;         /**< Argument type, one of {no,optional,required}_argument. */
};

/**
 * @class Options
 * @brief Container of one or many Option objects, both standard and custom options.
 *
 * This is an extension to @c std::vector, providing some interfaces for interacting
 * with the underlying option structure. It contains one or more Option objects.
 */
class Options : public std::vector<Option>
{
public:
    /**
     * @brief Print a help text from the configured options in the option vector
     *
     * Example output:
     * @verbatim
     Usage: askr [switches] [--] [programfile] [arguments]
        -D | --debug                    enable debugging information
        -V | --verbose                  enable verbose output and diagnostics
        -H | --help                     show the help message (this)
     @endverbatim
     */
    void print_help();

    /**
     * @brief Get the short options string out of a vector of Options.
     *
     * This is necessary to produce a configurable short-options string, which must be built up using the information
     * from the long-options. This does produce a std::string copy, but not important for performance obviously.
     *
     * @return @c       The string, e.g. "DVH"
     */
    std::string get_short_options();

    /**
     * @brief Convert the Options vector to the equivalent C-style option struct array.
     *
     * In addition to the necessary C++ -> C style conversion, we also strip out the fields out of the
     * askr::Options structure that do not have a place in the C option struct.
     *
     * @return          The converted struct array of options, in a unique_ptr
     */
    std::unique_ptr<GetoptOption[]> as_getopt();

    /**
     * @brief Parse the options section out of the YAML
     *
     * This is used for scripts to add additional, specific command line options. The format
     * of such configs must contain the following attributes:
     *
     *
     * | Attribute       | Description                                 | Required           | Default |
     * |-----------------|---------------------------------------------|--------------------|---------|
     * | **name**        | The identifying name of the configuration   | For plugin options |         |
     * | **description** | A brief description of the option           | yes                |         |
     * | **short**       | The short option character, e.g. -s         | yes                |         |
     * | **argument**    | Are arguments required or even allowed      | no                 | none    |
     * | **long**        | The long name of the option, e.g. '--select | no                 |         |
     *
     * @param node    The YAML node for the options section
     */
    void add_yaml(const YAML::Node &node);
};

/**
 * @class OptionValues
 * @brief Holds one, or more, values associated with a specific option
 *
 * Each stored option is a vector of strings. We always split the options on ',', as well as
 * multiple invocation of an option.
 */
class OptionValues
{
public:
    /**
     * @brief Parse and add element(s) to the vector for the provided option (key)
     *
     * @param key The option name, which is the key into the hash
     * @param str The string, possibly comma separated, to parse and add.
     * @return true  On success
     * @return false If any error during parsing / addition occured
     */
    bool add(const std::string &key, const std::string &str);

    /**
     * @brief Simple getter
     *
     * @param key The option name, which is the key into the hash
     * @return The vector of value string values (possibly empty)
     */
    const std::vector<std::string> &get(const std::string &key) const;

    /**
     * @brief Simple getter
     *
     * @param key The option name, which is the key into the hash
     * @return The vector of value string values (possibly empty)
     */
    const std::vector<std::string> &
    get(const char *key) const
    {
        return get(std::string{key});
    }

private:
    std::unordered_map<std::string, std::vector<std::string>> options_;
};

} // namespace askr

namespace YAML
{
/**
 * @brief Convert a YAML Node to a valid askr::Option object
 *
 * This is used when converting the YAML node to the corrensponding askr::Option object. All validation
 * of the YAML structure (for tnis Node) is done here as well.
 */
template <> struct convert<askr::Option> {
    static bool decode(const Node &node, askr::Option &option);
};
} // namespace YAML
