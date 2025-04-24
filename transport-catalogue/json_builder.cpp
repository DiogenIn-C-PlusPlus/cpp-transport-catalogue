#include "json_builder.h"
#include <exception>
#include <variant>
#include <utility>

using namespace std::literals;

namespace json 
{

Builder::Builder()
    : root_()
    , nodes_stack_{}
{}

Node Builder::Build() 
{
    if (!nodes_stack_.empty()) 
    {
        throw std::logic_error("Can't build - stack empty"s);
    }
    return std::move(root_);
}

Builder::DictValueContext Builder::Key(std::string key) 
{
    if (!GetLastNode()->IsMap()) 
    {
        throw std::logic_error("Key() outside a dict"s);
    }
    Dict& temp = const_cast<Dict&>(nodes_stack_.back()->AsMap());
    temp[std::move(key)];
    return BaseContext{*this};
}

Builder::BaseContext Builder::Value(Node::Value value) 
{
    AddObject(std::move(value), true);
    return *this;
}

Builder::DictItemContext Builder::StartDict() 
{
    AddObject(Dict{},  false);
    return BaseContext{*this};
}

Builder::ArrayItemContext Builder::StartArray() 
{
    AddObject(Array{}, false);
    return BaseContext{*this};
}

Builder::BaseContext Builder::EndDict() 
{
    if (!GetLastNode()->IsMap()) 
    {
        throw std::logic_error("EndDict() outside a dict"s);
    }
    nodes_stack_.pop_back();
    return *this;
}

Builder::BaseContext Builder::EndArray() 
{
    if (!GetLastNode()->IsArray()) 
    {
        throw std::logic_error("EndDict() outside an array"s);
    }
    nodes_stack_.pop_back();
    return *this;
}

Node::Value& Builder::GetCurrentValue() 
{
    if (nodes_stack_.empty()) 
    {
        throw std::logic_error("Attempt to change finalized JSON"s);
    }
    return nodes_stack_.back()->GetValue();
}

void Builder::AddObject(Node::Value value, bool one_shot) 
{
    Node::Value& host_value = GetCurrentValue();
    if (std::holds_alternative<Array>(host_value)) 
    {
        Node& node = std::get<Array>(host_value).emplace_back(std::move(value));
        if (!one_shot) 
        {
            nodes_stack_.push_back(&node);
        }
    } 
    else 
    {
        if (!std::holds_alternative<std::nullptr_t>(GetCurrentValue())) 
        {
                throw std::logic_error("New object in wrong context"s);
         }
        host_value = std::move(value);
        if (one_shot) 
        {
            nodes_stack_.pop_back();
        }
    }
}

json::Node* json::Builder::GetLastNode() const
{
    if(!nodes_stack_.empty())
    {
        return nodes_stack_.back();
    }
    return nullptr;
}
}  // namespace json
