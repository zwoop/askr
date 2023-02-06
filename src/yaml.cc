/**
 * @file
 * @brief All the class YAML decoders
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

#include "gsl/gsl"
#include "yaml.h"

namespace askr
{
  namespace yaml
  {
    void
    basic_validation(const YAML::Node &node, const std::vector<std::string> &validKeys)
    {
      for (auto const &item : node) {
        if (std::none_of(validKeys.begin(), validKeys.end(),
                         [&item](std::string s) { return s == item.first.as<std::string>(); })) {
          throw YAML::ParserException(item.Mark(), "unsupported key '" + item.first.as<std::string>() + "'");
        }
      }
    }
  } // namespace yaml
} // namespace askr
