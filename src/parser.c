#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "lexer.h"
#include "parser.h"

static Token atual;

static void avancar(void) {
    atual = proximo_token();
    if (atual.tipo == T_ERRO) exit(1);
}

static void erro_sintaxe(void) {
    printf("Erro de sintaxe no token [%s]\n", atual.lexema);
    exit(1);
}

static void consumir(TokenType esperado) {
    if (atual.tipo != esperado) erro_sintaxe();
    avancar();
}

static void comando(void);
static void bloco(void);
static void expressao(void);
static void expr_rel(void);

static void expr_basica(void) {
    switch (atual.tipo) {
        case T_ABRE_PAR:
            avancar();
            expressao();
            consumir(T_FECHA_PAR);
            break;
        case T_NOT:
            avancar();
            expr_rel();
            break;
        case T_INT_LIT:
        case T_REAL_LIT:
        case T_CHAR_LIT:
        case T_IDENT:
            avancar();
            break;
        default:
            erro_sintaxe();
    }
}

static void expr_mul(void) {
    expr_basica();
    while (atual.tipo == T_MULT || atual.tipo == T_DIVISAO_REAL || atual.tipo == T_DIV) {
        avancar();
        expr_basica();
    }
}

static void expr_add(void) {
    expr_mul();
    while (atual.tipo == T_MAIS || atual.tipo == T_MENOS) {
        avancar();
        expr_mul();
    }
}

static int eh_relacional(TokenType t) {
    return t == T_IGUAL || t == T_DIFERENTE || t == T_MENOR ||
           t == T_MAIOR || t == T_MENOR_IGUAL || t == T_MAIOR_IGUAL;
}

static void expr_rel(void) {
    expr_add();
    while (eh_relacional(atual.tipo)) {
        avancar();
        expr_add();
    }
}

static void expressao(void) {
    expr_rel();
    while (atual.tipo == T_OR || atual.tipo == T_AND) {
        avancar();
        expr_rel();
    }
}

static void atribuicao(void) {
    consumir(T_IDENT);
    consumir(T_ATRIBUICAO);
    expressao();
    consumir(T_PONTO_VIRGULA);
}

static void iteracao(void) {
    if (atual.tipo == T_WHILE) {
        avancar();
        expressao();
        consumir(T_DO);
        comando();
    } else { 
        avancar();
        comando();
        consumir(T_UNTIL);
        expressao();
        consumir(T_PONTO_VIRGULA);
    }
}

static void decisao(void) {
    consumir(T_IF);
    expressao();
    consumir(T_THEN);
    comando();
    if (atual.tipo == T_ELSE) {
        avancar();
        comando();
    }
}

static void escrita(void) {
    consumir(T_WRITE);
    consumir(T_ABRE_PAR);
    expressao();
    consumir(T_FECHA_PAR);
    consumir(T_PONTO_VIRGULA);
}

static void comando(void) {
    switch (atual.tipo) {
        case T_BEGIN:
            bloco();
            consumir(T_PONTO_VIRGULA);
            break;
        case T_IDENT:     atribuicao(); break;
        case T_WHILE:
        case T_REPEAT:    iteracao();   break;
        case T_IF:        decisao();    break;
        case T_WRITE:     escrita();    break;
        default:          erro_sintaxe();
    }
}

static int inicio_de_comando(TokenType t) {
    return t == T_BEGIN || t == T_IDENT || t == T_WHILE ||
           t == T_REPEAT || t == T_IF || t == T_WRITE;
}

static void bloco(void) {
    consumir(T_BEGIN);
    while (inicio_de_comando(atual.tipo))
        comando();
    consumir(T_END);
}

static void tipo(void) {
    if (atual.tipo == T_INTEGER || atual.tipo == T_REAL || atual.tipo == T_CHAR)
        avancar();
    else
        erro_sintaxe();
}

static void decl_var(void) {
    consumir(T_IDENT);
    while (atual.tipo == T_VIRGULA) {
        avancar();
        consumir(T_IDENT);
    }
    consumir(T_DOIS_PONTOS);
    tipo();
    consumir(T_PONTO_VIRGULA);
}

static void secao_var(void) {
    consumir(T_VAR);
    while (atual.tipo == T_IDENT)
        decl_var();
}

void parse_programa(void) {
    avancar();                 
    consumir(T_PROGRAM);
    consumir(T_IDENT);
    consumir(T_PONTO_VIRGULA);
    secao_var();
    bloco();
    consumir(T_PONTO);
    if (atual.tipo != T_EOF) erro_sintaxe();   
    printf("Analise sintatica concluida sem erros.\n");
}