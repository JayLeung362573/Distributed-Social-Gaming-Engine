#pragma once

#include <memory>
#include <string>
#include <tree_sitter/api.h>
#include "src/GameEngine/Rules.h"
#include "src/GameEngine/Types.h"

// Converts Tree-Sitter nodes to AST nodes
class ASTConverter {
public:
    // Convert expression nodes
    static std::unique_ptr<ast::Expression>
    convertExpression(const std::string& src, TSNode node);

    // Convert statement nodes
    static std::unique_ptr<ast::ASTNode>
    convertStatement(const std::string& src, TSNode node);

private:
    // Expression conversions
    static std::unique_ptr<ast::Constant>
    convertConstant(const std::string& src, TSNode node);

    static std::unique_ptr<ast::Variable>
    convertVariable(const std::string& src, TSNode node);

    static std::unique_ptr<ast::Attribute>
    convertQualifiedIdentifier(const std::string& src, TSNode node);

    // Value conversions for constants
    static Value convertValue(const std::string& src, TSNode node);
    static Value convertListLiteral(const std::string& src, TSNode node);
    static Value convertValueMap(const std::string& src, TSNode node);

    // Statement conversions
    static std::unique_ptr<ast::Assignment>
    convertAssignment(const std::string& src, TSNode node);

    // Utilities
    static std::string extractText(const std::string& src, TSNode node);
    static std::string extractQuotedString(const std::string& src, TSNode node);
};
