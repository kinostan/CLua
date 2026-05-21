import * as CLuaTypes from "../clua/types";

let UID = -1;

function get_uid()
{
    return ++UID;
};

export const NumberPatternMessageId = get_uid();
export const StringPatternMessageId = get_uid();
export const CharPatternMessageId = get_uid();

export class ErrorNode {
    error_message_construction_pattern = "";
};