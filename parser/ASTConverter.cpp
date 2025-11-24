#include "ASTConverter.h"
#include "NodeTypes.h"
#include "ParserUtil.h"

std::string ASTConverter::extractText(const std::string& src, TSNode node) {
    return slice(src, node);
}

std::string ASTConverter::extractQuotedString(const std::string& src, TSNode node) {
    return parseQuotedString(src, node);
}

std::unique_ptr<ast::Expression>
ASTConverter::convertExpression(const std::string& src, TSNode node) {
    TSSymbol symbol = ts_node_symbol(node);

    // Unwrap expression wrapper if present
    if (symbol == NodeType::EXPRESSION) {
        if (ts_node_named_child_count(node) > 0) {
            return convertExpression(src, ts_node_named_child(node, 0));
        }
        throw std::runtime_error("Expression node has no children");
    }

    // Handle different expression types
    if (symbol == NodeType::INTEGER || symbol == NodeType::QUOTED_STRING) {
        return convertConstant(src, node);
    }

    if (symbol == NodeType::IDENTIFIER) {
        return convertVariable(src, node);
    }

    if (symbol == NodeType::QUALIFIED_IDENTIFIER) {
        return convertQualifiedIdentifier(src, node);
    }

    if (symbol == NodeType::LIST_LITERAL || symbol == NodeType::VALUE_MAP) {
        return convertConstant(src, node);
    }

    throw std::runtime_error("Unknown expression type: " + std::string(ts_node_type(node)));
}

// TODO: Implement conversion functions
std::unique_ptr<ast::Constant>
ASTConverter::convertConstant(const std::string& src, TSNode node) {
    throw std::runtime_error("convertConstant not implemented");
}

std::unique_ptr<ast::Variable>
ASTConverter::convertVariable(const std::string& src, TSNode node) {
    throw std::runtime_error("convertVariable not implemented");
}

std::unique_ptr<ast::Attribute>
ASTConverter::convertQualifiedIdentifier(const std::string& src, TSNode node) {
    throw std::runtime_error("convertQualifiedIdentifier not implemented");
}

Value ASTConverter::convertValue(const std::string& src, TSNode node) {
    throw std::runtime_error("convertValue not implemented");
}

Value ASTConverter::convertListLiteral(const std::string& src, TSNode node) {
    throw std::runtime_error("convertListLiteral not implemented");
}

Value ASTConverter::convertValueMap(const std::string& src, TSNode node) {
    throw std::runtime_error("convertValueMap not implemented");
}

std::unique_ptr<ast::ASTNode>
ASTConverter::convertStatement(const std::string& src, TSNode node) {
    throw std::runtime_error("convertStatement not implemented");
}

std::unique_ptr<ast::Assignment>
ASTConverter::convertAssignment(const std::string& src, TSNode node) {
    throw std::runtime_error("convertAssignment not implemented");
}
