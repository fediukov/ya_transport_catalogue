#pragma once

#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace json {

    class Node;
    // Сохраните объявления Dict и Array без изменения
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
        const Value& GetValue() const { return value_; }

        Node() = default;
        //Node(Value value);
        Node(Array value);
        Node(Dict map);
        Node(int value);
        Node(std::string value);
        Node(double value);
        Node(bool value);
        Node(std::nullptr_t);

        int AsInt() const;
        double AsDouble() const;
        double AsPureDouble() const;
        bool AsBool() const;
        const std::string& AsString() const;
        nullptr_t AsNullptr() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        bool IsInt() const;
        bool IsDouble() const; // Возвращает true, если в Node хранится int либо double.
        bool IsPureDouble() const; // Возвращает true, если в Node хранится double.
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

        bool operator==(const Node& rhs) const;
        bool operator!=(const Node& rhs) const;

    private:
        Value value_ = nullptr;
    };

    class Document {
    public:
        Document() = default;
        explicit Document(Node root);

        const Node& GetRoot() const;

        bool operator==(const Document& rhs) const;
        bool operator!=(const Document& rhs) const;

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    std::ostream& operator<<(std::ostream& out, const Node& node);
    std::ostream& operator<<(std::ostream& out, const Dict& value);
    std::ostream& operator<<(std::ostream& out, const Array& value);

    void Print(const Document& doc, std::ostream& output);

}  // namespace json