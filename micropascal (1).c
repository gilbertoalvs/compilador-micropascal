/*
 * Compilador micro-Pascal - Parte 1 (analisador léxico + sintático)
 * Construção de Compiladores - UNICAP
 *
 * Compilar:  gcc micropascal.c -o micropascal
 * Usar:      ./micropascal arquivo.pas        (analisa o programa)
 *            ./micropascal -t arquivo.pas     (só imprime os tokens do lexer)
 *            ./micropascal                    (sem argumento: lê da entrada padrão)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ====================== DEFINIÇÕES DE TOKENS ====================== */
#define TAM_FONTE (1 << 20)   /* 1 MB */
#define TAM_LEXEMA 100

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

typedef struct {
    TokenType tipo;
    char lexema[TAM_LEXEMA];
} Token;

/* ======================= ANALISADOR LÉXICO ======================== */
// Variaveis globais
int posicao = 0;
char expr_texto[TAM_FONTE];

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

    return T_IDENT; // Se não for nenhuma, é um identificador normal.
}

/* Copia o lexema sem ultrapassar o tamanho do vetor.
 * O tamanho máximo é limitado pelo próprio tamanho da fonte lida. */
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

    // Elimina espaços em branco, quebras de linha e comentários //
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

    // Identificadores e palavras reservadas:
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

    // Inteiro: digito+   Real: digito* . digito+  (o '.' exige digito depois)
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

    // Literal char: '(letra|digito)', '\\n' ou '\\t'.
    if(atual == '\''){
        int inicio = posicao;
        posicao++;

        if(expr_texto[posicao] == '\\' &&
           (expr_texto[posicao + 1] == 'n' || expr_texto[posicao + 1] == 't')){
            posicao += 2;
        } else if(eh_letra(expr_texto[posicao]) || eh_digito(expr_texto[posicao])){
            posicao++;
        } else {
            printf("Erro léxico no caracter [%c]\n", '\'');
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

        posicao++; // inclui a aspa de fechamento
        copiar_lexema(t.lexema, sizeof(t.lexema), &expr_texto[inicio], (size_t)(posicao - inicio));
        t.tipo = T_CHAR_LIT;
        return t;
    }

    // Operadoes compostos e simples:
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
    printf("Erro léxico no caracter [%c]\n", atual);
    posicao++;
    return t;
}

/* ====================== ANALISADOR SINTÁTICO ====================== */
static Token atual;

static void avancar(void) {
    atual = proximo_token();
    if (atual.tipo == T_ERRO) exit(1);   /* o lexer já imprimiu o erro léxico */
}

static void erro_sintaxe(void) {
    printf("Erro de sintaxe no token [%s]\n", atual.lexema);
    exit(1);
}

/* Consome o token esperado ou emite erro. */
static void consumir(TokenType esperado) {
    if (atual.tipo != esperado) erro_sintaxe();
    avancar();
}

/* Declarações adiantadas */
static void comando(void);
static void bloco(void);
static void expressao(void);
static void expr_rel(void);

/* ---------------------------- Expressões ----------------------------
 * Gramática concreta (sem recursão à esquerda), todos associativos à
 * esquerda, do nível mais baixo de precedência para o mais alto:
 *
 *  expressao ::= expr_rel { (or | and) expr_rel }
 *  expr_rel  ::= expr_add { (= | <> | < | > | <= | >=) expr_add }
 *  expr_add  ::= expr_mul { (+ | -) expr_mul }
 *  expr_mul  ::= expr_basica { (* | / | div) expr_basica }
 *  expr_basica ::= ( expressao ) | not expr_rel | INT | REAL | CHAR | ID
 */
static void expr_basica(void) {
    switch (atual.tipo) {
        case T_ABRE_PAR:
            avancar();
            expressao();
            consumir(T_FECHA_PAR);
            break;
        case T_NOT:
            avancar();
            /* O enunciado define not seguido de uma expressão.
             * Aqui ele consome a expressão relacional, antes de and/or. */
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

/* ----------------------------- Comandos ---------------------------- */

/* <atribuicao> ::= ID := <expressao> ; */
static void atribuicao(void) {
    consumir(T_IDENT);
    consumir(T_ATRIBUICAO);
    expressao();
    consumir(T_PONTO_VIRGULA);
}

/* <iteracao> ::= while <expr> do <comando>
 *              | repeat <comando> until <expr> ;  */
static void iteracao(void) {
    if (atual.tipo == T_WHILE) {
        avancar();
        expressao();
        consumir(T_DO);
        comando();
    } else { /* T_REPEAT */
        avancar();
        comando();
        consumir(T_UNTIL);
        expressao();
        consumir(T_PONTO_VIRGULA);
    }
}

/* <decisao> ::= if <expr> then <comando> [ else <comando> ]
 * O else casa com o if mais próximo (resolvido naturalmente aqui). */
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

/* <escrita> ::= write ( <expressao> ) ; */
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
            consumir(T_PONTO_VIRGULA);      /* <bloco> ; */
            break;
        case T_IDENT:     atribuicao(); break;
        case T_WHILE:
        case T_REPEAT: iteracao();   break;
        case T_IF:     decisao();    break;
        case T_WRITE:  escrita();    break;
        default:        erro_sintaxe();
    }
}

