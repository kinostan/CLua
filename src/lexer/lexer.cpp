#include <lexer/lexer.hpp>

#include <string>
#include <array>

namespace Util { 

   using namespace std::string_literals;
   unsigned char get_binary_char_code_value(unsigned char bin_char)
   {
      if (bin_char >= '0' && bin_char <= '1')
      {
         return bin_char - '0';
      };
      LAssert(false,"bin_char is not in binary code"s
      )
      return 0;
   };

   unsigned char get_hexadecimal_char_code_value(unsigned char hex_char)
   {
      if (hex_char >= '0' && hex_char <= '9')
      {
         return hex_char - '0';
      } else if(hex_char >= 'a' && hex_char <= 'f')
      {
         return hex_char - 'a' + (unsigned char)10;
      } else if(hex_char >= 'A' && hex_char <= 'F')
      {
         return hex_char - 'A' + (unsigned char)10;
      } else {
         LAssert(false,
            
            std::to_string(hex_char) + //retarded
            " char code is not a hex code char"s 
            
         )
         return 0;
      };
      return 0;
   };

   static const auto character_map = [](){
      using namespace TypeClassificator;
      std::array<CharacterType,256> character_map;

      for (int character_index = 0;character_index <= 255;character_index++)
      {
         unsigned char current_character = static_cast<unsigned char>(character_index);
         if(!is_valid_char(current_character))
         {
            character_map[current_character] = CharacterType::Error;
            continue;
         }
         else if (is_numeric_char(current_character))
         {
            character_map[current_character] = CharacterType::Numeric;
            continue;
         }
         else if (is_letter_char(current_character))
         {
            character_map[current_character] = CharacterType::Letter;      
            continue;
         }
         else if (is_whitespace_char(current_character))
         {
            character_map[current_character] = CharacterType::Whitespace;
            continue;
         }
         else if(is_newline_char(current_character))
         {
            character_map[current_character] = CharacterType::NewLine;
            continue;
         }  
         else if (is_unicode(current_character))
         {
            character_map[current_character] = CharacterType::Unicode;
            continue;
         };
         character_map[current_character] = CharacterType::Symbol;
      };

      character_map['\0'] = CharacterType::EndOfFile;

      return character_map;
   }();

   Util::uint64 consume_and_eval_integer(LexerContext& lexer_context)
   {
      auto current_char = lexer_context.source.see_current();
      auto char_type = character_map[current_char]; 

      Util::uint64 integer_value = 0;

      while (char_type == CharacterType::Numeric)
      {
         integer_value *= 10;
         integer_value += (current_char - (unsigned char)'0');

         lexer_context.source.consume();
         current_char = lexer_context.source.see_current();
         char_type = character_map[current_char];
      };

      return integer_value;
   };

   long double consume_and_eval_fraction(LexerContext& lexer_context)
   {
      auto current_char = lexer_context.source.see_current();
      auto char_type = character_map[current_char]; 

      unsigned int length = 0;
      long double integer_value = 0;

      unsigned base_divisor = 1;

      while (char_type == CharacterType::Numeric)
      {
         integer_value *= 10;
         integer_value += (current_char - (unsigned char)'0');
         length++;
         base_divisor *= 10;
      
         lexer_context.source.consume();
         current_char = lexer_context.source.see_current();
         char_type = character_map[current_char];
      };

      if (length == 0)
      {
         return 0;
      };

      return integer_value / base_divisor;
   };

   inline void test_char_type(unsigned char index_char, CharacterType expected_type)
   {
    
      CharacterType actual_type = character_map[index_char];

      LAssert(
         actual_type == expected_type,
          
         "expected not to give an error"s + "Character mapping mismatch for char '"s + std::string(1,static_cast<char>(index_char)) + 
         "' (code: "s  + std::to_string(index_char) + ")."s + "Expected type "s + std::to_string((int)(expected_type)) + 
         ", but map returned " + std::to_string((int)(actual_type))  
      );
   };

   void consume_numbers_letters(LexerContext& lexer_context)
   {
      auto current_char = lexer_context.source.see_current();
      auto char_type = character_map[current_char]; 

      while (char_type == CharacterType::Numeric || char_type == CharacterType::Letter)
      {
         lexer_context.source.consume();

         current_char = lexer_context.source.see_current();
         char_type = character_map[current_char];
      };
   };

