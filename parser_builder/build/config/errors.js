"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.ErrorNode = exports.CharPatternMessageId = exports.StringPatternMessageId = exports.NumberPatternMessageId = void 0;
let UID = -1;
function get_uid() {
    return ++UID;
}
;
exports.NumberPatternMessageId = get_uid();
exports.StringPatternMessageId = get_uid();
exports.CharPatternMessageId = get_uid();
class ErrorNode {
    constructor() {
        this.error_message_construction_pattern = "";
    }
}
exports.ErrorNode = ErrorNode;
;
