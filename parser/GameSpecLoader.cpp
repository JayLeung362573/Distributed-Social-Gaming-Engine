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

