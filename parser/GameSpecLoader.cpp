//
// Created by Caiden Merklin on 10/13/25.
//
#include "GameSpecLoader.h"
#include <tree_sitter/api.h>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring>

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

