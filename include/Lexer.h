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

// PROTOTIPOS
int eh_letra(char c);
TokenType verificar_palavra_reservada(char* lexema);
Token proximo_token(void);