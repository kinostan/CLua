#include <common/language_processing/base.hpp>
#include <common/language_processing/node_handle.hpp>

#include "parser.hpp"

using namespace AST;

using ParserContext = Common::ParserContext;


NodeHandle parse_local_declaration_tail(Common::ParserContext& ctx, NodeHandle kw_handle);
NodeHandle parse_function_call_tail(Common::ParserContext& ctx, NodeHandle name_handle);

NodeHandle parse_statement_optimized(Common::ParserContext& ctx) {
    auto backtrack_state = ctx.record_cursor();
    uint64_t start_idx = ctx.source->peeked_char_index;

    // 1. Wspólna faza spekulatywna (czytamy ID raz)
    if (!is_alpha_or_underscore(ctx.see_current())) return NoPatternNode;
    
    ctx.consume(1);
    while (ctx.can_consume(1) && is_alnum_or_underscore(ctx.see_current())) ctx.consume(1);
    
    uint64_t end_idx = ctx.source->peeked_char_index;
    uint64_t length = end_idx - start_idx;

    // Tworzymy bazowy TokenSpan (rezerwacja w arenie)
    NodeHandle first_id_handle = ctx.reserve_node<IdentifierNode>();
    IdentifierNode& first_id = ctx.get_node_reference<IdentifierNode>(first_id_handle);
    first_id.node_type = static_cast<NodeType>(NodeID::AST_IDENTIFIER);
    first_id.start = start_idx;
    first_id.end = end_idx;

    // Opcjonalne spacje
    uint64_t spaces_after = 0;
    while (ctx.can_consume(1) && ctx.see_current() == ' ') { ctx.consume(1); spaces_after++; }

    char current_char = ctx.see_current();

    // ==========================================================
    // LEKKA DRABINKA ROZSTRZYGANIA (Brak Code Bloat)
    // ==========================================================
    
    if (length == 5 && ctx.equals_string(first_id, "local") && spaces_after > 0 && current_char != '(') {
        // Zamiast inliniować całe parsowanie deklaracji, skaczemy do delegata
        goto label_call_local;
    }

    if (current_char == '(') {
        ctx.consume(1); // konsumujemy '('
        goto label_call_func;
    }

    // Jeśli to był po prostu wolny identyfikator (np. nazwa zmiennej w przypisaniu)
    return first_id_handle;

    // ==========================================================
    // ETYKIETY DELEGACJI
    // ==========================================================

label_call_local:
    // Wywołujemy kompletną, wydzieloną funkcję. Przekazujemy stan.
    // L1i cache uwielbia to, bo ta funkcja jest zwarta!
    return parse_local_declaration_tail(ctx, first_id_handle);

label_call_func:
    // Reużywalna funkcja obsługująca parsowanie argumentów i zamykanie nawiasów
    return parse_function_call_tail(ctx, first_id_handle);
}

NodeHandle CLua::Parser::generate_AST(ParserContext& context)
{
      
}