/**
 * @file
 * @brief The public include file for the Key-Value object.
 *
 * This is a public include file, which plugins are expected to use.
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

#include <bitset>

namespace askr
{
namespace debug
{
    /**
     * @brief The various bit fields of debugging, one or several of these can be set
     *
     */
    enum Levels {
        NONE,          ///< 0 - No debugging
        BASIC,         ///< 1 - Basic and general debugging
        ADVANCED,      ///< 2 - More advanced, but general debugging
        MEMORY,        ///< 3 - Memory debugging, jemalloc etc.
        CPP,           ///< 4 - C++ level debugging, CTORs DTORs etc.
        PLUGIN_SETUP,  ///< 5 - Plugins setup and initialization
        PLUGIN_DETAILS ///< 6 - Detailed debugging of plugins
    };

    /**
     * @brief This is the global bitmask of currently enabled debug levels.
     *
     * See the above enum for the various bit-fields.
     */
    extern std::bitset<16> gLevel;

    inline bool
    Do(Levels level)
    {
        return gLevel[level];
    }

} // namespace debug
} // namespace askr
