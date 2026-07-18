import { Pattern } from "#common/pattern";
import { NodeDefinition } from "../ast/node";

export namespace IR {
    export class IRBase {
        children: Array<IRBase>;
        parent: IRBase | undefined;
        
        private remove_child(child: IRBase)
        {
            let child_index = this.children.findIndex((other_child: IRBase) => other_child == child);
            
            if (child_index === -1)
            {
                throw new Error(`Trying to remove child which doesn't exist in the parent`);
            };
            
            child.parent = undefined;

            let top = this.children.pop();

            if (top === undefined)
            {
                return;
            };

            this.children[child_index] = top;
        };

        remove()
        {
            if (this.parent === undefined)
            {
                return;
            };
            this.parent.remove_child(this);
        };

        insert_child(child: IRBase): this
        {
            this.children.push(child);
            return this;
        };  

        set_parent(parent: IRBase): this
        {
            this.parent = parent;
            parent.insert_child(this);
            return this;
        };

        constructor()
        {
            this.children = new Array<IRBase>();
            this.parent = undefined;
        };
    };

    export class IRDefinitionStart extends IRBase
    {
        language_name: string;
        constructor()
        {
            super();
            this.language_name = ""
        };

        /* 
        namespace [language_name] {
            ...
        }
        */
    };

    export class IdentifierToken {
        constructor(public debug_name?: string) {}
    }

    export class IRParseFunctionDefinition extends IRBase {
        constructor(public function_identifier: IdentifierToken)
        {
            super();
        };

        /*
            NodeHandle [function_identifier](ParserContext& context)
            {
            ...
            }; 
        */
    };

    /**
     * Records a cursor checkpoint for potential backtracking.
     * Maps to: auto [identifier_token] = context.record_cursor();
     */
    export class NewBacktrackState extends IRBase {
        constructor(public identifier_token: IdentifierToken) {
            super();
        }
        /* 
        auto [identifier_token] = context.record_cursor();
        */
    }

    /**
     * Restores the cursor position if a pattern fails.
     * Maps to: context.set_cursor([identifier_token]);
     */
    export class RestoreBacktrackState extends IRBase {
        constructor(public identifier_token: IdentifierToken) {
            super();
        }
        /* 
        context.set_cursor([identifier_token]);
        */
    }

    /**
     * Performs a direct sequence check on the source text.
     * Maps to: bool [result_token] = context.match_sequence("...");
     */
    export class MatchSequence extends IRBase {
        constructor(
            public sequence_literal: string, 
            public result_token: IdentifierToken
        ) {
            super();
        }
        /* 
            auto [result_token] = context.match_sequence([sequence_literal]);
        */
    }

    /**
     * Performs a lookahead check on a character range.
     * Maps to: auto [result_token] = context.see_current() == ' ';
     */
    export class IsCharRange extends IRBase {
        constructor(
            public char_range: Pattern.CharRange,
            public result_token: IdentifierToken,
            public char_token: IdentifierToken
        ) {
            super();
        }
        /* 
        //presuming that char_range is valid (no character ranges overlap)
        auto is_in_range = #for range,is_last_index in char_range.ranges 
        #emit range.min <= char_token && range.max >= char_token
        #emitif (!is_last_index), ||
        #end
        */
    }

    /**
     * Reserves memory in the compiler context pool for an AST Node.
     */
    export class NewReserveNode extends IRBase {
        constructor(
            public ast_node_type: NodeDefinition, 
            public handle_token: IdentifierToken, 
            public ref_token: IdentifierToken
        ) {
            super();
        }
        /* 
            auto [handle_token] = context.reserve_node<[get_full_name_path(ast_node_type)]>();
            auto& [ref_token] = context.get_node_reference<[get_full_name_path(ast_node_type)]>([handle_token]);
        */
    }

    /**
     * Finalizes structural data and properties for an allocated node.
     * Maps to: [handle_token].node_type = ...;
     */
    export class InitNode extends IRBase {
        constructor(
            public handle_token: IdentifierToken, 
            public node_type_enum: string, 
            public start_index_token: IdentifierToken
        ) {
            super();
        }
        /* 

        */
    }

    export class EmitError extends IRBase {
        /*
        auto [error_node_token] = context.reserve_node<AST::BaseErrorNode>();
        auto& [error_node_reference_token] = context.get_node_reference<AST::BaseErrorNode>(error_handle_token);
        [error_node_reference_token].node_type = static_cast<AST::NodeType>(AST::BaseTypes::Invalid);
        [error_node_reference_token].error_code = static_cast<AST::ErrorCode>([get_error_path(error_code_enum,LANG_ID]);
        [error_node_reference_token].error_span = Common::TokenSpan([start_index_token], context.source->peeked_char_index);
        [error_node_reference_token].ast_byproduct = #if (node_created_in_function) 
        #then #emit [get_previous_node_token_in_current_function()] 
        #else #emit context.get_null_node();
        #end
        */
    };

    /**
     * Signals a failure condition, instantiating an error node.
     */
    export class RecordError extends IRBase {
        constructor(
            public error_code_enum: number, 
            public start_index_token: IdentifierToken,
            public error_token: IdentifierToken
        ) {
            super();
        }

        /*
        auto [error_token] = context.record_error([error_node_token],[LANG_ID_TOKEN]);
        */
    }

    /**
     * Flags an error node as committed, disabling further upstream backtracking.
     */
    export class CommitError extends IRBase {
        constructor(public error_token: IdentifierToken) {
            super();
        }
        /* 
            [error_token].commit();
        */
    }

    /**
     * Explicit return statement from a parsing block or function.
     */
    export class Return extends IRBase {
        constructor() {
            super();
        }
        /*
        return [expression.to_string()];
        */
    }

    /**
     * Handles loop mechanics for quantifiers (+, *, {min, max}).
     */
    export class QuantityCheck extends IRBase {
        constructor(
            public min_count: number, 
            public max_count: number) {
            super();
        }

    }

    /**
     * Implements choice branch logic.
     */
    export class ChoiceBranch extends IRBase {
        constructor(public choice_pattern_names: string[]) {
            super();
        }
    }
};