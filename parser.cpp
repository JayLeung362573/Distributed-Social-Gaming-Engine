#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <cctype>


using namespace std;

int main(void){
    
    std::string filename;
    std::cout <<"filename : ";
    std::getline(cin,filename); 

    std::string text = load_file(filename);
    GameDoc game = parse_game_doc(text);

    return 0;
}

// take file into string 
static inline std::string load_file(const std::string& path) {
    std::ifstream in(path);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

// finds each block thats in the config 
static std::string find_block(const std::string& text, const std::string& header) {
    std::size_t hpos = text.find(header);
    if (hpos == std::string::npos) return {};     
    std::size_t brace = text.find('{', hpos);     
    if (brace == std::string::npos) return {};         
    int depth = 0;
    std::size_t start_content = brace + 1;
    for (std::size_t i = brace; i < text.size(); ++i) {
        if (text[i] == '{')      depth++;               
        else if (text[i] == '}') {
            depth--;                                     
            if (depth == 0) {
               
                return text.substr(start_content, i - start_content);
            }
        }
    }
    return {}; 
}



static void parse_config(const std::string& block, Configuration& cfg) {
    // nam
    {
        std::regex rx(R"(name\s*:\s*\"([^\"]*)\")");
        std::smatch m;
        if (std::regex_search(block, m, rx)) cfg.name = m[1].str();
    }
    // player range: (2, 4)
    {
        std::regex rx(R"(player\s*range\s*:\s*\(\s*(\d+)\s*,\s*(\d+)\s*\))");
        std::smatch m;
        if (std::regex_search(block, m, rx)) {
            cfg.player_min = std::stoi(m[1].str());
            cfg.player_max = std::stoi(m[2].str());
        }
    }
    // audience: true/false
    {
        std::regex rx(R"(audience\s*:\s*(true|false))");
        std::smatch m;
        if (std::regex_search(block, m, rx)) {
            cfg.audience = (m[1].str() == "true");
        }
    }

}


static void parse_const(const std::string& block, Constants& c);
static void parse_var(const std::string& block, Constants& c);
static void parse_player(const std::string& block, Constants& c);
static void parse_audience(const std::string& block, Constants& c);
static void parse_rules(const std::string& block, Constants& c);

static GameDoc parse_game_doc(const std::string& src) {
    GameDoc g;

    std::string conf = find_block(src, "configuration");
    if (!conf.empty()) parse_config(conf, g.config);

    std::string constants = find_block(src, "constants");
    if (!constants.empty()) parse_const(constants, g.constants);

    std::string vars = find_block(src, "variables");
    if (!vars.empty()) parse_var(vars, g.variables);

    std::string per_player = find_block(src, "per-player");
    if (!per_player.empty()) parse_player(per_player, g.per_player);

    std::string per_audience = find_block(src, "per-player");
    if (!per_player.empty()) parse_audience(per_player, g.per_player);

    std::string rules = find_block(src, "rules");
    if (!rules.empty()) parse_rules(rules, g.rules_raw);

    return g;
}

struct Configuration {
    std::string name;
    int player_min = 0;
    int player_max = 0;
    bool audience;
    //set up 
};

struct Constants{
};

struct Variables{
};

struct Per_player{};

struct Per_audience{};

struct rules{};

struct GameDoc {
    Configuration config;
    Constants constants;
    Variables variables;
    Per_player per_player;
    Per_audience per_audience;
    std::string rules_raw;
};