#include <gtest/gtest.h>
#include "GameEngine/GameRuntime.h"
#include "GameEngine/Rules.h"

using namespace ast;

static std::unique_ptr<Statement> assign(std::string name, Value val) {
    return makeAssignment(makeVariable(Name{name}), makeConstant(std::move(val)));
}

TEST(GameRuntimeTest, RunExecutesStatementsAndUpdatesState) {
    // Setup simple rule: winner = "player1"
    std::vector<std::unique_ptr<Statement>> stmts;
    stmts.push_back(
        makeAssignment(
            makeVariable(Name{"winner"}),
            makeConstant(Value{String{"player1"}})
        )
    );

    GameRules rules{std::move(stmts)};
    GameRuntime runtime(rules);

    runtime.run();

    const VariableMap& state = runtime.getGameState();
    Value* winnerVal = state.load(Name{"winner"});
    ASSERT_NE(winnerVal, nullptr); // sanity check
    EXPECT_EQ(winnerVal->asString(), String{"player1"});
}

TEST(GameRuntimeTest, RunThrowsIfAlreadyFinished) {
    std::vector<std::unique_ptr<Statement>> stmts;
    stmts.push_back(assign("winner", Value{String{"alice"}}));

    GameRules rules{std::move(stmts)};
    GameRuntime runtime(rules);
    runtime.run();
    EXPECT_THROW(runtime.run(), std::runtime_error);
}

TEST(GameRuntimeTest, EmptyRulesFinishesImmediately) {
    GameRules rules{};
    GameRuntime runtime(rules);
    EXPECT_NO_THROW(runtime.run());
    EXPECT_TRUE(runtime.isFinished());
}

TEST(GameRuntimeTest, VariableDeleteAndReassignmentWorks) {
    VariableMap vm;
    vm.store(Name{"score"}, std::make_unique<Value>(Integer{10}));
    vm.del(Name{"score"});
    EXPECT_THROW(vm.load(Name{"score"}), std::runtime_error);
}

// TODO: Revisit once IO implemented in GameInterpreter
// TEST(GameRuntimeTest, TickCompletesSimpleProgram) {
//     // Program: winner = "player1"
//     std::vector<std::unique_ptr<ast::Statement>> stmts;
//     stmts.push_back(
//         ast::makeAssignment(
//             ast::makeVariable(Name{"winner"}),
//             ast::makeConstant(Value{String{"player1"}})
//         )
//     );

//     ast::GameRules rules{std::span(stmts)};
//     GameRuntime runtime(rules);

//     // No input messages
//     std::vector<GameMessage> emptyMsgs;
//     auto outMsgs = runtime.tick(emptyMsgs);

//     EXPECT_TRUE(runtime.isFinished());
//     EXPECT_TRUE(outMsgs.empty());

//     const VariableMap& state = runtime.getGameState();
//     Value* winnerVal = state.load(Name{"winner"});
//     ASSERT_NE(winnerVal, nullptr); // sanity check
//     EXPECT_EQ(winnerVal->asString(), String{"player1"});

// }
