#include <stdio.h>

//新增加了6个，原本是11
//课设增加又2个
//我自己又加了2个
#define NRW        21     // number of reserved words

#define TXMAX      500    // length of identifier table
#define MAXNUMLEN  14     // maximum number of digits in numbers
#define NSYM       10     // maximum number of symbols in array ssym and csym
#define MAXIDLEN   10     // length of identifiers

#define MAXADDRESS 32767  // maximum address
#define MAXLEVEL   32     // maximum depth of nesting block
#define CXMAX      500    // size of code array

#define MAXSYM     30     // maximum number of symbols

#define STACKSIZE  1000   // maximum storage

enum symtype //snode节点的类型（snode是链表的节点，symset是整个链表）
{
	SYM_NULL,
	SYM_IDENTIFIER,//标识符
	SYM_NUMBER,
	SYM_PLUS,    //加
	SYM_MINUS,   //减
	SYM_TIMES,  //乘号
	SYM_SLASH,  //除号
	SYM_ODD,    //奇数
	SYM_EQU,    //相等
	SYM_NEQ,    //不等
	SYM_LES,    //小于
	SYM_LEQ,    //小于等于
	SYM_GTR,    //大于
	SYM_GEQ,    //大于等于
	SYM_LPAREN,
	SYM_RPAREN,
	SYM_COMMA,  //逗号,  17
	SYM_SEMICOLON,//分号; 18
	SYM_PERIOD,
	SYM_BECOMES, //赋值符号:=
	//
    SYM_BEGIN,   // 21
	SYM_END,     // 22
	SYM_IF,      // 23
	SYM_THEN,    // 24
	SYM_WHILE,   //
	SYM_DO,      //
	SYM_CALL,    //
	SYM_CONST,   // 常数的声明语句
	SYM_VAR,     // 变量的声明语句 28
	SYM_PROCEDURE,// 程序的声明
	///下面是我新增加的符号类型:
	SYM_ELSE,
	SYM_FOR,
	SYM_REPEAT,
    SYM_UNTIL,
    SYM_TYPEDEF,
    SYM_STEP,
    SYM_TEQ,  // *=
    SYM_SEQ,  // /=
    SYM_PEQ,  // +=
    SYM_MEQ,  // -=
    //课设增加
    SYM_PP,  // ++
    SYM_MM,  // --

    SYM_CHAR, // 字符声明
    SYM_FLOAT, // 浮点数声明
    SYM_WRITE, //写
    SYM_WRITECHAR//以字符读出
   // SYM_RETURN
};

enum idtype
{
	ID_CONSTANT, ID_VARIABLE, ID_PROCEDURE,
	//后面是课设增加
    ID_CHAR, // 字符
    ID_FLOAT // 浮点数
};

enum opcode//功能码（就是汇编语言的指令类型）参照ppt
{
	LIT, OPR, LOD, STO, CAL, INT, JMP, JPC
};

enum oprcode
//参照ppt的opr指令，PPT里是直接用数字，这里用的枚举代替了数字
{
	OPR_RET, OPR_NEG, OPR_ADD, OPR_MIN,
	OPR_MUL, OPR_DIV, OPR_ODD, OPR_EQU,
	OPR_NEQ, OPR_LES, OPR_GEQ,
	OPR_GTR,
	OPR_LEQ,
	OPR_WRITE,
	OPR_WRITECHAR

};//原本乱序得，我重新改了

//这个是用来生成存放汇编代码指令的
typedef struct
{
	int f; // function code  功能码
	int l; // level  层次差
	int a; // displacement address  //位移地址
} instruction;

