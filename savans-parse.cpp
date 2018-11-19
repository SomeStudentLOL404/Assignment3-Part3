/*
 * parse.cpp
 */

#include "parse.h"

// WRAPPER FOR PUSHBACK

map<string, int> identMap;


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
ParseTree *Slist(istream *in, int *line) {
	ParseTree *s = Stmt(in, line);
	if( s == 0 )
		return 0;

	if( Parser::GetNextToken(in, line) != SC ) {
		//cout << s << endl;
        ParseError(*line, "Missing semicolon");
		return 0;
	}

    return new StmtList(s, Slist(in,line));
}

ParseTree *Stmt(istream *in, int *line) {
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
		if( s == 0 ) {
			ParseError(*line, "Invalid Statement");
			return 0;
		}
		break;
	}


	return s;
}

ParseTree *IfStmt(istream *in, int *line) {
    
    
    ParseTree *ex = Expr(in, line);
    if( ex == 0 )
    {
		ParseError(*line, "rr");
        return 0;
    }
    
    Token t = Parser::GetNextToken(in, line);
    
    if(t != THEN)
    {
        Parser::PushBackToken(t);
		ParseError(*line, "helo there");
        return 0;
    }
    
    ParseTree *stmt = Stmt(in, line);
    
    if(stmt == 0)
    {
        ParseError(*line, "hello");
        return 0;
    }
    
    
    //return ex;1
	
    return new IfStatement(t.GetLinenum(), ex, stmt);
}

ParseTree *PrintStmt(istream *in, int *line) {
    //Token t = Parser::GetNextToken(in, line);
    //if(t != PRINT)
    {
        //Parser::PushBackToken(t);
		//ParseError(*line, "hello");
        //return 0;
    }
    
    ParseTree *ex = Expr(in, line);
    if( ex == 0 )
    {
		ParseError(*line, "Invalid statement");
        return 0;
    }
    
    //return 0;
	
	return new PrintStatement(1, ex);
}

ParseTree *Expr(istream *in, int *line) {
	ParseTree *t1 = LogicExpr(in, line);
	if( t1 == 0 ) {
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

ParseTree *LogicExpr(istream *in, int *line) {
	ParseTree *t1 = CompareExpr(in, line);
    //ParseTree *t2 = LogicExpr(in, line);
	if( t1 == 0 ) {
		return 0;
	}
    
    Token t = Parser::GetNextToken(in, line);

	if( t != LOGICAND && t != LOGICOR ) {
		Parser::PushBackToken(t);
		return t1;
        //cout << "Hello" << endl;
	}

    ParseTree *t2 = LogicExpr(in, line);
    
    if( t2 == 0 ) {
		ParseError(*line, "Missing expression after operator");
		return 0;
    }
    
    //return 0;
    if(t == LOGICAND)
    {
        t1 = new LogicAndExpr(t.GetLinenum(), t1, t2);
    }
    else
    {
        t1 = new LogicOrExpr(t.GetLinenum(), t1, t2);
    }
    return t1;
}

ParseTree *CompareExpr(istream *in, int *line) {
	ParseTree *t1 = AddExpr(in, line);
	if( t1 == 0 ) {
		return 0;
	}

    Token t = Parser::GetNextToken(in, line);

	if( t != EQ && t != NEQ && t != GT && t != GEQ && t != LT && t != LEQ ) {
		Parser::PushBackToken(t);
		return t1;
	}

    ParseTree *t2 = CompareExpr(in, line);
    
    if( t2 == 0 ) {
		ParseError(*line, "Missing expression after operator");
		return 0;
    }
    
    if(t == EQ)
    {
        t1 = new EqExpr(t.GetLinenum(), t1, t2);
    }
    else if (t == NEQ)
    {
        t1 = new NEqExpr(t.GetLinenum(), t1, t2);
    }
    else if (t == LT)
    {
        t1 = new LtExpr(t.GetLinenum(), t1, t2);
    }
    else if (t == LEQ)
    {
        t1 = new LEqExpr(t.GetLinenum(), t1, t2);
    }
    else if (t == GT)
    {
        t1 = new GtExpr(t.GetLinenum(), t1, t2);
    }
    else if (t == GEQ)
    {
        t1 = new GEqExpr(t.GetLinenum(), t1, t2);
    }
    else
    {
        ;
    }
    
    return t1;
    
}

ParseTree *AddExpr(istream *in, int *line) {
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
    return t1;
}

ParseTree *MulExpr(istream *in, int *line) {
	ParseTree *t1 = Factor(in, line);
	if( t1 == 0 ) {
		return 0;
	}

    while ( true ) {
		Token t = Parser::GetNextToken(in, line);

		if( t != STAR && t != SLASH ) {
			Parser::PushBackToken(t);
			return t1;
		}

		ParseTree *t2 = Factor(in, line);
		if( t2 == 0 ) {
			ParseError(*line, "Missing expression after operator");
			return 0;
		}

		if( t == STAR )
			t1 = new TimesExpr(t.GetLinenum(), t1, t2);
		else
			t1 = new DivideExpr(t.GetLinenum(), t1, t2);
	}
    return t1;
    // HANDLE OP
    //return 0;
}

ParseTree *Factor(istream *in, int *line) {
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
		//cout << p1 << endl;
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

ParseTree *Primary(istream *in, int *line) {
	ParseTree *tk;
    Token t = Parser::GetNextToken(in, line);

    /*if(t != IDENT && t != ICONST && t != SCONST && t != TRUE && t != FALSE && t != LPAREN && t != SC && t != RPAREN)
    {
	    cout << t << endl;
        Parser::PushBackToken(t);
        ParseError(*line, "hello");
        return 0;
    }*/
    
    if(t == LPAREN)
    {
        ParseTree *ex = Expr(in, line);
    
        if( ex == 0 ) 
        {
		    ParseError(*line, "Missing expression after operator");
		    return 0;
	    }
        
        Token t = Parser::GetNextToken(in, line);
        
        if(t != RPAREN)
        {
           //Parser::PushBackToken(t);
           ParseError(*line, "Invaled Token");
           return 0; 
        }
        return ex;
    }
    
    
    
    if(t == ICONST)
    {
        tk = new IConst(t);
    }
    
    else if(t == SCONST)
    {
        tk = new SConst(t);
    }
    else if(t == IDENT)
    {
        identMap[t.GetLexeme()]++;
        tk = new Ident(t);
    }
    
    else
    {
        if (t == TRUE)
        {
            tk = new BoolConst(t, true);
        }
        else if (t == FALSE)
        {
            tk = new BoolConst(t, false);
        }
        else
        {
            //return tk;
            ;
        }
    }

    return tk;
    
}