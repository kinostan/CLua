export abstract class IRBlock {
    parent_block?: IRBlock;
    readonly children: IRBlock[] = [];
    readonly local_symbols: Map<string, string> = new Map();

    add_child(child: IRBlock): void {
        child.parent_block = this;
        this.children.push(child);
    }

    // 1. Cellular Automaton Logic: Only manipulates structural states & graph links.
    // Returns true if a mutation occurred, feeding your fixed-point iteration loop.
    abstract process_step(): boolean;

    // 2. Pure Code Emission: Read-only traversal that fires after the graph stabilizes.
    abstract emit(context: CppEmitContext): void;

    // Helper to easily cascade emission down through structural children
    protected emit_children(context: CppEmitContext): void {
        for (const child of this.children) {
            child.emit(context);
        }
    }
}

export class NamespaceIRBlock extends IRBlock {
    constructor(public name: string) { super(); }

    process_step(): boolean {
        let changed = false;
        // Cellular Rule: e.g., if namespace is empty, mark for pruning, 
        // or merge with adjacent identical namespaces.
        return changed;
    }

    emit(context: CppEmitContext): void {
        // Pure layout orchestration
        context.emit_namespace(this.name, () => {
            this.emit_children(context);
        });
    }
}

export class IfIRBlock extends IRBlock {
    public branch_hint?: 'likely' | 'unlikely';

    constructor(public condition: string, hint?: 'likely' | 'unlikely') { 
        super(); 
        this.branch_hint = hint;
    }

    process_step(): boolean {
        let changed = false;
        
        // Cellular Rule: Look up the tree. If our condition checks a token type 
        // that a parent block already guaranteed was true, this whole block is dead!
        if (this.branch_hint !== 'unlikely' && this.condition.includes('Error')) {
            this.branch_hint = 'unlikely'; // Elevate high-level intent
            changed = true;
        }
        
        return changed;
    }

    emit(context: CppEmitContext): void {
        // Pure emission using the optimized hint found during the cellular steps
        context.emit_if(this.condition, () => {
            this.emit_children(context);
        }, undefined, this.branch_hint);
    }
}