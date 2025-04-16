#include "json.h"

using namespace std;

namespace json
{

void FirstNotSpace(std::istream& input)
{
    for(char c; input >> c; )
    {
        if(c != ' ')
        {
            input.putback(c);
            break;
        }
    }
    return;
}

namespace
{
Node LoadNode(istream& input);

Node LoadArray(istream& input)
{
    Array result;

    for (char c; input >> c && c != ']';)
    {
        if (c != ',')
        {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
        FirstNotSpace(input);
    }
    if (!input)
    {
        throw ParsingError("Array parsing error"s);
    }
    return Node(move(result));
}

Node LoadNull(std::istream& input)
{
    std::string result;
    while(std::isalpha(input.peek()))
    {
        result += static_cast<char>(input.get());
    }
    if(!input)
    {
        throw ParsingError("Mistake in Parsing");
    }
    if(result == "null")
    {
        return Node{nullptr};
    }
    throw ParsingError("Mistake in Parsing");
}

Node LoadString(istream& input)
{
    using namespace std::literals;

    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true)
    {
        if (it == end)  // Поток закончился до того, как встретили закрывающую кавычку?
        {
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"')    // Встретили закрывающую кавычку
        {
            ++it;
            break;
        }
        else if (ch == '\\')  // Встретили начало escape-последовательности
        {
            ++it;
            if (it == end)   // Поток завершился сразу после символа обратной косой черты
            {
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char)
            {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    // Встретили неизвестную escape-последовательность
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
            }
        }
        else if (ch == '\n' || ch == '\r')   // Строковый литерал внутри- JSON не может прерываться символами \r или \n
        {
            throw ParsingError("Unexpected end of line"s);
        }
        else  // Просто считываем очередной символ и помещаем его в результирующую строку
        {
            s.push_back(ch);
        }
        ++it;
    }
    return Node{s};
}



Node LoadDict(istream& input)
{
    Dict result;
    std::string key;
    if (input.peek() == -1)
    {
        throw ParsingError("Dictionary parsing error"s);
     }
    for (char c; input >> c && c != '}';)
    {
        if(c == ',')
        {
            continue;
        }
        if(c == '"')
        {
            key = LoadString(input).AsString();
        }
        FirstNotSpace(input);
        input >> c;
        if (c != ':')
        {
            throw ParsingError("between key and value - mistake");
        }
        FirstNotSpace(input);
        result.insert({move(key), LoadNode(input)});
    }
    return Node(move(result));
}

Node LoadNumber(istream& input)
{
    std::string number;
    size_t point = 0;
    size_t exp = 0;

    for(char c; input >> c;)
    {
        if(!((std::isdigit(c)) || (c == 'e') || c == 'E' || c == '-' || c == '+' || c == '.'))
        {
            input.putback(c);
            break;
        }
        switch (c)
        {
            case '.':
            {
                point = number.size();
                break;
            }
            case 'E': case 'e':
            {
                exp = number.size();
                break;
            }
        }
        number += c;
    }
    if(point == 0 && exp == 0)
    {
        return Node{static_cast<int>(std::stoi(number))};
    }
    if(exp != 0)
    {
        if(std::isdigit(number[exp + 1])) // without sign
        {
            size_t coefficient = 1;

            for(int i = 0; i < static_cast<int>(number[exp + 1] - '0'); ++i)
            {
                coefficient *=10;
            }
            return Node{static_cast<double>(std::stod(number.substr(0, exp)) * coefficient)};
        }
        size_t sign = exp + 1;
        if(size_t count_pow = sign + 1; !std::isdigit(number[count_pow]))
        {
            throw ParsingError("");
        }
        size_t pow = sign + 1;
        size_t coefficient = 1;

        for(int i = 0; i < static_cast<int>(number[pow] - '0'); ++i)
        {
            coefficient *=10;
        }

        if(number[sign] == '-')
        {
            return Node{static_cast<double>(std::stod(number.substr(0, exp)) / coefficient)};
        }
        else if(number[sign] == '+')
        {
            return Node{static_cast<double>(std::stod(number.substr(0, exp)) * coefficient)};
        }
        else
        {
            throw ParsingError("");
        }
    }
    return Node{static_cast<double>(std::stod(number))};
}

Node LoadBool(std::istream& input)
{
    std::string result;
    while(std::isalpha(input.peek()))
    {
        result += static_cast<char>(input.get());
    }
    if(result == "true")
    {
        return Node{true};
    }
    else if(result == "false")
    {
        return Node{false};
    }
    throw ParsingError("Not valid (not true and no false)");
}

Node LoadNode(istream& input)
{
    FirstNotSpace(input);
    char c;
    input >> c;
    switch (c)
    {
        case '[':
        {
            return LoadArray(input);
        }
        case '{':
        {
            return LoadDict(input);
        }
        case '"':
        {
            return LoadString(input);
        }
        case 'f': case 't':
        {
            input.putback(c);
            return LoadBool(input);
        }
        case 'n':
        {
            input.putback(c);
            return LoadNull(input);
        }
         case '0' : case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '-':
        {
              input.putback(c);
              return LoadNumber(input);
        }
    default:
        throw ParsingError("Input Load 294");
    }
}

}  // namespace

Node::Node(Array array)
    : value_(move(array)) {}

Node::Node(Dict map)
    : value_(move(map)) {}

Node::Node(int value)
    : value_(value) {}

Node::Node(string value)
    : value_(move(value)) {}

Node::Node(double value)
    : value_(move(value)) {}

Node::Node(bool flag)
    : value_(move(flag)) {}

Node::Node(std::nullptr_t)
    : Node() {}


const Node::Value& Node::GetValue() const
{
    return value_;
}

const Array& Node::AsArray() const // Почему не работает при использование здесь тернарного оператора?
{
    if(IsArray())
    {
        return std::get<Array>(value_);
    }
     throw std::logic_error("");
}

const Dict& Node::AsMap() const
{
    if(IsMap())
    {
        return std::get<Dict>(value_);
    }
    throw std::logic_error("");
}

int32_t Node::AsInt() const
{
    return IsInt() ? std::get<int>(value_) : throw std::logic_error("");
}

const string& Node::AsString() const // Почему ОС выбрасывает исключение при использование здесь тернарного оператора?
{
    if(IsString())
    {
         return std::get<std::string>(value_);
    }
    throw std::logic_error("");
}

double Node::AsDouble() const
{
    if(IsPureDouble())
    {
        return std::get<double>(value_) * 1.0;
    }
    return IsDouble() ? static_cast<double>(std::get<int>(value_)) : throw std::logic_error("");
}

bool Node::AsBool() const
{
    return IsBool() ? std::get<bool>(value_) : throw std::logic_error("");
}

// Check type value

bool Node::IsInt() const
{
    return std::holds_alternative<int>(value_);
}

bool Node::IsArray() const
{
    return std::holds_alternative<Array>(value_);
}

bool Node::IsBool() const
{
    return std::holds_alternative<bool>(value_);
}

bool Node::IsMap() const
{
    return std::holds_alternative<Dict>(value_);
}

bool Node::IsNull() const
{
    return std::holds_alternative<std::nullptr_t>(value_);
}

bool Node::IsDouble() const
{
    return (std::holds_alternative<double>(value_) || std::holds_alternative<int>(value_));
}

bool Node::IsPureDouble() const
{
    return std::holds_alternative<double>(value_);
}

bool Node::IsString() const
{
    return std::holds_alternative<std::string>(value_);
}


Document::Document(Node root)
    : root_(move(root)) {}

const Node& Document::GetRoot() const
{
    return root_;
}

Document Load(istream& input)
{
    return Document{LoadNode(input)};
}

// Вывод
void PrintNode(const Node& node, const PrintContext& out);

struct JsonPrinter
{
    const PrintContext& format;

