// Converted from the boost 1.55 wave sources (wave/src/cpplexer/re2clex/cpp.re).

/*=============================================================================
Boost.Wave: A Standard compliant C++ preprocessor library

Copyright (c) 2001 Daniel C. Nuffer
Copyright (c) 2001-2013 Hartmut Kaiser.
Distributed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

This is a lexer conforming to the Standard with a few exceptions.
So it does allow the '$' to be part of identifiers. If you need strict
Standards conforming behaviour, please include the lexer definition
provided in the file strict_cpp.re.

TODO:
handle errors better.
=============================================================================*/

#include "stdafx.h"

#include "cpp.h"
//#include "rev_regex.h"

#define act_in_cpp0x_mode 1
#define enable_ms_extensions 1

static macro_pair g_macros_[] =
{ {"any", "[\t\v\f\r\n\040-\377]"},
{"anyctrl", "[\001-\037]"},
{"OctalDigit", "[0-7]"},
{"Digit", "[0-9]"},
{"HexDigit", "[a-fA-F0-9]"},
{"Integer",
    "((0[xX]{HexDigit}+)|(0{OctalDigit}*)|([1-9]{Digit}*))"},
{"ExponentStart", "[Ee][-+]"},
{"ExponentPart", "[Ee][-+]?{Digit}+"},
{"FractionalConstant", "({Digit}*[.]{Digit}+)|({Digit}+[.])"},
{"FloatingSuffix", "[fF][lL]?|[lL][fF]?"},
{"IntegerSuffix", "[uU][lL]?|[lL][uU]?"},
{"LongIntegerSuffix", "[uU]([lL][lL])|([lL][lL])[uU]?"},
{"MSLongIntegerSuffix", "u?i64"},
{"Backslash", "[\\\\]|\"??/\""},
{"EscapeSequence",
    "{Backslash}([abfnrtv?'\"]|{Backslash}|x{HexDigit}+|"
    "{OctalDigit}{OctalDigit}?{OctalDigit}?)"},
{"HexQuad", "{HexDigit}{HexDigit}{HexDigit}{HexDigit}"},
{"UniversalChar", "{Backslash}(u{HexQuad}|U{HexQuad}{HexQuad})"},
{"Newline", "\r\n|\n|\r"},
{"PPSpace", "([ \t\f\v]|(\"/*\"({any}{-}[*]|{Newline}|"
    "([*]+({any}{-}[*/]|{Newline})))*[*]+[/]))*"},
{"Pound", "#|\"??=\"|%:"},
{ "NonDigit", "[a-zA-Z$]|{UniversalChar}" },
{0, 0} };

