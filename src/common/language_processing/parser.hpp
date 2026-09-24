#pragma once

#include <common/base.hpp>

#include <common/language_processing/source.hpp>

#include <common/language_processing/node_manager.hpp>
#include <common/language_processing/node_handle.hpp>
#include <common/language_processing/node_base.hpp>

#include <debugger/debugger.hpp>
#include <concepts>

namespace Common {
    using Source = Common::Source;

    struct ParserState {
        uint64 peek_index;
        uint64 memory_offset;
        uint64 error_count;
    };

    struct LanguageError {
        AST::NodeHandle error_node;
        uint64 language_id;

        LanguageError() = default;
        LanguageError(AST::NodeHandle error_node, uint64 language_id): error_node(error_node), language_id(language_id){};
    };

    class ParserContext {


        public:
        template <typename Node>
        requires (std::derived_from<Node, AST::BaseNode>)
        AST::NodeHandle reserve_node()
        {
            return node_manager.create_node<Node>();
        }

        template <typename Node>
        requires (std::derived_from<Node, AST::BaseNode>)
        AST::NodeHandle reserve_compiler_node()
        {
            return AST::NodeHandle(AST::NodeHandleTag::CompilerData,node_manager.create_node<Node>());
        }
    
        template <typename Node>
        requires (std::derived_from<Node, AST::BaseNode>)
        Node& get_node_reference(AST::NodeHandle node)
        {
            return node_manager.get_node_from_handle<Node>(node);
        };

        private:
        AST::NodeHandle null_node;

        public:

        Source* source;
        AST::NodeManager node_manager;
        std::vector<LanguageError> error_node_list;

        ParserContext():source(nullptr), node_manager(0){};

        ParserContext(Source* source) : 
            source(source),
            node_manager(source->source_size / 2),
            error_node_list()
        {
           null_node = reserve_compiler_node<AST::NullNode>();
        }
        
        public:
        bool match_sequence(char* word, const uint64 offset = 0) const
        {
            auto* _word = reinterpret_cast<unsigned char*>(word);

            size_t i = offset;
            do {
                if (!source->can_peek(i))
                {
                    return false;
                };
                const auto current_stream_char = source->peek(i);

                if (const auto current_word_char = _word[i - offset]; current_stream_char != current_word_char)
                {
                    return false;
                };

                i++;
            } while(_word[i - offset] != '\0');

            return true;
        };

        [[nodiscard]] bool can_consume(const uint64 consume_size = 1) const
        {
            return source->can_consume_sentinel(consume_size);
        };

        void consume(uint64 size = 1) const {
            source->consume(size);
        };

        [[nodiscard]] auto peek(const uint64 offset = 0) const
        {
            return source->peek(offset);
        }

        [[nodiscard]] auto see_current() const
        {
            return source->see_current();
        };

        void set_cursor(const ParserState &parser_state)
        {
            Assert(
                parser_state.memory_offset >= null_node.node_value + sizeof(AST::NullNode),
                "can't set cursor in null node reserved space"
            );

            source->peeked_char_index = parser_state.peek_index;
            error_node_list.resize(parser_state.error_count);
            node_manager.set_linear_memory_offset(parser_state.memory_offset);

            Assert(
                source->can_peek_sentinel(),
                "peek_index exceeds legal value"
            );
        };

        [[nodiscard]] ParserState record_cursor() const
        {
            auto parser_state = ParserState();

            parser_state.peek_index = source->peeked_char_index;
            parser_state.error_count = error_node_list.size();
            parser_state.memory_offset = node_manager.get_linear_memory_offset();

            return parser_state;
        };

        AST::NodeHandle record_error(AST::NodeHandle error_node, uint64 language_id)
        {
            error_node_list.emplace_back(
                error_node,language_id
            );
            return {
                AST::NodeHandleTag::Error,
                error_node_list.size() - 1
            };
        };

        [[nodiscard]] AST::NodeHandle get_null_node() const {
            return null_node;
        };
    };

    class Parser
    {
        private:
        ParserContext context;

        public:
        Parser(Source* source)
        {
            context = ParserContext(source);
        };

        public:
        ParserContext& get_context()
        {
            return context;
        };
    };

    class IParser 
    {
    public:
        virtual ~IParser() = default;

    private:
        virtual AST::NodeHandle generate_AST(ParserContext& parser_context) = 0;
    };
}   
