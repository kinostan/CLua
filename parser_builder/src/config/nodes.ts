import * as Types from "#root/common/parser-types";

class BaseNode {
    
};

class ASTNode {
    
};

let UID = -1;

function get_uid()
{
    return ++UID;
};

let ast_nodes: Array<ASTNode> = new Array<ASTNode>();