//////////////////////////////////////////////////////////////////////
char* err_msg[] =
{
/*  0 */    "",
/*  1 */    "Found ':=' when expecting '='.",
/*  2 */    "There must be a number to follow '='.",
/*  3 */    "There must be an '=' to follow the identifier.",
/*  4 */    "There must be an identifier to follow 'const', 'var', 'char','float' or 'procedure'.",
/*  5 */    "Missing ',' or ';'.",
/*  6 */    "Incorrect procedure name.",
/*  7 */    "Statement expected.",
/*  8 */    "Follow the statement is an incorrect symbol.",
/*  9 */    "'.' expected.",
/* 10 */    "';' expected.",
/* 11 */    "Undeclared identifier.",
/* 12 */    "Illegal assignment.",
/* 13 */    "':=' expected.",
/* 14 */    "There must be an identifier to follow the 'call'.",
/* 15 */    "A constant or variable can not be called.",
/* 16 */    "'then' expected.",
/* 17 */    "';' or 'end' expected.",
/* 18 */    "'do' expected.",
/* 19 */    "Incorrect symbol.",
/* 20 */    "Relative operators expected.",
/* 21 */    "Procedure identifier can not be in an expression.",
/* 22 */    "Missing ')'.",
/* 23 */    "The symbol can not be followed by a factor.",
/* 24 */    "The symbol can not be as the beginning of an expression.",
/* 25 */    "The number is too great.",
/* 26 */    "",
/* 27 */    "",
/* 28 */    "",
/* 29 */    "",
/* 30 */    "",
/* 31 */    "",
/* 32 */    "There are too many levels.",
/* 33 */    "缺少step",
/* 34 */    "缺少unyill",
/* 35 */    "缺少ident",
/* 36 */    "它不是char类型数据"

};

//////////////////////////////////////////////////////////////////////
char ch;         // last character read        最后读取的字符
int  sym;        // last symbol read           最后读取的符号
char id[MAXIDLEN + 1]; // last identifier read 最后读取的标识符
int  num;        // last number read           最后读取的数字
int  cc;         // character count            字符计数
int  ll;         // line length
int  kk;
int  err;
int  cx;         // index of current instruction to be generated.要生成的当前指令的索引。
int  level = 0;  //当前分程序所在的层次
int  tx = 0;     //当前名字表尾指针

char line[80];

instruction code[CXMAX];//用来存放生成的汇编语言指令代码

char* word[NRW + 1] =
{
	"", /* place holder */
	"begin", "call", "const", "do", "end","if",
	"odd", "procedure", "then", "var", "while",
    "else",//这个开始以及下面的都是我新增的
	"for",
	"repeat",
    "untill",
    "typedef",
    "step",
    "char",
    "float",
    "write",
    "writechar"
   // "return"

};//保留关键字
//跟下面是一一对应的关系！要新增的话顺序要一样。
int wsym[NRW + 1] =
{
	SYM_NULL, SYM_BEGIN, SYM_CALL, SYM_CONST, SYM_DO, SYM_END,
	SYM_IF, SYM_ODD, SYM_PROCEDURE, SYM_THEN, SYM_VAR, SYM_WHILE,
    SYM_ELSE,//这个开始以及下面的都是我新增的
	SYM_FOR,
	SYM_REPEAT,
    SYM_UNTIL,
    SYM_TYPEDEF,
    SYM_STEP,
    SYM_CHAR, // 字符声明
    SYM_FLOAT, // 浮点数声明
    SYM_WRITE,
    SYM_WRITECHAR
   // SYM_RETURN
};

int ssym[NSYM + 1] =
{
	SYM_NULL, SYM_PLUS, SYM_MINUS, SYM_TIMES, SYM_SLASH,
	SYM_LPAREN, SYM_RPAREN, SYM_EQU, SYM_COMMA, SYM_PERIOD, SYM_SEMICOLON
};

char csym[NSYM + 1] =
{
	' ', '+', '-', '*', '/', '(', ')', '=', ',', '.', ';'
};

#define MAXINS   8
char* mnemonic[MAXINS] =
{
	"LIT", "OPR", "LOD", "STO", "CAL", "INT", "JMP", "JPC"
};

//table表
typedef struct
{
	char name[MAXIDLEN + 1];
	int  kind;
	int  value;
} comtab;

comtab table[TXMAX];

typedef struct
{
	char  name[MAXIDLEN + 1];
	int   kind;
	short level;
	short address;
} mask;

FILE* infile;

// EOF PL0.h
