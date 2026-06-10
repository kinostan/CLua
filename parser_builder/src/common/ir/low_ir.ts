import { CppEmitContext } from "#common/emitter";
import { NumericTokenType, Pattern } from "#common/pattern";

export interface BuildContext {
    get_variable_name_from_reference_id(id: number): string;
    get_new_variable_id(type: string): number;
    get_ir_nodes_with_variable_id(id: number): Array<IRBlock>;
}

export abstract class IRBlock {
    public is_dead: boolean = false;
    public readonly real_constructor: Function = this.constructor;

    private child_elements: Array<IRBlock>;
    parent: IRBlock | null = null;

    constructor(constructor: Function) {
        this.real_constructor = constructor;
        this.child_elements = new Array<IRBlock>()
    }

    abstract process_step(build_context: BuildContext): boolean;
    
    append_child(ir_child: IRBlock)
    {
        this.child_elements.push(ir_child);
        ir_child.parent = this;
    };

    remove_child(ir_child: IRBlock): void
    {
        const index = this.child_elements.indexOf(ir_child);
        
        if (index !== -1) {
            this.child_elements.splice(index, 1);
            ir_child.parent = null;
        }
    };

    remove(): void
    {
        if (this.parent == null)
        {
            return;
        };
        this.parent.remove_child(this);
    };
    
    get_children(): Array<IRBlock>
    {
        return this.child_elements.slice();
    };
}

export class IRRoot extends IRBlock{
    parser_name: string = "DefaultParserName";

    constructor(parser_name: string)
    {
        super(IRRoot);
        this.parser_name = parser_name;
    };

    process_step(build_context: BuildContext): boolean {
        return false;
    }  
};

export enum ParserDataType {
    Token,  
    Integer,
    Double,
    NodeHandle,
    Keyword,
    Symbol
};

export enum TokenType {
    Unspecified,
    String,
    Char,
    Keyword,
    Symbol,
    Identifer,
    Numeric
};

export namespace LowIR {
    export type LIRBlockType = Declaration.LIRDeclaration | Expression.LIRExpression | Statement.LIRStatement;


    export namespace Declaration {
        export type LIRDeclaration = IRNodeDeclare | IRTokenDeclare | IRExpectPatternFunctionDeclaration;
       
        abstract class VariableDeclaration<AssignmentType extends Statement.Assignment<any>> extends IRBlock {
            // Holds the actual assignment statement node as a child/property!
            public readonly assignment: AssignmentType;

            constructor(foreign_constructor: Function, assignment: AssignmentType) {
                super(foreign_constructor);
                this.assignment = assignment;
                
                // Track it structurally in the IR tree
                this.append_child(assignment);
            }

            process_step(build_context: BuildContext): boolean { return false; }
        }

        export class IRTokenDeclare extends VariableDeclaration<Statement.IRTokenAssign> {
            constructor(assignment: Statement.IRTokenAssign) {
                super(IRTokenDeclare, assignment);
            }
        }

        export class IRKeywordDeclare extends VariableDeclaration<Statement.IRKeywordAssign> {
            constructor(assignment: Statement.IRKeywordAssign) {
                super(IRKeywordDeclare, assignment);
            }
        }

        export class IRSymbolDeclare extends VariableDeclaration<Statement.IRSymbolAssign> {
            constructor(assignment: Statement.IRSymbolAssign) {
                super(IRSymbolDeclare, assignment);
            }
        }

        export class IRNodeDeclare extends VariableDeclaration<Statement.IRNodeAssign> {
            constructor(assignment: Statement.IRNodeAssign) {
                super(IRNodeDeclare, assignment);
            }
        }

        export class IRExpectPatternFunctionDeclaration extends IRBlock
        {
            linked_pattern: Pattern;

            constructor(pattern: Pattern)
            {
                super(IRExpectPatternFunctionDeclaration);
                this.linked_pattern = pattern;
            }
    
            process_step(build_context: BuildContext): boolean {
                return false;
            };
        };

        export class IRExpectPatternFunctionDefinition extends IRBlock 
        {
            linked_pattern: Pattern;

            constructor(pattern: Pattern)
            {
                super(IRExpectPatternFunctionDefinition);
                this.linked_pattern = pattern;
            };

            process_step(build_context: BuildContext): boolean {
                return false;
            }
        };
    };

    export namespace Expression {
        export type LIRExpression = FreeExpression; //| BoundedExpression;

        export type FreeExpression = 
        IRPatternCall
        | IRSeeCurrentNumber 
        | IRSeeCurrentSymbol 
        | IRSeeCurrentKeyword 
        | IRSeeCurrentToken; 
        //export type BoundedExpression;

        export class IRSeeCurrentNumber extends IRBlock {
            see_type: NumericTokenType = NumericTokenType.integer;

