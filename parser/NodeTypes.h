#pragma once
#include <tree_sitter/api.h>

namespace NodeType {
    inline TSSymbol INTEGER;
    inline TSSymbol BOOLEAN;
    inline TSSymbol QUOTED_STRING;
    inline TSSymbol NUMBER_RANGE;
    inline TSSymbol CONFIGURATION;
    inline TSSymbol CONFIGURATION_BLOCK;
    inline TSSymbol JSON_OBJECT;
    inline TSSymbol SETUP_BLOCK;
    inline TSSymbol CONSTANTS;
    inline TSSymbol VARIABLES;
    inline TSSymbol RULES;
    inline TSSymbol IDENTIFIER;

    // Assignment-related nodes
    inline TSSymbol BODY;
    inline TSSymbol RULE;
    inline TSSymbol ASSIGNMENT;
    inline TSSymbol QUALIFIED_IDENTIFIER;
    inline TSSymbol LIST_LITERAL;
    inline TSSymbol VALUE_MAP;
    inline TSSymbol EXPRESSION;

    inline void init(const TSLanguage* language) {
        INTEGER = ts_language_symbol_for_name(language, "integer", 7, true);
        BOOLEAN = ts_language_symbol_for_name(language, "boolean", 7, true);
        QUOTED_STRING = ts_language_symbol_for_name(language, "quoted_string", 13, true);
        NUMBER_RANGE = ts_language_symbol_for_name(language, "number_range", 12, true);
        CONFIGURATION = ts_language_symbol_for_name(language, "configuration", 13, true);
        CONFIGURATION_BLOCK = ts_language_symbol_for_name(language, "configuration_block", 19, true);
        JSON_OBJECT = ts_language_symbol_for_name(language, "json_object", 11, true);
        SETUP_BLOCK = ts_language_symbol_for_name(language, "setup_block", 11, true);
        CONSTANTS = ts_language_symbol_for_name(language, "constants", 9, true);
        VARIABLES = ts_language_symbol_for_name(language, "variables", 9, true);
        RULES = ts_language_symbol_for_name(language, "rules", 5, true);
        IDENTIFIER = ts_language_symbol_for_name(language, "identifier", 10, true);

        // Assignment-related nodes
        BODY = ts_language_symbol_for_name(language, "body", 4, true);
        RULE = ts_language_symbol_for_name(language, "rule", 4, true);
        ASSIGNMENT = ts_language_symbol_for_name(language, "assignment", 10, true);
        QUALIFIED_IDENTIFIER = ts_language_symbol_for_name(language, "qualified_identifier", 20, true);
        LIST_LITERAL = ts_language_symbol_for_name(language, "list_literal", 12, true);
        VALUE_MAP = ts_language_symbol_for_name(language, "value_map", 9, true);
        EXPRESSION = ts_language_symbol_for_name(language, "expression", 10, true);
    }
}
