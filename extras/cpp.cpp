// Boost cpp rules.

#include "stdafx.h"

#include "cpp.h"
#include "lexertl/rules.hpp"

#define BOOST_WAVE_SUPPORT_MS_EXTENSIONS 1
#define BOOST_WAVE_SUPPORT_INCLUDE_NEXT 1
#define BOOST_WAVE_SUPPORT_IMPORT_KEYWORD 1
#define BOOST_WAVE_USE_STRICT_LEXER 1

void build_cpp (lexertl::rules &rules_)
{
    //#define OR                  "|"
    //#define Q(c)                "\\" c
    //#define TRI(c)              Q("?") Q("?") c
    rules_.insert_macro("TRI", "\\?\\?");
    // definition of some sub-token regexp's to simplify the regex definitions
    //#define BLANK               "[ \\t]"
    rules_.insert_macro("BLANK", "[ \\t]");
    //#define CCOMMENT
    //    Q("/") Q("*") "[^*]*" Q("*") "+" "(" "[^/*][^*]*" Q("*") "+" ")*" Q("/")
    rules_.insert_macro("CCOMMENT", "\\/\\*[^*]*\\*+([^/*][^*]*\\*+*\\/"));
    //#define PPSPACE             "(" BLANK OR CCOMMENT ")*"
    rules_.insert_macro("PPSPACE", "({BLANK}|{CCOMMENT}*"));
    //#define OCTALDIGIT          "[0-7]"
    rules_.insert_macro("OCTALDIGIT", "[0-7]");
    //#define DIGIT               "[0-9]"
    rules_.insert_macro("DIGIT", "[0-9]");
    //#define HEXDIGIT            "[0-9a-fA-F]"
    rules_.insert_macro("HEXDIGIT", "[0-9a-fA-F]");
    //#define OPTSIGN             "[-+]?"
    rules_.insert_macro("OPTSIGN", "[-+]?");
    //#define EXPSTART            "[eE]" "[-+]"
    rules_.insert_macro("EXPSTART", "[eE][-+]");
    //#define EXPONENT            "(" "[eE]" OPTSIGN "[0-9]+" ")"
    rules_.insert_macro("EXPONENT", "([eE]{OPTSIGN}[0-9]+"));
    //#define NONDIGIT            "[a-zA-Z_]"
    rules_.insert_macro("NONDIGIT", "[a-zA-Z_]");
    //#define INTEGER
    //    "(" "(0x|0X)" HEXDIGIT "+" OR "0" OCTALDIGIT "*" OR "[1-9]" DIGIT "*" ")"
    rules_.insert_macro("INTEGER", "((0x|0X{HEXDIGIT}+|0{OCTALDIGIT}*|[1-9]{DIGIT}*)"));
    //#define INTEGER_SUFFIX      "(" "[uU][lL]?|[lL][uU]?" ")"
    rules_.insert_macro("INTEGER_SUFFIX", "([uU][lL]?|[lL][uU]?"));
#if BOOST_WAVE_SUPPORT_MS_EXTENSIONS != 0
    //#define LONGINTEGER_SUFFIX  "(" "[uU]" "(" "[lL][lL]" ")" OR
    //                            "(" "[lL][lL]" ")" "[uU]" "?" OR
    //                            "i64"
    //                        ")"
    rules_.insert_macro("LONGINTEGER_SUFFIX", "([uU]([lL][lL]|([lL][lL])[uU]?|i64)"));
#else
    //#define LONGINTEGER_SUFFIX  "(" "[uU]" "(" "[lL][lL]" ")" OR
    //                        "(" "[lL][lL]" ")" "[uU]" "?" ")"
    rules_.insert_macro("LONGINTEGER_SUFFIX", "([uU]([lL][lL]|([lL][lL])[uU]?)"));
#endif

    //#define FLOAT_SUFFIX        "(" "[fF][lL]?|[lL][fF]?" ")"
    rules_.insert_macro("FLOAT_SUFFIX", "([fF][lL]?|[lL][fF]?"));
    //#define CHAR_SPEC           "L?"
    rules_.insert_macro("CHAR_SPEC", "L?");
    //#define BACKSLASH           "(" Q("\\") OR TRI(Q("/")) ")"
    rules_.insert_macro("BACKSLASH", "(\\\\|{TRI}\\/"));
    //#define ESCAPESEQ           BACKSLASH "("
    //                            "[abfnrtv?'\"]" OR
    //                            BACKSLASH OR
    //                            "x" HEXDIGIT "+" OR
    //                            OCTALDIGIT OCTALDIGIT "?" OCTALDIGIT "?"
    //                        ")"
    rules_.insert_macro("ESCAPESEQ", "{BACKSLASH}([abfnrtv?'\"]|"
        "{BACKSLASH}|x{HEXDIGIT}+|{OCTALDIGIT}{1,3}"));
    //#define HEXQUAD             HEXDIGIT HEXDIGIT HEXDIGIT HEXDIGIT
    rules_.insert_macro("HEXQUAD", "{HEXDIGIT}{4}");
    //#define UNIVERSALCHAR       BACKSLASH "("
    //                            "u" HEXQUAD OR
    //                            "U" HEXQUAD HEXQUAD
    //                        ")"
    rules_.insert_macro("UNIVERSALCHAR", "{BACKSLASH}(u{HEXQUAD}|U{HEXQUAD}{2}"));
    //#define POUNDDEF            "(" "#" OR TRI("=") OR Q("%:") ")"
    rules_.insert_macro("POUNDDEF", "(#|{TRI}=|\\%:"));
    //#define NEWLINEDEF          "(" "\\n" OR "\\r" OR "\\r\\n" ")"
    rules_.insert_macro("NEWLINEDEF", "(\\n|\\r|\\r\\n"));

#if BOOST_WAVE_SUPPORT_INCLUDE_NEXT != 0
    //#define INCLUDEDEF          "(include|include_next)"
    rules_.insert_macro("INCLUDEDEF", "(include|include_next"));
#else
    //#define INCLUDEDEF          "include"
    rules_.insert_macro("INCLUDEDEF", "include");
#endif

    //#define PP_NUMBERDEF        Q(".") "?" DIGIT "(" DIGIT OR NONDIGIT OR EXPSTART OR Q(".") ")*"
    rules_.insert_macro("PP_NUMBERDEF", "\\.?{DIGIT}({DIGIT}|{NONDIGIT}|{EXPSTART}|\\.*"));

    // C++ only token definitions
    //TOKEN_DATA(T_AND_ALT, "bitand"),
    rules_.push("bitand", T_AND_ALT);
    //TOKEN_DATA(T_ANDASSIGN_ALT, "and_eq"),
    rules_.push("and_eq", T_ANDASSIGN_ALT);
    //TOKEN_DATA(T_ANDAND_ALT, "and"),
    rules_.push("and", T_ANDAND_ALT);
    //TOKEN_DATA(T_OR_ALT, "bitor"),
    rules_.push("bitor", T_OR_ALT);
    //TOKEN_DATA(T_ORASSIGN_ALT, "or_eq"),
    rules_.push("or_eq", T_ORASSIGN_ALT);
    //TOKEN_DATA(T_OROR_ALT, "or"),
    rules_.push("or", T_OROR_ALT);
    //TOKEN_DATA(T_XORASSIGN_ALT, "xor_eq"),
    rules_.push("xor_eq", T_XORASSIGN_ALT);
    //TOKEN_DATA(T_XOR_ALT, "xor"),
    rules_.push("xor", T_XOR_ALT);
    //TOKEN_DATA(T_NOTEQUAL_ALT, "not_eq"),
    rules_.push("not_eq", T_NOTEQUAL_ALT);
    //TOKEN_DATA(T_NOT_ALT, "not"),
    rules_.push("not", T_NOT_ALT);
    //TOKEN_DATA(T_COMPL_ALT, "compl"),
    rules_.push("compl", T_COMPL_ALT);
#if BOOST_WAVE_SUPPORT_IMPORT_KEYWORD != 0
    //TOKEN_DATA(T_IMPORT, "import"),
    rules_.push("import", T_IMPORT);
