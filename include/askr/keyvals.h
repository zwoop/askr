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

#include <memory>
#include <string_view>
#include <vector>
#include <unordered_map>

#include <askr/buffers.h>

namespace askr
{
/**
 * @brief A container which gets populated, and passed along, extensively through records processing
 *
 * This is a public class, that plugins will rely on extensively. It manages the memory itself, so it's
 * important to retain it's shared-ptr semantics through all plugins. The store can iterate over the
 * key-values in the order which they were added, essentially preversing the input order.
 *
 * We have sub-classed std::unordered map here, to allow us to use it mostly as-is, but overloading
 * certain methods and operators such that we can retain order.
 *
 * Important: A KeyValueStore is intended to contain exactly one (1) record! This is important for
 * memory management: This assures that all std::string_views in the key-val store are all from the
 * same memory blob. One such blob can of course be used for multiple key-val stores, but one key-val
 * store can belong to one, and exactly one, blob.
 */
class KeyValueStore : public std::unordered_map<std::string_view, const std::string_view>
{
    KeyValueStore() = delete;
    KeyValueStore(askr::Buffer *) {}

private:
    std::vector<std::string_view> _order;
    std::shared_ptr<askr::Buffer> _buffer;
};
} // namespace askr
