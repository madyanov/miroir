# miroir

YAML schema validator for C++.

Features:

- Custom and generic types
- Type and value variants
- No runtime dependencies (well, technically)

Designed to work with [yaml-cpp](https://github.com/jbeder/yaml-cpp), but can be used with any library for which the [`miroir::NodeAccessor` template](https://gitlab.com/madyanov/miroir/-/blob/master/include/miroir/miroir.hpp#L15) specialization exists.

## Requirements

- GCC/Clang with C++20 standard support

## Usage

```cpp
#define MIROIR_IMPLEMENTATION           // define once to instantiate implementation
#define MIROIR_YAMLCPP_SPECIALIZATION   // define once to instantiate yaml-cpp bindings
#include <miroir/miroir.hpp>

#include <yaml-cpp/yaml.h>

...

// load schema and create validator
auto schema = YAML::LoadFile("path/to/schema.yml");
auto validator = miroir::Validator<YAML::Node>(schema);

// validate yaml document
auto document = YAML::LoadFile("path/to/document.yml");
auto errors = validator.validate(document);

// print validation errors
for (const auto &err : errors) {
    std::cerr << err.description() << std::endl;
}

```

Real-life usage example:

- [Loading](https://gitlab.com/madyanov/cgen/-/blob/master/src/libcgen/config.cpp#L113) and [validation](https://gitlab.com/madyanov/cgen/-/blob/master/src/libcgen/config.cpp#L123)
- [Schema](https://gitlab.com/madyanov/cgen/-/blob/master/src/libcgen/cgen.schema.yml.in) and [conforming document](https://gitlab.com/madyanov/cgen/-/blob/master/.cgen.yml)

## Built-in types

> :information_source: Quoted values are always represent a string.

<table>
<tr>
<td>

```yml
any     # any type
map     # any map
list    # any sequence
scalar  # any scalar
```

</td>
<td>

```yml
numeric
num

integer
int
```

</td>
<td>

```yml
boolean
bool

string
str
```

</td>
</tr>
</table>

## Schema specification

> :boom: Creating a `miroir::Validator` with invalid schema can lead to assertion failures on `Debug` builds and undefined behavior or crashes on `Release` builds.

```yml

# schema settings, optional
# defines validator behavior
settings:
    default_required: bool      # all fields are required unless otherwise specified, default: true
    optional_tag: string        # tag to mark optional fields, default: "optional"
    required_tag: string        # tag to mark required fields, default: "required"
    embed_tag: string           # tag to mark embedded fields, default: "embed"
    variant_tag: string         # tag to mark value variants, default: "variant"
    key_type_prefix: char       # prefix to mark typed keys, default: "$"
    generic_brackets: char[2]   # generic type specifiers, default: "<>"
    generic_separator: char     # generic arguments separator, default: ";"
    ignore_attributes: bool     # ignore key attributes, default: false

# custom types, optional
types:
    # types can be generic
    list<T>: [T]
    map<K;V>: { $K: V }
    bool_list: list<bool>           # same as `[bool]`
    string_map: map<string;string>  # same as `{ $string: string }`

    # sequence with one type takes sequence of values of that type
    # following type takes sequence of integer values
    int_sequence: [int]

    # sequence of more than one type is a type variant
    # following type takes any string or bool value
    string_or_bool:
        - string
        - bool

    # `!variant` tag is used to define value variants
    # following type takes only specified values: red, green or blue
    color: !variant
        - red
        - green
        - blue

    # types can be compound
    car:
        # brand field is required and takes string value
        brand: string
        # color field is required and takes values: red, green or blue
        color: color
        # owner_id field is optional and takes integer value
        owner_id: !optional int

    # compound types can be embedded into each other
    # following type reuses fields from the car type
    self_driving_car:
        is_self_driving: bool
        # embed fields from the car type
        # name of the key is irrelevant but must be unique
        irrelevant_key_name: !embed car

    # keys also can be typed when prefixed with `$`
    # following type key takes values: red, gree or blue
    color_descriptions:
        $color: string

# document schema, required
# represents document format
root:
    date: !optional string
    cars: [self_driving_car]
    colors: !embed color_descriptions

```

## Contributing

- Use [cgen](https://gitlab.com/madyanov/cgen) to generate the `CMakeLists.txt` file
- Run all CI checks locally with `make ci`
- List available Make targets with `make help`
- Be sure not to use almost 20-years-old default Make 3.81 on macOS
