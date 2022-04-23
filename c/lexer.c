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
  int line;
  char *val;
};

struct token_list_t {
  token *token;
  token_list *next;
};

#define READ_BUFFER 1024

typedef struct {
  char state;
  char *buffer;
  int index;
  int eof;
  FILE *fh;
  int line;
} lexer;

int match(lexer *l, char expected) {
  if (l->index > READ_BUFFER - 1) {
    return 0;                   /* FIXME next buffer */
  }

  if (l->buffer[l->index + 1] == expected) {
    l->index++;
    return 1;
  }
  return 0;
}

char peek(lexer *l) {
  return l->buffer[l->index + 1];
}

void add_token(token_list *ts, token_type t, int line) {
  token *tok = malloc(sizeof(token));
  tok->t = t;
  tok->line = line;

  token_list *cell = malloc(sizeof(token_list));
  cell->token = tok;
  cell->next = ts;
  ts = cell;
}

int eof(lexer *l) {
  return l->eof < READ_BUFFER && l->index == l->eof;
}

char advance(lexer *l) {
  int read_count;

  if (eof(l)) return '\0';

  if (l->index >= READ_BUFFER - 1) {
    read_count = fread(l->buffer, sizeof(char), READ_BUFFER, l->fh);
    l->index = -1;
    l->eof = read_count - 1;
  }

  l->index++;
  return l->buffer[l->index];
}

int main(int argc, char **argv) {
  FILE *fd;
  char c;
  token_list *tokens;

  if (argc < 1 || ((fd = fopen(argv[0], "r")) == NULL)) {
    fprintf(stderr, "usage: lexer file.lox\n");
    exit(64);
  }

  int line = 0;
  char buffer[READ_BUFFER] = "";
  lexer *l;
  l->buffer = buffer;
  l->fh = fd;

  while (1) {
    c = advance(l);

    switch (c) {
    case '(': add_token(tokens, LEFT_PAREN, line); break;
    case ')': add_token(tokens, RIGHT_PAREN, line); break;
    case '{': add_token(tokens, LEFT_BRACE, line); break;
    case '}': add_token(tokens, RIGHT_BRACE, line); break;
    case ',': add_token(tokens, COMMA, line); break;
    case '.': add_token(tokens, DOT, line); break;
    case '-': add_token(tokens, MINUS, line); break;
    case '+': add_token(tokens, PLUS, line); break;
    case ';': add_token(tokens, SEMICOLON, line); break;
    case '*': add_token(tokens, STAR, line); break;

    case '!':
      add_token(tokens, (match(l, '=')) ? BANG_EQUAL : BANG, line);
      break;
    case '=':
      add_token(tokens, (match(l, '=')) ? EQUAL_EQUAL : EQUAL, line);
      break;
    case '<':
      add_token(tokens, (match(l, '=')) ? LESS_EQUAL : LESS, line);
      break;
    case '>':
      add_token(tokens, (match(l, '=')) ? GREATER_EQUAL : GREATER, line);
      break;
    case '/':
      if (match(l, '/')) {
        while (peek(l) != '\n' && !eof(l)) advance(l);
      } else {
        add_token(tokens, SLASH, line);
      }
      break;

    case ' ': break;
    case '\r': break;
    case '\n': line++; break;

    case '"':
      while (peek(l) != '"' && !eof(l)) {
        c = advance(l);
        if (c == '\n') line++;
      }
    }
  }

  fclose(fd);
  return 0;
}
