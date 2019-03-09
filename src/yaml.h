/**
 * @file
 * @brief Include file for various YAML tools and helpers
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

#include <memory>
#include <yaml-cpp/yaml.h>

namespace askr
{
namespace yaml
{
    /**
     * @brief Validate that elements of a node are only of the allowed keys
     *
     * @param node       The YAML node
     * @param validKeys  A vector of all allowed keys
     */
    void basic_validation(const YAML::Node &node, const std::vector<std::string> &validKeys);
} // namespace yaml
} // namespace askr