static rule_pair g_regexes_[] =
{ { "\\\\{Newline}", T_BS_NEWLINE },
{ "[/][*].{+}[\r\n]*?[*][/]", T_CCOMMENT },
{ "[/][/].*", T_CPPCOMMENT },
#ifdef detect_pp_numbers
{"[.]?{Digit}[.]?{Digit}({Digit}|{NonDigit}|{ExponentStart}|[.])*", T_PP_NUMBER},
#else
{ "[.]?{Digit}({FractionalConstant}{ExponentPart}?|"
"{Digit}+{ExponentPart}){FloatingSuffix}?", T_FLOATLIT },

#ifdef enable_ms_extensions
{ "{Integer}({LongIntegerSuffix}|{MSLongIntegerSuffix})",
T_LONGINTLIT },
#else
{"{Integer}{LongIntegerSuffix}", T_LONGINTLIT},
#endif
{"{Integer}{IntegerSuffix}?", T_INTLIT},
#endif

#ifdef act_in_cpp0x_mode
{"alignas", T_ALIGNAS},
{"alignof", T_ALIGNOF},
{"char16_t", T_CHAR16_T},
{"char32_t", T_CHAR32_T},
{"constexpr", T_CONSTEXPR},
{"decltype", T_DECLTYPE},
{"noexcept", T_NOEXCEPT},
{"nullptr", T_NULLPTR},
{"static_assert", T_STATICASSERT},
{"thread_local", T_THREADLOCAL},
{"(L|[uU]|u8)?R[\"][(].{+}[\r\n]*?[)][\"]", T_RAWSTRINGLIT},
{"(L|[uU])?'({EscapeSequence}|{UniversalChar}|"
    "{any}{-}[\n\r\\\\'])'", T_CHARLIT},
{"(L|[uU]|u8)?[\"]({EscapeSequence}|{UniversalChar}|"
    "{any}{-}[\n\r\\\\\"]|\\\\{Newline})*[\"]", T_STRINGLIT},
#else
{"alignas", T_IDENTIFIER},
{"alignof", T_IDENTIFIER},
{"char16_t", T_IDENTIFIER},
{"char32_t", T_IDENTIFIER},
{"constexpr", T_IDENTIFIER},
{"decltype", T_IDENTIFIER},
{"noexcept", T_IDENTIFIER},
{"nullptr", T_IDENTIFIER},
{"static_assert", T_IDENTIFIER},
{"thread_local", T_IDENTIFIER},
#endif
{"asm", T_ASM},
{"auto", T_AUTO},
{"bool", T_BOOL},
{"break", T_BREAK},
{"case", T_CASE},
{"catch", T_CATCH},
{"char", T_CHAR},
{"class", T_CLASS},
{"const", T_CONST},
{"const_cast", T_CONSTCAST},
{"continue", T_CONTINUE},
{"default", T_DEFAULT},
{"delete", T_DELETE},
{"do", T_DO},
{"double", T_DOUBLE},
{"dynamic_cast", T_DYNAMICCAST},
{"else", T_ELSE},
{"enum", T_ENUM},
{"explicit", T_EXPLICIT},
{"export", T_EXPORT},
{"extern", T_EXTERN},
{"false", T_FALSE},
{"float", T_FLOAT},
{"for", T_FOR},
{"friend", T_FRIEND},
{"goto", T_GOTO},
{"if", T_IF},
#ifdef enable_import_keyword
{"import", T_IMPORT},
#else
{"import", T_IDENTIFIER},
#endif
{"inline", T_INLINE},
{"int", T_INT},
{"long", T_LONG},
{"mutable", T_MUTABLE},
{"namespace", T_NAMESPACE},
{"new", T_NEW},
{"operator", T_OPERATOR},
{"private", T_PRIVATE},
{"protected", T_PROTECTED},
{"public", T_PUBLIC},
{"register", T_REGISTER},
{"reinterpret_cast", T_REINTERPRETCAST},
{"return", T_RETURN},
{"short", T_SHORT},
{"signed", T_SIGNED},
{"sizeof", T_SIZEOF},
{"static", T_STATIC},
{"static_cast", T_STATICCAST},
{"struct", T_STRUCT},
{"switch", T_SWITCH},
{"template", T_TEMPLATE},
{"this", T_THIS},
{"throw", T_THROW},
{"true", T_TRUE},
{"try", T_TRY},
{"typedef", T_TYPEDEF},
{"typeid", T_TYPEID},
{"typename", T_TYPENAME},
{"union", T_UNION},
{"unsigned", T_UNSIGNED},
{"using", T_USING},
{"virtual", T_VIRTUAL},
{"void", T_VOID},
{"volatile", T_VOLATILE},
{"wchar_t", T_WCHART},
{"while", T_WHILE},

#ifdef enable_ms_extensions
{"__int8", T_MSEXT_INT8},
{"__int16", T_MSEXT_INT16},
{"__int32", T_MSEXT_INT32},
{"__int64", T_MSEXT_INT64},
{"_?_based", T_MSEXT_BASED},
{"_?_declspec", T_MSEXT_DECLSPEC},
{"_?_cdecl", T_MSEXT_CDECL},
{"_?_fastcall", T_MSEXT_FASTCALL},
{"_?_stdcall", T_MSEXT_STDCALL},
{"__try", T_MSEXT_TRY},
{"__except", T_MSEXT_EXCEPT},
{"__finally", T_MSEXT_FINALLY},
{"__leave", T_MSEXT_LEAVE},
{"_?_inline", T_MSEXT_INLINE},
{"_?_asm", T_MSEXT_ASM},
{"{Pound}{PPSpace}using{PPSpace}"
    "<({any}{-}[\n\r>])+>", T_PP_HHEADER},
{"{Pound}{PPSpace}(import|using){PPSpace}[\"]"
    "({any}{-}[\n\r\"])+[\"]", T_PP_QHEADER},
#else
{"__int8", T_IDENTIFIER},
{"__int16", T_IDENTIFIER},
{"__int32", T_IDENTIFIER},
{"__int64", T_IDENTIFIER},
{"_?_based", T_IDENTIFIER},
{"_?_declspec", T_IDENTIFIER},
{"_?_cdecl", T_IDENTIFIER},
{"_?_fastcall", T_IDENTIFIER},
{"_?_stdcall", T_IDENTIFIER},
{"__try", T_IDENTIFIER},
{"__except", T_IDENTIFIER},
{"__finally", T_IDENTIFIER},
{"__leave", T_IDENTIFIER},
{"_?_inline", T_IDENTIFIER},
{"_?_asm", T_IDENTIFIER},
#endif

{"[{]", T_LEFTBRACE},
{"\"??<\"", T_LEFTBRACE_TRIGRAPH},
{"<%", T_LEFTBRACE_ALT},
{"[}]", T_RIGHTBRACE},
{"\"??>\"", T_RIGHTBRACE_TRIGRAPH},
{"%>", T_RIGHTBRACE_ALT},
{"[[]", T_LEFTBRACKET},
{"\"??(\"", T_LEFTBRACKET_TRIGRAPH},
{"<:", T_LEFTBRACKET_ALT},
{"\\]", T_RIGHTBRACKET},
{"\"??)\"", T_RIGHTBRACKET_TRIGRAPH},
{":>", T_RIGHTBRACKET_ALT},
{"#", T_POUND},
{"%:", T_POUND_ALT},
{"\"??=\"", T_POUND_TRIGRAPH},
{"##", T_POUND_POUND},
{"\"#??=\"", T_POUND_POUND_TRIGRAPH},
{"\"??=#\"", T_POUND_POUND_TRIGRAPH},
{"\"??=??=\"", T_POUND_POUND_TRIGRAPH},
{"%:%:", T_POUND_POUND_ALT},
{"[(]", T_LEFTPAREN},
{"[)]", T_RIGHTPAREN},
{";", T_SEMICOLON},
{":", T_COLON},
{"\"...\"", T_ELLIPSIS},
{"[?]", T_QUESTION_MARK},
{"::", T_COLON_COLON},
{"[.]", T_DOT},
{"\".*\"", T_DOTSTAR},
{"[+]", T_PLUS},
{"-", T_MINUS},
{"[*]", T_STAR},
{"[/]", T_DIVIDE},
{"%", T_PERCENT},
{"\\^", T_XOR},
{"\"??'\"", T_XOR_TRIGRAPH},
{"xor", T_XOR_ALT},
{"&", T_AND},
{"bitand", T_AND_ALT},
{"[|]", T_OR},
{"bitor", T_OR_ALT},
{"\"??!\"", T_OR_TRIGRAPH},
{"~", T_COMPL},
{"\"??-\"", T_COMPL_TRIGRAPH},
{"compl", T_COMPL_ALT},
{"!", T_NOT},
{"not", T_NOT_ALT},
{"=", T_ASSIGN},
{"<", T_LESS},
{">", T_GREATER},
{"[+]=", T_PLUSASSIGN},
{"-=", T_MINUSASSIGN},
{"[*]=", T_STARASSIGN},
{"[/]=", T_DIVIDEASSIGN},
{"%=", T_PERCENTASSIGN},
{"\\^=", T_XORASSIGN},
{"xor_eq", T_XORASSIGN_ALT},
{"\"??'=\"", T_XORASSIGN_TRIGRAPH},
{"&=", T_ANDASSIGN},
{"and_eq", T_ANDASSIGN_ALT},
{"[|]=", T_ORASSIGN},
{"or_eq", T_ORASSIGN_ALT},
{"\"??!=\"", T_ORASSIGN_TRIGRAPH},
{"<<", T_SHIFTLEFT},
{">>", T_SHIFTRIGHT},
{">>=", T_SHIFTRIGHTASSIGN},
{"<<=", T_SHIFTLEFTASSIGN},
{"==", T_EQUAL},
{"!=", T_NOTEQUAL},
{"not_eq", T_NOTEQUAL_ALT},
{"<=", T_LESSEQUAL},
{">=", T_GREATEREQUAL},
{"&&", T_ANDAND},
{"and", T_ANDAND_ALT},
{"\"||\"", T_OROR},
{"\"??!|\"", T_OROR_TRIGRAPH},
{"\"|??!\"", T_OROR_TRIGRAPH},
{"or", T_OROR_ALT},
{"\"??!??!\"", T_OROR_TRIGRAPH},
{"\"++\"", T_PLUSPLUS},
{"--", T_MINUSMINUS},
{",", T_COMMA},
{"->[*]", T_ARROWSTAR},
{"->", T_ARROW},
{"\"??/\"", T_ANY_TRIGRAPH},

{"L?('({EscapeSequence}|{UniversalChar}|"
    "{any}{-}[\n\r\\\\'])+')", T_CHARLIT},
{"L?([\"]({EscapeSequence}|{UniversalChar}|"
    "{any}{-}[\n\r\\\\\"]|\\\\{Newline})*[\"])", T_STRINGLIT},

{"([a-zA-Z_$]|{UniversalChar})([a-zA-Z_0-9$]|"
    "{UniversalChar})*", T_IDENTIFIER},
{"{Pound}{PPSpace}(include|include_next){PPSpace}"
    "<({any}{-}[\n\r>])+>", T_PP_HHEADER},
{"{Pound}{PPSpace}(include|include_next){PPSpace}[\"]"
    "({any}{-}[\n\r\"])+[\"]", T_PP_QHEADER},
{"{Pound}{PPSpace}(include|include_next){PPSpace}",
    T_PP_INCLUDE},
{"{Pound}{PPSpace}if", T_PP_IF},
{"{Pound}{PPSpace}ifdef", T_PP_IFDEF},
{"{Pound}{PPSpace}ifndef", T_PP_IFNDEF},
{"{Pound}{PPSpace}else", T_PP_ELSE},
{"{Pound}{PPSpace}elif", T_PP_ELIF},
{"{Pound}{PPSpace}endif", T_PP_ENDIF},
{"{Pound}{PPSpace}define", T_PP_DEFINE},
{"{Pound}{PPSpace}undef", T_PP_UNDEF},
{"{Pound}{PPSpace}line", T_PP_LINE},
{"{Pound}{PPSpace}error", T_PP_ERROR},
{"{Pound}{PPSpace}pragma", T_PP_PRAGMA},
{"{Pound}{PPSpace}warning", T_PP_WARNING},

{"{Pound}{PPSpace}region", T_MSEXT_PP_REGION},
{"{Pound}{PPSpace}endregion", T_MSEXT_PP_ENDREGION},

{"[ \t\v\f]+", T_SPACE},
{ "{Newline}", T_NEWLINE },
{ 0, 0 } };

void build_cpp(lexertl::rules &rules_)
{
    for (macro_pair *ptr_ = g_macros_; ptr_->_name; ++ptr_)
    {
        rules_.insert_macro(ptr_->_name, ptr_->_regex);
    }

    for (rule_pair *ptr_ = g_regexes_; ptr_->_regex; ++ptr_)
    {
        rules_.push(ptr_->_regex, ptr_->_id);
    }
}
// This can now be done with lexertl::rules::reverse().
/*
void build_rev_cpp(lexertl::rules &rules_)
{
    lexertl::state_machine rev_rx_sm_;

    build_rev_regex_lexer(rev_rx_sm_);

    for (macro_pair *ptr_ = g_macros_; ptr_->_name; ++ptr_)
    {
        rules_.insert_macro(ptr_->_name,
            rev_regex(ptr_->_regex, rev_rx_sm_).c_str());
    }

    for (rule_pair *ptr_ = g_regexes_; ptr_->_regex; ++ptr_)
    {
        rules_.push(rev_regex(ptr_->_regex, rev_rx_sm_).c_str(), ptr_->_id);
    }
}
*/