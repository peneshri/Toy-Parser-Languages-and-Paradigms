#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "toyTokenizer.cpp"
using namespace std;

// =====================================================================
// PARSER  — recursive descent following the BNF
// =====================================================================
struct Parser {
    Token*    tokens;
    int       count;
    int       pos;
};

const Token* prCurrent(const Parser &p) { return &p.tokens[p.pos]; }
bool prCheck(const Parser &p, TokenType t) { return prCurrent(p)->tokenType == t; }

void prAdvance(Parser &p) {
    p.pos++;
}

bool prMatch(Parser &p, TokenType t) {
    if (prCheck(p, t)) { prAdvance(p); return true; }
    return false;
}

void prError(const Parser &p, string msg) {
    const Token* t = prCurrent(p);
	cerr << "Parser error at line " << t->line << ": column " << t->column << ": " << msg << " (got " << tokenTypeName(t->tokenType) << ")\n";
    exit(1);
}

const Token* prExpect(Parser &p, TokenType t, string what) {
    if (!prCheck(p, t)) {
		prError(p, "expected " + what + " (" + tokenTypeName(t) + ")");
    }
    const Token* tok = prCurrent(p);
    prAdvance(p);
    return tok;
}

// -------- Forward decl --------
void parseExpression(Parser &p);

// -------- Terminals --------
void parseVariable(Parser &p) {
    prAdvance(p);
}

// -------- <A> -> a<A> | a --------
void parseA(Parser &p) {
    prExpect(p, TOK_a, "a");
    while (prCheck(p, TOK_a)) {
        prAdvance(p);
    }

}

// -------- <B> -> b<B> | b --------
void parseB(Parser &p) {
	while (1)
    {
        if (prCheck(p, TOK_b))
        {
            prAdvance(p);
        }
        else break;
    }
}

// -------- <C> -> c --------
void parseC(Parser &p) {
	if (prExpect(p, TOK_c, "c"))
    {
        parseB(p);
    }
}

// -------- <program> -> <A><C><B> | <A> --------
void parseProgram(Parser &p) {
    parseA(p);
    if (prCheck(p, TOK_c)) {
        parseC(p);
    }
	prExpect(p, TOK_EOF, "EOF");
}

// =====================================================================
// MAIN
// =====================================================================
int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Cannot open file: %s\n", argv[1]);
        return 1;
    }
    // ---- Tokenize ----
    Tokenizer tk;
    tkInit(&tk, fp);

    Token tokens[MAX_TOKENS];
    int tokenCount = tokenize(&tk, tokens, MAX_TOKENS);

    fclose(fp);

	Parser parser;
	parser.tokens = tokens;
	parser.pos    = 0;

	parseProgram(parser);
	cout << "No errors found\n";

    return 0;
}
