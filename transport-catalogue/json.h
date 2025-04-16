#pragma once

#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>

namespace json
{

class Node;
// Сохраните объявления Dict и Array без изменения
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};

class Node
{
public:
   /* Реализуйте Node, используя std::variant */
    using Value = std::variant<std::nullptr_t, Array, Dict, int32_t, std::string, double, bool>;
     Node(Array array);
     Node(Dict map);
     Node(int32_t value);
     Node(std::string value);
     Node(double value);
     Node(bool flag);
     Node(std::nullptr_t);
     Node() = default;

    const Value& GetValue() const;

    const Array& AsArray() const;
    const Dict& AsMap() const;
    int32_t AsInt() const;
    const std::string& AsString() const;
    double AsDouble() const;
    bool AsBool() const;

    bool IsInt() const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsBool() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const;

    bool operator==(const Node& lhs) const
    {
        return value_ == lhs.value_;
    }
    bool operator!=(const Node& lhs) const
    {
        return !(value_ == lhs.value_);
    }

private:
    Value value_;
};

class Document
{
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

    bool operator==(const Document& lhs) const
    {
        return root_ == lhs.root_;
    }

    bool operator!=(const Document& lhs) const
    {
        return !(root_ == lhs.root_);
    }

private:
    Node root_;
};

// Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
struct PrintContext
{
//    explicit PrintContext(std::ostream& out_put)
//        : out(out_put)
//    {}
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const
    {
        for (int i = 0; i < indent; ++i)
        {
            out.put(' ');
        }
    }
    // Возвращает новый контекст вывода с увеличенным смещением
    PrintContext Indented() const
    {
        return json::PrintContext{out, indent_step, indent_step + indent};
    }
};


Document Load(std::istream& input);

void FirstNotSpace(std::istream& input);
void Print(const Document& doc, std::ostream& output);
void PrintNode(const Node& node, std::ostream& out);
}  // namespace json
