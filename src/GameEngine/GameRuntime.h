#pragma once

#include <vector>
#include <span>
#include "GameInterpreter.h"
#include "Rules.h"

struct GameRules
{
    std::span<std::unique_ptr<ast::Statement>> statements;
};

/**
 * wrapper for interpreter for executing game logic
 * Jobs:
 * manage execution state (pause / resume for new input)
 */
class GameRuntime{
public:
    GameRuntime(GameRules rules)
    : m_rules(rules), m_interpreter()
    {}

    void run(){
        for(auto& statement : m_rules.statements){
            statement->accept(m_interpreter);
        }
    }

    std::vector<GameMessage>
    tick(const std::vector<GameMessage>& inMessages){
        if(m_finished){
            return {};
        }

        m_interpreter.setInGameMessages(inMessages);

        while(m_currentStatement < m_rules.statements.size()){
            auto& statement = m_rules.statements[m_currentStatement];
            VisitResult result = statement->accept(m_interpreter);

            if(result.isPending()){
                return m_interpreter.consumeOutGameMessages();
            }
            m_currentStatement++;
        }
        m_finished = true;
        return m_interpreter.consumeOutGameMessages();
    }

    const VariableMap& getGameState() const{
        return m_interpreter.getGameState();
    }

    bool
    isFinished() const{
        return m_finished;
    }

private:
    GameRules m_rules;
    GameInterpreter m_interpreter;
    size_t m_currentStatement = 0;
    bool m_finished = false;
};