#endif
    //TOKEN_DATA(T_ARROWSTAR, Q("->") Q("*")),
    rules_.push("\\->\\*", T_ARROWSTAR);
    //TOKEN_DATA(T_DOTSTAR, Q(".") Q("*")),
    rules_.push("\\.\\*", T_DOTSTAR);
    //TOKEN_DATA(T_COLON_COLON, "::"),
    rules_.push("::", T_COLON_COLON);

    // common C++/C99 token definitions
    //TOKEN_DATA(T_AND, "&"),
    rules_.push("&", T_AND),
    //TOKEN_DATA(T_ANDAND, "&&"),
    rules_.push("&&", T_ANDAND);
    //TOKEN_DATA(T_ASSIGN, "="),
    rules_.push("=", T_ASSIGN);
    //TOKEN_DATA(T_ANDASSIGN, "&="),
    rules_.push("&=", T_ANDASSIGN);
    //TOKEN_DATA(T_OR, Q("|")),
    rules_.push("\\|", T_OR);
    //TOKEN_DATA(T_OR_TRIGRAPH, TRI("!")),
    rules_.push("{TRI}!", T_OR_TRIGRAPH);
    //TOKEN_DATA(T_ORASSIGN, Q("|=")),
    rules_.push("\\|=", T_ORASSIGN);
    //TOKEN_DATA(T_ORASSIGN_TRIGRAPH, TRI("!=")),
    rules_.push("{TRI}!=", T_ORASSIGN_TRIGRAPH);
    //TOKEN_DATA(T_XOR, Q("^")),
    rules_.push("\\^", T_XOR);
    //TOKEN_DATA(T_XOR_TRIGRAPH, TRI("'")),
    rules_.push("{TRI}'", T_XOR_TRIGRAPH);
    //TOKEN_DATA(T_XORASSIGN, Q("^=")),
    rules_.push("\\^=", T_XORASSIGN);
    //TOKEN_DATA(T_XORASSIGN_TRIGRAPH, TRI("'=")),
    rules_.push("{TRI}'=", T_XORASSIGN_TRIGRAPH);
    //TOKEN_DATA(T_COMMA, ","),
    rules_.push(",", T_COMMA);
    //TOKEN_DATA(T_COLON, ":"),
    rules_.push(":", T_COLON);
    //TOKEN_DATA(T_DIVIDEASSIGN, Q("/=")),
    rules_.push("\\/=", T_DIVIDEASSIGN);
    //TOKEN_DATA(T_DIVIDE, Q("/")),
    rules_.push("\\/", T_DIVIDE);
    //TOKEN_DATA(T_DOT, Q(".")),
    rules_.push("\\.", T_DOT);
    //TOKEN_DATA(T_ELLIPSIS, Q(".") Q(".") Q(".")),
    rules_.push("\\.{3}", T_ELLIPSIS);
    //TOKEN_DATA(T_EQUAL, "=="),
    rules_.push("==", T_EQUAL);
    //TOKEN_DATA(T_GREATER, ">"),
    rules_.push(">", T_GREATER);
    //TOKEN_DATA(T_GREATEREQUAL, ">="),
    rules_.push(">=", T_GREATEREQUAL);
    //TOKEN_DATA(T_LEFTBRACE, Q("{")),
    rules_.push("\\{", T_LEFTBRACE);
    //TOKEN_DATA(T_LEFTBRACE_ALT, "<" Q("%")),
    rules_.push("<\\%", T_LEFTBRACE_ALT);
    //TOKEN_DATA(T_LEFTBRACE_TRIGRAPH, TRI("<")),
    rules_.push("{TRI}<", T_LEFTBRACE_TRIGRAPH);
    //TOKEN_DATA(T_LESS, "<"),
    rules_.push("<", T_LESS);
    //TOKEN_DATA(T_LESSEQUAL, "<="),
    rules_.push("<=", T_LESSEQUAL);
    //TOKEN_DATA(T_LEFTPAREN, Q("(")),
    rules_.push("\\(", T_LEFTPAREN);
    //TOKEN_DATA(T_LEFTBRACKET, Q("[")),
    rules_.push("\\[", T_LEFTBRACKET);
    //TOKEN_DATA(T_LEFTBRACKET_ALT, "<:"),
    rules_.push("<:", T_LEFTBRACKET_ALT);
    //TOKEN_DATA(T_LEFTBRACKET_TRIGRAPH, TRI(Q("("))),
    rules_.push("{TRI}\\(", T_LEFTBRACKET_TRIGRAPH);
    //TOKEN_DATA(T_MINUS, Q("-")),
    rules_.push("\\-", T_MINUS);
    //TOKEN_DATA(T_MINUSASSIGN, Q("-=")),
    rules_.push("\\-=", T_MINUSASSIGN);
    //TOKEN_DATA(T_MINUSMINUS, Q("-") Q("-")),
    rules_.push("\\-{2}", T_MINUSMINUS);
    //TOKEN_DATA(T_PERCENT, Q("%")),
    rules_.push("\\%", T_PERCENT);
    //TOKEN_DATA(T_PERCENTASSIGN, Q("%=")),
    rules_.push("\\%=", T_PERCENTASSIGN);
    //TOKEN_DATA(T_NOT, "!"),
    rules_.push("!", T_NOT);
    //TOKEN_DATA(T_NOTEQUAL, "!="),
    rules_.push("!=", T_NOTEQUAL);
    //TOKEN_DATA(T_OROR, Q("|") Q("|")),
    rules_.push("\\|{2}", T_OROR);
    //TOKEN_DATA(T_OROR_TRIGRAPH, TRI("!") Q("|") OR Q("|") TRI("!") OR TRI("!") TRI("!")),
    rules_.push("{TRI}!\\||\\|{TRI}!|({TRI}!{2}"), T_OROR_TRIGRAPH);
    //TOKEN_DATA(T_PLUS, Q("+")),
    rules_.push("\\+", T_PLUS);
    //TOKEN_DATA(T_PLUSASSIGN, Q("+=")),
    rules_.push("\\+=", T_PLUSASSIGN);
    //TOKEN_DATA(T_PLUSPLUS, Q("+") Q("+")),
    rules_.push("\\+\\+", T_PLUSPLUS);
    //TOKEN_DATA(T_ARROW, Q("->")),
    rules_.push("\\->", T_ARROW);
    //TOKEN_DATA(T_QUESTION_MARK, Q("?")),
    rules_.push("\\?", T_QUESTION_MARK);
    //TOKEN_DATA(T_RIGHTBRACE, Q("}")),
    rules_.push("\\}", T_RIGHTBRACE);
    //TOKEN_DATA(T_RIGHTBRACE_ALT, Q("%>")),
    rules_.push("\\%>", T_RIGHTBRACE_ALT);
    //TOKEN_DATA(T_RIGHTBRACE_TRIGRAPH, TRI(">")),
    rules_.push("{TRI}>", T_RIGHTBRACE_TRIGRAPH);
    //TOKEN_DATA(T_RIGHTPAREN, Q(")")),
    rules_.push("\\"), T_RIGHTPAREN);
    //TOKEN_DATA(T_RIGHTBRACKET, Q("]")),
    rules_.push("\\]", T_RIGHTBRACKET);
    //TOKEN_DATA(T_RIGHTBRACKET_ALT, ":>"),
    rules_.push(":>", T_RIGHTBRACKET_ALT);
    //TOKEN_DATA(T_RIGHTBRACKET_TRIGRAPH, TRI(Q(")"))),
    rules_.push("{TRI}\\"), T_RIGHTBRACKET_TRIGRAPH);
    //TOKEN_DATA(T_SEMICOLON, ";"),
    rules_.push(";", T_SEMICOLON);
    //TOKEN_DATA(T_SHIFTLEFT, "<<"),
    rules_.push("<<", T_SHIFTLEFT);
    //TOKEN_DATA(T_SHIFTLEFTASSIGN, "<<="),
    rules_.push("<<=", T_SHIFTLEFTASSIGN);
    //TOKEN_DATA(T_SHIFTRIGHT, ">>"),
    rules_.push(">>", T_SHIFTRIGHT);
    //TOKEN_DATA(T_SHIFTRIGHTASSIGN, ">>="),
    rules_.push(">>=", T_SHIFTRIGHTASSIGN);
    //TOKEN_DATA(T_STAR, Q("*")),
    rules_.push("\\*", T_STAR);
    //TOKEN_DATA(T_COMPL, Q("~")),
    rules_.push("\\~", T_COMPL);
    //TOKEN_DATA(T_COMPL_TRIGRAPH, TRI("-")),
    rules_.push("{TRI}-", T_COMPL_TRIGRAPH);
    //TOKEN_DATA(T_STARASSIGN, Q("*=")),
    rules_.push("\\*=", T_STARASSIGN);
    //TOKEN_DATA(T_ASM, "asm"),
    rules_.push("asm", T_ASM);
    //TOKEN_DATA(T_AUTO, "auto"),
    rules_.push("auto", T_AUTO);
    //TOKEN_DATA(T_BOOL, "bool"),
    rules_.push("bool", T_BOOL);
    //TOKEN_DATA(T_FALSE, "false"),
    rules_.push("false", T_FALSE);
    //TOKEN_DATA(T_TRUE, "true"),
    rules_.push("true", T_TRUE);
    //TOKEN_DATA(T_BREAK, "break"),
    rules_.push("break", T_BREAK);
    //TOKEN_DATA(T_CASE, "case"),
    rules_.push("case", T_CASE);
    //TOKEN_DATA(T_CATCH, "catch"),
    rules_.push("catch", T_CATCH);
    //TOKEN_DATA(T_CHAR, "char"),
    rules_.push("char", T_CHAR);
    //TOKEN_DATA(T_CLASS, "class"),
    rules_.push("class", T_CLASS);
    //TOKEN_DATA(T_CONST, "const"),
    rules_.push("const", T_CONST);
    //TOKEN_DATA(T_CONSTCAST, "const_cast"),
    rules_.push("const_cast", T_CONSTCAST);
    //TOKEN_DATA(T_CONTINUE, "continue"),
    rules_.push("continue", T_CONTINUE);
    //TOKEN_DATA(T_DEFAULT, "default"),
    rules_.push("default", T_DEFAULT);
    //TOKEN_DATA(T_DELETE, "delete"),
    rules_.push("delete", T_DELETE);
    //TOKEN_DATA(T_DO, "do"),
    rules_.push("do", T_DO);
    //TOKEN_DATA(T_DOUBLE, "double"),
    rules_.push("double", T_DOUBLE);
    //TOKEN_DATA(T_DYNAMICCAST, "dynamic_cast"),
    rules_.push("dynamic_cast", T_DYNAMICCAST);
    //TOKEN_DATA(T_ELSE, "else"),
    rules_.push("else", T_ELSE);
    //TOKEN_DATA(T_ENUM, "enum"),
    rules_.push("enum", T_ENUM);
    //TOKEN_DATA(T_EXPLICIT, "explicit"),
    rules_.push("explicit", T_EXPLICIT);
    //TOKEN_DATA(T_EXPORT, "export"),
    rules_.push("export", T_EXPORT);
    //TOKEN_DATA(T_EXTERN, "extern"),
    rules_.push("extern", T_EXTERN);
    //TOKEN_DATA(T_FLOAT, "float"),
    rules_.push("float", T_FLOAT);
    //TOKEN_DATA(T_FOR, "for"),
    rules_.push("for", T_FOR);
    //TOKEN_DATA(T_FRIEND, "friend"),
    rules_.push("friend", T_FRIEND);
    //TOKEN_DATA(T_GOTO, "goto"),
    rules_.push("goto", T_GOTO);
    //TOKEN_DATA(T_IF, "if"),
    rules_.push("if", T_IF);
    //TOKEN_DATA(T_INLINE, "inline"),
    rules_.push("inline", T_INLINE);
    //TOKEN_DATA(T_INT, "int"),
    rules_.push("int", T_INT);
    //TOKEN_DATA(T_LONG, "long"),
    rules_.push("long", T_LONG);
    //TOKEN_DATA(T_MUTABLE, "mutable"),
    rules_.push("mutable", T_MUTABLE);
    //TOKEN_DATA(T_NAMESPACE, "namespace"),
    rules_.push("namespace", T_NAMESPACE);
    //TOKEN_DATA(T_NEW, "new"),
    rules_.push("new", T_NEW);
    //TOKEN_DATA(T_OPERATOR, "operator"),
    rules_.push("operator", T_OPERATOR);
    //TOKEN_DATA(T_PRIVATE, "private"),
    rules_.push("private", T_PRIVATE);
    //TOKEN_DATA(T_PROTECTED, "protected"),
    rules_.push("protected", T_PROTECTED);
    //TOKEN_DATA(T_PUBLIC, "public"),
    rules_.push("public", T_PUBLIC);
    //TOKEN_DATA(T_REGISTER, "register"),
    rules_.push("register", T_REGISTER);
    //TOKEN_DATA(T_REINTERPRETCAST, "reinterpret_cast"),
    rules_.push("reinterpret_cast", T_REINTERPRETCAST);
    //TOKEN_DATA(T_RETURN, "return"),
    rules_.push("return", T_RETURN);
    //TOKEN_DATA(T_SHORT, "short"),
    rules_.push("short", T_SHORT);
    //TOKEN_DATA(T_SIGNED, "signed"),
    rules_.push("signed", T_SIGNED);
    //TOKEN_DATA(T_SIZEOF, "sizeof"),
    rules_.push("sizeof", T_SIZEOF);
    //TOKEN_DATA(T_STATIC, "static"),
    rules_.push("static", T_STATIC);
    //TOKEN_DATA(T_STATICCAST, "static_cast"),
    rules_.push("static_cast", T_STATICCAST);
    //TOKEN_DATA(T_STRUCT, "struct"),
    rules_.push("struct", T_STRUCT);
    //TOKEN_DATA(T_SWITCH, "switch"),
    rules_.push("switch", T_SWITCH);
    //TOKEN_DATA(T_TEMPLATE, "template"),
    rules_.push("template", T_TEMPLATE);
    //TOKEN_DATA(T_THIS, "this"),
    rules_.push("this", T_THIS);
    //TOKEN_DATA(T_THROW, "throw"),
    rules_.push("throw", T_THROW);
    //TOKEN_DATA(T_TRY, "try"),
    rules_.push("try", T_TRY);
    //TOKEN_DATA(T_TYPEDEF, "typedef"),
    rules_.push("typedef", T_TYPEDEF);
    //TOKEN_DATA(T_TYPEID, "typeid"),
    rules_.push("typeid", T_TYPEID);
    //TOKEN_DATA(T_TYPENAME, "typename"),
    rules_.push("typename", T_TYPENAME);
    //TOKEN_DATA(T_UNION, "union"),
    rules_.push("union", T_UNION);
    //TOKEN_DATA(T_UNSIGNED, "unsigned"),
    rules_.push("unsigned", T_UNSIGNED);
    //TOKEN_DATA(T_USING, "using"),
    rules_.push("using", T_USING);
    //TOKEN_DATA(T_VIRTUAL, "virtual"),
    rules_.push("virtual", T_VIRTUAL);
    //TOKEN_DATA(T_VOID, "void"),
    rules_.push("void", T_VOID);
    //TOKEN_DATA(T_VOLATILE, "volatile"),
    rules_.push("volatile", T_VOLATILE);
    //TOKEN_DATA(T_WCHART, "wchar_t"),
    rules_.push("wchar_t", T_WCHART);
    //TOKEN_DATA(T_WHILE, "while"),
    rules_.push("while", T_WHILE);
    //TOKEN_DATA(T_PP_DEFINE, POUNDDEF PPSPACE "define"),
    rules_.push("{POUNDDEF}{PPSPACE}define", T_PP_DEFINE);
    //TOKEN_DATA(T_PP_IF, POUNDDEF PPSPACE "if"),
    rules_.push("{POUNDDEF}{PPSPACE}if", T_PP_IF);
    //TOKEN_DATA(T_PP_IFDEF, POUNDDEF PPSPACE "ifdef"),
    rules_.push("{POUNDDEF}{PPSPACE}ifdef", T_PP_IFDEF);
    //TOKEN_DATA(T_PP_IFNDEF, POUNDDEF PPSPACE "ifndef"),
    rules_.push("{POUNDDEF}{PPSPACE}ifndef", T_PP_IFNDEF);
    //TOKEN_DATA(T_PP_ELSE, POUNDDEF PPSPACE "else"),
    rules_.push("{POUNDDEF}{PPSPACE}else", T_PP_ELSE);
    //TOKEN_DATA(T_PP_ELIF, POUNDDEF PPSPACE "elif"),
    rules_.push("{POUNDDEF}{PPSPACE}elif", T_PP_ELIF);
    //TOKEN_DATA(T_PP_ENDIF, POUNDDEF PPSPACE "endif"),
    rules_.push("{POUNDDEF}{PPSPACE}endif", T_PP_ENDIF);
    //TOKEN_DATA(T_PP_ERROR, POUNDDEF PPSPACE "error"),
    rules_.push("{POUNDDEF}{PPSPACE}error", T_PP_ERROR);
    //TOKEN_DATA(T_PP_QHEADER, POUNDDEF PPSPACE
    //    INCLUDEDEF PPSPACE Q("\"") "[^\\n\\r\"]+" Q("\"")),
    rules_.push("{POUNDDEF}{PPSPACE}{INCLUDEDEF}{PPSPACE}"
        "\\\"[^\\n\\r\"]+\\\"", T_PP_QHEADER);
    //TOKEN_DATA(T_PP_HHEADER, POUNDDEF PPSPACE
    //    INCLUDEDEF PPSPACE "<" "[^\\n\\r>]+" ">"),
    rules_.push("{POUNDDEF}{PPSPACE}{INCLUDEDEF}{PPSPACE}<[^\\n\\r>]+>",
        T_PP_HHEADER);
    //TOKEN_DATA(T_PP_INCLUDE, POUNDDEF PPSPACE
    //    INCLUDEDEF PPSPACE),
    rules_.push("{POUNDDEF}{PPSPACE}{INCLUDEDEF}{PPSPACE}", T_PP_INCLUDE);
    //TOKEN_DATA(T_PP_LINE, POUNDDEF PPSPACE "line"),
    rules_.push("{POUNDDEF}{PPSPACE}line", T_PP_LINE);
    //TOKEN_DATA(T_PP_PRAGMA, POUNDDEF PPSPACE "pragma"),
    rules_.push("{POUNDDEF}{PPSPACE}pragma", T_PP_PRAGMA);
    //TOKEN_DATA(T_PP_UNDEF, POUNDDEF PPSPACE "undef"),
    rules_.push("{POUNDDEF}{PPSPACE}undef", T_PP_UNDEF);
    //TOKEN_DATA(T_PP_WARNING, POUNDDEF PPSPACE "warning"),
    rules_.push("{POUNDDEF}{PPSPACE}warning", T_PP_WARNING);
