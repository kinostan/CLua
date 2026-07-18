#pragma once

#include <common/base.hpp>

#include <common/language_processing/base.hpp>
#include <common/language_processing/source.hpp>

#include <common/language_processing/node_manager.hpp>
#include <common/language_processing/node_handle.hpp>
#include <common/language_processing/node_base.hpp>

#include <debugger/debugger.hpp>

namespace Common {
    using Source = Common::Source;

    struct ParserState {
        Common::uint64 peek_index;
        Common::uint64 memory_offset;
        Common::uint64 error_count;
    };

    struct LanguageError {
        NodeHandle error_node;
        Common::uint64 language_id;
        
        LanguageError(NodeHandle error_node, Common::uint64 language_id): error_node(error_node), language_id(language_id){};
    };

    class ParserContext {


        public:
        template <typename Node>
        requires (std::derived_from<Node,BaseNode>)
        NodeHandle reserve_node()
        {
            return node_manager.create_node<Node>();
        }

        template <typename Node>
        requires (std::derived_from<Node,BaseNode>)
        NodeHandle reserve_compiler_node()
        {
            return NodeHandle(NodeHandleTag::CompilerData,node_manager.create_node<Node>());
        }
    
        template <typename Node>
        requires (std::derived_from<Node,BaseNode>)
        Node& get_node_reference(NodeHandle node)
        {
            return node_manager.get_node_from_handle(node);
        };

        private:
        NodeHandle null_node;

        public:

        Source* source;
        NodeManager node_manager;
        std::vector<LanguageError> error_node_list;

        ParserContext():node_manager(0){};

        ParserContext(Source* source) : 
            source(source),
            node_manager(source->source_size / 2),
            error_node_list()
        {
           null_node = reserve_compiler_node<AST::NullNode>();
        }
        
        public:
        bool match_sequence(char* word,Common::uint64 offset = 0)
        {
            unsigned char* _word = reinterpret_cast<unsigned char*>(word);

            size_t i = offset;
            do {
                if (!source->can_peek(i))
                {
                    return false;
                };
                auto current_stream_char = source->peek(i);
                auto current_word_char = _word[i - offset];
                
                if (current_stream_char != current_word_char)
                {
                    return false;
                };

                i++;
            } while(_word[i - offset] != '\0');

            return true;
        };

        inline bool can_consume(Common::uint64 consume_size = 1)
        {
            return source->can_consume_sentinel(consume_size);
        };

        inline void consume(Common::uint64 size = 1)
        {
            source->consume(size);
        };

        inline auto peek(Common::uint64 offset = 0)
        {
            return source->peek(offset);
        }

        inline auto see_current()
        {
            return source->see_current();
        };

        inline void set_cursor(ParserState parser_state)
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

        inline ParserState record_cursor()
        {
            auto parser_state = ParserState();

            parser_state.peek_index = source->peeked_char_index;
            parser_state.error_count = error_node_list.size();
            parser_state.memory_offset = node_manager.get_linear_memory_offset();

            return parser_state;
        };
    
        NodeHandle record_error(NodeHandle error_node, Common::uint64 language_id)
        {
            error_node_list.push_back(
                LanguageError(error_node,language_id)
            );
            return NodeHandle(
                NodeHandleTag::Error,
                error_node_list.size() - 1
            );
        };  

        NodeHandle get_null_node()
        {
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
        virtual NodeHandle generate_AST(ParserContext& parser_context) = 0;
    };
}   
