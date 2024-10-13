#include "file.c"

#define in_range(a, m, x) (a >= m && a <= x)
#define is_digit(c) in_range(c, 48, 57)

typedef enum TokenType {
	TT_EOF,
	TT_SEMICOLON,
	TT_INT,
	TT_FLOAT,
	TT_STRING,
	TT_NOT,
	TT_PLUS,
	TT_MINUS,
	TT_MUL,
	TT_DIV,
	TT_MOD,
	TT_POW,
	TT_ASSIGN,
	TT_ADDASSIGN,
	TT_SUBASSIGN,
	TT_MULASSIGN,
	TT_DIVASSIGN,
	TT_MODASSIGN,
	TT_POWASSIGN,
	TT_INCREMENT,
	TT_DECREMENT,
	TT_EQ,
	TT_NOTEQ,
	TT_GT,
	TT_LT,
	TT_GTE,
	TT_LTE,
	TT_AND,
	TT_OR,
	TT_ARROW,
	TT_LPAREN,
	TT_RPAREN,
	TT_LBRACKET,
	TT_RBRACKET,
	TT_LCURLY,
	TT_RCURLY,
	TT_COLON,
	TT_TRUE,
	TT_FALSE,
	TT_NULL,
	TT_IF,
	TT_ELSE,
	TT_WHILE,
	TT_FOR,
	TT_DO,
	TT_FUNC
} TokenType;

typedef	struct Token {
	TokenType type;
	PosRange *range;
	void *value;
} Token;

typedef struct TokenList {
	Token *tokens;
	int len;
} TokenList;

Token *Token_create(TokenType type, PosRange *range, void *value) {
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = type;
    token->range = range;
    token->value = value;
    return token;
}

TokenList *TokenList_create() {
	TokenList *list = (TokenList *)malloc(sizeof(TokenList));
	list->tokens = NULL;
	list->len = 0;
	return list;
}

void TokenList_add(TokenList *list, Token *token) {
	list->tokens = (Token *)realloc(list->tokens, sizeof(Token) * (list->len + 1));
	list->tokens[list->len] = *token;
	list->len++;
}

Token *TokenList_get(TokenList *list, int idx) {
	if (idx < 0 || idx >= list->len) return NULL;
	return &list->tokens[idx];
}

// TokenList *Lexer_tokenize(Position *p) {
// 	TokenList *tokens = TokenList_create();
// 	char c;

// 	while ((c = p->cur_char) != EOF) {
// 		Position_forward(p);
// 	}

// 	TokenList_add(tokens, Token_create(TT_EOF, NULL, NULL));
// 	return tokens;
// }