    void operator()(Dict dict) const
    {
        format.out.put('\n');
        bool first = true;
        auto out = format.Indented();
        out.PrintIndent();
        format.out << "{";
        for(const auto& [key, value] : dict)
        {
            if(first)
            {
                format.out << std::endl;
                first = false;
                out.PrintIndent();
                out.PrintIndent();
                format.out << "\"" << key << "\"" << ": ";
                PrintNode(value, format);
                continue;
            }

            format.out << "," << std::endl;
            out.PrintIndent();
            out.PrintIndent();
            format.out << "\"" << key << "\"" << ": ";
            PrintNode(value, format);
        }
        format.out.put('\n');
        out.PrintIndent();
        format.out << "}";
    }

    void operator()(double number) const
    {
        format.out << number;
    }

    void operator()(nullptr_t) const
    {
        format.out << "null"sv;
    }

    void operator()(int number) const
    {
        format.out << number;
    }

    void operator()(bool temp) const
    {
        format.out << std::boolalpha << temp;
    }

    void operator()(const std::string& text) const // Возможно не хватает \"
    {
       format.out << "\"";
        for (const char c : text)
        {
            switch (c)
            {
            case '"':
                format.out << "\\\""sv;
                break;
            case '\n':
                format.out << "\\n"sv;
                break;
            case '\t':
                format.out << "\\t"sv;   // Возможно здесь просто слеш
                break;
            case '\r':
                format.out << "\\r"sv;
                break;
            case '\\':
                format.out << "\\\\"sv;
                break;
            default:
                format.out << c;
                break;
            }
        }
        format.out << "\"";
    }

    void operator()(Array arr) const
    {
        format.out << "[\n";
        bool first = true;
        auto out = format.Indented();
        for(const auto& temp : arr)
        {
            if(first)
            {
               // format.out << std::endl;
                out.PrintIndent(); // Колхоз
                out.PrintIndent();
                out.PrintIndent();
                PrintNode(temp, format);
                first = false;
                continue;
            }
            format.out << "," << std::endl;
            out.PrintIndent(); // Здесь нужно проверить нужны ли они или нет (если будет два маршрута)
            out.PrintIndent();
            out.PrintIndent();
            PrintNode(temp, format);
        }
        format.out.put('\n');
        out.PrintIndent();
        out.PrintIndent();
        format.out << "]";
    }
};

void PrintNode(const Node& node, const PrintContext& out)
{
    std::visit(JsonPrinter{out}, node.GetValue());
}

void Print(const Document& doc, std::ostream& output)
{
    PrintNode(doc.GetRoot(), PrintContext{output});
}

}  // namespace json
