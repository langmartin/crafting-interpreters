#include <stdio.h>
#include <stdlib.h>

typedef enum {
  // Single-character tokens.
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

  // One or two character tokens.
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,

  // Literals.
  IDENTIFIER, STRING, NUMBER,

  // Keywords.
  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

  END
} token_type;

typedef struct token_t token;
typedef struct token_list_t token_list;

struct token_t {
  token_type t;
  char *val;
};

struct token_list_t {
  token token;
  token_list *next;
};

int main(int argc, char **argv) {
  FILE *fd;
  char c;
  char buffer[1024] = "";
  token_list *tokens;
  int index = 0;

  if (argc < 1 || ((fd = fopen(argv[0], "r")) == NULL)) {
    fprintf(stderr, "usage: lexer file.lox\n");
    exit(64);
  }

  while ((c = getc(fd)) != EOF) {
    
  }

  fclose(fd);
  return 0;
}
