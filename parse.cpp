/*
 * parse.cpp
 */
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "parse.h"
#include "parsetree.h"
#include "tokens.h"
#include <string.h>

// WRAPPER FOR PUSHBACK

map<string, int> mapv;


// WRAPPER FOR PUSHBACK
namespace Parser {
    bool pushed_back = false;
    Token	pushed_token;

    static Token GetNextToken(istream *in, int *line) {
        if( pushed_back ) {
            pushed_back = false;
            return pushed_token;
        }
        return getNextToken(in, line);
    }

    static void PushBackToken(Token& t) {
        if( pushed_back ) {
            abort();
        }
        pushed_back = true;
        pushed_token = t;
    }

}

static int error_count = 0;

void
ParseError(int line, string msg)
{
    ++error_count;
    cout << line << ": " << msg << endl;
}

ParseTree *Prog(istream *in, int *line)
{
    ParseTree *sl = Slist(in, line);

    if( sl == 0 )
        ParseError(*line, "No statements in program");

    if( error_count )
        return 0;

    return sl;
}

// Slist is a Statement followed by a Statement List
ParseTree *Slist(istream *in, int *line)
{
    // cout << "Do we Get to SList???" << endl;
    ParseTree *s = Stmt(in, line);
    if( s == 0 )
        return 0;

    if( Parser::GetNextToken(in, line) != SC ) {
        ParseError(*line, "Missing semicolon");
        return 0;
    }

    return new StmtList(s, Slist(in,line));
}

ParseTree *Stmt(istream *in, int *line) {
    /*
     * Stmt Grammar Rule:
     * Stmt := IfStmt | PrintStmt | Expr
     */
    //Given so dont change it

    //cout << "Do We Get To Stmt??" << endl;

    ParseTree *s;

    Token t = Parser::GetNextToken(in, line);
    switch( t.GetTokenType() ) {
        case IF:
            s = IfStmt(in, line);
            break;
        case PRINT:
            s = PrintStmt(in, line);
            break;
        case DONE:
            return 0;
        case ERR:
            ParseError(*line, "Invalid token");
            return 0;
        default:
            // put back the token and then see if it's an Expr
            Parser::PushBackToken(t);
            s = Expr(in, line);
            if( s == 0 )
            {
                ParseError(*line, "Invalid statement");
                return 0;
            }
            break;
    }
    return s;
}

ParseTree *IfStmt(istream *in, int *line)
{
    /*
     * IfStmt Grammar Rule:
     * IfStmt := IF Expr THEN Stmt
     */
    //cout << "Do We Get To IfStmt??" << endl;

    ParseTree *t5 = Expr(in, line);
    if (t5 == 0) {
        ParseError(*line, "Error at IfStmt - 1");
        return 0;
    }
    Token t = Parser::GetNextToken(in, line);
    if(t.GetTokenType() != THEN ) {
        ParseError(*line, "Error at IfStmt - 2");
        return 0;
    }

    ParseTree *stmt = Stmt(in, line);

    return new IfStatement(*line, t5, stmt);
}

ParseTree *PrintStmt(istream *in, int *line)
{
    /*
     * PrintStmt Grammar Rule:
     * PrintStmt := PRINT Expr
     */
    //Create new ParseTree for *PrintStmt
    // cout << "Do We Get To PrintStmt??" << endl;

    ParseTree *t2 = Expr(in, line);
    if(t2 == 0) {
        ParseError(*line, "Error at PrintStmt - 1");
        return 0;
    }
    return new PrintStatement(t2->GetLinenum(), t2);
}

ParseTree *Expr(istream *in, int *line)
{
    /*
     * Expr Grammar Rule:
     * Expr := LogicExpr { ASSIGN LogicExpr }
     */
    //cout << "Do we Get to Expr???" << endl;

    ParseTree *t1 = LogicExpr(in, line);
    if( t1 == 0 ) {
        ParseError(*line, "Error at Expression");
        return 0;
    }
    Token t = Parser::GetNextToken(in, line);

    if( t != ASSIGN ) {
        Parser::PushBackToken(t);
        return t1;
    }

    ParseTree *t2 = Expr(in, line); // right assoc
    if( t2 == 0 ) {
        ParseError(*line, "Missing expression after operator");
        return 0;
    }
    return new Assignment(t.GetLinenum(), t1, t2);
}

ParseTree *LogicExpr(istream *in, int *line)
{
    /*
     * LogicExpr Grammar Rule:
     * LogicExpr := CompareExpr { (LOGICAND | LOGICOR) CompareExpr }
     */
    //cout << "Do we Get to LogicExpr??" << endl;


    ParseTree *t1 = CompareExpr(in, line);
    if( t1 == 0 ) {
        ParseError(*line, "Error at Logic Expression - t1");
        return 0;
    }

    Token t = Parser::GetNextToken(in, line);
    if(t.GetTokenType() != LOGICAND && t.GetTokenType() != LOGICOR)
    {
        Parser::PushBackToken(t);
        return t1;
    }

    ParseTree *t2 = CompareExpr(in, line);
    if(t2 == 0)
    {
        ParseError(*line, "Error at Logic Expression - t2");
        return 0;
    }


    //LogicAndExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    if( t == LOGICAND )
        t1 = new LogicAndExpr(t.GetLinenum(), t1, t2);
    else if (t == LOGICOR)
        t1 = new LogicOrExpr(t.GetLinenum(), t1, t2);

    return t1;
}

