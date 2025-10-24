#include "GameSpecLoader.h"
#include <tree_sitter/api.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <iostream>


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

    //make sure it's valid range
    if (start >= size || end <= start) return {}; // returns an empty string

    return {src.begin() + start, src.begin() + end};

}

//this will read the hello-test.game, and print out to the terminal the config snippet, for testing that it can see the config block as a node
bool GameSpecLoader::HelloWorldSmokeTest(const char *path) {
    try {
        //read file
        const std::string src = slurp(path);

        //parse with tree-sitter
        TSParser *parser = ts_parser_new();
        ts_parser_set_language(parser, tree_sitter_socialgaming());
        TSTree *tree = ts_parser_parse_string(parser, nullptr, src.c_str(), static_cast<uint32_t>(src.size()));

        if (!tree) {
            std::cerr << "Parse failed: " << path << std::endl;
            ts_parser_delete(parser);
            return false;
        }

        TSNode root = ts_tree_root_node(tree);
        std::cout << "Parsed OK: " << path
                  << " | root =" << ts_node_type(root)
                  << " | children=" << ts_node_child_count(root)
                  << std::endl;

        //display the snippet of what the config file says so we can check if it parses
        uint32_t child_count = ts_node_child_count(root);
        for (uint32_t i = 0; i < child_count; ++i) {
            TSNode ch = ts_node_child(root, i);
            const char *ty = ts_node_type(ch);
            if (std::strcmp(ty, "configuration") == 0 ||
                std::strcmp(ty, "configuration_block") == 0) {
                std::string snippet = slice(src, ch);
                if (snippet.size() > 200) {
                    snippet.erase(200);
                    snippet.append("...");
                }
                std::cout << "\n Configuration snippet \n"
                          << snippet
                          << "\n";
            }
        }
        ts_tree_delete(tree);
        ts_parser_delete(parser);
        return true;
    } catch (const std::exception &e) {
        std::cerr << "HelloWorldSmokeTest error: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "HelloWorldSmokeTest error: unknown exception" << std::endl;
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
    ts_parser_set_language(parser, tree_sitter_socialgaming());
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
        const char *type = ts_node_type(child);

        //if it's config we will use a diff function because this can vary quite a bit.
        if (strcmp(type, "configuration") == 0) {
            parseConfiguration(text, child, spec);
        } else if (strcmp(type, "constants") == 0) {
            spec.constants = slice(text, child);
        } else if (strcmp(type, "variables") == 0) {
            spec.variables = slice(text, child);
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
        const char* type = ts_node_type(child);

        // Parse name (quoted string)
        if (strcmp(type, "quoted_string") == 0) {
            std::string nameWithQuotes = slice(src, child);
            if (nameWithQuotes.size() >= 2) {
                spec.name = nameWithQuotes.substr(1, nameWithQuotes.size() - 2);
            }
        }
        // Parse player range
        else if (strcmp(type, "number_range") == 0) {
            parsePlayerRange(src, child, spec);
        }
        // Parse audience
        else if (strcmp(type, "boolean") == 0) {
            std::string boolVal = slice(src, child);
            spec.hasAudience = (boolVal == "true");
        }
        // Parse setup block -- for later
        else if (strcmp(type, "json_object") == 0 || strcmp(type, "setup_block") == 0) {
            parseSetup(src, child, spec);
        }
    }
}

void GameSpecLoader::parsePlayerRange(const std::string &src, TSNode node, GameSpec &spec) {
    // Player range is typically (min, max)
    uint32_t child_count = ts_node_child_count(node);

    for (uint32_t i = 0; i < child_count; ++i) {
        TSNode child = ts_node_child(node, i);
        const char* type = ts_node_type(child);

        if (strcmp(type, "integer") == 0) {
            std::string numStr = slice(src, child);
            int value = std::stoi(numStr);

            // First integer is min, second is max
            if (spec.playerRange.min == 0) {
                spec.playerRange.min = value;
            } else {
                spec.playerRange.max = value;
            }
        }
    }
}

void GameSpecLoader::parseSetup(const std::string &src, TSNode node, GameSpec &spec) {
    // For now, we'll just capture the setup rules without deep parsing
    uint32_t child_count = ts_node_child_count(node);

    for (uint32_t i = 0; i < child_count; ++i) {
        TSNode child = ts_node_child(node, i);
        const char* type = ts_node_type(child);

        // Look for setup rule definitions
        // Each rule will have an id, kind, prompt, and optional range

    }
}



