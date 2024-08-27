#include <stdio.h>

//��������6����ԭ����11
//����������2��
//���Լ��ּ���2��
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

enum symtype //snode�ڵ�����ͣ�snode������Ľڵ㣬symset����������
{
	SYM_NULL,
	SYM_IDENTIFIER,//��ʶ��
	SYM_NUMBER,
	SYM_PLUS,    //��
	SYM_MINUS,   //��
	SYM_TIMES,  //�˺�
	SYM_SLASH,  //����
	SYM_ODD,    //����
	SYM_EQU,    //���
	SYM_NEQ,    //����
	SYM_LES,    //С��
	SYM_LEQ,    //С�ڵ���
	SYM_GTR,    //����
	SYM_GEQ,    //���ڵ���
	SYM_LPAREN,
	SYM_RPAREN,
	SYM_COMMA,  //����,  17
	SYM_SEMICOLON,//�ֺ�; 18
	SYM_PERIOD,
	SYM_BECOMES, //��ֵ����:=
	//
    SYM_BEGIN,   // 21
	SYM_END,     // 22
	SYM_IF,      // 23
	SYM_THEN,    // 24
	SYM_WHILE,   //
	SYM_DO,      //
	SYM_CALL,    //
	SYM_CONST,   // �������������
	SYM_VAR,     // ������������� 28
	SYM_PROCEDURE,// ���������
	///�������������ӵķ�������:
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
    //��������
    SYM_PP,  // ++
    SYM_MM,  // --

    SYM_CHAR, // �ַ�����
    SYM_FLOAT, // ����������
    SYM_WRITE, //д
    SYM_WRITECHAR//���ַ�����
   // SYM_RETURN
};

enum idtype
{
	ID_CONSTANT, ID_VARIABLE, ID_PROCEDURE,
	//�����ǿ�������
    ID_CHAR, // �ַ�
    ID_FLOAT // ������
};

enum opcode//�����루���ǻ�����Ե�ָ�����ͣ�����ppt
{
	LIT, OPR, LOD, STO, CAL, INT, JMP, JPC
};

enum oprcode
//����ppt��oprָ�PPT����ֱ�������֣������õ�ö�ٴ���������
{
	OPR_RET, OPR_NEG, OPR_ADD, OPR_MIN,
	OPR_MUL, OPR_DIV, OPR_ODD, OPR_EQU,
	OPR_NEQ, OPR_LES, OPR_GEQ,
	OPR_GTR,
	OPR_LEQ,
	OPR_WRITE,
	OPR_WRITECHAR

};//ԭ������ã������¸���

//������������ɴ�Ż�����ָ���
typedef struct
{
	int f; // function code  ������
	int l; // level  ��β�
	int a; // displacement address  //λ�Ƶ�ַ
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
/* 33 */    "ȱ��step",
/* 34 */    "ȱ��unyill",
/* 35 */    "ȱ��ident",
/* 36 */    "������char��������"

};

//////////////////////////////////////////////////////////////////////
char ch;         // last character read        ����ȡ���ַ�
int  sym;        // last symbol read           ����ȡ�ķ���
char id[MAXIDLEN + 1]; // last identifier read ����ȡ�ı�ʶ��
int  num;        // last number read           ����ȡ������
int  cc;         // character count            �ַ�����
int  ll;         // line length
int  kk;
int  err;
int  cx;         // index of current instruction to be generated.Ҫ���ɵĵ�ǰָ���������
int  level = 0;  //��ǰ�ֳ������ڵĲ��
int  tx = 0;     //��ǰ���ֱ�βָ��

char line[80];

instruction code[CXMAX];//����������ɵĻ������ָ�����

char* word[NRW + 1] =
{
	"", /* place holder */
	"begin", "call", "const", "do", "end","if",
	"odd", "procedure", "then", "var", "while",
    "else",//�����ʼ�Լ�����Ķ�����������
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

};//�����ؼ���
//��������һһ��Ӧ�Ĺ�ϵ��Ҫ�����Ļ�˳��Ҫһ����
int wsym[NRW + 1] =
{
	SYM_NULL, SYM_BEGIN, SYM_CALL, SYM_CONST, SYM_DO, SYM_END,
	SYM_IF, SYM_ODD, SYM_PROCEDURE, SYM_THEN, SYM_VAR, SYM_WHILE,
    SYM_ELSE,//�����ʼ�Լ�����Ķ�����������
	SYM_FOR,
	SYM_REPEAT,
    SYM_UNTIL,
    SYM_TYPEDEF,
    SYM_STEP,
    SYM_CHAR, // �ַ�����
    SYM_FLOAT, // ����������
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

//table��
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
