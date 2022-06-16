#include "json.h"

using namespace std;

namespace json {

    // --- Node ----------------------------------------------------
    // --- Load Node -----------------------------------------------
    namespace {

        Node LoadNode(istream& input);

        using Number = std::variant<int, double>;

        Node LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return Node(std::stoi(parsed_num));
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return Node(std::stod(parsed_num));
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        // Считывает содержимое строкового литерала JSON-документа
        // Функцию следует использовать после считывания открывающего символа ":
        Node LoadString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
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
                else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }

            return Node(s);
        }

        Node LoadArray(istream& input)
        {
            Array result;

            for (char c; input >> c;)
            {
                if (c == ']')
                {
                    return Node(move(result));
                }
                if (c != ',')
                {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            throw ParsingError("no end with load array"s);
        }

        Node LoadDict(istream& input)
        {
            Dict result;

            for (char c; input >> c;)
            {
                if (c == '}')
                {
                    return Node(move(result));
                }
                if (c == ',')
                {
                    input >> c;
                }

                string key = LoadString(input).AsString();
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }

            throw ParsingError("there's no closed paren's"s);
        }

        Node LoadNullptr()
        {
            return Node{ nullptr };
        }

        Node LoadBool(bool b)
        {
            return Node{ b };
        }//*/

        Node LoadNode(istream& input)
        {
            char c;
            input >> c;

            if (c == '[')
            {
                return LoadArray(input);
            }
            else if (c == '{')
            {
                return LoadDict(input);
            }
            else if (c == '"')
            {
                return LoadString(input);
            }
            else if (c == 'n')
            {
                string line = "n";
                for (char c; input >> c;) {
                    if (c == ',' || c == ']' || c == '}') {
                        input.putback(c);
                        break;
                    }
                    line += c;
                }
                if (line == "null"s)
                {
                    return LoadNullptr();
                }
                else
                {
                    throw ParsingError("something wrong with load null"s);
                }
            }
            else if (c == 't')
            {
                input >> c;
                if (c == 'r')
                {
                    input >> c;
                    if (c == 'u')
                    {
                        input >> c;
                        if (c == 'e')
                        {
                            return LoadBool(true);
                        }
                    }
                }
                throw ParsingError("something wrong with load bool"s);
            }//*/
            else if (c == 'f')
            {
                input >> c;
                if (c == 'a')
                {
                    input >> c;
                    if (c == 'l')
                    {
                        input >> c;
                        if (c == 's')
                        {
                            input >> c;
                            if (c == 'e')
                            {
                                return LoadBool(false);
                            }
                        }
                    }
                }
                throw ParsingError("something wrong with load bool"s);
            }//*/
            else
            {
                input.putback(c);
                return LoadNumber(input);
            }
        }

    }  // namespace

    // --- Node constructors ---------------------------------------
    /*Node::Node(Value value)
        : value_(value)
    {
    }//*/

    Node::Node(nullptr_t)
        : value_(nullptr)
    {
    }//*/

    Node::Node(bool b)
        : value_(b)
    {
    }//*/

    Node::Node(Array value)
        : value_(value)
    {
    }

    Node::Node(Dict value)
        : value_(value)
    {
    }

    Node::Node(int value)
        : value_(value)
    {
    }

    Node::Node(std::string value)
        : value_(value)
    {
    }

    Node::Node(double value)
        : value_(value)
    {
    }


    // --- As something functions of nodes -------------------------
    const Array& Node::AsArray() const
    {
        if (IsArray())
        {
            return get<Array>(value_);
        }
        else
        {
            throw logic_error("isn't array"s);
        }
    }

    const Dict& Node::AsMap() const
    {
        if (IsMap())
        {
            return get<Dict>(value_);
        }
        else
        {
            throw logic_error("isn't map"s);
        }
    }

    int Node::AsInt() const
    {
        if (IsInt())
        {
            return get<int>(value_);
        }
        else
        {
            throw logic_error("isn't int"s);
        }
    }

    bool Node::AsBool() const
    {
        if (IsBool())
        {
            return get<bool>(value_);
        }
        else
        {
            throw logic_error("isn't bool"s);
        }
    }

    const string& Node::AsString() const
    {
        if (IsString())
        {
            return get<string>(value_);
        }
        else
        {
            throw logic_error("isn't string"s);
        }
    }

    double Node::AsDouble() const
    {
        if (IsInt() || IsDouble())
        {
            if (const double* data = get_if<double>(&value_))
            {
                return *data;
            }
            else
            {
                return get<int>(value_);
            }
        }
        else
        {
            throw logic_error("isn't pure double"s);
        }
    }

    double Node::AsPureDouble() const
    {
        if (IsPureDouble())
        {
            return get<double>(value_);
        }
        else
        {
            throw logic_error("isn't double"s);
        }
    }

    nullptr_t Node::AsNullptr() const
    {
        if (IsNull())
        {
            return nullptr;
        }
        else
        {
            throw logic_error("isn't nullptr_t"s);
        }
    }

    // --- Is something functions of nodes -------------------------
    bool Node::IsNull() const
    {
        return holds_alternative<std::nullptr_t>(value_);
    }

    bool Node::IsBool() const
    {
        return holds_alternative<bool>(value_);
    }

    bool Node::IsArray() const
    {
        return holds_alternative<Array>(value_);
    }

    bool Node::IsMap() const
    {
        return holds_alternative<Dict>(value_);
    }

    bool Node::IsInt() const
    {
        return holds_alternative<int>(value_);
    }

    bool Node::IsDouble() const
    {
        return holds_alternative<double>(value_) || IsInt();
    }

    bool Node::IsPureDouble() const
    {
        return holds_alternative<double>(value_);
    }

    bool Node::IsString() const
    {
        return holds_alternative<string>(value_);
    }

    // --- Equal nodes ---------------------------------------------
    bool Node::operator==(const Node& rhs) const
    {
        return this->value_ == rhs.value_;
    }

    bool Node::operator!=(const Node& rhs) const
    {
        return !(*this == rhs);
    }

    // --- Document ------------------------------------------------
    Document::Document(Node root)
        : root_(move(root))
    {
    }

    const Node& Document::GetRoot() const
    {
        return root_;
    }

    bool Document::operator==(const Document& rhs) const
    {
        return this->GetRoot() == rhs.GetRoot();
    }

    bool Document::operator!=(const Document& rhs) const
    {
        return !(*this == rhs);
    }


    Document Load(istream& input)
    {
        return Document{ LoadNode(input) };
    }

    // --- Print ---------------------------------------------------
    std::ostream& operator<<(std::ostream& out, const Node& node) {
        if (node.IsNull()) {
            out << "null"s;
        }
        else if (node.IsInt()) {
            out << node.AsInt();
        }
        else if (node.IsString()) {
            out << "\""s;
            for (const auto c : node.AsString()) {
                switch (c) {
                case '"':
                    out << "\\\""s;
                    break;
                case '\n':
                    out << "\\n"s;
                    break;
                case '\r':
                    out << "\\r"s;
                    break;
                case '\t':
                    out << "\t"s;
                    break;
                case '\\':
                    out << "\\\\"s;
                    break;
                default:
                    out << c;
                    break;
                }
            }
            out << "\"";
        }
        else if (node.IsMap()) {
            out << node.AsMap();
        }
        else if (node.IsArray()) {
            out << node.AsArray();
        }
        else if (node.IsBool()) {
            if (node.AsBool()) {
                out << "true"s;
            }
            else {
                out << "false"s;
            }
        }
        else if (node.IsDouble()) {
            out << node.AsDouble();
        }
        else {
            out << node.AsPureDouble();
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const Dict& value)
    {
        out << '{' << endl;
        bool first_elem = true;
        for (const auto& [key, val] : value)
        {
            if (first_elem)
            {
                first_elem = false;
            }
            else
            {
                out << ',';
            }
            out << '"' << key << '"' << ':' << val;
        }
        out << '}';
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const Array& value)
    {
        out << '[';
        bool first_elem = true;
        for (const auto& elem : value)
        {
            if (first_elem)
            {
                first_elem = false;
            }
            else
            {
                out << ',';
            }
            out << elem;
        }
        out << ']';
        return out;
    }


    void Print(const Document& doc, std::ostream& output) {
        output << doc.GetRoot();

        // stub
        (void)&doc;
        (void)&output;
    }

}  // namespace json