   void consume_identifier_token(LexerContext& lexer_context)
   {
      auto current_char = lexer_context.source.see_current();

      test_char_type(current_char,CharacterType::Letter);

      Util::uint64 offset = lexer_context.source.index;
      consume_numbers_letters(lexer_context);
      Util::uint64 length = lexer_context.source.index - offset;
   
      std::string_view identifier_view = std::string_view(reinterpret_cast<char*>(lexer_context.source.get_source_buffer() + offset),length);

      lexer_context.record_identifier(identifier_view);
   };

   void consume_numbers(LexerContext& lexer_context)
   {
      auto current_char = lexer_context.source.see_current();
      auto char_type = character_map[current_char]; 
      while (char_type == CharacterType::Numeric)
      {
         lexer_context.source.consume();
         current_char = lexer_context.source.see_current();
         char_type = character_map[current_char];
      };
   };

   void consume_hex_numeric_token(LexerContext& lexer_context)
   {
      auto current_char = lexer_context.source.see_current();
      auto next_char = lexer_context.source.peek();

      LAssert(current_char == '0' && next_char == 'x',
         "expected hex code number, got something else"s)

      lexer_context.source.consume(2);

      current_char = lexer_context.source.see_current();

      if (!TypeClassificator::is_hex_code(current_char))
      {
         return lexer_context.record_error(ErrorCode::MalformedNumber);
      };

      Util::uint64 length = 0;
      Util::uint64 number_integer = 0;

      while (TypeClassificator::is_hex_code(current_char))
      {
         number_integer *= 16;
         number_integer += get_hexadecimal_char_code_value(current_char);

         length++;

         lexer_context.source.consume();
         current_char = lexer_context.source.see_current();
      }

      if (!TypeClassificator::is_number_compitable_char_type(character_map[current_char]))
      {
         return lexer_context.record_error(ErrorCode::MalformedNumber);
      };

      if (length == 0)
      {
         return lexer_context.record_error(ErrorCode::TruncatedNumberSequence);
      };

      return lexer_context.record_number(NumberBase::Hexadecimal,NumberType::Integer,number_integer);
   };

   void consume_bin_numeric_token(LexerContext& lexer_context)
   {
      auto current_char = lexer_context.source.see_current();
      auto next_char = lexer_context.source.peek();

      LAssert(current_char == '0' && next_char == 'b',"expected hex code number, got something else"s 
         
      )

      lexer_context.source.consume(2);

      current_char = lexer_context.source.see_current();

      if(!TypeClassificator::is_bin_code(current_char))
      {
         return lexer_context.record_error(ErrorCode::MalformedNumber);
      };

      Util::uint64 length = 0;
      Util::uint64 number_integer = 0;

      while (TypeClassificator::is_bin_code(current_char))
      {
         number_integer *= 2;
         number_integer += get_binary_char_code_value(current_char);

         length++;

         lexer_context.source.consume();
         current_char = lexer_context.source.see_current();
      }

      if (!TypeClassificator::is_number_compitable_char_type(character_map[current_char]))
      {
         return lexer_context.record_error(ErrorCode::MalformedNumber);
      };

      if (length == 0)
      {
         return lexer_context.record_error(ErrorCode::TruncatedNumberSequence);
      };

      return lexer_context.record_number(NumberBase::Binary, NumberType::Integer,number_integer);
   }; 

   void consume_decimal_numeric_token(LexerContext& lexer_context)
   {
      auto current_char = lexer_context.source.see_current();
      auto first_char = current_char;

      Util::uint64 number_integer = 0;
      long double number_fraction = 0;

      if (current_char == '.')
      {
         //consume if .[numbers] case 
         lexer_context.source.consume();
         current_char = lexer_context.source.see_current();
      
         LAssert(
            character_map[current_char] == CharacterType::Numeric,
            "unexpected behaviour: token guesser misslcassified token type"s
         )

         number_fraction = consume_and_eval_fraction(lexer_context);
         return lexer_context.record_number(NumberBase::Decimal,NumberType::Float,number_integer,number_fraction);
      };

      //implicit [numbers].?[numbers] case
      number_integer = consume_and_eval_integer(lexer_context);

      auto middle_char = lexer_context.source.see_current();
      auto middle_char_type = character_map[middle_char];

      if (middle_char == '.')
      {
         //both cases:
         //[numbers](consume_index).[numbers]
         //[numbers](consume_index).
         lexer_context.source.consume();
         number_fraction = consume_and_eval_fraction(lexer_context);
      } else if (TypeClassificator::is_number_compitable_char_type(middle_char_type)) [[likely]]
      {
         //[numbers]
         return lexer_context.record_number(NumberBase::Decimal,NumberType::Integer,number_integer);
      } 
      else {
         return lexer_context.record_error(ErrorCode::MalformedNumber);
      };

      auto end_char = lexer_context.source.see_current();
      auto end_char_type = character_map[end_char];

      if (TypeClassificator::is_number_compitable_char_type(end_char_type)) [[likely]] 
      {
         //[numbers].[numbers]
         //[numbers].
         return lexer_context.record_number(NumberBase::Decimal,NumberType::Float,number_integer,number_fraction);
      } else {
         return lexer_context.record_error(ErrorCode::MalformedNumber);
      }; 
   }