            constructor(see_type: NumericTokenType)
            {
                super(IRSeeCurrentNumber);
                this.see_type = see_type;
            };

            process_step(build_context: BuildContext): boolean {
                return false;
            }
        };

        export class IRSeeCurrentKeyword extends IRBlock {
             constructor()
            {
                super(IRSeeCurrentKeyword);
            };

            process_step(build_context: BuildContext): boolean {
                return false;
            }           
        };

        export class IRSeeCurrentToken extends IRBlock {
            constructor()
            {
                super(IRSeeCurrentToken);
            };

            process_step(build_context: BuildContext): boolean {
                return false;
            }
        };

        export class IRSeeCurrentSymbol extends IRBlock {
            constructor()
            {
                super(IRSeeCurrentSymbol);
            };

            process_step(build_context: BuildContext): boolean {
                return false;
            }
        };

        export class IRPatternCall extends IRBlock {
            assigned_variable_id: number = -1;
            called_function: Declaration.IRExpectPatternFunctionDeclaration

            constructor(expect_function: Declaration.IRExpectPatternFunctionDeclaration)
            {
                super(IRPatternCall);
                this.called_function = expect_function;
            };

            process_step(build_context: BuildContext): boolean {
                return false;
            };
        };
    };

    export namespace Statement {

        export type LIRStatement = 
        | Statement.IRNodeAssign 
        | Statement.IRTokenAssign
        | Statement.IRKeywordAssign
        | Statement.IRSymbolAssign
        | Statement.IRTokenChoice 
        | Statement.IRKeywordChoice 
        | Statement.IRSymbolChoice

        class Multiple<Type> {
            expressions: Array<Type> = new Array<Type>();

            insert_expression(expression: Type)
            {
                this.expressions.push(expression);
            };
        };

        class IRCase extends IRBlock {
            matching_expression: ParserDataType | Multiple<ParserDataType> 

            constructor(matching_expression: ParserDataType | Multiple<ParserDataType>)
            {
                super(IRCase);
                this.matching_expression = matching_expression;
            };

            process_step(build_context: BuildContext): boolean {
                return false;
            };
        };

        export abstract class Assignment<ExpressionType extends Expression.FreeExpression> extends IRBlock {
            public expression: ExpressionType | null = null;
            
            public var_id: number = -1;

            constructor(foreign_constructor: Function, var_id: number, expression?: ExpressionType) {
                super(foreign_constructor);
                this.var_id = var_id;
                if (expression) {
                    this.expression = expression;
                }
            }

            process_step(build_context: BuildContext): boolean {
                return false;
            }
        }

        abstract class SwitchStatement extends IRBlock {
            constructor(constructor: Function)
            {
                super(constructor);
            };
        };

        export class IRTokenChoice extends SwitchStatement{
            constructor()
            {
                super(IRTokenChoice);
            }

            process_step(build_context: BuildContext): boolean {
                return false;
            }
        };

        export class IRKeywordChoice extends SwitchStatement{
            constructor()
            {
                super(IRKeywordChoice);
            }

            process_step(build_context: BuildContext): boolean {
                return false;
            }
        };

        export class IRSymbolChoice extends SwitchStatement {
            constructor()
            {
                super(IRSymbolChoice);
            }

            process_step(build_context: BuildContext): boolean {
                return false;
            }
        };

        export class IRNodeAssign extends Assignment<Expression.IRPatternCall> {
            constructor(var_id: number, expression?: Expression.IRPatternCall) {
                super(IRNodeAssign, var_id, expression);
            }
        }

        export class IRTokenAssign extends Assignment<Expression.IRSeeCurrentToken> {
            constructor(var_id: number, expression?: Expression.IRSeeCurrentToken) {
                super(IRTokenAssign, var_id, expression);
            }
        }

        export class IRKeywordAssign extends Assignment<Expression.IRSeeCurrentKeyword> {
            constructor(var_id: number, expression?: Expression.IRSeeCurrentKeyword) {
                super(IRKeywordAssign, var_id, expression);
            }
        }

        export class IRSymbolAssign extends Assignment<Expression.IRSeeCurrentSymbol> {
            constructor(var_id: number, expression?: Expression.IRSeeCurrentSymbol) {
                super(IRSymbolAssign, var_id, expression);
            }
        }

        export class IRParserStateRecord extends IRBlock {
            public readonly state_var_id: number;
            constructor(state_var_id: number) {
                super(IRParserStateRecord);
                this.state_var_id = state_var_id;
            }
            process_step(build_context: BuildContext): boolean { return false; }
        }

        export class IRParserStateSet extends IRBlock {
            public readonly linked_ir_parser_record: IRParserStateRecord;
            constructor(parser_record: IRParserStateRecord) {
                super(IRParserStateSet);
                this.linked_ir_parser_record = parser_record;
            }
            process_step(build_context: BuildContext): boolean { return false; }
        }
    };
}