ParseTree *CompareExpr(istream *in, int *line) {

    //cout << "Do We Get To CompareExpr??" << endl;

    ParseTree *t1 = AddExpr(in, line);
    if( t1 == 0 )
    {
        ParseError(*line, "Error at Compare Expression - 1");
        return 0;
    }

    Token t = Parser::GetNextToken(in, line);
    if(t.GetTokenType() != EQ && t.GetTokenType() != NEQ && t.GetTokenType() != GT && t.GetTokenType() != LT && t.GetTokenType() != LEQ)
    {
        Parser::PushBackToken(t);
        return t1;
    }
    ParseTree *t2 = CompareExpr(in, line);
    if( t2 == 0 )
    {
        ParseError(*line, "Error at Compare Expression - 1");
        return 0;
    }

    //if we have a token listed above, look for the second factor, and pass it into __Expr
    //t2 would be the second factor
    //the mult follows a similar pattern
    //boop on discord

    //Do Checks
    if( t == EQ )
    {
        return new EqExpr(t.GetLinenum(), t1, t2);
    }
    else if (t == NEQ)
    {
        return new NEqExpr(t.GetLinenum(), t1, t2);
    }
    else if (t == GT)
    {
        return new GtExpr(t.GetLinenum(), t1, t2);
    }
    else if (t == GEQ)
    {
        return new GEqExpr(t.GetLinenum(), t1, t2);
    }
    else if (t == LT)
    {
        return new LtExpr(t.GetLinenum(), t1, t2);
    }
    else if (t == LEQ)
    {
        return new LEqExpr(t.GetLinenum(), t1, t2);
    }
    else
    {
        return 0;
    }

    //PlusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    //return new PlusExpr(t.GetLinenum(), t1, t2)

}

ParseTree *AddExpr(istream *in, int *line)
{
    //cout << "Do We Get To AddExpr?" << endl;

    //Part Done Already - DO NOT TOUCH
    ParseTree *t1 = MulExpr(in, line);
    if( t1 == 0 ) {
        return 0;
    }

    while ( true ) {
        Token t = Parser::GetNextToken(in, line);
        if( t != PLUS && t != MINUS ) {
            Parser::PushBackToken(t);
            return t1;
        }
        ParseTree *t2 = MulExpr(in, line);
        if( t2 == 0 ) {
            ParseError(*line, "Missing expression after operator");
            return 0;
        }
        if( t == PLUS )
            t1 = new PlusExpr(t.GetLinenum(), t1, t2);
        else
            t1 = new MinusExpr(t.GetLinenum(), t1, t2);
    }
}

ParseTree *MulExpr(istream *in, int *line)
{
    //cout << "Do we Get to Mult???" << endl;

    ParseTree *t1 = Factor(in, line);

    if( t1 == 0 ) {
        ParseError(*line, "Error at MulExpr - 1");
        return 0;
    }

    Token t = Parser::GetNextToken(in, line);
    if(t.GetTokenType() != STAR && t.GetTokenType() != SLASH)
    {
        Parser::PushBackToken(t);
        return t1;
    }
    ParseTree *t2 = Factor(in, line);
    if(t2 == 0)
    {
        return 0;
    }
    if( t == STAR )
    {
        return new TimesExpr(t.GetLinenum(), t1, t2);
    }
    else if( t == SLASH )
    {
        return new TimesExpr(t.GetLinenum(), t1, t2);
    }
        //Check back
    else
    {
        return 0;
    }

}

ParseTree *Factor(istream *in, int *line)
{
    // cout << "Do We Get To Factor??" << endl;

    //DO NOT TOUCH
    bool neg = false;
    Token t = Parser::GetNextToken(in, line);

    if( t == MINUS ) {
        neg = true;
    }
    else {
        Parser::PushBackToken(t);
    }

    ParseTree *p1 = Primary(in, line);
    if( p1 == 0 ) {
        ParseError(*line, "Missing primary");
        return 0;
    }

    if( neg ) {
        // handle as -1 * Primary
        return new TimesExpr(t.GetLinenum(), new IConst(t.GetLinenum(), -1), p1);
    }
    else
        return p1;
}

ParseTree *Primary(istream *in, int *line)
{
    // PROCESS TOKEN, IDENTIFY PRIMARY, RETURN SOMETHING
    /*
     * Primary Grammar Rule:
     * Primary := IDENT | ICONST | SCONST | TRUE | FALSE | LPAREN Expr
     */
    // cout << "Do we Get to Primary???" << endl;

    Token t = Parser::GetNextToken(in, line);
    if(t.GetTokenType() == IDENT)
    {

        mapv[t.GetLexeme()]++;
        return new Ident(t);
    }
    else if(t.GetTokenType() == ICONST)
    {
        return new IConst(t);
    }
    else if(t.GetTokenType() == SCONST)
    {
        return new SConst(t);
    }
    else if(t.GetTokenType() == TRUE)
    {
        return new BoolConst(t, TRUE);
    }
    else if(t.GetTokenType() == FALSE)
    {
        return new BoolConst(t, FALSE);
    }
    else if(t.GetTokenType() == LPAREN)
    {
        ParseTree *t1 = Expr(in, line);
        Token t = Parser::GetNextToken(in, line);

        if(t1 == 0)
        {
            ParseError(*line, "Error at Primary -  LPAREN - 1");
            Parser::PushBackToken(t);
            return 0;
        }
        else if(t.GetTokenType() != RPAREN)
        {
            ParseError(*line, "Error at Primary -  LPAREN - 2");
            return 0;
        }
        return t1;
    }
    return 0;
}

