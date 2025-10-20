// See LICENSE.txt in project root
// SPDX-License-Identifier: MIT

#include "csv_parser.h"

namespace {

enum class Status {
  kEndOfField=0,
  kEndOfEntry,
  kEndOfFile,
};
Status ReadField(std::istream& input_stream, std::string& field, char escape_character) {
  bool first_char = true;
  bool quoted_field = false;
  bool escape_sequence = false; // true iff previous char was the escape character
  char current_char = '\0';
  field.clear();
  while (true) {
    current_char = input_stream.get();

    // Check for end of file
    if (!input_stream) {
      return Status::kEndOfFile;
    }

    if (first_char) {
      if (current_char == ' ') {     // ignore leading whitespace
        continue;
      } // else
      first_char = false;
      if (current_char == '"') {     // Check if field is quoted
        quoted_field = true;
        continue;
      } //else
      if (current_char == ',') {     // Check for sequential colons
        return Status::kEndOfField;
      }
    }

    ///////////////////////////////////////// BEGIN ESCAPE SEQUENCE PARSING
    // If previous character was escape char, put escaped value into string
    if (escape_sequence) {
      escape_sequence = false; // Reset
      // Handle consecutive escape characters
      if (current_char == escape_character) {
        field.push_back(escape_character);
        continue;
      }

      // Handle escaped double quotes
      if (current_char == '"') {
        field.push_back('"');
        continue;
      }

      // Handle End-Of-Field edge-case where escape_character is double quote, treating (",)
      //  like an escape sequence. Since docs.md declares that escaping chars
      //  outside of double quotes or escape_char itself is undefined, this
      //  should be fine in theory.
      if (current_char == ',' and escape_character == '"') {
        return Status::kEndOfField;
      }
      // Similar to above, except for newlines
      if (current_char == '\n' and escape_character == '"') {
        return Status::kEndOfEntry;
      }
    }
    ///////////////////////////////////////// END ESCAPE SEQUENCE PARSING

    // Special handling for quoted fields
    if (quoted_field) {
      if (current_char == escape_character) { // Detect if entering escape sequence starting next char
        escape_sequence = true;
        continue;
      }
      // Because of escape sequence checking above, this evaluates to true iff
      //  at end of field and escape_char != '"' (assuming file semantics
      //  defined in docs.md are followed.)
      if (current_char == '"') {
        return Status::kEndOfField;
      }
      field.push_back(current_char);
      continue;
    }

    if (current_char == '\n') {
      return Status::kEndOfEntry;
    }

    if (current_char == ',') {
      return Status::kEndOfField;
    }

    field.push_back(current_char);
    // continue
  }

}

}

namespace csv {
// Very (very) basic csv parser, with no support for escape sequences
std::vector<std::string> ReadLine(std::istream& input_stream, char escape_character) {
  std::vector<std::string> result;
  Status status;
  do { // Return vector at end of line
    bool first_char = true;
    std::string current_field;
    status = ReadField(input_stream, current_field, escape_character);
    result.push_back(std::move(current_field));
  } while(status != Status::kEndOfEntry && status != Status::kEndOfFile);
  return result;
}

}
