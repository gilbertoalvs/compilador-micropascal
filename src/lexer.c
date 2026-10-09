#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include "lexer.h"

int posicao = 0;
char expr_texto[TAM_FONTE];

const char *nome_token[] = {
    "ERRO", "EOF", "IDENTIFICADOR", "INTEIRO_LITERAL", "REAL_LITERAL", "CHAR_LITERAL",
    "PROGRAM", "IF", "THEN", "ELSE", "WHILE", "DO", "REPEAT", "UNTIL", "INTEGER", "REAL", "CHAR", "BEGIN", "END", "WRITE", "VAR",
    "DIV", "AND", "OR", "NOT",
    "MAIS", "MENOS", "MULT", "DIVISAO_REAL",
    "ATRIBUICAO", "IGUAL", "DIFERENTE",
    "MENOR", "MAIOR", "MENOR_IGUAL", "MAIOR_IGUAL",
    "DOIS_PONTOS", "PONTO_VIRGULA", "VIRGULA", "PONTO",
    "ABRE_PAR", "FECHA_PAR"
};

int eh_letra(char c){
    return isalpha((unsigned char)c) || c == '_';
}

int eh_digito(char c){
    return isdigit((unsigned char)c);
}

TokenType verificar_palavra_reservada(char* lexema){
    if(strcmp(lexema, "program") == 0) return T_PROGRAM;
    if(strcmp(lexema, "if") == 0) return T_IF;
    if(strcmp(lexema, "then") == 0) return T_THEN;
    if(strcmp(lexema, "else") == 0) return T_ELSE;
    if(strcmp(lexema, "while") == 0) return T_WHILE;
    if(strcmp(lexema, "do") == 0) return T_DO;
    if(strcmp(lexema, "repeat") == 0) return T_REPEAT;
    if(strcmp(lexema, "until") == 0) return T_UNTIL;
    if(strcmp(lexema, "integer") == 0) return T_INTEGER;
    if(strcmp(lexema, "real") == 0) return T_REAL;
    if(strcmp(lexema, "char") == 0) return T_CHAR;
    if(strcmp(lexema, "begin") == 0) return T_BEGIN;
    if(strcmp(lexema, "end") == 0) return T_END;
    if(strcmp(lexema, "write") == 0) return T_WRITE;
    if(strcmp(lexema, "var") == 0) return T_VAR;
    if(strcmp(lexema, "div") == 0) return T_DIV;
    if(strcmp(lexema, "and") == 0) return T_AND;
    if(strcmp(lexema, "or") == 0) return T_OR;
    if(strcmp(lexema, "not") == 0) return T_NOT;
    return T_IDENT; 
}

static void copiar_lexema(char *destino, size_t capacidade, const char *origem, size_t tamanho) {
    if (capacidade == 0) return;
    if (tamanho >= capacidade) tamanho = capacidade - 1;
    memcpy(destino, origem, tamanho);
    destino[tamanho] = '\0';
}

