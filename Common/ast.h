// AST.H
#ifndef AST_H
#define AST_H
#include <stdio.h>
#include <algorithm>
#include <memory>
#include <queue>
#include <string>
#include <variant>
#include <vector>
#ifndef TOKEN_H
#define TOKEN_H
#include "token.h"
#endif
class Node;
using NodeList = std::vector<std::shared_ptr<Node>>;
// using NodeValue = std::variant<std::string , int , float>;
using NodeValue = std::string;
class Node {
   public:
    int id;
    NodeValue type;
    NodeValue value;
    NodeList children;
    std::weak_ptr<Node> parent;

    explicit Node(NodeValue val) : value(std::move(val)) {}

    Node(NodeValue val, int i) : value(std::move(val)), id(i) {}

    Node(NodeValue tp, NodeValue val, int i)
        : type(std::move(tp)), value(std::move(val)), id(i) {}

    Node(NodeValue tp,
         NodeValue val,
         std::vector<std::shared_ptr<Node>> children)
        : type(std::move(tp)),
          value(std::move(val)),
          children(std::move(children)) {}
    Node(NodeValue val, std::vector<std::shared_ptr<Node>> children)
        : type(std::move(val)),
          value(std::move(val)),
          children(std::move(children)) {}

    Node(NodeValue tp,
         NodeValue val,
         std::vector<std::shared_ptr<Node>> children,
         std::weak_ptr<Node> parent)
        : type(std::move(tp)),
          value(std::move(val)),
          children(std::move(children)),
          parent(std::move(parent)) {}
};

class ATree {
   private:
    bool isToken(std::string s) {
        if (s == "TYPE" || s == "CmpOp" || s == "ExtDefRest" ||
            s == "VarListRest" || s == "VarList" || s == "FunDec" ||
            s == "Factor" || s == "Term" || s == "Exp" || s == "CompExp" ||
            s == "RelationExp" || s == "ConditionalExp" ||
            s == "ConditionalStmt" || s == "ActParamList" || s == "CallStmt" ||
            s == "LoopStmt" || s == "AssignmentStmt" || s == "ReturnStmt" ||
            s == "BreakStmt" || s == "LocalVariableDeclaration" ||
            s == "Stmt" || s == "StmtList" || s == "CompSt" || s == "ExtDef" ||
            s == "ExtDefList" || s == "Program") {
            return false;
        }
        return true;
    }

   public:
    int cnt;
    std::shared_ptr<Node> root;

    ATree()
        : root(std::make_shared<Node>(std::move("Program"),
                                      std::move("Program"),
                                      0)),
          cnt(1) {}

    std::shared_ptr<Node> addNode(std::shared_ptr<Node> current_node,
                                  NodeValue value) {
        // std::cout << "addNode: " << value << "\n";
        auto new_node = std::make_shared<Node>(std::move(value), cnt++);
        new_node->parent = current_node;
        current_node->children.push_back(new_node);
        return new_node;
    }

    void addLeafNode(std::shared_ptr<Node> current_node, NodeValue value) {
        // std::cout << "addLeafNode: " << value << "\n";
        auto new_node = std::make_shared<Node>(std::move(value), cnt++);
        new_node->parent = current_node;
        current_node->children.push_back(new_node);
    }
    void addLeafNode(std::shared_ptr<Node> current_node, TokenInfo token) {
        // std::cout << "addLeafNode: " << token.lexeme << "\n";
        auto new_node =
            std::make_shared<Node>(std::move(tok_transformed[token.token]),
                                   std::move(token.lexeme), cnt++);
        new_node->parent = current_node;
        current_node->children.push_back(new_node);
    }

    std::shared_ptr<Node> backTrace(const std::shared_ptr<Node>& cur) {
        // std::cout << "backTrace: " << cur->value << "\n";
        if (cur->parent.lock() != nullptr) {
            return cur->parent.lock();
        } else if (cur->value == "Program") {
            return cur;
        } else {
            throw "empty parent Node";
        }
    }

    std::shared_ptr<Node> deleteNode(std::shared_ptr<Node>& delete_node) {
        // std::cout << "delete: " << delete_node->value << "\n";
        auto parent_node = delete_node->parent.lock();
        if (parent_node) {
            auto& siblings = parent_node->children;
            siblings.erase(
                std::remove(siblings.begin(), siblings.end(), delete_node),
                siblings.end());
        } else {
            throw "empty parent Node";
        }
        cnt--;
        return parent_node;
    }
    void NodeDictGenerate(std::ofstream& out) {
        if (!root)
            return;  // 如果根节点为空，则直接返回

        std::queue<std::shared_ptr<Node>> queue;  // 创建一个队列存储节点
        queue.push(root);                         // 将根节点加入队列

        while (!queue.empty()) {           // 当队列不为空时持续遍历
            int levelSize = queue.size();  // 当前层的节点数
            for (int i = 0; i < levelSize; ++i) {
                auto current_node = queue.front();  // 获取当前节点
                queue.pop();  // 将当前节点从队列中移除

                // 此处处理当前节点，例如打印其值
                out << current_node->id << ":" << current_node->value << "\n";

                // 将当前节点的子节点加入队列
                for (auto& child : current_node->children) {
                    if (child) {
                        queue.push(child);
                        // out << child->id << " ";
                    }
                }
                // out << "\n";
            }
        }
    }
    void levelOrderTraversal(std::ofstream& out) {
        if (!root)
            return;  // 如果根节点为空，则直接返回

        std::queue<std::shared_ptr<Node>> queue;  // 创建一个队列存储节点
        queue.push(root);                         // 将根节点加入队列

        while (!queue.empty()) {           // 当队列不为空时持续遍历
            int levelSize = queue.size();  // 当前层的节点数
            for (int i = 0; i < levelSize; ++i) {
                auto current_node = queue.front();  // 获取当前节点
                queue.pop();  // 将当前节点从队列中移除

                // 此处处理当前节点，例如打印其值
                if (current_node->children.size() == 0) {
                    if (isToken(current_node->value)) {
                        out << current_node->id;
                    } else {
                        current_node->value = " ";
                        out << current_node->id;
                    }
                    // continue;
                } else
                    out << current_node->id << "——";
                // 将当前节点的子节点加入队列
                for (auto& child : current_node->children) {
                    if (child) {
                        queue.push(child);
                        out << child->id << " ";
                    }
                }
                out << "\n";
            }
        }
    }
};

#endif  // AST_H