   void consume_numeric_token(LexerContext& lexer_context)
   {
      auto current_char = lexer_context.source.see_current();

      if (current_char != '.')
      {
       test_char_type(current_char,CharacterType::Numeric);
      }

      auto next_char = lexer_context.source.peek();

      if (current_char == '0' && next_char == 'x')
      {
         return consume_hex_numeric_token(lexer_context);
      } else if(current_char == '0' && next_char == 'b')
      {
         return consume_bin_numeric_token(lexer_context);
      } else [[likely]] {
         return consume_decimal_numeric_token(lexer_context);
      };
   };

   void consume_eof_token(LexerContext& lexer_context)
   {
      test_char_type(lexer_context.source.see_current(),CharacterType::EndOfFile);
      lexer_context.source.consume();
   };

   void consume_error_token(LexerContext& lexer_context)
   {
      lexer_context.record_error(ErrorCode::UnexpectedCharacter);
      lexer_context.source.consume(); 
   };

   void consume_symbol_token(LexerContext& lexer_context)
   {
      using namespace SymbolClassifier;
      auto current_char = lexer_context.source.see_current();
      auto char_type = character_map[current_char];

      test_char_type(current_char,CharacterType::Symbol);

      auto start = lexer_context.source.index;
      auto start_ptr = reinterpret_cast<char*>(lexer_context.source.get_source_buffer() + start);

      auto symbol_kind = SymbolKind::Unknown;
      
      while (char_type == CharacterType::Symbol)
      {
         auto length = lexer_context.source.index - start + 1; 
         auto next_symbol = get_symbol_from_buffer(start_ptr,length);
      
         if (next_symbol == SymbolKind::Unknown)
         {
            return lexer_context.record_symbol(symbol_kind);
         }

         symbol_kind = next_symbol;
         lexer_context.source.consume();
         current_char = lexer_context.source.see_current();
         char_type = character_map[current_char];
      }

      if (symbol_kind == SymbolKind::Unknown)
      {
         return lexer_context.record_error(ErrorCode::UnknownSymbol);
      }

      return lexer_context.record_symbol(symbol_kind);
   };

   void consume_whitespace_token(LexerContext& lexer_context)
   {
      auto current_char = lexer_context.source.see_current();

      test_char_type(current_char,CharacterType::Whitespace);

      auto char_type = character_map[current_char]; 
      while (char_type == CharacterType::Whitespace)
      {
         lexer_context.source.consume();
         current_char = lexer_context.source.see_current();
         char_type = character_map[current_char];
      };
   };

   void consume_inline_comment(LexerContext& lexer_context)
   {
      auto current_char = lexer_context.source.see_current();

      test_char_type(current_char,CharacterType::Symbol);

      auto next_char = lexer_context.source.peek();
      LAssert(current_char == '/' && next_char == '/',
          
         "expected inline comment char start, got somethign else"
      );

      auto inline_char = current_char;
      while (character_map[inline_char] != CharacterType::NewLine && character_map[inline_char] != CharacterType::EndOfFile)
      {
         lexer_context.source.consume();
         inline_char = lexer_context.source.see_current();
      };

      /*
         if (character_map[inline_char] == CharacterType::EndOfFile)
         {
            return lexer_context.record_error(ErrorCode::UnclosedComment);
         };
      */

      return;
   };

   void consume_block_comment(LexerContext& lexer_context)
   {
      auto current_char = lexer_context.source.see_current();

      test_char_type(current_char,CharacterType::Symbol);

      auto next_char = lexer_context.source.peek();
      LAssert(current_char == '/' && next_char == '*',
          
         "expected inline comment char start, got somethign else"
      );
      
      lexer_context.source.consume(2);

      auto inline_char = lexer_context.source.see_current();

      while (character_map[inline_char] != CharacterType::EndOfFile)
      {
         auto next_char = lexer_context.source.peek();

         if (inline_char == '*' && next_char == '/')
         {
            lexer_context.source.consume(2);
            return;
         };

         lexer_context.source.consume();
         inline_char = lexer_context.source.see_current();
      };

      return lexer_context.record_error(ErrorCode::UnclosedComment);
   };