Token proximo_token(void){
    Token t;
    memset(t.lexema, 0, sizeof(t.lexema));
    t.tipo = T_ERRO;

    char atual = expr_texto[posicao];

    while(1){
        while(atual == ' ' || atual == '\n' || atual == '\t' || atual == '\r'){
            posicao++;
            atual = expr_texto[posicao];
        }
        if(atual == '/' && expr_texto[posicao+1] == '/'){
            while(atual != '\n' && atual != '\0'){
                posicao++;
                atual = expr_texto[posicao];
            }
            continue;
        }
        break;
    }

    if(atual == '\0'){
        t.tipo = T_EOF;
        strcpy(t.lexema, "EOF");
        return t;
    }

    if(eh_letra(atual)){
        int inicio = posicao;
        while(eh_letra(atual) || eh_digito(atual)){
            posicao++;
            atual = expr_texto[posicao];
        }
        copiar_lexema(t.lexema, sizeof(t.lexema), &expr_texto[inicio], (size_t)(posicao - inicio));
        t.tipo = verificar_palavra_reservada(t.lexema);
        return t;
    }

    if(eh_digito(atual) || (atual == '.' && eh_digito(expr_texto[posicao + 1]))){
        int inicio = posicao;
        while(eh_digito(expr_texto[posicao])) posicao++;

        if(expr_texto[posicao] == '.' && eh_digito(expr_texto[posicao + 1])){
            posicao++;
            while(eh_digito(expr_texto[posicao])) posicao++;
            t.tipo = T_REAL_LIT;
        } else {
            t.tipo = T_INT_LIT;
        }
        copiar_lexema(t.lexema, sizeof(t.lexema), &expr_texto[inicio], (size_t)(posicao - inicio));
        return t;
    }

    if(atual == '\''){
        int inicio = posicao;
        posicao++;
        if(expr_texto[posicao] == '\\' && (expr_texto[posicao + 1] == 'n' || expr_texto[posicao + 1] == 't')){
            posicao += 2;
        } else if(eh_letra(expr_texto[posicao]) || eh_digito(expr_texto[posicao])){
            posicao++;
        } else {
            printf("Erro lexico no caracter [%c]\n", '\'');
            t.tipo = T_ERRO;
            strcpy(t.lexema, "'");
            return t;
        }
        if(expr_texto[posicao] != '\''){
            printf("Erro léxico no caracter [%c]\n", '\'');
            t.tipo = T_ERRO;
            strcpy(t.lexema, "'");
            return t;
        }
        posicao++; 
        copiar_lexema(t.lexema, sizeof(t.lexema), &expr_texto[inicio], (size_t)(posicao - inicio));
        t.tipo = T_CHAR_LIT;
        return t;
    }

    switch(atual){
        case ':':
            if(expr_texto[posicao+1] == '='){
                strcpy(t.lexema, ":=");
                t.tipo = T_ATRIBUICAO;
                posicao+=2;
                return t;
            }
            strcpy(t.lexema, ":");
            t.tipo = T_DOIS_PONTOS;
            posicao++;
            return t;
        case '<':
            if(expr_texto[posicao+1] == '>'){
                strcpy(t.lexema, "<>");
                t.tipo = T_DIFERENTE;
                posicao += 2;
                return t;
            }
            if(expr_texto[posicao+1] == '='){
                strcpy(t.lexema, "<=");
                t.tipo = T_MENOR_IGUAL;
                posicao += 2;
                return t;
            }
            strcpy(t.lexema, "<");
            t.tipo = T_MENOR;
            posicao++;
            return t;
        case '>':
            if(expr_texto[posicao+1] == '='){
                strcpy(t.lexema, ">=");
                t.tipo = T_MAIOR_IGUAL;
                posicao += 2;
                return t;
            }
            strcpy(t.lexema, ">");
            t.tipo = T_MAIOR;
            posicao++;
            return t;
        case '=': t.tipo = T_IGUAL; strcpy(t.lexema, "="); posicao++; return t;
        case '+': t.tipo = T_MAIS; strcpy(t.lexema, "+"); posicao++; return t;
        case '-': t.tipo = T_MENOS; strcpy(t.lexema, "-"); posicao++; return t;
        case '*': t.tipo = T_MULT; strcpy(t.lexema, "*"); posicao++; return t;
        case '/': t.tipo = T_DIVISAO_REAL; strcpy(t.lexema, "/"); posicao++; return t;
        case ';': t.tipo = T_PONTO_VIRGULA; strcpy(t.lexema, ";"); posicao++; return t;
        case ',': t.tipo = T_VIRGULA; strcpy(t.lexema, ","); posicao++; return t;
        case '.': t.tipo = T_PONTO; strcpy(t.lexema, "."); posicao++; return t;
        case '(': t.tipo = T_ABRE_PAR; strcpy(t.lexema, "("); posicao++; return t;
        case ')': t.tipo = T_FECHA_PAR; strcpy(t.lexema, ")"); posicao++; return t;
    }

    printf("Erro léxico no caracter [%c]\n", atual);
    posicao++;
    return t;
}