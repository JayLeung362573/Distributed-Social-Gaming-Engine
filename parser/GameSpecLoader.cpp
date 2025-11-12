#include "GameSpecLoader.h"
#include <tree_sitter/api.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <spdlog/spdlog.h>


//for now we will just parse the config
extern "C" const TSLanguage *tree_sitter_socialgaming();
//for tree-sitters TSLLanguage structure, it's rules etc.
//tree_sitter_socialgaming() will generate when you run "tree-sitter generate" from the terminal

/*
 * Read a file into memory as a single string
 */
static std::string slurp(const std::string &path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("Cannot open file: " + path);

    std::ostringstream ss;
    ss << in.rdbuf(); // read entire file stream
    return ss.str(); // return as str

    /*
     ex: const std::string src = slurp("rock-paper-scissors.game");
     - This will read the files text into memory and lets Tree-Sitter parse it.
     */
}

/*
 * Take the full source text and the TSNode (we has start/end indices) and extract the substring that node has
 */
static std::string slice(const std::string &src, TSNode node) {
    auto start = ts_node_start_byte(node);
    auto end = ts_node_end_byte(node);
    auto size = static_cast<uint32_t>(src.size());

    // Bounds checking: ensure valid range
    if (start >= size || end <= start || end > size) {
        std::cerr << "Warning: slice() bounds error - start=" << start
                  << " end=" << end << " size=" << size << std::endl;
        return {};
    }

    return {src.begin() + start, src.begin() + end};

}

static int parseInteger(const std::string &src, TSNode node) {
    std::string numStr = slice(src, node);
    try {
        return std::stoi(numStr);
    } catch (const std::exception& e) {
        std::cerr << "Warning: parseInteger() failed for '" << numStr << "': " << e.what() << std::endl;
        return 0;
    }
}

static std::string parseQuotedString(const std::string &src, TSNode node) {
    std::string withQuotes = slice(src, node);
    if (withQuotes.size() >= 2) {
        return withQuotes.substr(1, withQuotes.size() - 2);
    }
    return withQuotes;
}

static bool parseBoolean(const std::string &src, TSNode node) {
    std::string boolStr = slice(src, node);
    return boolStr == "true";
}

//this will read the hello-test.game, and print out to the terminal the config snippet, for testing that it can see the config block as a node
bool GameSpecLoader::HelloWorldSmokeTest(const char *path) {
    try {
        //read file
        const std::string src = slurp(path);

        //parse with tree-sitter
        TSParser *parser = ts_parser_new();
        const TSLanguage* language = tree_sitter_socialgaming();
        ts_parser_set_language(parser, language);

        NodeType::init(language);

        TSTree *tree = ts_parser_parse_string(parser, nullptr, src.c_str(), static_cast<uint32_t>(src.size()));

        if (!tree) {
            spdlog::error("Parse failed: {}", path);
            ts_parser_delete(parser);
            return false;
        }

        TSNode root = ts_tree_root_node(tree);
        spdlog::info("Parsed OK: {} | root ={} | children={}", path, ts_node_type(root), ts_node_child_count(root));

        //display the snippet of what the config file says so we can check if it parses
        uint32_t child_count = ts_node_child_count(root);
        for (uint32_t i = 0; i < child_count; ++i) {
            TSNode ch = ts_node_child(root, i);
            if (ts_node_symbol(ch) == NodeType::CONFIGURATION ||
                ts_node_symbol(ch) == NodeType::CONFIGURATION_BLOCK) {
                std::string snippet = slice(src, ch);
                if (snippet.size() > 200) {
                    snippet.erase(200);
                    snippet.append("...");
                }
                spdlog::info("Configuration snippet:\n{}", snippet);
            }
        }
        ts_tree_delete(tree);
        ts_parser_delete(parser);
        return true;
    } catch (const std::exception &e) {
        spdlog::error("HelloWorldSmokeTest error: {}", e.what());
        return false;
    } catch (...) {
        spdlog::error("HelloWorldSmokeTest error: unknown exception");
        return false;
    }

}

GameSpec GameSpecLoader::loadFile(const char *path) {
    const std::string src = slurp(path);
    return loadString(src);
}

GameSpec GameSpecLoader::loadString(const std::string &text) {
    GameSpec spec;

    //Parse using Tree-Sitter.
    TSParser *parser = ts_parser_new();
    const TSLanguage* language = tree_sitter_socialgaming();
    ts_parser_set_language(parser, language);

    NodeType::init(language);

    TSTree *tree = ts_parser_parse_string(parser, nullptr, text.c_str(), text.size());

    if (!tree) {
        ts_parser_delete(parser);
        throw std::runtime_error("Parse failed");
    }

    TSNode root = ts_tree_root_node(tree);

    //loop through the children to parse the config of each
    uint32_t child_count = ts_node_child_count(root);

    for (uint32_t i = 0; i < child_count; ++i) {
        TSNode child = ts_node_child(root, i);
        const char *field_name = ts_node_field_name_for_child(root, i);

        if (!field_name) {
            if (ts_node_symbol(child) == NodeType::CONFIGURATION) {
                parseConfiguration(text, child, spec);
            }
            continue;
        }

        //if it's config we will use a diff function because this can vary quite a bit.
        if (strcmp(field_name, "configuration") == 0 || ts_node_symbol(child) == NodeType::CONFIGURATION) {
            parseConfiguration(text, child, spec);
        } else if (strcmp(field_name, "constants") == 0 || ts_node_symbol(child) == NodeType::CONSTANTS) {
            spec.constants = slice(text, child);
        } else if (strcmp(field_name, "variables") == 0 || ts_node_symbol(child) == NodeType::VARIABLES) {
            spec.variables = slice(text, child);
        } else if (strcmp(field_name, "rules") == 0 || ts_node_symbol(child) == NodeType::RULES) {
            parseRules(text, child, spec);
        }
    }


    //no memory leaks :).
    ts_tree_delete(tree);
    ts_parser_delete(parser);
    return spec;
}

