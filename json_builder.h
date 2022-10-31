#pragma once

#include <deque>
#include <string>
#include <vector>

#include "json.h"

namespace json {

	class ValueValueContext;
	class ValueKeyContext;
	class KeyContext;
	class ArrayContext;
	class DictContext;

	// --- Builder ------------------------------------------------------------
	class Builder {
	public:
		Builder() = default;
		~Builder() = default;
		Builder(Node node)
			: root_(node)
		{
		}//*/

		Builder& Value(Node node);
		KeyContext Key(std::string);
		ArrayContext StartArray();
		Builder& EndArray();
		DictContext StartDict();
		Builder& EndDict();
		Node Build();

	private:
		Node* AddNode(Node node);

	private:
		Node root_;
		std::deque<Node*> nodes_stack_;
	};

	// --- Context classes ----------------------------------------------------
	class ValueValueContext {
	public:
		ValueValueContext(Builder& builder)
			: builder_(builder) {}

		ValueValueContext Value(Node value);
		ArrayContext StartArray();
		Builder& EndArray();
		DictContext StartDict();
	private:
		Builder& builder_;
	};

	class ValueKeyContext {
	public:
		ValueKeyContext(Builder& builder)
			: builder_(builder) {}

		KeyContext Key(std::string s);
		Builder& EndDict();
	private:
		Builder& builder_;
	};

	class KeyContext {
	public:
		KeyContext(Builder& builder)
			: builder_(builder) {}

		ValueKeyContext Value(Node value);
		ArrayContext StartArray();
		DictContext StartDict();
	private:
		Builder& builder_;
	};

	class ArrayContext {
	public:
		ArrayContext(Builder& builder)
			: builder_(builder) {}

		ValueValueContext Value(Node value);
		ArrayContext StartArray();
		Builder& EndArray();
		DictContext StartDict();
	private:
		Builder& builder_;
	};

	class DictContext {
	public:
		DictContext(Builder& builder)
			: builder_(builder) {}

		KeyContext Key(std::string s);
		Builder& EndDict();
	private:
		Builder& builder_;
	};

} // namespace json