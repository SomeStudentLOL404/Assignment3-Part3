/*
 * parsetree.h
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include <vector>
#include <map>
using std::vector;
using std::map;

// NodeType represents all possible types
enum NodeType { ERRTYPE, INTTYPE, STRTYPE, BOOLTYPE, IDENTTYPE };

// a "forward declaration" for a class to hold values
class Value;

class ParseTree {
    int			linenum;
    ParseTree	*left;
    ParseTree	*right;

public:
    ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0)
            : linenum(linenum), left(l), right(r) {}

    virtual ~ParseTree() {
        delete left;
        delete right;
    }

    int GetLinenum() const { return linenum; }

    virtual NodeType GetType() const { return ERRTYPE; }

    int LeafCount() const {
        int lc = 0;
        if( left ) lc += left->LeafCount();
        if( right ) lc += right->LeafCount();
        if( left == 0 && right == 0 )
            lc++;
        return lc;
    }
    //Other Methods
    int StringCount() const {
        int sc = 0;
        if( left ) sc += left->StringCount();
        if( right ) sc += right->StringCount();
        if((*this).GetType() == STRTYPE)
            sc++;
        return sc;
    }
    int IdentCount() const {
        int id = 0;
        if( left ) id += left->IdentCount();
        if( right ) id += right->IdentCount();
        if((*this).GetType() == IDENTTYPE)
            id++;
        return id;
    }
};

class StmtList : public ParseTree {

public:
    StmtList(ParseTree *l, ParseTree *r) : ParseTree(0, l, r) {}

};

class IfStatement : public ParseTree {
public:
    IfStatement(int line, ParseTree *ex, ParseTree *stmt) : ParseTree(line, ex, stmt) {}
};

class Assignment : public ParseTree {
public:
    Assignment(int line, ParseTree *lhs, ParseTree *rhs) : ParseTree(line, lhs, rhs) {}
};

class PrintStatement : public ParseTree {
public:
    PrintStatement(int line, ParseTree *e) : ParseTree(line, e) {}
};

class PlusExpr : public ParseTree {
public:
    PlusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class MinusExpr : public ParseTree {
public:
    MinusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class TimesExpr : public ParseTree {
public:
    TimesExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class DivideExpr : public ParseTree {
public:
    DivideExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class LogicAndExpr : public ParseTree {
public:
    LogicAndExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class LogicOrExpr : public ParseTree {
public:
    LogicOrExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class EqExpr : public ParseTree {
public:
    EqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class NEqExpr : public ParseTree {
public:
    NEqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class LtExpr : public ParseTree {
public:
    LtExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class LEqExpr : public ParseTree {
public:
    LEqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class GtExpr : public ParseTree {
public:
    GtExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class GEqExpr : public ParseTree {
public:
    GEqExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};

class IConst : public ParseTree {
    int val;

public:
    IConst(int l, int i) : ParseTree(l), val(i) {}
    IConst(Token& t) : ParseTree(t.GetLinenum()) {
        val = stoi(t.GetLexeme());
    }

    NodeType GetType() const { return INTTYPE; }
};

class BoolConst : public ParseTree {
    bool val;

public:
    BoolConst(Token& t, bool val) : ParseTree(t.GetLinenum()), val(val) {}

    NodeType GetType() const { return BOOLTYPE; }
};

class SConst : public ParseTree {
    string val;

public:
    SConst(Token& t) : ParseTree(t.GetLinenum()) {
        val = t.GetLexeme();
    }

    NodeType GetType() const { return STRTYPE; }
};

class Ident : public ParseTree {
    string id;

public:
    Ident(Token& t) : ParseTree(t.GetLinenum()), id(t.GetLexeme()) {}
    NodeType GetType() const { return IDENTTYPE; }
};

#endif /* PARSETREE_H_ */
