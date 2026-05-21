class ASTNode {
    
};

let UID = -1;

function get_uid()
{
    return ++UID;
};

let ast_nodes: Array<ASTNode> = new Array<ASTNode>();