   void consume_comment_token(LexerContext& lexer_context)
   {
      auto current_char = lexer_context.source.see_current();
      test_char_type(current_char,CharacterType::Symbol);
      auto next_char = lexer_context.source.peek();

      LAssert(current_char == '/' && (next_char == '/' || next_char == '*'), "expected comment symbol sequence, got something else")

      bool is_multiline_comment = next_char == '*';

      if (is_multiline_comment)
      {
         return consume_block_comment(lexer_context);
      } else {
         return consume_inline_comment(lexer_context);
      }
   };

   void consume_new_line_token(LexerContext& lexer_context)
   {
      auto current_char = lexer_context.source.see_current();

      test_char_type(current_char,CharacterType::NewLine);
   
      lexer_context.source.consume();
   };

   void consume_string_token(LexerContext& lexer_context)
   {
      auto current_char = lexer_context.source.see_current();
      auto char_type = character_map[current_char];

      LAssert(
         current_char == '"',"expected string to begin with \"\"\", got something else instead"s
      )

      do
      {
         lexer_context.source.consume();
         current_char = lexer_context.source.see_current();
         char_type = character_map[current_char];if (char_type == CharacterType::EndOfFile) [[unlikely]]
         {
            return lexer_context.record_error(ErrorCode::UnclosedString);
         } else if (current_char == '\\') 
         {
            lexer_context.source.consume(); // skip '\' unless nullptr
            auto next_char = lexer_context.source.see_current();
            if (character_map[next_char] == CharacterType::EndOfFile)
            {
               return lexer_context.record_error(ErrorCode::UnclosedString);
            };
            continue;
         }
      } while (current_char != '"');
      
      lexer_context.source.consume(); //consume '"'      
      return;
   };

   void consume_char_token(LexerContext& lexer_context) {
      auto get_escape_sequence = [](char selected_char) {
         switch (selected_char) {
            case 'n':  return '\n';
            case 't':  return '\t';
            case 'r':  return '\r';
            case '\\': return '\\';
            case '\'': return '\'';
            case '\"': return '\"';
            case 'a':  return '\a';
            case 'b':  return '\b';
            case 'f':  return '\f';
            case 'v':  return '\v';
            default:   return selected_char;
         }
      };

      LAssert(
         lexer_context.source.see_current() == '\'',"expected string to begin with \"\'\", got something else instead"s
      )

      lexer_context.source.consume(); 
      char current_char = static_cast<char>(lexer_context.source.see_current());

      char char_value = 0;

      Util::uint64 counter = 0;
      while (current_char != '\'')
      {
         if (current_char == '\0')
         {
            return lexer_context.record_error(ErrorCode::UnclosedChar);
         };
         char_value = current_char;
         if (current_char == '\\') {
            lexer_context.source.consume(); 

            char next = static_cast<char>(lexer_context.source.see_current());
            if (next == '\0')
            {
               return lexer_context.record_error(ErrorCode::UnclosedChar);
            }
            char_value = get_escape_sequence(next);
         }  
         lexer_context.source.consume(); 

         counter++;
         current_char = lexer_context.source.see_current();
      }

      lexer_context.source.consume(); 

      if (counter == 0)
      {
         return lexer_context.record_error(ErrorCode::InvalidCharCode);
      } else if (counter == 1){
         return lexer_context.record_char_value(char_value);
      } else if(counter > 1)
      {
         return lexer_context.record_error(ErrorCode::TooLongChar);
      };
      
      return;
   }
   
   namespace CLua {
      TokenType guess_token_type(LexerContext& lexer_context)
      {
         auto current_char = static_cast<unsigned char>(lexer_context.source.see_current());
         auto character_type = character_map[current_char];

         switch (character_type)
         {
            case CharacterType::Error:
               return TokenKind<ErrorToken>::value;
            case CharacterType::Letter: 
               return TokenKind<IdentifierToken>::value;
            case CharacterType::Numeric:
               return TokenKind<NumericToken>::value;
            case CharacterType::Symbol:
            {
               auto next_char = static_cast<unsigned char>(lexer_context.source.peek());
               
               if (current_char == '/' && (next_char == '/' || next_char == '*')) [[unlikely]] 
               {  
                  return TokenKind<CommentToken>::value;
               } else if(current_char == '"')
               {
                  return TokenKind<StringToken>::value;
               } else if(current_char == '\'')
               {
                  return TokenKind<CharToken>::value; 
               } else if (current_char == '.' && character_map[next_char] == CharacterType::Numeric)
               {
                  return TokenKind<NumericToken>::value;
               };

               return TokenKind<SymbolToken>::value;
            }
            case CharacterType::Whitespace:
               return TokenKind<WhitespaceToken>::value;
            case CharacterType::NewLine:
               return TokenKind<NewLineToken>::value;
            case CharacterType::EndOfFile:
               return TokenKind<EOFToken>::value;
            default:
               return TokenKind<ErrorToken>::value;
         };

         return TokenKind<NoToken>::value;
      };

