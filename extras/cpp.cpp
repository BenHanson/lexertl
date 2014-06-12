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
#include "../lexertl/rules.hpp"

#define act_in_cpp0x_mode 1
#define enable_ms_extensions 1

void build_cpp(lexertl::rules &rules_)
{
    rules_.insert_macro("any", "[\t\v\f\r\n\040-\377]");
    rules_.insert_macro("anyctrl", "[\001-\037]");
    rules_.insert_macro("OctalDigit", "[0-7]");
    rules_.insert_macro("Digit", "[0-9]");
    rules_.insert_macro("HexDigit", "[a-fA-F0-9]");
    rules_.insert_macro("Integer",
        "((0[xX]{HexDigit}+)|(0{OctalDigit}*)|([1-9]{Digit}*))");
    rules_.insert_macro("ExponentStart", "[Ee][-+]");
    rules_.insert_macro("ExponentPart", "[Ee][-+]?{Digit}+");
    rules_.insert_macro("FractionalConstant", "({Digit}*[.]{Digit}+)|({Digit}+[.])");
    rules_.insert_macro("FloatingSuffix", "[fF][lL]?|[lL][fF]?");
    rules_.insert_macro("IntegerSuffix", "[uU][lL]?|[lL][uU]?");
    rules_.insert_macro("LongIntegerSuffix", "[uU]([lL][lL])|([lL][lL])[uU]?");
    rules_.insert_macro("MSLongIntegerSuffix", "u?i64");
    rules_.insert_macro("Backslash", "[\\\\]|\"??/\"");
    rules_.insert_macro("EscapeSequence",
        "{Backslash}([abfnrtv?'\"]|{Backslash}|x{HexDigit}+|"
        "{OctalDigit}{OctalDigit}?{OctalDigit}?)");
    rules_.insert_macro("HexQuad", "{HexDigit}{HexDigit}{HexDigit}{HexDigit}");
    rules_.insert_macro("UniversalChar", "{Backslash}(u{HexQuad}|U{HexQuad}{HexQuad})");
    rules_.insert_macro("Newline", "\r\n|\n|\r");
    rules_.insert_macro("PPSpace", "([\t\f\v]|(\"/*\"({any}{-}[*]|{Newline}|"
        "([*]+({any}{-}[*/ ]|{Newline})))*[*]+[/]))*");
    rules_.insert_macro("Pound", "#|\"??=\"|%:");
    rules_.insert_macro("NonDigit", "[a-zA-Z$]|{UniversalChar}");

    rules_.push("\\/\\*(.|\n)*?\\*\\/", T_CCOMMENT);
    rules_.push("\\/\\/.*", T_CPPCOMMENT);
#ifdef detect_pp_numbers
    rules_.push("[.]?{Digit}[.]?{Digit}({Digit}|{NonDigit}|{ExponentStart}|"
        "[.])*", T_PP_NUMBER);
#else
    rules_.push("[.]?{Digit}({FractionalConstant}{ExponentPart}?|"
        "{Digit}+{ExponentPart}){FloatingSuffix}?", T_FLOATLIT);

#ifdef enable_ms_extensions
    rules_.push("{Integer}({LongIntegerSuffix}|{MSLongIntegerSuffix})",
        T_LONGINTLIT);
#else
    rules_.push("{Integer}{LongIntegerSuffix}", T_LONGINTLIT);
#endif
    rules_.push("{Integer}{IntegerSuffix}?", T_INTLIT);
#endif

#ifdef act_in_cpp0x_mode
    rules_.push("alignas", T_ALIGNAS);
    rules_.push("alignof", T_ALIGNOF);
    rules_.push("char16_t", T_CHAR16_T);
    rules_.push("char32_t", T_CHAR32_T);
    rules_.push("constexpr", T_CONSTEXPR);
    rules_.push("decltype", T_DECLTYPE);
    rules_.push("noexcept", T_NOEXCEPT);
    rules_.push("nullptr", T_NULLPTR);
    rules_.push("static_assert", T_STATICASSERT);
    rules_.push("thread_local", T_THREADLOCAL);
    rules_.push("(L|[uU]|u8)?R[\"]({EscapeSequence}|{UniversalChar}|"
        "{any}{-}[\r\n\\\\\"])[\"]", T_RAWSTRINGLIT);
    rules_.push("[uU]'({EscapeSequence}|{UniversalChar}|{any}{-}[\n\r\\\\'])'",
        T_CHARLIT);
    rules_.push("([uU]|u8)[\"]({EscapeSequence}|UniversalChar|"
        "{any}{-}[\n\r\\\\\"])*[\"]", T_STRINGLIT);
#else
    rules_.push("alignas", T_IDENTIFIER);
    rules_.push("alignof", T_IDENTIFIER);
    rules_.push("char16_t", T_IDENTIFIER);
    rules_.push("char32_t", T_IDENTIFIER);
    rules_.push("constexpr", T_IDENTIFIER);
    rules_.push("decltype", T_IDENTIFIER);
    rules_.push("noexcept", T_IDENTIFIER);
    rules_.push("nullptr", T_IDENTIFIER);
    rules_.push("static_assert", T_IDENTIFIER);
    rules_.push("thread_local", T_IDENTIFIER);
#endif
    rules_.push("asm", T_ASM);
    rules_.push("auto", T_AUTO);
    rules_.push("bool", T_BOOL);
    rules_.push("break", T_BREAK);
    rules_.push("case", T_CASE);
    rules_.push("catch", T_CATCH);
    rules_.push("char", T_CHAR);
    rules_.push("class", T_CLASS);
    rules_.push("const", T_CONST);
    rules_.push("const_cast", T_CONSTCAST);
    rules_.push("continue", T_CONTINUE);
    rules_.push("default", T_DEFAULT);
    rules_.push("delete", T_DELETE);
    rules_.push("do", T_DO);
    rules_.push("double", T_DOUBLE);
    rules_.push("dynamic_cast", T_DYNAMICCAST);
    rules_.push("else", T_ELSE);
    rules_.push("enum", T_ENUM);
    rules_.push("explicit", T_EXPLICIT);
    rules_.push("export", T_EXPORT);
    rules_.push("extern", T_EXTERN);
    rules_.push("false", T_FALSE);
    rules_.push("float", T_FLOAT);
    rules_.push("for", T_FOR);
    rules_.push("friend", T_FRIEND);
    rules_.push("goto", T_GOTO);
    rules_.push("if", T_IF);
#ifdef enable_import_keyword
    rules_.push("import", T_IMPORT);
#else
    rules_.push("import", T_IDENTIFIER);
#endif
    rules_.push("inline", T_INLINE);
    rules_.push("int", T_INT);
    rules_.push("long", T_LONG);
    rules_.push("mutable", T_MUTABLE);
    rules_.push("namespace", T_NAMESPACE);
    rules_.push("new", T_NEW);
    rules_.push("operator", T_OPERATOR);
    rules_.push("private", T_PRIVATE);
    rules_.push("protected", T_PROTECTED);
    rules_.push("public", T_PUBLIC);
    rules_.push("register", T_REGISTER);
    rules_.push("reinterpret_cast", T_REINTERPRETCAST);
    rules_.push("return", T_RETURN);
    rules_.push("short", T_SHORT);
    rules_.push("signed", T_SIGNED);
    rules_.push("sizeof", T_SIZEOF);
    rules_.push("static", T_STATIC);
    rules_.push("static_cast", T_STATICCAST);
    rules_.push("struct", T_STRUCT);
    rules_.push("switch", T_SWITCH);
    rules_.push("template", T_TEMPLATE);
    rules_.push("this", T_THIS);
    rules_.push("throw", T_THROW);
    rules_.push("true", T_TRUE);
    rules_.push("try", T_TRY);
    rules_.push("typedef", T_TYPEDEF);
    rules_.push("typeid", T_TYPEID);
    rules_.push("typename", T_TYPENAME);
    rules_.push("union", T_UNION);
    rules_.push("unsigned", T_UNSIGNED);
    rules_.push("using", T_USING);
    rules_.push("virtual", T_VIRTUAL);
    rules_.push("void", T_VOID);
    rules_.push("volatile", T_VOLATILE);
    rules_.push("wchar_t", T_WCHART);
    rules_.push("while", T_WHILE);

#ifdef enable_ms_extensions
    rules_.push("__int8", T_MSEXT_INT8);
    rules_.push("__int16", T_MSEXT_INT16);
    rules_.push("__int32", T_MSEXT_INT32);
    rules_.push("__int64", T_MSEXT_INT64);
    rules_.push("_?_based", T_MSEXT_BASED);
    rules_.push("_?_declspec", T_MSEXT_DECLSPEC);
    rules_.push("_?_cdecl", T_MSEXT_CDECL);
    rules_.push("_?_fastcall", T_MSEXT_FASTCALL);
    rules_.push("_?_stdcall", T_MSEXT_STDCALL);
    rules_.push("__try", T_MSEXT_TRY);
    rules_.push("__except", T_MSEXT_EXCEPT);
    rules_.push("__finally", T_MSEXT_FINALLY);
    rules_.push("__leave", T_MSEXT_LEAVE);
    rules_.push("_?_inline", T_MSEXT_INLINE);
    rules_.push("_?_asm", T_MSEXT_ASM);
#else
    rules_.push("__int8", T_IDENTIFIER);
    rules_.push("__int16", T_IDENTIFIER);
    rules_.push("__int32", T_IDENTIFIER);
    rules_.push("__int64", T_IDENTIFIER);
    rules_.push("_?_based", T_IDENTIFIER);
    rules_.push("_?_declspec", T_IDENTIFIER);
    rules_.push("_?_cdecl", T_IDENTIFIER);
    rules_.push("_?_fastcall", T_IDENTIFIER);
    rules_.push("_?_stdcall", T_IDENTIFIER);
    rules_.push("__try", T_IDENTIFIER);
    rules_.push("__except", T_IDENTIFIER);
    rules_.push("__finally", T_IDENTIFIER);
    rules_.push("__leave", T_IDENTIFIER);
    rules_.push("_?_inline", T_IDENTIFIER);
    rules_.push("_?_asm", T_IDENTIFIER);
#endif

    rules_.push("[{]", T_LEFTBRACE);
    rules_.push("\"??<\"", T_LEFTBRACE_TRIGRAPH);
    rules_.push("<%", T_LEFTBRACE_ALT);
    rules_.push("[}]", T_RIGHTBRACE);
    rules_.push("\"??>\"", T_RIGHTBRACE_TRIGRAPH);
    rules_.push("%>", T_RIGHTBRACE_ALT);
    rules_.push("[[]", T_LEFTBRACKET);
    rules_.push("\"??(\"", T_LEFTBRACKET_TRIGRAPH);
    rules_.push("<:", T_LEFTBRACKET_ALT);
    rules_.push("\\]", T_RIGHTBRACKET);
    rules_.push("\"??)\"", T_RIGHTBRACKET_TRIGRAPH);
    rules_.push(":>", T_RIGHTBRACKET_ALT);
    rules_.push("#", T_POUND);
    rules_.push("%:", T_POUND_ALT);
    rules_.push("\"??=\"", T_POUND_TRIGRAPH);
    rules_.push("##", T_POUND_POUND);
    rules_.push("\"#??=\"", T_POUND_POUND_TRIGRAPH);
    rules_.push("\"??=#\"", T_POUND_POUND_TRIGRAPH);
    rules_.push("\"??=??=\"", T_POUND_POUND_TRIGRAPH);
    rules_.push("%:%:", T_POUND_POUND_ALT);
    rules_.push("[(]", T_LEFTPAREN);
    rules_.push("[)]", T_RIGHTPAREN);
    rules_.push(";", T_SEMICOLON);
    rules_.push(":", T_COLON);
    rules_.push("...", T_ELLIPSIS);
    rules_.push("[?]", T_QUESTION_MARK);
    rules_.push("::", T_COLON_COLON);
    rules_.push("[.]", T_DOT);
    rules_.push("\".*\"", T_DOTSTAR);
    rules_.push("[+]", T_PLUS);
    rules_.push("-", T_MINUS);
    rules_.push("[*]", T_STAR);
    rules_.push("[/]", T_DIVIDE);
    rules_.push("%", T_PERCENT);
    rules_.push("[^]", T_XOR);
    rules_.push("\"??'\"", T_XOR_TRIGRAPH);
    rules_.push("xor", T_XOR_ALT);
    rules_.push("&", T_AND);
    rules_.push("bitand", T_AND_ALT);
    rules_.push("[|]", T_OR);
    rules_.push("bitor", T_OR_ALT);
    rules_.push("\"??!\"", T_OR_TRIGRAPH);
    rules_.push("~", T_COMPL);
    rules_.push("\"??-\"", T_COMPL_TRIGRAPH);
    rules_.push("compl", T_COMPL_ALT);
    rules_.push("!", T_NOT);
    rules_.push("not", T_NOT_ALT);
    rules_.push("=", T_ASSIGN);
    rules_.push("<", T_LESS);
    rules_.push(">", T_GREATER);
    rules_.push("[+]=", T_PLUSASSIGN);
    rules_.push("-=", T_MINUSASSIGN);
    rules_.push("[*]=", T_STARASSIGN);
    rules_.push("[/]=", T_DIVIDEASSIGN);
    rules_.push("%=", T_PERCENTASSIGN);
    rules_.push("[^]=", T_XORASSIGN);
    rules_.push("xor_eq", T_XORASSIGN_ALT);
    rules_.push("\"??'=\"", T_XORASSIGN_TRIGRAPH);
    rules_.push("&=", T_ANDASSIGN);
    rules_.push("and_eq", T_ANDASSIGN_ALT);
    rules_.push("[|]=", T_ORASSIGN);
    rules_.push("or_eq", T_ORASSIGN_ALT);
    rules_.push("\"??!=\"", T_ORASSIGN_TRIGRAPH);
    rules_.push("<<", T_SHIFTLEFT);
    rules_.push(">>", T_SHIFTRIGHT);
    rules_.push(">>=", T_SHIFTRIGHTASSIGN);
    rules_.push("<<=", T_SHIFTLEFTASSIGN);
    rules_.push("==", T_EQUAL);
    rules_.push("!=", T_NOTEQUAL);
    rules_.push("not_eq", T_NOTEQUAL_ALT);
    rules_.push("<=", T_LESSEQUAL);
    rules_.push(">=", T_GREATEREQUAL);
    rules_.push("&&", T_ANDAND);
    rules_.push("and", T_ANDAND_ALT);
    rules_.push("\"||\"", T_OROR);
    rules_.push("\"??!|\"", T_OROR_TRIGRAPH);
    rules_.push("\"|??!\"", T_OROR_TRIGRAPH);
    rules_.push("or", T_OROR_ALT);
    rules_.push("\"??!??!\"", T_OROR_TRIGRAPH);
    rules_.push("\"++\"", T_PLUSPLUS);
    rules_.push("--", T_MINUSMINUS);
    rules_.push(",", T_COMMA);
    rules_.push("->[*]", T_ARROWSTAR);
    rules_.push("->", T_ARROW);
    rules_.push("\"??/\"", T_ANY_TRIGRAPH);

    rules_.push("L?('({EscapeSequence}|{UniversalChar}|"
        "{any}{-}[\n\r\\\\'])+')", T_CHARLIT);
    rules_.push("L?([\"]({EscapeSequence}|{UniversalChar}|"
        "{any}{-}[\n\r\\\\\"])*[\"])", T_STRINGLIT);

    rules_.push("([a-zA-Z_$]|{UniversalChar})([a-zA-Z_0-9$]|"
        "{UniversalChar})*", T_IDENTIFIER);
    rules_.push("{Pound}{PPSpace}(include|include_next){PPSpace}"
        "<({any}{-}[\n\r>])+>", T_PP_HHEADER);
    rules_.push("{Pound}{PPSpace}(include|include_next){PPSpace}[\"]"
        "({any}{-}[\n\r\"])+[\"]", T_PP_QHEADER);
    rules_.push("{Pound}{PPSpace}(include|include_next){PPSpace}",    
        T_PP_INCLUDE);
    rules_.push("{Pound}{PPSpace}if", T_PP_IF);
    rules_.push("{Pound}{PPSpace}ifdef", T_PP_IFDEF);
    rules_.push("{Pound}{PPSpace}ifndef", T_PP_IFNDEF);
    rules_.push("{Pound}{PPSpace}else", T_PP_ELSE);
    rules_.push("{Pound}{PPSpace}elif", T_PP_ELIF);
    rules_.push("{Pound}{PPSpace}endif", T_PP_ENDIF);
    rules_.push("{Pound}{PPSpace}define", T_PP_DEFINE);
    rules_.push("{Pound}{PPSpace}undef", T_PP_UNDEF);
    rules_.push("{Pound}{PPSpace}line", T_PP_LINE);
    rules_.push("{Pound}{PPSpace}error", T_PP_ERROR);
    rules_.push("{Pound}{PPSpace}pragma", T_PP_PRAGMA);
    rules_.push("{Pound}{PPSpace}warning", T_PP_WARNING);

    rules_.push("{Pound}{PPSpace}region", T_MSEXT_PP_REGION);
    rules_.push("{Pound}{PPSpace}endregion", T_MSEXT_PP_ENDREGION);

    rules_.push("[\t\v\f]+", T_SPACE);
    rules_.push("{Newline}", T_NEWLINE);
}