#if BOOST_WAVE_SUPPORT_MS_EXTENSIONS != 0
    //TOKEN_DATA(T_MSEXT_INT8, "__int8"),
    rules_.push("__int8", T_MSEXT_INT8);
    //TOKEN_DATA(T_MSEXT_INT16, "__int16"),
    rules_.push("__int16", T_MSEXT_INT16);
    //TOKEN_DATA(T_MSEXT_INT32, "__int32"),
    rules_.push("__int32", T_MSEXT_INT32);
    //TOKEN_DATA(T_MSEXT_INT64, "__int64"),
    rules_.push("__int64", T_MSEXT_INT64);
    //TOKEN_DATA(T_MSEXT_BASED, "_?" "_based"),
    rules_.push("_?_based", T_MSEXT_BASED);
    //TOKEN_DATA(T_MSEXT_DECLSPEC, "_?" "_declspec"),
    rules_.push("_?_declspec", T_MSEXT_DECLSPEC);
    //TOKEN_DATA(T_MSEXT_CDECL, "_?" "_cdecl"),
    rules_.push("_?_cdecl", T_MSEXT_CDECL);
    //TOKEN_DATA(T_MSEXT_FASTCALL, "_?" "_fastcall"),
    rules_.push("_?_fastcall", T_MSEXT_FASTCALL);
    //TOKEN_DATA(T_MSEXT_STDCALL, "_?" "_stdcall"),
    rules_.push("_?_stdcall", T_MSEXT_STDCALL);
    //TOKEN_DATA(T_MSEXT_TRY , "__try"),
    rules_.push("__try", T_MSEXT_TRY),
    //TOKEN_DATA(T_MSEXT_EXCEPT, "__except"),
    rules_.push("__except", T_MSEXT_EXCEPT);
    //TOKEN_DATA(T_MSEXT_FINALLY, "__finally"),
    rules_.push("__finally", T_MSEXT_FINALLY);
    //TOKEN_DATA(T_MSEXT_LEAVE, "__leave"),
    rules_.push("__leave", T_MSEXT_LEAVE);
    //TOKEN_DATA(T_MSEXT_INLINE, "_?" "_inline"),
    rules_.push("_?_inline", T_MSEXT_INLINE);
    //TOKEN_DATA(T_MSEXT_ASM, "_?" "_asm"),
    rules_.push("_?_asm", T_MSEXT_ASM);
    //TOKEN_DATA(T_MSEXT_PP_REGION, POUNDDEF PPSPACE "region"),
    rules_.push("{POUNDDEF}{PPSPACE}region", T_MSEXT_PP_REGION);
    //TOKEN_DATA(T_MSEXT_PP_ENDREGION, POUNDDEF PPSPACE "endregion"),
    rules_.push("{POUNDDEF}{PPSPACE}endregion", T_MSEXT_PP_ENDREGION);