      void get_next_token(LexerContext& lexer_context, TokenType token_type){
         switch (token_type)
         {
         case TokenType::Identifier:
            consume_identifier_token(lexer_context);
            break;
         case TokenType::Numeric:
            consume_numeric_token(lexer_context);
            break;
         case TokenType::Symbol:
            consume_symbol_token(lexer_context);
            break;
         case TokenType::Whitespace:
            consume_whitespace_token(lexer_context);
            break;
         case TokenType::Comment:
            consume_comment_token(lexer_context);
            break;
         case TokenType::String:
            consume_string_token(lexer_context);
            break;
         case TokenType::Char:
            consume_char_token(lexer_context);
            break;
         case TokenType::NewLine:
            consume_new_line_token(lexer_context);
            break;
         case TokenType::EndOfFile:
            consume_eof_token(lexer_context);
            break;
         case TokenType::Error:
            consume_error_token(lexer_context);
            break;
         case TokenType::None:
            LAssert(false,
               
               "unexpected token type: got none"s        
            );
         default:
            LAssert(false,
               
               "unhandled token type: one at least has been forgotten"s        
            );
            break;
         }
      };
   }

   namespace MetaCLua {
      using CLua::guess_token_type;

      void get_next_token(LexerContext& lexer_context, TokenType token_type){
         switch (token_type)
         {
         default:
            CLua::get_next_token(lexer_context,token_type);
         }
      }
   };

   namespace LuaUCapture {
      using CLua::guess_token_type;

      void get_next_token(LexerContext& lexer_context, TokenType token_type){
         switch (token_type)
         {
         case TokenType::Symbol:
         {
            auto current_symbol = lexer_context.source.see_current();

            if (current_symbol == '[')
            {
               lexer_context.luau_capture_state.met_first_brace = true;
               lexer_context.luau_capture_state.brace_balance++;
            } else if (current_symbol == ']')
            {
               lexer_context.luau_capture_state.met_first_brace = true;
               lexer_context.luau_capture_state.brace_balance--;
            };

            consume_symbol_token(lexer_context);
            break;
         }
         default:
            CLua::get_next_token(lexer_context,token_type);
         }
      }
   };

   namespace LuaUCode {
      bool is_valid_lua_block(LexerContext& lexer_context,Util::uint64 peek_offset)
      {
         auto current_char = lexer_context.source.peek(peek_offset);

         while (current_char == '=')
         {
            peek_offset++;
            current_char = lexer_context.source.peek(peek_offset);
         };

         return current_char == '[';
      };

      bool is_lua_string(LexerContext& lexer_context)
      {
         auto current_char = lexer_context.source.see_current();
         auto is_not_string_start = current_char != '\'' && current_char != '"' && current_char != '[' && current_char != '`';

         if (is_not_string_start)
         {
            return false;
         };

         auto next_char = lexer_context.source.peek();

         if (current_char == '[' && is_valid_lua_block(lexer_context,1)) 
         {
            return true; 
         }  else if(current_char != '[') [[likely]] {
            return true;
         };

         return false;
      };

      bool is_lua_comment(LexerContext& lexer_context)
      {
         auto current_char = lexer_context.source.see_current();
         if (current_char != '-')
         {
            return false;
         };

         auto next_char = lexer_context.source.peek();
         if (next_char != '-')
         {
            return false;
         };

         return true;
      };

      void consume_unexpected_token(LexerContext& lexer_context)
      {
         lexer_context.record_error(ErrorCode::UnexpectedTokenType);
         lexer_context.source.consume();
         return;
      };

      enum class LuaUTokenType: Util::uint8 {
         LBracket,
         RBracket,
         String,
         Comment,
         Other,
         Error,
         EndOfFile,
         None
      };

      enum class LuaUCharType: Util::uint8 {
         Symbol,
         Other,
         EndOfFile,
         LBracket,
         RBracket,
         Error
      };

