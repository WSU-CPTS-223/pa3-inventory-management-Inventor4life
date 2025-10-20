// Copyright (c) 2025 Ethan Goode
// See LICENSE.txt in project root
// SPDX-License-Identifier: MIT

#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <iostream>
#include <vector>
#include <string>

namespace csv {

  std::vector<std::string> ReadLine(std::istream& input_stream, char escape_character = '"');

}
#endif
