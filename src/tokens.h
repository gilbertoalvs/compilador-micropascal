#ifndef TOKENS_H
#define TOKENS_H

#define TAM_FONTE (1 << 20)
#define TAM_LEXEMA 100

typedef enum {
    T_ERRO, T_EOF, T_IDENT, T_INT_LIT, T_REAL_LIT, T_CHAR_LIT,
    T_PROGRAM, T_IF, T_THEN, T_ELSE, T_WHILE, T_DO, T_REPEAT, T_UNTIL, T_INTEGER, T_REAL, T_CHAR, T_BEGIN, T_END, T_WRITE, T_VAR,
    T_DIV, T_AND, T_OR, T_NOT,
    T_MAIS, T_MENOS, T_MULT, T_DIVISAO_REAL,
    T_ATRIBUICAO, T_IGUAL, T_DIFERENTE,
    T_MENOR, T_MAIOR, T_MENOR_IGUAL, T_MAIOR_IGUAL,
    T_DOIS_PONTOS, T_PONTO_VIRGULA, T_VIRGULA, T_PONTO,
    T_ABRE_PAR, T_FECHA_PAR
} TokenType;

typedef struct {
    TokenType tipo;
    char lexema[TAM_LEXEMA];
} Token;

extern int posicao;
extern char expr_texto[TAM_FONTE];
extern const char *nome_token[];

#endif