      auto luau_char_type_map = []()
      {
         std::array<LuaUCharType,256> luau_char_type_map;

         for (Util::uint64 char_code = 0; char_code < 256; char_code++)
         {
            auto real_char_code = static_cast<char>(char_code);
            
            if (!TypeClassificator::is_valid_char(real_char_code))
            {
               luau_char_type_map[char_code] = LuaUCharType::Error;
               continue;
            };

            if (TypeClassificator::is_special_char(real_char_code))
            {
               luau_char_type_map[char_code] = LuaUCharType::Symbol;
               continue;
            };

            luau_char_type_map[char_code] = LuaUCharType::Other;
         }

         luau_char_type_map[static_cast<unsigned char>('{')] = LuaUCharType::LBracket;
         luau_char_type_map[static_cast<unsigned char>('}')] = LuaUCharType::RBracket;

         luau_char_type_map[static_cast<unsigned char>('\0')] = LuaUCharType::EndOfFile;return luau_char_type_map;
      }();

      LuaUTokenType guess_luau_token_type(LexerContext& lexer_context)
      {
         auto current_char = lexer_context.source.see_current();
         auto character_type = luau_char_type_map[current_char];

         switch (character_type)
         {
            case LuaUCharType::Error:
               return LuaUTokenType::Error;
            case LuaUCharType::Other:
               return LuaUTokenType::Other;
            case LuaUCharType::Symbol:
            {
               if (is_lua_comment(lexer_context))
               {
                  return LuaUTokenType::Comment;
               } else if(is_lua_string(lexer_context))
               {
                  return LuaUTokenType::String;
               }
               return LuaUTokenType::Other;
            }
            case LuaUCharType::LBracket:
               return LuaUTokenType::LBracket;
            case LuaUCharType::RBracket:
               return LuaUTokenType::RBracket;
            case LuaUCharType::EndOfFile:
               return LuaUTokenType::EndOfFile;
            default:
               return LuaUTokenType::Error;
         };

         return LuaUTokenType::None;
      };

      void assert_is_lua_comment(LexerContext &lexer_context)
      {
         LAssert(
            lexer_context.source.see_current() == '-' && lexer_context.source.peek() == '-',
            
            "expected comment mode character, got something else"s   
         )
         lexer_context.source.consume(2);
      };    

      bool process_end_of_lua_block_token(LexerContext& lexer_context,Util::uint64 equal_sign_count)
      {
         Util::uint64 equal_signs_in_row = 0;

         LAssert(
            lexer_context.source.see_current() == ']',
            
            "assertion broken, expected comment closing character, got something else instead"s   
         )

         lexer_context.source.consume();

         auto current_char = lexer_context.source.see_current();
         while (current_char == '=')
         {
            equal_signs_in_row++;
            lexer_context.source.consume();
            current_char = lexer_context.source.see_current();
         };

         if (current_char != ']')
         {
            return false;
         }

         lexer_context.source.consume();
         return equal_signs_in_row == equal_sign_count;
      };

      bool process_is_lua_block(LexerContext& lexer_context,Util::uint64& equal_sign_count)
      {  
         if (lexer_context.source.see_current() != '[')
         {
            return false;
         };

         while (lexer_context.source.peek(equal_sign_count + 1) == '='){
            equal_sign_count++;
         };

         lexer_context.source.consume(equal_sign_count + 1);if (lexer_context.source.see_current() != '[')
         {
            return false;
         };

         lexer_context.source.consume();
         return true;
      };

      void consume_lua_block_token(LexerContext& lexer_context,Util::uint64 equal_sign_count)
      {

         auto current_char = lexer_context.source.see_current();
         auto char_type = character_map[current_char];

         while (char_type != CharacterType::EndOfFile)
         {
            if (current_char == ']' && process_end_of_lua_block_token(lexer_context,equal_sign_count))
            {
               return;
            } else {
               lexer_context.source.consume();
            };

            current_char = lexer_context.source.see_current();
            char_type = character_map[current_char];
         };return lexer_context.record_error(ErrorCode::UnclosedLuaBlock);
      };

