"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
class BaseNode {
}
;
class ASTNode {
}
;
let UID = -1;
function get_uid() {
    return ++UID;
}
;
let ast_nodes = new Array();
