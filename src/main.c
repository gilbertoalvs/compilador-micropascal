#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "lexer.h"
#include "parser.h"

static void listar_tokens(void) {
    while (1) {
        Token t = proximo_token();
        if (t.tipo == T_ERRO) exit(1);
        printf("%-16s [%s]\n", nome_token[t.tipo], t.lexema);
        if (t.tipo == T_EOF) break;
    }
}

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