void GameSpecLoader::parseConfiguration(const std::string &src, TSNode node, GameSpec &spec) {
    // Walk through children of the configuration block
    uint32_t child_count = ts_node_child_count(node);
    for (uint32_t i = 0; i < child_count; ++i) {
        TSNode child = ts_node_child(node, i);
        const char* field_name = ts_node_field_name_for_child(node, i);

        if (!field_name) continue;

        // Parse based on field name, not just type
        if (strcmp(field_name, "name") == 0 && ts_node_symbol(child) == NodeType::QUOTED_STRING) {
            spec.name = parseQuotedString(src, child);
        }
        else if (strcmp(field_name, "player_count") == 0 && ts_node_symbol(child) == NodeType::NUMBER_RANGE) {
            parsePlayerRange(src, child, spec);
        }
        else if (strcmp(field_name, "audience") == 0 && ts_node_symbol(child) == NodeType::BOOLEAN) {
            spec.hasAudience = parseBoolean(src, child);
        }
        else if (strcmp(field_name, "setup") == 0 &&
                 (ts_node_symbol(child) == NodeType::JSON_OBJECT || ts_node_symbol(child) == NodeType::SETUP_BLOCK)) {
            parseSetup(src, child, spec);
        }
    }
}

void GameSpecLoader::parsePlayerRange(const std::string &src, TSNode node, GameSpec &spec) {
    // Player range is typically (min, max)
    uint32_t child_count = ts_node_child_count(node);

    for (uint32_t i = 0; i < child_count; ++i) {
        TSNode child = ts_node_child(node, i);
        const char* field_name = ts_node_field_name_for_child(node, i);

        if (!field_name && ts_node_symbol(child) != NodeType::INTEGER) continue;

        if (ts_node_symbol(child) == NodeType::INTEGER) {
            int value = parseInteger(src, child);

            // Use field name to determine min vs max, fallback to order
            if (field_name && strcmp(field_name, "min") == 0) {
                spec.playerRange.min = value;
            } else if (field_name && strcmp(field_name, "max") == 0) {
                spec.playerRange.max = value;
            } else {
                // Fallback: first integer is min, second is max
                if (spec.playerRange.min == 0) {
                    spec.playerRange.min = value;
                } else {
                    spec.playerRange.max = value;
                }
            }
        }
    }
}

void GameSpecLoader::parseSetup(const std::string &src, TSNode node, GameSpec &spec) {
    // For now, we'll just capture the setup rules without deep parsing
    uint32_t child_count = ts_node_child_count(node);

    for (uint32_t i = 0; i < child_count; ++i) {
        TSNode child = ts_node_child(node, i);
        const char* field_name = ts_node_field_name_for_child(node, i);

        if (!field_name) continue;

        // Look for setup rule definitions
        // Each rule will have fields like: id, kind, prompt, and optional range
        // TODO: Implement parsing for setup rules based on field names

    }
}


//TODO: PARSE RULES
// AS they are read create an AST from that.


//TODO: Change the couts into logging
void GameSpecLoader::parseRules(const std::string &src, TSNode node, GameSpec &spec) {
    // The rules node might contain a body node, so we need to check
    uint32_t child_count = ts_node_named_child_count(node);

    std::cout << "\nPARSING RULES " << std::endl;

    // If there's only one child and it's a body, parse that instead.
    // This is specified in the grammar, it's a hidden node that comes after the rules bit now ma
    TSNode statementsNode = node;
    if (child_count == 1) {
        TSNode firstChild = ts_node_named_child(node, 0);
        if (ts_node_symbol(firstChild) == NodeType::BODY) {
            std::cout << "Rules contains a body node, parsing its contents" << std::endl;
            statementsNode = firstChild;
            child_count = ts_node_named_child_count(statementsNode);
        }
    }

    std::cout << "Found " << child_count << " statements" << std::endl;

    for (uint32_t i = 0; i < child_count; ++i) {
        TSNode child = ts_node_named_child(statementsNode, i);
        TSSymbol symbol = ts_node_symbol(child);

        // Each statement might be wrapped in a "rule" node,
        TSNode statementNode = child;
        if (symbol == NodeType::RULE) {
            // Get the actual statement inside the rule wrapper
            if (ts_node_named_child_count(child) > 0) {
                statementNode = ts_node_named_child(child, 0);
                symbol = ts_node_symbol(statementNode);
            }
        }

        // Check if this is an assignment
        if (symbol == NodeType::ASSIGNMENT) {
            std::cout << "\n[Statement " << i << "] Assignment found" << std::endl;

            // Use field names to get target and value directly
            TSNode target = ts_node_child_by_field_name(statementNode, "target", 6);
            TSNode value = ts_node_child_by_field_name(statementNode, "value", 5);

            if (!ts_node_is_null(target) && !ts_node_is_null(value)) {
                std::string targetText = slice(src, target);
                std::string valueText = slice(src, value);
                const char* targetType = ts_node_type(target);
                const char* valueType = ts_node_type(value);

                std::cout << "  Target: " << targetText << " (type: " << targetType << ")" << std::endl;
                std::cout << "  Value: " << valueText << " (type: " << valueType << ")" << std::endl;

                // TODO: Build AST nodes from target and value
            } else {
                std::cerr << "  Warning: Assignment missing target or value" << std::endl;
            }
        } else {
            // For now, skip non-assignment statements
            // TODO: Do all non-assignment sections
        }
    }

    std::cout << " END PARSING RULES " << std::endl;
}