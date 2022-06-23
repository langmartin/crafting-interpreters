#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  char **overflow;
  int overflow_index;
  int start;
  int index;
  int eof;
  FILE *fh;
  int line;
} lexer;

int l_match(lexer *l, char expected) {
  if (l->index > READ_BUFFER - 1) {
    return 0;                   /* FIXME next buffer */
  }

  if (l->buffer[l->index + 1] == expected) {
    l->index++;
    return 1;
  }
  return 0;
}

char l_peek(lexer *l) {
  return l->buffer[l->index + 1];
}

void l_add_token(token_list *ts, token_type t, int line, char* val) {
  token *tok = malloc(sizeof(token));
  tok->t = t;
  tok->line = line;
  tok->val = val;

  token_list *cell = malloc(sizeof(token_list));
  cell->token = tok;
  cell->next = ts;
  ts = cell;
}

int l_eob(lexer *l) {
  return l->index >= READ_BUFFER - 1;
}

int l_eof(lexer *l) {
  return l->eof < READ_BUFFER && l->index == l->eof;
}

void l_overflow(lexer *l) {
  char *stash;
  stash = (char *)malloc(l->index - l->start);
  for (int i = l->start; i <= l->index; i++) {
    stash[i] = l->buffer[i];
  }
  l->overflow[l->overflow_index++] = stash;
}

char l_advance(lexer *l) {
  int read_count;

  if (l_eof(l)) return '\0';

  if (l->index >= READ_BUFFER - 1) {
    if (l->start < l->index) {
      l_overflow(l);
    }

    read_count = fread(l->buffer, sizeof(char), READ_BUFFER, l->fh);
    l->index = -1;
    l->eof = read_count - 1;
  }

  l->index++;
  return l->buffer[l->index];
}

char* l_val(lexer *l) {
  int i, size, size_o, size_b = 0;
  for (; i<=l->overflow_index; i++) {
    size_o += strlen(l->overflow[i]);
  }
  size_b = l->index - l->start;
  size = size_o + size_b;

  char* result = malloc(size + 1);

  if (result) {
    for (i=0; i<=l->overflow_index; i++) {
      strcpy(result, l->overflow[i]);
    }

    for (i=0; i<=size_b; i++) {
      result[size_o + i] = l->buffer[l->start + i];
    }
  }

  // Reset overflow
  l->start = l->index;
  for (i=0; i<=l->overflow_index; i++) {
    free(l->overflow[i]);
  }
  l->overflow_index = 0;

  return result;
}

int main(int argc, char **argv) {
  FILE *fd;
  char c;
  token_list *tokens;

  if (argc < 1 || ((fd = fopen(argv[0], "r")) == NULL)) {
    fprintf(stderr, "usage: lexer file.lox\n");
    exit(64);
  }

  char *empty;
  int line = 0;
  char buffer[READ_BUFFER] = "";
  lexer *l;
  l->buffer = buffer;
  l->fh = fd;

  while (1) {
    c = l_advance(l);

    switch (c) {
    case '(': l_add_token(tokens, LEFT_PAREN, line, empty); break;
    case ')': l_add_token(tokens, RIGHT_PAREN, line, empty); break;
    case '{': l_add_token(tokens, LEFT_BRACE, line, empty); break;
    case '}': l_add_token(tokens, RIGHT_BRACE, line, empty); break;
    case ',': l_add_token(tokens, COMMA, line, empty); break;
    case '.': l_add_token(tokens, DOT, line, empty); break;
    case '-': l_add_token(tokens, MINUS, line, empty); break;
    case '+': l_add_token(tokens, PLUS, line, empty); break;
    case ';': l_add_token(tokens, SEMICOLON, line, empty); break;
    case '*': l_add_token(tokens, STAR, line, empty); break;

    case '!':
      l_add_token(tokens, (l_match(l, '=')) ? BANG_EQUAL : BANG, line, empty);
      break;
    case '=':
      l_add_token(tokens, (l_match(l, '=')) ? EQUAL_EQUAL : EQUAL, line, empty);
      break;
    case '<':
      l_add_token(tokens, (l_match(l, '=')) ? LESS_EQUAL : LESS, line, empty);
      break;
    case '>':
      l_add_token(tokens, (l_match(l, '=')) ? GREATER_EQUAL : GREATER, line, empty);
      break;
    case '/':
      if (l_match(l, '/')) {
        while (l_peek(l) != '\n' && !l_eof(l)) l_advance(l);
      } else {
        l_add_token(tokens, SLASH, line, empty);
      }
      break;

    case ' ': break;
    case '\r': break;
    case '\n': line++; break;

    case '"':
      l->start = l->index;
      while (l_peek(l) != '"' && !l_eof(l)) {
        c = l_advance(l);
        if (c == '\n') line++;

        if (l_eof(l)) {
          fprintf(stderr, "unterminated string line %d\n", line);
          exit(1);
        }

        l_add_token(tokens, STRING, line, l_val(l));
        // trailing "
        l_advance(l);
      }
    }
  }

  fclose(fd);
  return 0;
}
