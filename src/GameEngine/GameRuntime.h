#pragma once

#include <vector>
#include <span>
#include "GameInterpreter.h"
#include "Rules.h"

struct GameRules
{
    std::span<std::unique_ptr<ast::ASTNode>> statements;
};

class GameRuntime{
public:
    GameRuntime(GameRules rules)
    : m_rules(std::move(rules)), m_interpreter()
    {}

    void run(){
        for(auto& statement : m_rules.statements){
            statement->accept(m_interpreter);
        }
    }

    const VariableMap& getGameState() const{
        return m_interpreter.getGameState();
    }

private:
    GameRules m_rules;
    GameInterpreter m_interpreter;
};