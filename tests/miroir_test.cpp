#define MIROIR_IMPLEMENTATION
#define MIROIR_YAMLCPP_SPECIALIZATION
#include <miroir/miroir.hpp>

#include <doctest/doctest.h>
#include <yaml-cpp/yaml.h>

#include <vector>

/// Misc

static inline void validate(const std::string &schema_str, const std::string &doc_str) {
    const YAML::Node schema = YAML::Load(schema_str);
    const miroir::Validator<YAML::Node> validator{schema};
    const YAML::Node doc = YAML::Load(doc_str);
    const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
    CHECK(errors.empty());
}

/// Built-in types

TEST_CASE("any type validation") {
    const YAML::Node schema = YAML::Load("root: any");
    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("scalar value is valid") {
        const YAML::Node doc = YAML::Load("42.0");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("sequence value is valid") {
        const YAML::Node doc = YAML::Load("[ 1, 2, 3 ]");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("map value is valid") {
        const YAML::Node doc = YAML::Load("{ key: value }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }
}

TEST_CASE("scalar type validation") {
    const YAML::Node schema = YAML::Load("root: scalar");
    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("scalar value is valid") {
        const YAML::Node doc = YAML::Load("42.0");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("sequence value is invalid") {
        const YAML::Node doc = YAML::Load("[ 1, 2, 3 ]");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value type: scalar");
    }

    SUBCASE("map value is invalid") {
        const YAML::Node doc = YAML::Load("{ key: value }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value type: scalar");
    }
}

TEST_CASE("numeric type validation") {
    const YAML::Node schema = YAML::Load("root: numeric");
    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("integer value is valid") {
        const YAML::Node doc = YAML::Load("42");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("float valud is valid") {
        const YAML::Node doc = YAML::Load("42.0");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("string value is invalid") {
        const YAML::Node doc = YAML::Load("some string");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value type: numeric");
    }
}

TEST_CASE("integer type validation") {
    const YAML::Node schema = YAML::Load("root: integer");
    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("integer value is valid") {
        const YAML::Node doc = YAML::Load("42");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("float value is invalid") {
        const YAML::Node doc = YAML::Load("42.0");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value type: integer");
    }

    SUBCASE("string value is invalid") {
        const YAML::Node doc = YAML::Load("some string");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value type: integer");
    }
}

TEST_CASE("boolean type validation") {
    const YAML::Node schema = YAML::Load("root: [boolean]");
    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("boolean values are valid") {
        const YAML::Node doc = YAML::Load("[ true, false, y, n, yes, no, on, off ]");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("string value is invalid") {
        const YAML::Node doc = YAML::Load("[ true, some string ]");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/1: expected value type: boolean");
    }
}

TEST_CASE("string type validation") {
    const YAML::Node schema = YAML::Load("root: [string]");
    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("string values are valid") {
        const YAML::Node doc = YAML::Load("[ some string, '42', 'true', '42.0' ]");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 0);
        CHECK(errors.empty());
    }

    SUBCASE("values of other scalar types are invalid") {
        const YAML::Node doc = YAML::Load("[ true, 42, 42.0 ]");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 3);
        CHECK(errors[0].description() == "/0: expected value type: string");
        CHECK(errors[1].description() == "/1: expected value type: string");
        CHECK(errors[2].description() == "/2: expected value type: string");
    }
}

TEST_CASE("any sequence validation") {
    const YAML::Node schema = YAML::Load("root: []");
    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("sequence of integers is valid") {
        const YAML::Node doc = YAML::Load("[ 1, 2, 3 ]");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("scalar value is invalid") {
        const YAML::Node doc = YAML::Load("42.0");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value type: []");
    }
}

TEST_CASE("any map validation") {
    const YAML::Node schema = YAML::Load("root: {}");
    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("map value is valid") {
        const YAML::Node doc = YAML::Load("{ key: value }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("scalar value is invalid") {
        const YAML::Node doc = YAML::Load("42.0");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value type: {}");
    }
}

TEST_CASE("type aliases validation") {
    SUBCASE("map type aliases") {
        validate("root: map", "{ key: value }");
        validate("root: {}", "{ key: value }");
    }

    SUBCASE("sequence type aliases") {
        validate("root: list", "[ 1, 2, 3 ]");
        validate("root: []", "[ 1, 2, 3 ]");
    }

    SUBCASE("numeric type aliases") {
        validate("root: numeric", "42.0");
        validate("root: num", "42.0");
    }

    SUBCASE("integer type aliases") {
        validate("root: integer", "42");
        validate("root: int", "42");
    }

    SUBCASE("boolean type aliases") {
        validate("root: boolean", "true");
        validate("root: bool", "true");
    }

    SUBCASE("string type aliases") {
        validate("root: string", "hello");
        validate("root: str", "hello");
    }
}

/// Custom types

TEST_CASE("custom type validation") {
    const YAML::Node schema = YAML::Load(R"(
    types:
      custom_type: scalar
    root: custom_type
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("scalar value is valid") {
        const YAML::Node doc = YAML::Load("42.0");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("sequence value is invalid") {
        const YAML::Node doc = YAML::Load("[ 1, 2, 3 ]");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value type: scalar");
    }

    SUBCASE("map value is invalid") {
        const YAML::Node doc = YAML::Load("{ key: value }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value type: scalar");
    }
}

/// Sequence

TEST_CASE("sequence validation") {
    const YAML::Node schema = YAML::Load(R"(
    root:
      - name: scalar
        description: any
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("sequence value is valid") {
        const YAML::Node doc = YAML::Load(R"(
        - name: Some name 1
          description: Some description 1
        - name: Some name 2
          description: Some description 2
        )");

        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("node at index 1 has invalid type") {
        const YAML::Node doc = YAML::Load(R"(
        - name: Some name 1
          description: Some description 1
        - name: [ 1, 2, 3 ]
          description: Some description 2
        )");

        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/1.name: expected value type: scalar");
    }

    SUBCASE("node at index 1 has invalid structure") {
        const YAML::Node doc = YAML::Load(R"(
        - name: Some name 1
          description: Some description 1
        - description: Some description 2
        )");

        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/1.name: node not found");
    }

    SUBCASE("scalar value is invalid") {
        const YAML::Node doc = YAML::Load("42");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() ==
              "/: expected value type: [{name: scalar, description: any}]");
    }
}

/// Variants

TEST_CASE("value variant validation") {
    const YAML::Node schema = YAML::Load(R"(
    root: !variant
      - 42
      - some string
      - [ 1, 2, 3 ]
      - { key: key, value: value }
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("value '42' is valid") {
        const YAML::Node doc = YAML::Load("42");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("value 'some string' is valid") {
        const YAML::Node doc = YAML::Load("some string");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("sequence '[ 1, 2, 3 ]' is valid") {
        const YAML::Node doc = YAML::Load("[ 1, 2, 3 ]");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("map '{ key: key, value: value }' is valid") {
        const YAML::Node doc = YAML::Load("{ key: key, value: value }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("value '420' is invalid") {
        const YAML::Node doc = YAML::Load("420");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value: one of"
                                         "\n\t- 42"
                                         "\n\t- some string"
                                         "\n\t- [1, 2, 3]"
                                         "\n\t- {key: key, value: value}");
    }
}

TEST_CASE("key value variant validation") {
    const YAML::Node schema = YAML::Load(R"(
    types:
      key: !variant
        - first
        - second
    root:
      $key: any
      required: any
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("map with key 'first' is valid") {
        const YAML::Node doc = YAML::Load("{ first: 42, required: 24 }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("map with key 'second' is valid") {
        const YAML::Node doc = YAML::Load("{ second: 42, required: 24 }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("map with key 'third' is invalid") {
        const YAML::Node doc = YAML::Load("{ third: 42, required: 24 }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 2);
        CHECK(errors[0].description() == "/: missing key with type: key");
        CHECK(errors[1].description() == "/third: undefined node");
    }
}

TEST_CASE("type variant validation") {
    const YAML::Node schema = YAML::Load(R"(
    root:
      - scalar
      - [scalar]
      - { key: scalar, value: [scalar], optional: !optional scalar }
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("scalar value is valid") {
        const YAML::Node doc = YAML::Load("42");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("sequence of scalar values is valid") {
        const YAML::Node doc = YAML::Load("[ 1, 2, 3 ]");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("value of type '{ key: scalar, value: [scalar] }' is valid") {
        const YAML::Node doc = YAML::Load("{ key: 42, value: [ 1, 2, 3 ] }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("value of type '{ key: scalar, value: scalar }' is invalid") {
        const YAML::Node doc = YAML::Load("{ key: 42, value: 420 }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value type: one of"
                                         "\n\t- scalar"
                                         "\n\t- [scalar]"
                                         "\n\t- {key: scalar, value: [scalar], "
                                         "optional: !<!optional> scalar}"
                                         "\n\t* failed variant 0:"
                                         "\n\t\t/: expected value type: scalar"
                                         "\n\t* failed variant 1:"
                                         "\n\t\t/: expected value type: [scalar]"
                                         "\n\t* failed variant 2:"
                                         "\n\t\t/value: expected value type: {key: scalar, value: "
                                         "[scalar], optional: !<!optional> scalar}");
    }

    SUBCASE("value of type '{ name: scalar, description: scalar }' is invalid") {
        const YAML::Node doc = YAML::Load("{ name: 42, description: 420 }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() ==
              "/: expected value type: one of"
              "\n\t- scalar"
              "\n\t- [scalar]"
              "\n\t- {key: scalar, value: [scalar], optional: !<!optional> scalar}"
              "\n\t* failed variant 0:"
              "\n\t\t/: expected value type: scalar"
              "\n\t* failed variant 1:"
              "\n\t\t/: expected value type: [scalar]"
              "\n\t* failed variant 2:"
              "\n\t\t/key: node not found"
              "\n\t\t/value: node not found"
              "\n\t\t/name: undefined node"
              "\n\t\t/description: undefined node");
    }
}

TEST_CASE("nested errors") {
    const YAML::Node schema = YAML::Load(R"(
    types:
      target:
        - library: string
        - executable: string
    root:
      targets: [target]
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    const YAML::Node doc = YAML::Load(R"(
        targets:
          - library: library
          - executable: executable
            undefined_key: anything
        )");

    const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);

    SUBCASE("infinite error depth") {
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/targets.1: expected value type: target"
                                         "\n\t* failed variant 0:"
                                         "\n\t\t/targets.1.library: node not found"
                                         "\n\t\t/targets.1.executable: undefined node"
                                         "\n\t\t/targets.1.undefined_key: undefined node"
                                         "\n\t* failed variant 1:"
                                         "\n\t\t/targets.1.undefined_key: undefined node");
    }

    SUBCASE("single error depth") {
        CHECK(errors.size() == 1);
        CHECK(errors[0].description(1) == "/targets.1: expected value type: target");
    }
}

/// Structure

TEST_CASE("required structure validation") {
    const YAML::Node schema = YAML::Load(R"(
    types:
      custom_type:
        name: scalar
        description: any
    root: custom_type
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("structure is valid") {
        const YAML::Node doc = YAML::Load(R"(
        name: some name
        description: [ 1, 2, 3 ]
        )");

        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("'description' node not found") {
        const YAML::Node doc = YAML::Load("name: some name");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/description: node not found");
    }

    SUBCASE("both nodes aren't found") {
        const YAML::Node doc = YAML::Load("{}");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 2);
        CHECK(errors[0].description() == "/name: node not found");
        CHECK(errors[1].description() == "/description: node not found");
    }

    SUBCASE("value is not a map") {
        const YAML::Node doc = YAML::Load("");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 2);
        CHECK(errors[0].description() == "/name: node not found");
        CHECK(errors[1].description() == "/description: node not found");
    }
}

TEST_CASE("optional structure validation") {
    const YAML::Node schema = YAML::Load(R"(
    types:
      custom_type:
        name: !optional scalar
        description: !optional any
    root: custom_type
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("structure is valid") {
        const YAML::Node doc = YAML::Load(R"(
        name: some name
        description: [ 1, 2, 3 ]
        )");

        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("'description' node is optional") {
        const YAML::Node doc = YAML::Load("name: some name");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("value is not a map") {
        const YAML::Node doc = YAML::Load("");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value type: custom_type");
    }
}

TEST_CASE("embedded structure validation") {
    const YAML::Node schema = YAML::Load(R"(
    types:
      custom_type:
        name: scalar
        description: any
    root:
      _: !embed custom_type
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("structure is valid") {
        const YAML::Node doc = YAML::Load(R"(
        name: some name
        description: [ 1, 2, 3 ]
        )");

        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("both nodes aren't found") {
        const YAML::Node doc = YAML::Load("{}");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 2);
        CHECK(errors[0].description() == "/name: node not found");
        CHECK(errors[1].description() == "/description: node not found");
    }

    SUBCASE("value is not a map") {
        const YAML::Node doc = YAML::Load("");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value type: {_: !<!embed> custom_type}");
    }
}

TEST_CASE("optional embedded structure validation") {
    const YAML::Node schema = YAML::Load(R"(
    types:
      custom_type:
        name: !optional scalar
        description: !optional any
    root:
      _: !embed custom_type
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("value is not a map") {
        const YAML::Node doc = YAML::Load("");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value type: {_: !<!embed> custom_type}");
    }
}

TEST_CASE("key type validation") {
    const YAML::Node schema = YAML::Load(R"(
    root:
      $numeric: any
      $boolean: any
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("numeric and boolean keys are valid") {
        const YAML::Node doc = YAML::Load("{ 420: [ 1, 2, 3 ], 42.0: 123, true: 1 }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("missing numeric and boolean keys") {
        const YAML::Node doc = YAML::Load("{ some_key: [ 1, 2, 3 ], another_key: 123 }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 4);
        CHECK(errors[0].description() == "/: missing key with type: numeric");
        CHECK(errors[1].description() == "/: missing key with type: boolean");
        CHECK(errors[2].description() == "/some_key: undefined node");
        CHECK(errors[3].description() == "/another_key: undefined node");
    }

    SUBCASE("scalar value is invalid") {
        const YAML::Node doc = YAML::Load("some string");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value type: {$numeric: any, $boolean: any}");
    }
}

TEST_CASE("embedded key type validation") {
    const YAML::Node schema = YAML::Load(R"(
    types:
      embedded:
        $integer: any
    root:
      _1: !embed
        $numeric: any
      _2: !embed embedded
      $boolean: any
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("numeric and boolean keys are valid") {
        const YAML::Node doc = YAML::Load("{ 420: [ 1, 2, 3 ], 42.0: 123, true: 1 }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("missing numeric and boolean keys") {
        const YAML::Node doc = YAML::Load("{ some_key: [ 1, 2, 3 ], another_key: 123 }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 5);
        CHECK(errors[0].description() == "/: missing key with type: numeric");
        CHECK(errors[1].description() == "/: missing key with type: integer");
        CHECK(errors[2].description() == "/: missing key with type: boolean");
        CHECK(errors[3].description() == "/some_key: undefined node");
        CHECK(errors[4].description() == "/another_key: undefined node");
    }

    SUBCASE("scalar value is invalid") {
        const YAML::Node doc = YAML::Load("some string");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: expected value type: {_1: !<!embed> {$numeric: any}, "
                                         "_2: !<!embed> embedded, $boolean: any}");
    }
}

/// Schema settings

TEST_CASE("schema settings with default_required = false") {
    const YAML::Node schema = YAML::Load(R"(
    settings:
      default_required: false
    root:
      name: !required scalar
      description: any
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("'description' node is optional by default") {
        const YAML::Node doc = YAML::Load("name: some name");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("'name' node not found") {
        const YAML::Node doc = YAML::Load("{}");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/name: node not found");
    }
}

TEST_CASE("schema settings with custom tag names") {
    const YAML::Node schema = YAML::Load(R"(
    settings:
      optional_tag: my_optional
      required_tag: my_required
      embed_tag: my_embed
    root:
      _: !my_embed
        name: !my_required scalar
        description: !my_optional any
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("structure is valid") {
        const YAML::Node doc = YAML::Load("name: some name");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }
}

/// Attributes

TEST_CASE("ignore attributes = false (default)") {
    const YAML::Node schema = YAML::Load("root: { key: string }");
    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("key without attributes is valid") {
        const YAML::Node doc = YAML::Load("key: some string");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("key with attribute is invalid") {
        const YAML::Node doc = YAML::Load("key:ATTR: some string");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 2);
        CHECK(errors[0].description() == "/key: node not found");
        CHECK(errors[1].description() == "/key:ATTR: undefined node");
    }
}

TEST_CASE("ignore attributes = true") {
    const YAML::Node schema = YAML::Load(R"(
    settings:
      ignore_attributes: true
    root:
      key: string
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("key without attributes is valid") {
        const YAML::Node doc = YAML::Load("key: some string");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("key with one attribute is valid") {
        const YAML::Node doc = YAML::Load("key:ATTR: some string");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("key with few attributes is valid") {
        const YAML::Node doc = YAML::Load("key:ATTR:ATTR: some string");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }
}

/// Generic types

TEST_CASE("generic list validation") {
    const YAML::Node schema = YAML::Load(R"(
    types:
      custom_boolean: boolean
      list<T>:
        - T
        - [T]
    root:
      boolean_list: list<custom_boolean>
      scalar_list: list<scalar>
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("list of boolean values and list of scalar values are valid") {
        const YAML::Node doc = YAML::Load(R"(
        boolean_list: true
        scalar_list: [ 1, 2, some string ]
        )");

        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("list of boolean values with string values is invalid") {
        const YAML::Node doc = YAML::Load(R"(
        boolean_list: some string
        scalar_list: []
        )");

        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/boolean_list: expected value type: list<custom_boolean>"
                                         "\n\t* failed variant 0:"
                                         "\n\t\t/boolean_list: expected value type: boolean"
                                         "\n\t* failed variant 1:"
                                         "\n\t\t/boolean_list: expected value type: [T]");
    }
}

TEST_CASE("generic key validation") {
    const YAML::Node schema = YAML::Load(R"(
    types:
      generic<T>: T
    root:
      $generic<string>: any
      $generic<boolean>: any
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("boolean and string keys are valid") {
        const YAML::Node doc = YAML::Load("{ '42': value, true: value }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("missing string key") {
        const YAML::Node doc = YAML::Load("true: value");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/: missing key with type: generic<string>");
    }
}

TEST_CASE("multiple generic args validation") {
    const YAML::Node schema = YAML::Load(R"(
    types:
      one_of<T;Y>: [T, Y]
    root: [one_of<boolean;integer>]
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("generic type accept boolean and integer values") {
        const YAML::Node doc = YAML::Load("[ true, 42, false, 12 ]");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("generic type doesn't accept string value") {
        const YAML::Node doc = YAML::Load("[ true, 42, false, 12, some string ]");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/4: expected value type: one_of<boolean;integer>"
                                         "\n\t* failed variant 0:"
                                         "\n\t\t/4: expected value type: boolean"
                                         "\n\t* failed variant 1:"
                                         "\n\t\t/4: expected value type: integer");
    }
}

TEST_CASE("nested generic args validation") {
    const YAML::Node schema = YAML::Load(R"(
    types:
      one_of<T;Y>: [T, Y]
    root: [one_of<boolean;one_of<integer;string>>]
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("generic type accept boolean, integer and string values") {
        const YAML::Node doc = YAML::Load("[ true, 42, false, 12, some string ]");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }
}

TEST_CASE("passed generic args validation") {
    const YAML::Node schema = YAML::Load(R"(
    types:
      some<T>: T
      list<T>: [some<T>]
      single_or_list<T>:
        - T
        - list<T>
    root: single_or_list<string>
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("generic type accept list of string values") {
        const YAML::Node doc = YAML::Load("[ hello, world ]");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("generic type accept single string value") {
        const YAML::Node doc = YAML::Load("hello");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }
}

TEST_CASE("generic map validation") {
    const YAML::Node schema = YAML::Load(R"(
    types:
      map<K;V>: { $K: V }
    root: map<integer;boolean>
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("map of integer keys and boolean values is valid") {
        const YAML::Node doc = YAML::Load("{ 42: true, 24: false }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("map with string value is invalid") {
        const YAML::Node doc = YAML::Load("{ 42: true, 24: some string }");
        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() == "/24: expected value type: boolean");
    }
}

TEST_CASE("'if' generic type validation") {
    const YAML::Node schema = YAML::Load(R"(
    types:
      if<T>:
        - T
        - - - if: string
              then: T
            - T
    root: if<integer>
    )");

    const miroir::Validator<YAML::Node> validator{schema};

    SUBCASE("'if' is valid") {
        const YAML::Node doc = YAML::Load(R"(
        - if: hello
          then: 42
        - 24
        - 420
        )");

        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.empty());
    }

    SUBCASE("'if' is invalid") {
        const YAML::Node doc = YAML::Load(R"(
        - if: hello
          then: not an integer
        - not an integer
        - 42
        )");

        const std::vector<miroir::Error<YAML::Node>> errors = validator.validate(doc);
        CHECK(errors.size() == 1);
        CHECK(errors[0].description() ==
              "/: expected value type: if<integer>"
              "\n\t* failed variant 0:"
              "\n\t\t/: expected value type: integer"
              "\n\t* failed variant 1:"
              "\n\t\t/0: expected value type: [[{if: string, then: T}, T]]"
              "\n\t\t\t* failed variant 0:"
              "\n\t\t\t\t/0.then: expected value type: integer"
              "\n\t\t\t* failed variant 1:"
              "\n\t\t\t\t/0: expected value type: integer"
              "\n\t\t/1: expected value type: [[{if: string, then: T}, T]]"
              "\n\t\t\t* failed variant 0:"
              "\n\t\t\t\t/1.if: node not found"
              "\n\t\t\t\t/1.then: node not found"
              "\n\t\t\t* failed variant 1:"
              "\n\t\t\t\t/1: expected value type: integer");
    }
}

// todo: test custom generic brackets and separator
// todo: test custom attribute separator