static int inicio_de_comando(TokenType t) {
    return t == T_BEGIN || t == T_IDENT || t == T_WHILE ||
           t == T_REPEAT || t == T_IF || t == T_WRITE;
}

/* <bloco> ::= begin <lista_comandos> end */
static void bloco(void) {
    consumir(T_BEGIN);
    while (inicio_de_comando(atual.tipo))   /* <lista_comandos> ::= {<comando>}* */
        comando();
    consumir(T_END);
}

/* ----------------------------- Declarações ------------------------- */

static void tipo(void) {
    if (atual.tipo == T_INTEGER || atual.tipo == T_REAL || atual.tipo == T_CHAR)
        avancar();
    else
        erro_sintaxe();
}

/* <decl_var> ::= ID { , ID } : <tipo> ; */
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

/* <secao_var> ::= var { <decl_var> }* */
static void secao_var(void) {
    consumir(T_VAR);
    while (atual.tipo == T_IDENT)
        decl_var();
}

/* <programa> ::= program ID ; <secao_var> <bloco> . */
void parse_programa(void) {
    avancar();                 /* lê o primeiro token */
    consumir(T_PROGRAM);
    consumir(T_IDENT);
    consumir(T_PONTO_VIRGULA);
    secao_var();
    bloco();
    consumir(T_PONTO);
    if (atual.tipo != T_EOF) erro_sintaxe();   /* lixo após o '.' */
    printf("Análise sintática concluída sem erros.\n");
}

static const char *nome_token[] = {
    "ERRO", "EOF", "IDENTIFICADOR", "INTEIRO_LITERAL", "REAL_LITERAL", "CHAR_LITERAL",
    "PROGRAM", "IF", "THEN", "ELSE", "WHILE", "DO", "REPEAT", "UNTIL", "INTEGER", "REAL", "CHAR", "BEGIN", "END", "WRITE", "VAR",
    "DIV", "AND", "OR", "NOT",
    "MAIS", "MENOS", "MULT", "DIVISAO_REAL",
    "ATRIBUICAO", "IGUAL", "DIFERENTE",
    "MENOR", "MAIOR", "MENOR_IGUAL", "MAIOR_IGUAL",
    "DOIS_PONTOS", "PONTO_VIRGULA", "VIRGULA", "PONTO",
    "ABRE_PAR", "FECHA_PAR"
};

/* Modo -t: imprime a sequência de tokens (demonstra só o lexer). */
static void listar_tokens(void) {
    while (1) {
        Token t = proximo_token();
        if (t.tipo == T_ERRO) exit(1);   /* o lexer já imprimiu o erro */
        printf("%-16s [%s]\n", nome_token[t.tipo], t.lexema);
        if (t.tipo == T_EOF) break;
    }
}

/* ========================== MAIN ========================== */
int main(int argc, char **argv) {
    int modo_tokens = 0;
    const char *arquivo = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0) modo_tokens = 1;
        else arquivo = argv[i];
    }

    FILE *f = stdin;
    if (arquivo) {
        f = fopen(arquivo, "r");
        if (!f) {
            printf("Nao foi possivel abrir %s\n", arquivo);
            return 1;
        }
    } else {
        printf("Cole o codigo micro-Pascal e finalize com Ctrl+D (Ctrl+Z no Windows):\n");
    }
    size_t n = fread(expr_texto, 1, TAM_FONTE - 1, f);
    expr_texto[n] = '\0';
    if (n == TAM_FONTE - 1 && fgetc(f) != EOF)
        printf("Aviso: arquivo maior que %d bytes, o restante foi ignorado.\n", TAM_FONTE - 1);
    if (f != stdin) fclose(f);

    posicao = 0;
    if (modo_tokens) listar_tokens();
    else parse_programa();
    return 0;
}