#endif // BOOST_WAVE_SUPPORT_MS_EXTENSIONS != 0
    //TOKEN_DATA(T_LONGINTLIT, INTEGER LONGINTEGER_SUFFIX),
    rules_.push("{INTEGER}{LONGINTEGER_SUFFIX}", T_LONGINTLIT);
    //TOKEN_DATA(T_INTLIT, INTEGER INTEGER_SUFFIX "?"),
    rules_.push("{INTEGER}{INTEGER_SUFFIX}?", T_INTLIT);
    //TOKEN_DATA(T_FLOATLIT,
    //    "(" DIGIT "*" Q(".") DIGIT "+" OR DIGIT "+" Q(".") ")"
    //    EXPONENT "?" FLOAT_SUFFIX "?" OR
    //    DIGIT "+" EXPONENT FLOAT_SUFFIX "?"),
    rules_.push("({DIGIT}*\\.{DIGIT}+|{DIGIT}+\\.{EXPONENT}?{FLOAT_SUFFIX}?")
        "|{DIGIT}+{EXPONENT}{FLOAT_SUFFIX}?",T_FLOATLIT);
#if BOOST_WAVE_USE_STRICT_LEXER != 0
    //TOKEN_DATA(T_IDENTIFIER, "([a-zA-Z_]" OR UNIVERSALCHAR ")([a-zA-Z0-9_]" OR UNIVERSALCHAR ")*"),
    rules_.push("([a-zA-Z_]|{UNIVERSALCHAR}([a-zA-Z0-9_]|{UNIVERSALCHAR})*"), T_IDENTIFIER);
#else
    //TOKEN_DATA(T_IDENTIFIER, "([a-zA-Z_$]" OR UNIVERSALCHAR ")([a-zA-Z0-9_$]" OR UNIVERSALCHAR ")*"),
    rules_.push("([a-zA-Z_$]|{UNIVERSALCHAR}([a-zA-Z0-9_$]|{UNIVERSALCHAR})*"), T_IDENTIFIER);
#endif
    //TOKEN_DATA(T_CCOMMENT, CCOMMENT),
    rules_.push("{CCOMMENT}", T_CCOMMENT);
    //TOKEN_DATA(T_CPPCOMMENT, Q("/") Q("/[^\\n\\r]*") NEWLINEDEF ),
    rules_.push("\\/\\/[^\\n\\r]*{NEWLINEDEF}", T_CPPCOMMENT);
    //TOKEN_DATA(T_CHARLIT, CHAR_SPEC "'"
    //            "(" ESCAPESEQ OR "[^\\n\\r']" OR UNIVERSALCHAR ")+" "'"),
    rules_.push("{CHAR_SPEC}'({ESCAPESEQ}|[^\\n\\r']|{UNIVERSALCHAR}+'"),
        T_CHARLIT);
    //TOKEN_DATA(T_STRINGLIT, CHAR_SPEC Q("\"")
    //            "(" ESCAPESEQ OR "[^\\n\\r\"]" OR UNIVERSALCHAR ")*" Q("\"")),
    rules_.push("{CHAR_SPEC}\\\"({ESCAPESEQ}|[^\\n\\r\"]|"
        "{UNIVERSALCHAR}*\\\""), T_STRINGLIT);
    //TOKEN_DATA(T_SPACE, BLANK "+"),
    rules_.push("{BLANK}+", T_SPACE);
    //TOKEN_DATA(T_SPACE2, "[\\v\\f]+"),
    rules_.push("[\\v\\f]+", T_SPACE2);
    //TOKEN_DATA(T_CONTLINE, Q("\\") "\\n"),
    rules_.push("\\\\\\n", T_CONTLINE);
    //TOKEN_DATA(T_NEWLINE, NEWLINEDEF),
    rules_.push("{NEWLINEDEF}", T_NEWLINE);
    //TOKEN_DATA(T_POUND_POUND, "##"),
    rules_.push("##", T_POUND_POUND);
    //TOKEN_DATA(T_POUND_POUND_ALT, Q("%:") Q("%:")),
    rules_.push("\\%:\\%:", T_POUND_POUND_ALT);
    //TOKEN_DATA(T_POUND_POUND_TRIGRAPH, TRI("=") TRI("=")),
    rules_.push("({TRI}={2}"), T_POUND_POUND_TRIGRAPH);
    //TOKEN_DATA(T_POUND, "#"),
    rules_.push("#", T_POUND);
    //TOKEN_DATA(T_POUND_ALT, Q("%:")),
    rules_.push("\\%:", T_POUND_ALT);
    //TOKEN_DATA(T_POUND_TRIGRAPH, TRI("=")),
    rules_.push("{TRI}=", T_POUND_TRIGRAPH);
    //TOKEN_DATA(T_ANY_TRIGRAPH, TRI(Q("/"))),
    rules_.push("{TRI}\\/", T_ANY_TRIGRAPH);
    //TOKEN_DATA(T_ANY, "."), // this should be the last recognized token
    rules_.push(".", T_ANY); // this should be the last recognized token
}

