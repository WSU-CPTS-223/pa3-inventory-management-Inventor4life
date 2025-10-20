# Table Of Contents:
- [Including the library](#including-the-library)
- [Library dependencies](#library-dependencies)
- [Available Methods](#available-methods)
  - [ReadLine()](#readline) -- Reads a single line from the csv stream
- [Escape sequences](#escape-sequences)

---

# Including the library

1. Copy the module directory into your project
2. Add the following to your CMakeLists.txt file:
```
add_subdirectory({path-to-csv_parser-dir})
target_link_libraries({your-target-name} PUBLIC csv_parser)
```

---

# Library dependencies

### Included Automatically
| Dependency | Reasoning                        |
|------------|----------------------------------|
|std::istream|Input stream to parse as CSV      |
|std::vector |Used to return multiple strings   |
|std::string |To store a field from the csv file|

---
# Available methods

## ReadLine()
### `std::vector<std::string> ReadLine(std::istream& input_stream, char escape_character='"')`

### Arguments: {#readline-arguments}

|name            |type         |description                                               |
|----------------|-------------|----------------------------------------------------------|
|input_stream    |std::istream&|Stream to read characters from (interpreted as csv stream)|
|escape_character|char         |Character used to mark escape sequences. (See [Escape sequences](#escape-sequences))|

### Description: {#readline-description}
Takes an input stream representing csv-parseable text and returns a vector of individual fields. Accepts an optional `escape_character` argument, which causes any char in the current entry in `input_stream` that matches `escape_character` to be ignored (see [Example](#readline-example))

### Example: {#readline-example}

<table><thead><th>
test.csv
</th></thead><td><pre>
one," two", "three"
</pre></td></table>

<table><thead><th>
main.cpp
</th></thead><td><pre>
#include "csv_parser.h"
...
std::ifstream my_csv("test.csv");
std::vector&ltstring&gt csv_entry = csv::ReadLine(my_csv);
</pre></td></table>

Result:

<table><thead><th>
<code>csv_entry</code> contains
</th></thead><td>
<code>one</code>,<code> two</code>,<code>three</code>
</td></table>

---

# Escape sequences:

Any field containing the following characters: (`,`, `"`, `\n`) or the escape character itself (default: `"`) must be entirely enclosed in double-quotes. Double quotes (`"`) and instances of `escape_character` not intended for escaping must be prefixed with `escape_character`. Escaping characters other than double quotes (`"`) or the `escape_character` results in undefined behavior. Setting `escape_character` to `,`, ` `(space), or `\n` is undefined behavior. Leading whitespace in fields will be ignored unless quoted.

### Examples {#escape-sequences-examples}

To highlight the difference between the quote character and escape character, we are using `^` as the escape character.

|input                                       |output                                           |
|---                                         |---                                              |
|`Ordinary String,Second String`             |`Ordinary String`, `Second String`               |
|`"Quoted String", "Second Quote"`           |`Quoted String`, `Second Quote`                  |
|`"  Quoted Whitespace",  Leading Whitespace`|`  Quoted Whitespace`,`Leading Whitespace`       |
|`Unescaped " Special Character`             |undefined                                        |
|`"Unescaped " Special Character in Quotes"` |undefined                                        |
|`Escaped ^" Special Character, no Quotes`   |undefined                                        | 
|`"Escaped ^" Special Character in Quotes"`  |`Escaped " Special Character in Quotes`          |
|`" Mismatched containing quotes`            |undefined                                        |