      void consume_lua_basic_string_token(LexerContext& lexer_context)
      {
         auto start_char = lexer_context.source.see_current();

         LAssert(
            start_char == '\'' ||
            start_char == '"' ||
            start_char == '`',
             
            "assertion broken, the tested char is not a string start char"s   
         )

         lexer_context.source.consume();
         auto current_char = lexer_context.source.see_current();
         while (character_map[current_char] != CharacterType::EndOfFile)
         {
            if (current_char == start_char)
            {
               lexer_context.source.consume();
               break;
            };
            if (current_char == '\\')
            {
               lexer_context.source.consume();
               auto next_char = lexer_context.source.see_current(); 
               if (character_map[next_char] == CharacterType::EndOfFile)
               {
                  return lexer_context.record_error(ErrorCode::UnclosedLuaBlock);
                  //more specifically string is not closed properly but it's LuaU's VM problem
               };
            };
            lexer_context.source.consume();
            current_char = lexer_context.source.see_current();
         };
      };

      void consume_lua_string_token(LexerContext& lexer_context)
      {
         Util::uint64 equal_sign_count = 0;
         if (process_is_lua_block(lexer_context,equal_sign_count))
         {
            return consume_lua_block_token(lexer_context,equal_sign_count);
         } else {
            return consume_lua_basic_string_token(lexer_context);
         };
      };

      void consume_lua_inline_comment_token(LexerContext& lexer_context)
      {
         auto char_type = character_map[lexer_context.source.see_current()];
         while (char_type != CharacterType::NewLine)
         {
            if (char_type == CharacterType::EndOfFile)
            {
               return lexer_context.record_error(ErrorCode::UnclosedLuaBlock);
            };
            lexer_context.source.consume();
         };
      };

      void consume_lua_comment_token(LexerContext& lexer_context)
      {
         Util::uint64 equal_sign_count = 0;
         assert_is_lua_comment(lexer_context);
         if (process_is_lua_block(lexer_context,equal_sign_count))
         {
            return consume_lua_block_token(lexer_context,equal_sign_count);
         } else {
            /*
               LAssert(
                  equal_sign_count == 0,
                  
                  "unexpected state where equal sign count is non-zero, where it should be"s  
                  
               )

               //ignore byproduct as it's not harmful
            */
            return consume_lua_inline_comment_token(lexer_context);
         }
      };

      void consume_lua_other_token(LexerContext& lexer_context)
      {
         lexer_context.source.consume();
      };

      void consume_l_bracket(LexerContext& lexer_context)
      {
         LAssert(
            lexer_context.source.see_current() == '{',
            
            "expected { token, got something else instead"s
            
         )

         auto& brace_balance = lexer_context.luau_code_state.brace_balance;
         brace_balance++;

         lexer_context.source.consume();
      };

      void consume_r_bracket(LexerContext& lexer_context)
      {
         LAssert(
            lexer_context.source.see_current() == '}',
            
            "expected } token, got something else instead"s 
            
         )

         auto& brace_balance = lexer_context.luau_code_state.brace_balance;
         auto& met_first_brace = lexer_context.luau_code_state.met_first_brace;
         if (brace_balance <= 0)
         {
            //invalid state, error should be reported
            return lexer_context.record_error(ErrorCode::UnexpectedTokenType); //could be something more specific but no ideas   
         } 
         brace_balance--;   
         lexer_context.source.consume();
      };

      void consume_lua_block(LexerContext& lexer_context)
      {
         auto& brace_balance = lexer_context.luau_code_state.brace_balance;
         do {
            auto luau_token_type = guess_luau_token_type(lexer_context);

            switch (luau_token_type)
            {
            case LuaUTokenType::Comment:
               consume_lua_comment_token(lexer_context);
               break;
            case LuaUTokenType::String:
               consume_lua_string_token(lexer_context);
               break;
            case LuaUTokenType::LBracket:
               consume_l_bracket(lexer_context);
               break;
            case LuaUTokenType::RBracket:
               consume_r_bracket(lexer_context);
               break;
            case LuaUTokenType::Other:
               consume_lua_other_token(lexer_context);
               break;
            case LuaUTokenType::Error:
               consume_error_token(lexer_context);
            case LuaUTokenType::EndOfFile:
               return lexer_context.record_error(ErrorCode::UnclosedLuaBlock);
            default:
               consume_unexpected_token(lexer_context);
               break;
            }
         } while (brace_balance != 0);
         return;
      };

      TokenType guess_token_type(LexerContext& lexer_context)
      {
         auto current_char = static_cast<unsigned char>(lexer_context.source.see_current());
         auto character_type = character_map[current_char];

         switch (character_type)
         {
            case CharacterType::Symbol:
            {
               auto next_char = static_cast<unsigned char>(lexer_context.source.peek());
               if (current_char == '/' && (next_char == '/' || next_char == '*')) [[unlikely]] 
               {  
                  return TokenKind<CommentToken>::value;
               } else if (current_char == '{')
               {
                  return TokenKind<LuaBlockToken>::value;
               };
               return TokenKind<SymbolToken>::value;
            }
            default:
               return CLua::guess_token_type(lexer_context);
         };

         return TokenKind<NoToken>::value;
      };

