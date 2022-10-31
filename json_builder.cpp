#include "json_builder.h"

#include <iostream>

namespace json {

    // --- Builder -----------------------------------------------------------
    Builder& Builder::Value(Node value)
    {
        AddNode(value);
        return *this;
    }

    KeyContext Builder::Key(std::string key)
    {
        KeyContext context(*this);
        if (nodes_stack_.size() && nodes_stack_.back()->IsDict())
        {
            Dict& cur = const_cast<Dict&>(nodes_stack_.back()->AsDict());
            cur.emplace(key, key);
            nodes_stack_.push_back(&cur.at(key));
        }
        else
        {
            std::cout << "something wrong with key" << std::endl;
            throw std::logic_error("something wrong with key");
        }
        return context;
    }

    ArrayContext Builder::StartArray()
    {
        ArrayContext context(*this);
        nodes_stack_.push_back(AddNode(Array{}));
        return context;
    }

    Builder& Builder::EndArray()
    {
        if (nodes_stack_.size() && nodes_stack_.back()->IsArray())
        {
            nodes_stack_.pop_back();
        }
        else
        {
            std::cout << "something wrong with array" << std::endl;
            throw std::logic_error("something wrong with array");
        }
        return *this;
    }//*/

    DictContext Builder::StartDict()
    {
        DictContext context(*this);
        nodes_stack_.push_back(AddNode(Dict{}));
        return context;
    }

    Builder& Builder::EndDict()
    {
        if (nodes_stack_.size() && nodes_stack_.back()->IsDict())
        {
            nodes_stack_.pop_back();
        }
        else
        {
            std::cout << "something wrong with dictionary" << std::endl;
            throw std::logic_error("something wrong with dictionary");
        }
        return *this;
    }//*/

    // --- Additional method --------------------------------------------------
    Node* Builder::AddNode(Node node)
    {
        // working with first node
        if (nodes_stack_.size() == 0 && root_.IsNull())
        {
            root_ = Node(node);
            return &root_;
        }
        // working with array
        else if (nodes_stack_.size() && nodes_stack_.back()->IsArray())
        {
            Array& cur = const_cast<Array&>(nodes_stack_.back()->AsArray());
            cur.push_back(node);
            return &cur.back();
        }
        // working with dictionary
        else if (nodes_stack_.size() && nodes_stack_.back()->IsString())
        {
            const std::string key = nodes_stack_.back()->AsString();
            nodes_stack_.pop_back();
            if (nodes_stack_.back()->IsDict())
            {
                Dict& cur = const_cast<Dict&>(nodes_stack_.back()->AsDict());
                cur[key] = node;
                return &cur.at(key);
            }
            else
            {
                std::cout << "something wrong with key and value" << std::endl;
                throw std::logic_error("something wrong with key and value");
            }
        }
        else
        {
            std::cout << "something wrong with add node" << std::endl;
            throw std::logic_error("something wrong with add node");
        }
    }

    // --- Builder::Build() method --------------------------------------------
    Node Builder::Build()
    {
        if (root_.IsNull() || nodes_stack_.size())
        {
            std::cout << "something wrong with building" << std::endl;
            throw std::logic_error("something wrong with building");
        }
        return root_;
    }

    // --- Context classes ----------------------------------------------------
    // ----- ValueValueContext methods ----------------------------------------
    ValueValueContext ValueValueContext::Value(Node value)
    {
        return builder_.Value(value);
    }

    ArrayContext ValueValueContext::StartArray()
    {
        return builder_.StartArray();
    }

    Builder& ValueValueContext::EndArray()
    {
        return builder_.EndArray();
    }

    DictContext ValueValueContext::StartDict()
    {
        return builder_.StartDict();
    }

    // ----- ValueKeyConext methods -------------------------------------------
    KeyContext ValueKeyContext::Key(std::string s)
    {
        return builder_.Key(s);
    }

    Builder& ValueKeyContext::EndDict()
    {
        return builder_.EndDict();
    }

    // ----- KeyConext methods ------------------------------------------------
    ValueKeyContext KeyContext::Value(Node value)
    {
        return builder_.Value(value);
    }

    ArrayContext KeyContext::StartArray()
    {
        return builder_.StartArray();
    }

    DictContext KeyContext::StartDict()
    {
        return builder_.StartDict();
    }

    // ----- ArrayContext methods ---------------------------------------------
    ValueValueContext ArrayContext::Value(Node value)
    {
        return builder_.Value(value);
    }

    ArrayContext ArrayContext::StartArray()
    {
        return builder_.StartArray();
    }

    Builder& ArrayContext::EndArray()
    {
        return builder_.EndArray();
    }

    DictContext ArrayContext::StartDict()
    {
        return builder_.StartDict();
    }

    // ----- DictContext methods ----------------------------------------------
    KeyContext DictContext::Key(std::string s)
    {
        return builder_.Key(s);
    }

    Builder& DictContext::EndDict()
    {
        return builder_.EndDict();
    }

} // namespace json