/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    http://www.boost.org/

    Copyright (c) 2001-2006 Hartmut Kaiser. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
/*
#if !defined(BOOST_WAVE_LEXERTL_LEXER_HPP_INCLUDED)
#define BOOST_WAVE_LEXERTL_LEXER_HPP_INCLUDED

#include <boost/iterator/iterator_traits.hpp>

#include <boost/wave/wave_config.hpp>
#include <boost/wave/language_support.hpp>
#include <boost/wave/token_ids.hpp>

#include <boost/wave/cpplexer/validate_universal_char.hpp>
#include <boost/wave/cpplexer/convert_trigraphs.hpp>
#include <boost/wave/cpplexer/cpplexer_exceptions.hpp>

#include "../lexertl_iterator.hpp"

#include "lexertl/generator.hpp"
#include "lexertl/rules.hpp"
#include "lexertl/state_machine.hpp"
#include "lexertl/consts.h"

///////////////////////////////////////////////////////////////////////////////
namespace boost { namespace wave { namespace cpplexer { namespace lexertl
{

///////////////////////////////////////////////////////////////////////////////
//  The following numbers are the array sizes of the token regex's which we
//  need to specify to make the CW compiler happy (at least up to V9.5).
#if BOOST_WAVE_SUPPORT_MS_EXTENSIONS != 0
#define INIT_DATA_SIZE              176
#else
#define INIT_DATA_SIZE              159
#endif
#define INIT_DATA_CPP_SIZE          15
#define INIT_DATA_PP_NUMBER_SIZE    2

///////////////////////////////////////////////////////////////////////////////
namespace lexer
{

///////////////////////////////////////////////////////////////////////////////
//  this is the wrapper for the lexertl lexer library
template <typename Iterator, typename Position>
class lexertl
{
private:
    typedef BOOST_WAVE_STRINGTYPE string_type;
    typedef typename boost::detail::iterator_traits<Iterator>::value_type
        char_type;

public:
    lexertl() : has_compiled_dfa(false) {}

    wave::token_id next_token(Iterator &first, Iterator const &last,
        string_type& token_value);

    void init_dfa(wave::language_support lang);

private:
    ::lexertl::rules rules_;
    ::lexertl::state_machine state_machine_;
    bool has_compiled_dfa;

// initialization data (regular expressions for the token definitions)
    struct lexer_data {
        token_id tokenid;               // token data
        char_type const *tokenregex;    // associated token to match
    };

    static lexer_data const init_data[INIT_DATA_SIZE];          // common patterns
    static lexer_data const init_data_cpp[INIT_DATA_CPP_SIZE];  // C++ only patterns
    static lexer_data const init_data_pp_number[INIT_DATA_PP_NUMBER_SIZE];  // pp-number only patterns
};

///////////////////////////////////////////////////////////////////////////////
// token regex definitions

#define OR                  "|"
#define Q(c)                "\\" c
#define TRI(c)              Q("?") Q("?") c

// definition of some sub-token regexp's to simplify the regex definitions
#define BLANK               "[ \\t]"
#define CCOMMENT            \
    Q("/") Q("*") "[^*]*" Q("*") "+" "(" "[^/*][^*]*" Q("*") "+" ")*" Q("/")

#define PPSPACE             "(" BLANK OR CCOMMENT ")*"

#define OCTALDIGIT          "[0-7]"
#define DIGIT               "[0-9]"
#define HEXDIGIT            "[0-9a-fA-F]"
#define OPTSIGN             "[-+]?"
#define EXPSTART            "[eE]" "[-+]"
#define EXPONENT            "(" "[eE]" OPTSIGN "[0-9]+" ")"
#define NONDIGIT            "[a-zA-Z_]"

#define INTEGER             \
    "(" "(0x|0X)" HEXDIGIT "+" OR "0" OCTALDIGIT "*" OR "[1-9]" DIGIT "*" ")"

#define INTEGER_SUFFIX      "(" "[uU][lL]?|[lL][uU]?" ")"
#if BOOST_WAVE_SUPPORT_MS_EXTENSIONS != 0
#define LONGINTEGER_SUFFIX  "(" "[uU]" "(" "[lL][lL]" ")" OR \
                                "(" "[lL][lL]" ")" "[uU]" "?" OR \
                                "i64" \
                            ")"
#else
#define LONGINTEGER_SUFFIX  "(" "[uU]" "(" "[lL][lL]" ")" OR \
                            "(" "[lL][lL]" ")" "[uU]" "?" ")"
#endif
#define FLOAT_SUFFIX        "(" "[fF][lL]?|[lL][fF]?" ")"
#define CHAR_SPEC           "L?"

#define BACKSLASH           "(" Q("\\") OR TRI(Q("/")) ")"
#define ESCAPESEQ           BACKSLASH "(" \
                                "[abfnrtv?'\"]" OR \
                                BACKSLASH OR \
                                "x" HEXDIGIT "+" OR \
                                OCTALDIGIT OCTALDIGIT "?" OCTALDIGIT "?" \
                            ")"
#define HEXQUAD             HEXDIGIT HEXDIGIT HEXDIGIT HEXDIGIT
#define UNIVERSALCHAR       BACKSLASH "(" \
                                "u" HEXQUAD OR \
                                "U" HEXQUAD HEXQUAD \
                            ")"

#define POUNDDEF            "(" "#" OR TRI("=") OR Q("%:") ")"
#define NEWLINEDEF          "(" "\\n" OR "\\r" OR "\\r\\n" ")"

#if BOOST_WAVE_SUPPORT_INCLUDE_NEXT != 0
#define INCLUDEDEF          "(include|include_next)"
#else
#define INCLUDEDEF          "include"
#endif

#define PP_NUMBERDEF        Q(".") "?" DIGIT "(" DIGIT OR NONDIGIT OR EXPSTART OR Q(".") ")*"

//  helper for initializing token data
#define //TOKEN_DATA(id, regex) { id, regex }

// common C++/C99 token definitions
template <typename Iterator, typename Position>
typename lexertl<Iterator, Position>::lexer_data const
lexertl<Iterator, Position>::init_data[INIT_DATA_SIZE] =
{
    TOKEN_DATA(T_AND, "&"),
    TOKEN_DATA(T_ANDAND, "&&"),
    TOKEN_DATA(T_ASSIGN, "="),
    TOKEN_DATA(T_ANDASSIGN, "&="),
    TOKEN_DATA(T_OR, Q("|")),
    TOKEN_DATA(T_OR_TRIGRAPH, TRI("!")),
    TOKEN_DATA(T_ORASSIGN, Q("|=")),
    TOKEN_DATA(T_ORASSIGN_TRIGRAPH, TRI("!=")),
    TOKEN_DATA(T_XOR, Q("^")),
    TOKEN_DATA(T_XOR_TRIGRAPH, TRI("'")),
    TOKEN_DATA(T_XORASSIGN, Q("^=")),
    TOKEN_DATA(T_XORASSIGN_TRIGRAPH, TRI("'=")),
    TOKEN_DATA(T_COMMA, ","),
    TOKEN_DATA(T_COLON, ":"),
    TOKEN_DATA(T_DIVIDEASSIGN, Q("/=")),
    TOKEN_DATA(T_DIVIDE, Q("/")),
    TOKEN_DATA(T_DOT, Q(".")),
    TOKEN_DATA(T_ELLIPSIS, Q(".") Q(".") Q(".")),
    TOKEN_DATA(T_EQUAL, "=="),
    TOKEN_DATA(T_GREATER, ">"),
    TOKEN_DATA(T_GREATEREQUAL, ">="),
    TOKEN_DATA(T_LEFTBRACE, Q("{")),
    TOKEN_DATA(T_LEFTBRACE_ALT, "<" Q("%")),
    TOKEN_DATA(T_LEFTBRACE_TRIGRAPH, TRI("<")),
    TOKEN_DATA(T_LESS, "<"),
    TOKEN_DATA(T_LESSEQUAL, "<="),
    TOKEN_DATA(T_LEFTPAREN, Q("(")),
    TOKEN_DATA(T_LEFTBRACKET, Q("[")),
    TOKEN_DATA(T_LEFTBRACKET_ALT, "<:"),
    TOKEN_DATA(T_LEFTBRACKET_TRIGRAPH, TRI(Q("("))),
    TOKEN_DATA(T_MINUS, Q("-")),
    TOKEN_DATA(T_MINUSASSIGN, Q("-=")),
    TOKEN_DATA(T_MINUSMINUS, Q("-") Q("-")),
    TOKEN_DATA(T_PERCENT, Q("%")),
    TOKEN_DATA(T_PERCENTASSIGN, Q("%=")),
    TOKEN_DATA(T_NOT, "!"),
    TOKEN_DATA(T_NOTEQUAL, "!="),
    TOKEN_DATA(T_OROR, Q("|") Q("|")),
    TOKEN_DATA(T_OROR_TRIGRAPH, TRI("!") Q("|") OR Q("|") TRI("!") OR TRI("!") TRI("!")),
    TOKEN_DATA(T_PLUS, Q("+")),
    TOKEN_DATA(T_PLUSASSIGN, Q("+=")),
    TOKEN_DATA(T_PLUSPLUS, Q("+") Q("+")),
    TOKEN_DATA(T_ARROW, Q("->")),
    TOKEN_DATA(T_QUESTION_MARK, Q("?")),
    TOKEN_DATA(T_RIGHTBRACE, Q("}")),
    TOKEN_DATA(T_RIGHTBRACE_ALT, Q("%>")),
    TOKEN_DATA(T_RIGHTBRACE_TRIGRAPH, TRI(">")),
    TOKEN_DATA(T_RIGHTPAREN, Q(")")),
    TOKEN_DATA(T_RIGHTBRACKET, Q("]")),
    TOKEN_DATA(T_RIGHTBRACKET_ALT, ":>"),
    TOKEN_DATA(T_RIGHTBRACKET_TRIGRAPH, TRI(Q(")"))),
    TOKEN_DATA(T_SEMICOLON, ";"),
    TOKEN_DATA(T_SHIFTLEFT, "<<"),
    TOKEN_DATA(T_SHIFTLEFTASSIGN, "<<="),
    TOKEN_DATA(T_SHIFTRIGHT, ">>"),
    TOKEN_DATA(T_SHIFTRIGHTASSIGN, ">>="),
    TOKEN_DATA(T_STAR, Q("*")),
    TOKEN_DATA(T_COMPL, Q("~")),
    TOKEN_DATA(T_COMPL_TRIGRAPH, TRI("-")),
    TOKEN_DATA(T_STARASSIGN, Q("*=")),
    TOKEN_DATA(T_ASM, "asm"),
    TOKEN_DATA(T_AUTO, "auto"),
    TOKEN_DATA(T_BOOL, "bool"),
    TOKEN_DATA(T_FALSE, "false"),
    TOKEN_DATA(T_TRUE, "true"),
    TOKEN_DATA(T_BREAK, "break"),
    TOKEN_DATA(T_CASE, "case"),
    TOKEN_DATA(T_CATCH, "catch"),
    TOKEN_DATA(T_CHAR, "char"),
    TOKEN_DATA(T_CLASS, "class"),
    TOKEN_DATA(T_CONST, "const"),
    TOKEN_DATA(T_CONSTCAST, "const_cast"),
    TOKEN_DATA(T_CONTINUE, "continue"),
    TOKEN_DATA(T_DEFAULT, "default"),
    TOKEN_DATA(T_DELETE, "delete"),
    TOKEN_DATA(T_DO, "do"),
    TOKEN_DATA(T_DOUBLE, "double"),
    TOKEN_DATA(T_DYNAMICCAST, "dynamic_cast"),
    TOKEN_DATA(T_ELSE, "else"),
    TOKEN_DATA(T_ENUM, "enum"),
    TOKEN_DATA(T_EXPLICIT, "explicit"),
    TOKEN_DATA(T_EXPORT, "export"),
    TOKEN_DATA(T_EXTERN, "extern"),
    TOKEN_DATA(T_FLOAT, "float"),
    TOKEN_DATA(T_FOR, "for"),
    TOKEN_DATA(T_FRIEND, "friend"),
    TOKEN_DATA(T_GOTO, "goto"),
    TOKEN_DATA(T_IF, "if"),
    TOKEN_DATA(T_INLINE, "inline"),
    TOKEN_DATA(T_INT, "int"),
    TOKEN_DATA(T_LONG, "long"),
    TOKEN_DATA(T_MUTABLE, "mutable"),
    TOKEN_DATA(T_NAMESPACE, "namespace"),
    TOKEN_DATA(T_NEW, "new"),
    TOKEN_DATA(T_OPERATOR, "operator"),
    TOKEN_DATA(T_PRIVATE, "private"),
    TOKEN_DATA(T_PROTECTED, "protected"),
    TOKEN_DATA(T_PUBLIC, "public"),
    TOKEN_DATA(T_REGISTER, "register"),
    TOKEN_DATA(T_REINTERPRETCAST, "reinterpret_cast"),
    TOKEN_DATA(T_RETURN, "return"),
    TOKEN_DATA(T_SHORT, "short"),
    TOKEN_DATA(T_SIGNED, "signed"),
    TOKEN_DATA(T_SIZEOF, "sizeof"),
    TOKEN_DATA(T_STATIC, "static"),
    TOKEN_DATA(T_STATICCAST, "static_cast"),
    TOKEN_DATA(T_STRUCT, "struct"),
    TOKEN_DATA(T_SWITCH, "switch"),
    TOKEN_DATA(T_TEMPLATE, "template"),
    TOKEN_DATA(T_THIS, "this"),
    TOKEN_DATA(T_THROW, "throw"),
    TOKEN_DATA(T_TRY, "try"),
    TOKEN_DATA(T_TYPEDEF, "typedef"),
    TOKEN_DATA(T_TYPEID, "typeid"),
    TOKEN_DATA(T_TYPENAME, "typename"),
    TOKEN_DATA(T_UNION, "union"),
    TOKEN_DATA(T_UNSIGNED, "unsigned"),
    TOKEN_DATA(T_USING, "using"),
    TOKEN_DATA(T_VIRTUAL, "virtual"),
    TOKEN_DATA(T_VOID, "void"),
    TOKEN_DATA(T_VOLATILE, "volatile"),
    TOKEN_DATA(T_WCHART, "wchar_t"),
    TOKEN_DATA(T_WHILE, "while"),
    TOKEN_DATA(T_PP_DEFINE, POUNDDEF PPSPACE "define"),
    TOKEN_DATA(T_PP_IF, POUNDDEF PPSPACE "if"),
    TOKEN_DATA(T_PP_IFDEF, POUNDDEF PPSPACE "ifdef"),
    TOKEN_DATA(T_PP_IFNDEF, POUNDDEF PPSPACE "ifndef"),
    TOKEN_DATA(T_PP_ELSE, POUNDDEF PPSPACE "else"),
    TOKEN_DATA(T_PP_ELIF, POUNDDEF PPSPACE "elif"),
    TOKEN_DATA(T_PP_ENDIF, POUNDDEF PPSPACE "endif"),
    TOKEN_DATA(T_PP_ERROR, POUNDDEF PPSPACE "error"),
    TOKEN_DATA(T_PP_QHEADER, POUNDDEF PPSPACE \
        INCLUDEDEF PPSPACE Q("\"") "[^\\n\\r\"]+" Q("\"")),
    TOKEN_DATA(T_PP_HHEADER, POUNDDEF PPSPACE \
        INCLUDEDEF PPSPACE "<" "[^\\n\\r>]+" ">"),
    TOKEN_DATA(T_PP_INCLUDE, POUNDDEF PPSPACE \
        INCLUDEDEF PPSPACE),
    TOKEN_DATA(T_PP_LINE, POUNDDEF PPSPACE "line"),
    TOKEN_DATA(T_PP_PRAGMA, POUNDDEF PPSPACE "pragma"),
    TOKEN_DATA(T_PP_UNDEF, POUNDDEF PPSPACE "undef"),
    TOKEN_DATA(T_PP_WARNING, POUNDDEF PPSPACE "warning"),
#if BOOST_WAVE_SUPPORT_MS_EXTENSIONS != 0
    TOKEN_DATA(T_MSEXT_INT8, "__int8"),
    TOKEN_DATA(T_MSEXT_INT16, "__int16"),
    TOKEN_DATA(T_MSEXT_INT32, "__int32"),
    TOKEN_DATA(T_MSEXT_INT64, "__int64"),
    TOKEN_DATA(T_MSEXT_BASED, "_?" "_based"),
    TOKEN_DATA(T_MSEXT_DECLSPEC, "_?" "_declspec"),
    TOKEN_DATA(T_MSEXT_CDECL, "_?" "_cdecl"),
    TOKEN_DATA(T_MSEXT_FASTCALL, "_?" "_fastcall"),
    TOKEN_DATA(T_MSEXT_STDCALL, "_?" "_stdcall"),
    TOKEN_DATA(T_MSEXT_TRY , "__try"),
    TOKEN_DATA(T_MSEXT_EXCEPT, "__except"),
    TOKEN_DATA(T_MSEXT_FINALLY, "__finally"),
    TOKEN_DATA(T_MSEXT_LEAVE, "__leave"),
    TOKEN_DATA(T_MSEXT_INLINE, "_?" "_inline"),
    TOKEN_DATA(T_MSEXT_ASM, "_?" "_asm"),
    TOKEN_DATA(T_MSEXT_PP_REGION, POUNDDEF PPSPACE "region"),
    TOKEN_DATA(T_MSEXT_PP_ENDREGION, POUNDDEF PPSPACE "endregion"),
#endif // BOOST_WAVE_SUPPORT_MS_EXTENSIONS != 0
    TOKEN_DATA(T_LONGINTLIT, INTEGER LONGINTEGER_SUFFIX),
    TOKEN_DATA(T_INTLIT, INTEGER INTEGER_SUFFIX "?"),
    TOKEN_DATA(T_FLOATLIT,
        "(" DIGIT "*" Q(".") DIGIT "+" OR DIGIT "+" Q(".") ")"
        EXPONENT "?" FLOAT_SUFFIX "?" OR
        DIGIT "+" EXPONENT FLOAT_SUFFIX "?"),
#if BOOST_WAVE_USE_STRICT_LEXER != 0
    TOKEN_DATA(T_IDENTIFIER, "([a-zA-Z_]" OR UNIVERSALCHAR ")([a-zA-Z0-9_]" OR UNIVERSALCHAR ")*"),
#else
    TOKEN_DATA(T_IDENTIFIER, "([a-zA-Z_$]" OR UNIVERSALCHAR ")([a-zA-Z0-9_$]" OR UNIVERSALCHAR ")*"),
#endif
    TOKEN_DATA(T_CCOMMENT, CCOMMENT),
    TOKEN_DATA(T_CPPCOMMENT, Q("/") Q("/[^\\n\\r]*") NEWLINEDEF ),
    TOKEN_DATA(T_CHARLIT, CHAR_SPEC "'"
                "(" ESCAPESEQ OR "[^\\n\\r']" OR UNIVERSALCHAR ")+" "'"),
    TOKEN_DATA(T_STRINGLIT, CHAR_SPEC Q("\"")
                "(" ESCAPESEQ OR "[^\\n\\r\"]" OR UNIVERSALCHAR ")*" Q("\"")),
    TOKEN_DATA(T_SPACE, BLANK "+"),
    TOKEN_DATA(T_SPACE2, "[\\v\\f]+"),
    TOKEN_DATA(T_CONTLINE, Q("\\") "\\n"),
    TOKEN_DATA(T_NEWLINE, NEWLINEDEF),
    TOKEN_DATA(T_POUND_POUND, "##"),
    TOKEN_DATA(T_POUND_POUND_ALT, Q("%:") Q("%:")),
    TOKEN_DATA(T_POUND_POUND_TRIGRAPH, TRI("=") TRI("=")),
    TOKEN_DATA(T_POUND, "#"),
    TOKEN_DATA(T_POUND_ALT, Q("%:")),
    TOKEN_DATA(T_POUND_TRIGRAPH, TRI("=")),
    TOKEN_DATA(T_ANY_TRIGRAPH, TRI(Q("/"))),
    TOKEN_DATA(T_ANY, "."),     // this should be the last recognized token
    { token_id(0) }       // this should be the last entry
};

// C++ only token definitions
template <typename Iterator, typename Position>
typename lexertl<Iterator, Position>::lexer_data const
lexertl<Iterator, Position>::init_data_cpp[INIT_DATA_CPP_SIZE] =
{
    TOKEN_DATA(T_AND_ALT, "bitand"),
    TOKEN_DATA(T_ANDASSIGN_ALT, "and_eq"),
    TOKEN_DATA(T_ANDAND_ALT, "and"),
    TOKEN_DATA(T_OR_ALT, "bitor"),
    TOKEN_DATA(T_ORASSIGN_ALT, "or_eq"),
    TOKEN_DATA(T_OROR_ALT, "or"),
    TOKEN_DATA(T_XORASSIGN_ALT, "xor_eq"),
    TOKEN_DATA(T_XOR_ALT, "xor"),
    TOKEN_DATA(T_NOTEQUAL_ALT, "not_eq"),
    TOKEN_DATA(T_NOT_ALT, "not"),
    TOKEN_DATA(T_COMPL_ALT, "compl"),
#if BOOST_WAVE_SUPPORT_IMPORT_KEYWORD != 0
    TOKEN_DATA(T_IMPORT, "import"),
#endif
    TOKEN_DATA(T_ARROWSTAR, Q("->") Q("*")),
    TOKEN_DATA(T_DOTSTAR, Q(".") Q("*")),
    TOKEN_DATA(T_COLON_COLON, "::"),
    { token_id(0) }       // this should be the last entry
};

// pp-number specific token definitions
template <typename Iterator, typename Position>
typename lexertl<Iterator, Position>::lexer_data const
lexertl<Iterator, Position>::init_data_pp_number[INIT_DATA_PP_NUMBER_SIZE] =
{
    TOKEN_DATA(T_PP_NUMBER, PP_NUMBERDEF),
    { token_id(0) }       // this should be the last entry
};

#undef TOKEN_DATA

///////////////////////////////////////////////////////////////////////////////
// initialize lexertl lexer from C++ token regex's
template <typename Iterator, typename Position>
inline void
lexertl<Iterator, Position>::init_dfa(wave::language_support lang)
{
    if (has_compiled_dfa)
        return;

// if pp-numbers should be preferred, insert the corresponding rule first
    if (wave::need_prefer_pp_numbers(lang)) {
        for (int j = 0; 0 != init_data_pp_number[j].tokenid; ++j) {
            rules_.push(init_data_pp_number[j].tokenregex,
                init_data_pp_number[j].tokenid);
        }
    }

// if in C99 mode, some of the keywords are not valid
    if (!wave::need_c99(lang)) {
        for (int j = 0; 0 != init_data_cpp[j].tokenid; ++j) {
            rules_.push(init_data_cpp[j].tokenregex,
                init_data_cpp[j].tokenid);
        }
    }

    for (int i = 0; 0 != init_data[i].tokenid; ++i) {
        rules_.push(init_data[i].tokenregex, init_data[i].tokenid);
    }

// generate minimized DFA
    ::lexertl::generator::build (rules_, state_machine_);
    ::lexertl::generator::minimise_dfa (state_machine_._dfa_alphabet,
        state_machine_._dfa);

    has_compiled_dfa = true;
}

///////////////////////////////////////////////////////////////////////////////
// return next token from the input stream
template <typename Iterator, typename Position>
inline wave::token_id
lexertl<Iterator, Position>::next_token(Iterator &first, Iterator const &last,
    string_type& token_value)
{
    size_t const* dfa_start = &state_machine_._dfa.front();
    size_t const* ptr = dfa_start + state_machine_._dfa_alphabet + ::lexertl::dfa_offset;
    Iterator curr = first;
    Iterator end_token = first;
    bool end_state = (*ptr != 0);
    size_t id = *(ptr + 1);

    while (curr != last && ptr != dfa_start) {
        size_t charset = state_machine_._lookup[*curr];
        ++curr;

        ptr = &dfa_start[ptr[charset] * (state_machine_._dfa_alphabet +
            ::lexertl::dfa_offset)];

        if (0 != *ptr) {
            end_state = true;
            id = *(ptr + 1);
            end_token = curr;
        }
    }

    if (end_state) {
        // return longest match
        string_type str(first, end_token);
        token_value.swap(str);
        first = end_token;
        return wave::token_id(id);
    }
    return T_EOF;
}

}   // namespace lexer

///////////////////////////////////////////////////////////////////////////////
template <typename Iterator, typename Position = wave::util::file_position_type>
class lexertl_functor
:   public lexertl_input_interface<wave::cpplexer::lex_token<Position> >
{
public:
    typedef wave::util::position_iterator<Iterator, Position> iterator_type;
    typedef typename boost::detail::iterator_traits<Iterator>::value_type
        char_type;
    typedef BOOST_WAVE_STRINGTYPE string_type;
    typedef wave::cpplexer::lex_token<Position> token_type;

    lexertl_functor(Iterator const &first_, Iterator const &last_,
            Position const &pos_, wave::language_support language)
    :   first(first_, last_, pos_), language(language), at_eof(false)
    {
        lexer_.init_dfa(language);
    }
    ~lexertl_functor() {}

// get the next token from the input stream
    token_type get()
    {
        token_type token;

        if (!at_eof) {
            do {
            // generate and return the next token
            string_type token_val;
            Position pos = first.get_position();   // begin of token position
            wave::token_id id = lexer_.next_token(first, last, token_val);

                if (T_CONTLINE != id) {
                //  The cast should avoid spurious warnings about missing case labels
                //  for the other token ids's.
                    switch (static_cast<unsigned int>(id)) {
                    case T_IDENTIFIER:
                    // test identifier characters for validity (throws if
                    // invalid chars found)
                        if (!(language & support_option_no_character_validation)) {
                            using boost::wave::cpplexer::impl::validate_identifier_name;
                            validate_identifier_name(token_val,
                                pos.get_line(), pos.get_column(), pos.get_file());
                        }
                        break;

                    case T_STRINGLIT:
                    case T_CHARLIT:
                    // test literal characters for validity (throws if invalid
                    // chars found)
                        if (language & support_option_convert_trigraphs) {
                            using wave::cpplexer::impl::convert_trigraphs;
                            token_val = convert_trigraphs(token_val);
                        }
                        if (!(language & support_option_no_character_validation)) {
                            using wave::cpplexer::impl::validate_literal;
                            validate_literal(token_val,
                                pos.get_line(), pos.get_column(), pos.get_file());
                        }
                        break;

                    case T_LONGINTLIT:  // supported in C99 and long_long mode
                        if (!wave::need_long_long(language)) {
                        // syntax error: not allowed in C++ mode
                            BOOST_WAVE_LEXER_THROW(
                                boost::wave::cpplexer::lexing_exception,
                                invalid_long_long_literal, token_val.c_str(),
                                pos.get_line(), pos.get_column(),
                                pos.get_file().c_str());
                        }
                        break;

#if BOOST_WAVE_SUPPORT_INCLUDE_NEXT != 0
                    case T_PP_HHEADER:
                    case T_PP_QHEADER:
                    case T_PP_INCLUDE:
                    // convert to the corresponding ..._next token, if appropriate
                        {
                        // Skip '#' and whitespace and see whether we find an
                        // 'include_next' here.
                            typename string_type::size_type start = token_val.find("include");
                            if (0 == token_val.compare(start, 12, "include_next", 12))
                                id = token_id(id | AltTokenType);
                            break;
                        }
#endif // BOOST_WAVE_SUPPORT_INCLUDE_NEXT != 0

                    case T_EOF:
                    // T_EOF is returned as a valid token, the next call will
                    // return T_EOI, i.e. the actual end of input
                        at_eof = true;
                        token_val.clear();
                        break;

                    case T_OR_TRIGRAPH:
                    case T_XOR_TRIGRAPH:
                    case T_LEFTBRACE_TRIGRAPH:
                    case T_RIGHTBRACE_TRIGRAPH:
                    case T_LEFTBRACKET_TRIGRAPH:
                    case T_RIGHTBRACKET_TRIGRAPH:
                    case T_COMPL_TRIGRAPH:
                    case T_POUND_TRIGRAPH:
                    case T_ANY_TRIGRAPH:
                        if (language & support_option_convert_trigraphs)
                        {
                            using wave::cpplexer::impl::convert_trigraph;
                            token_val = convert_trigraph(token_val);
                        }
                        break;
                    }
                    return token_type(id, token_val, pos);
                }

            // skip the T_CONTLINE token
            } while (true);
        }
        return token;       // return T_EOI
    }

    void set_position(Position const &pos)
    {
        // set position has to change the file name and line number only
        first.get_position().set_file(pos.get_file());
        first.get_position().set_line(pos.get_line());
    }

#if BOOST_WAVE_SUPPORT_PRAGMA_ONCE != 0
    bool has_include_guards(std::string& guard_name) const { return false; }
#endif

private:
    iterator_type first;
    iterator_type last;

    wave::language_support language;
    bool at_eof;

    static lexer::lexertl<iterator_type, Position> lexer_;
};

template <typename Iterator, typename Position>
lexer::lexertl<
    typename lexertl_functor<Iterator, Position>::iterator_type, Position>
        lexertl_functor<Iterator, Position>::lexer_;

#undef INIT_DATA_SIZE
#undef INIT_DATA_CPP_SIZE
#undef INIT_DATA_PP_NUMBER_SIZE

///////////////////////////////////////////////////////////////////////////////
//
//  The new_lexer_gen<>::new_lexer function (declared in lexertl_interface.hpp)
//  should be defined inline, if the lex_functor shouldn't be instantiated
//  separately from the lex_iterator.
//
//  Separate (explicit) instantiation helps to reduce compilation time.
//
///////////////////////////////////////////////////////////////////////////////

#if BOOST_WAVE_SEPARATE_LEXER_INSTANTIATION != 0
#define BOOST_WAVE_FLEX_NEW_LEXER_INLINE
#else
#define BOOST_WAVE_FLEX_NEW_LEXER_INLINE inline
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  The 'new_lexer' function allows the opaque generation of a new lexer object.
//  It is coupled to the iterator type to allow to decouple the lexer/iterator
//  configurations at compile time.
//
//  This function is declared inside the xlex_interface.hpp file, which is
//  referenced by the source file calling the lexer and the source file, which
//  instantiates the lex_functor. But is is defined here, so it will be
//  instantiated only while compiling the source file, which instantiates the
//  lex_functor. While the xlex_interface.hpp file may be included everywhere,
//  this file (xlex_lexer.hpp) should be included only once. This allows
//  to decouple the lexer interface from the lexer implementation and reduces
//  compilation time.
//
///////////////////////////////////////////////////////////////////////////////

template <typename Iterator, typename Position>
BOOST_WAVE_FLEX_NEW_LEXER_INLINE
wave::cpplexer::lex_input_interface<wave::cpplexer::lex_token<Position> > *
new_lexer_gen<Iterator, Position>::new_lexer(Iterator const &first,
    Iterator const &last, Position const &pos, wave::language_support language)
{
    return new lexertl_functor<Iterator, Position>(first, last, pos, language);
}

#undef BOOST_WAVE_FLEX_NEW_LEXER_INLINE

///////////////////////////////////////////////////////////////////////////////
}}}}   // namespace boost::wave::cpplexer::lexertl

#endif // !defined(BOOST_WAVE_LEXERTL_LEXER_HPP_INCLUDED)
*/