      void process_next_token(LexerContext& lexer_context){
         TokenType token_type = guess_token_type(lexer_context);
         lexer_context.original_token_type = token_type;
         lexer_context.ultimate_token_type = token_type;switch (token_type)
         {
         case TokenType::LuaBlock:
            consume_lua_block(lexer_context);
            break;
         case TokenType::Whitespace:
            CLua::get_next_token(lexer_context,TokenType::Whitespace);
            break;
         case TokenType::None:
            CLua::get_next_token(lexer_context,TokenType::None);
            break;
         case TokenType::Error:
            CLua::get_next_token(lexer_context,TokenType::Error);
            break;
         default:
            consume_unexpected_token(lexer_context);
            break;
         };
      };
   };

   void get_next_token_in_meta_clua(LexerContext& lexer_context)
   {

      switch (lexer_context.see_current_meta_consumer_mode())
      {
         case MetaConsumerMode::Meta:
         {
         /*
            There should be 2 consumer modes for CLua and MetaCLua

            MetaCLua consumer type splits into:
            Meta and MetaLuaEmbedCapture, then MetaLuaEmbedCode
         */
         auto token_type = MetaCLua::guess_token_type(lexer_context);
         lexer_context.original_token_type = token_type;
         lexer_context.ultimate_token_type = token_type;
         MetaCLua::get_next_token(lexer_context,token_type);

         if (lexer_context.ultimate_token_type == TokenType::Identifier 
            && lexer_context.current_keyword == KeywordClassifier::Keyword::LuaEmbed)
         {
            lexer_context.switch_meta_consumer_mode(MetaConsumerMode::LuaCapture);
         } else if(lexer_context.ultimate_token_type == TokenType::NewLine)
         {
            lexer_context.switch_consumer_mode(ConsumerMode::CLua);
         };
         break;
      };
      case MetaConsumerMode::LuaCapture:
      {
         auto token_type = LuaUCapture::guess_token_type(lexer_context);
         lexer_context.original_token_type = token_type;
         lexer_context.ultimate_token_type = token_type;
         LuaUCapture::get_next_token(lexer_context,token_type);
         if (lexer_context.luau_capture_state.brace_balance == 0 && lexer_context.luau_capture_state.met_first_brace)
         {
            lexer_context.switch_meta_consumer_mode(MetaConsumerMode::LuaEmbed);
         };
         break;
      }
      case MetaConsumerMode::LuaEmbed:
      {
         LuaUCode::process_next_token(lexer_context);
         if (lexer_context.ultimate_token_type == TokenType::LuaBlock)
         {
            lexer_context.switch_consumer_mode(ConsumerMode::CLua);
         }
         break;
      }
      case MetaConsumerMode::None:
         LAssert(false,
             
            "Unexpected behaviour of the code, this case should not be happening"s      
         );
      }
   };

   TokenGeneric Lexer::get_next_token()
   {
      using namespace SymbolClassifier;
      
      Util::uint64 start = lexer_context.source.index;
      
      switch (lexer_context.see_current_consumer_mode())
      {
      case ConsumerMode::CLua:
      {
         auto token_type = CLua::guess_token_type(lexer_context);
         lexer_context.original_token_type = token_type;
         lexer_context.ultimate_token_type = token_type;
         CLua::get_next_token(lexer_context,token_type);
         if (lexer_context.ultimate_token_type == TokenType::Symbol && lexer_context.current_symbol == SymbolKind::AtSign)
         {
            lexer_context.switch_consumer_mode(ConsumerMode::MetaCLua);
         };
         break;
      }
      case ConsumerMode::MetaCLua:
      {
         get_next_token_in_meta_clua(lexer_context); 
         if (lexer_context.ultimate_token_type == TokenType::Error)
         {
            lexer_context.switch_consumer_mode(ConsumerMode::CLua);
         }
         break;
      }      
      default:{
         LAssert(false,
            
            "unhandled case for consumer type"s   
         )
         break;
      }
      }

      Util::uint64 end = lexer_context.source.index;
      Util::uint64 length = end - start;
      TokenGeneric token;
      token.token_type = lexer_context.ultimate_token_type;
      token.offset = start;
      token.length = length;

      return token;
   };
}
