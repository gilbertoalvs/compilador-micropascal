#include <stdio.h>
#include <stdlib.h>
//#include <setjmp.h>
#include <string.h>
#include <ctype.h>

// Definição de todos os tokens
typedef enum{
    T_ERRO, T_EOF, T_IDENT, T_INT_LIT, T_REAL_LIT, T_CHAR_LIT,
    // Palavras reservadas:
    T_PROGRAM, T_IF, T_THEN, T_ELSE, T_WHILE, T_DO, T_REPEAT, T_UNTIL, T_INTEGER, T_REAL, T_CHAR, T_BEGIN, T_END, T_WRITE, T_VAR,
    // Operadores que também são palavras reservadas:
    T_DIV, T_AND, T_OR, T_NOT,
    // Simbolos e operadores:
    T_MAIS, T_MENOS, T_MULT, T_DIVISAO_REAL, // + - * / 
    T_ATRIBUICAO, T_IGUAL, T_DIFERENTE, // :=, =, <>
    T_MENOR, T_MAIOR, T_MENOR_IGUAL, T_MAIOR_IGUAL, // < > <= >=
    T_DOIS_PONTOS, T_PONTO_VIRGULA, T_VIRGULA, T_PONTO, // : ; , .
    T_ABRE_PAR, T_FECHA_PAR // ()
} TokenType;

//Struct Token
typedef struct {
    TokenType tipo;
    char lexema[100];
} Token;


// Variaveis globais
int posicao = 0;
char expr_texto[2048];

// PROTOTIPOS
int eh_letra(char c);
TokenType verificar_palavra_reservada(char* lexema);
Token proximo_token(void);

// Funções:

int eh_letra(char c){
    return isalpha(c) || c == '_';
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

    return T_IDENT; // Se não for nenhuma, é um identificador normal.
}

Token proximo_token(void){
    Token t;
    memset(t.lexema, 0, sizeof(t.lexema));
    t.tipo = T_ERRO;

    char atual = expr_texto[posicao];

    //Elimina espaços em branco e quebras de linha
    while(atual == ' ' || atual == '\n' || atual == '\t' || atual == '\r'){
        posicao++;
        atual = expr_texto[posicao];
    }

    if(atual == '\0'){
        t.tipo = T_EOF;
        strcpy(t.lexema, "EOF");
        return t;
    }

    // Identificadores e palavras reservadas:
    if(eh_letra(atual)){
        int inicio = posicao;
        while(eh_letra(atual) || isdigit(atual)){
            posicao++;
            atual = expr_texto[posicao];
        }
        strncpy(t.lexema, &expr_texto[inicio], posicao - inicio);
        t.tipo = verificar_palavra_reservada(t.lexema);
        return t;
    }

    // Numeros inteiros e reais:
    if(isdigit(atual)){
        int inicio = posicao;
        int tem_ponto = 0;

        while(isdigit(atual) || atual == '.'){
            if(atual == '.'){
                if(tem_ponto) break;
                tem_ponto = 1;
            }
            posicao++;
            atual = expr_texto[posicao];
        }

        strncpy(t.lexema, &expr_texto[inicio], posicao - inicio);
        if(tem_ponto){
            t.tipo = T_REAL_LIT;
        } else{
            t.tipo = T_INT_LIT;
        }

        return t;
    }

    // Char literal ('a', '\n'):
    if (atual == '\''){
        int inicio = posicao;
        posicao++;

        while(expr_texto[posicao] != '\'' && expr_texto[posicao] != '\0'){
            posicao++;
        }

        if(expr_texto[posicao] == '\''){
            posicao++; // Inclui aspas de fechamento
        }

        strncpy(t.lexema, &expr_texto[inicio], posicao - inicio);
        t.tipo = T_CHAR_LIT;
        return t;
    }

    // Operadoes compostos e simples:
    int inicio = posicao;

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
        
        //Operadores de um caractere
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

    // Se chegou aqui o caractere é inválido.
    printf("Erro lexico no caracter [%c]\n", atual);
    posicao++;
    return t;
}