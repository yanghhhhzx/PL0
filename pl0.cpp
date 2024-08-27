// pl0 compiler source code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pl0.h"  //包括很多得声明：
#include "set.c"
///sym是全局变量，代表最后读取的单词或符号，在pl0.h的118行那里声明了。
//////////////////////////////////////////////////////////////////////
// print error message.
void error(int n)
{
	int i;

	printf("      ");
	for (i = 1; i <= cc - 1; i++)
		printf(" ");
	printf("^\n");
	printf("Error %3d: %s\n", n, err_msg[n]);
	err++;
} // error
//在pl0.h有每一个ERROR的原因声明

//////////////////////////////////////////////////////////////////////
void getch(void)///读取下一个字符。
{
	if (cc == ll)  //cc是读取到第几个字符，ll是行长度，如果cc=ll说明读完一行了
	{
		if (feof(infile))
		{
			printf("\nPROGRAM INCOMPLETE\n");
			exit(1);
		}
		ll = cc = 0;
		printf("%5d  ", cx);
		while ( (!feof(infile)) // added & modified by alex 01-02-09
			    && ((ch = getc(infile)) != '\n'))
		{
			printf("%c", ch);
			line[++ll] = ch;
		} // while
		printf("\n");
		line[++ll] = ' ';
	}
	ch = line[++cc];
} // getch

//////////////////////////////////////////////////////////////////////
// gets a symbol from input stream.
void getsym(void)///读取下一个符号，getch是读取下一个字符
{
	int i, k;
	char f;//f是注释符号记录前面一个值的
	char a[MAXIDLEN + 1];//用来存储一整个单词

	while (ch == ' ')//如果为空，
		getch();//读取下一个字符

	if (isalpha(ch))///如果读到的是一个英文字母
	{ // symbol is a reserved word or an identifier.
		k = 0;
		do
		{
			if (k < MAXIDLEN)
				a[k++] = ch;
			getch();
		}
		while (isalpha(ch) || isdigit(ch));//上面这个循环把整个单词放入a中
		a[k] = 0;//为什么要给最后一位赋予0？？不懂。
		strcpy(id, a);//把a拷贝到名为id的字符串，id在pl0中声明了，也是一个字符数组
		word[0] = id;//把新单词放到word的第0号，可以覆盖之前的。
		i = NRW;//17
		///我怕原本的代码可能出错，用我记得的方式改了：
		while ((strcmp(id, word[i--]))!=0);
		// 原本代码：
		// while (strcmp(id, word[i--])); //strcmp用来对比两个字符串，
		/**strcmp用来对比两个字符串，
		//若str1=str2，则返回零；若str1<str2，则返回负数；若str1>str2，则返回正数
		它这里会一直比到若str1>str2，则返回正数为止，
		*/
		if (++i)//如果++i大于1，则为真，说明它不是在0号位找到的，所以word里面本来就有。说明是保留关键字。
           {
               //因为它是i--，如果是在0号找到，i会是-1，这样++i就是0.
			sym = wsym[i]; // symbol is a reserved word 保留字
			//根据它在word中的排，对应它在wsym中的排序。

			/**
			在word中包含的都会被当作是保留关键字，
			*/
            //printf("%d\n",sym);

           }
		else
			sym = SYM_IDENTIFIER;   // symbol is an identifier
	}
	else if (isdigit(ch))
	{ // symbol is a number.
		k = num = 0;
		sym = SYM_NUMBER;
		do
		{
			num = num * 10 + ch - '0';
			k++;
			getch();
		}
		while (isdigit(ch));
		if (k > MAXNUMLEN)
			error(25);     // The number is too great.
	}
	///下面都是它如果是读的是运算符
	else if (ch == ':')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_BECOMES; // :=
			getch();
		}
		else
		{
			sym = SYM_NULL;       // illegal?
		}
	}
	else if (ch == '>')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_GEQ;     // >=
			getch();
		}
		else
		{
			sym = SYM_GTR;     // >
		}
	}
	else if (ch == '<')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_LEQ;     // <=
			getch();
		}
		/**这里是它原本的不等号
		else if (ch == '>')
		{
			sym = SYM_NEQ;     // <>
			getch();
		}
		*/
		else
		{
			sym = SYM_LES;     // <
		}
	}
	//下面是我自己增加的不等号
		else if (ch == '!')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_NEQ;     // !=
			getch();
		}
	}
	//下面是我加的
		else if (ch == '*')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_TEQ;     // *=
			getch();
		}
		else sym=SYM_TIMES;

	}
        else if (ch == '/')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_SEQ;     // /=
			getch();
		}
        else if (ch == '/') // 注释符号//
		{
            while (cc != ll) {//没有读到下一行之前，一直getch不处理
            getch();
        }
            getsym();
		}
        else if (ch == '*')  // 注释符号/*
        {
            f = ch;
            getch();
            while (f != '*' || ch != '/')  //f是前面一个字符，ch是后面一个
            {
                f = ch;
                getch();
            }
            if (i != '*' && ch != '/') error(19);
            else
            {
                getch();
                getsym();
            }
        }
		else sym=SYM_SLASH;
	}
		else if (ch == '+')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_PEQ;     // +=
			getch();
		}
        if (ch == '+')
		{
			sym = SYM_PP;     // 课设新增++
			getch();
		}
		else sym=SYM_PLUS;
	}
		else if (ch == '-')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_MEQ;     // -=
			getch();
		}
        if (ch == '-')
		{
			sym = SYM_MM;     // 课设新增--
			getch();
		}
		else sym=SYM_MINUS;
	}

	else
	{ // other tokens
	    //包括所有的单个字符组成的符号，都在这里识别
		i = NSYM;
		csym[0] = ch;
		while (csym[i--] != ch);
		if (++i)
		{
			sym = ssym[i];
			getch();
		}
		else
		{
			printf("Fatal Error: Unknown character.\n");
			exit(1);
		}
	}
} // getsym

//////////////////////////////////////////////////////////////////////
// generates (assembles) an instruction. 生成汇编指令
/**
gen函数是用来实现代码生成的，生成汇编语言代码
gen有三个参数，分别代表目标代码的功能码，层差和位移量。
生成的代码顺序放在数组code
*/
void gen(int x, int y, int z)
{
	if (cx > CXMAX)
	{
		printf("Fatal Error: Program too long.\n");
		exit(1);
	}
	code[cx].f = x;
	code[cx].l = y;
	code[cx++].a = z;
} // gen

//////////////////////////////////////////////////////////////////////
// tests if error occurs and skips all symbols that do not belongs to s1 or s2.
/**测试它是否在s1中，如果不在就报错。
这里就是用来测试下一个词语是否合法，s1就是合法的，我们需要的
*/
//报错之后会把剩下的所有s1、s2中的sym全部读完
void test(symset s1, symset s2, int n)
{
	symset s;

	if (! inset(sym, s1))//在的话返回1，它加上非就是在的话不执行，
        ///所以就是如果sym不在s1中就报错
	{
		error(n);
		s = uniteset(s1, s2);
		//应该是要取全集，然后读完。
		while(! inset(sym, s))
			getsym();
		destroyset(s);
	}
} // test

//////////////////////////////////////////////////////////////////////
int dx;  // data allocation index 数据分配索引

// enter object(constant, variable or procedre) into table.
//在table中输入对象（常量、变量或过程）。
void enter(int kind)
{
	mask* mk;
	tx++;
	strcpy(table[tx].name, id);
	table[tx].kind = kind;
	switch (kind)
	{
	case ID_CONSTANT:
		if (num > MAXADDRESS)
		{
			error(25); // The number is too great.
			num = 0;
		}
		table[tx].value = num;
		break;
	case ID_VARIABLE:
		mk = (mask*) &table[tx];
		mk->level = level;
		mk->address = dx++;
		break;
    case ID_CHAR://这是我增加的
		mk = (mask*) &table[tx];
		mk->level = level;
		mk->address = dx++;
		break;
    case ID_FLOAT://这是我增加的
		mk = (mask*) &table[tx];
		mk->level = level;
		mk->address = dx++;
		break;
	case ID_PROCEDURE:
		mk = (mask*) &table[tx];
		mk->level = level;
		break;
	} // switch
} // enter

//////////////////////////////////////////////////////////////////////
// locates identifier in symbol table.
//在符号表中查找标识符。
int position(char* id)
{
	int i;
	strcpy(table[0].name, id);
	i = tx + 1;
	while (strcmp(table[--i].name, id) != 0);
	return i;
} // position

//////////////////////////////////////////////////////////////////////
void constdeclaration()  //const常数声明
{
	if (sym == SYM_IDENTIFIER)
	{
		getsym();
		if (sym == SYM_EQU || sym == SYM_BECOMES)
		{
			if (sym == SYM_BECOMES)
				error(1); // Found ':=' when expecting '='.
			getsym();
			if (sym == SYM_NUMBER) //如果下一个符号是数字，那么就是:=number
			{
				enter(ID_CONSTANT);//
				getsym();
			}
			else
			{
				error(2); // There must be a number to follow '='.
			}
		}
		else
		{
			error(3); // There must be an '=' to follow the identifier.
		}
	}
	 else	error(4);
	// "There must be an identifier to follow 'const', 'var', 'char','float' or 'procedure'."
} // constdeclaration

//////////////////////////////////////////////////////////////////////
void vardeclaration(void)//变量声明的检测
{
	if (sym == SYM_IDENTIFIER)
	{
		enter(ID_VARIABLE);
		getsym();
	}
	else
	{
		error(4); // "There must be an identifier to follow 'const', 'var', 'char','float' or 'procedure'.",
	}
} // vardeclaration

//////////////////////////////////////////////////////////////////////
void floatdeclaration(void)//浮点声明的检测
{
	if (sym == SYM_IDENTIFIER)
	{
		enter(ID_FLOAT);
		getsym();
	}
	else
	{
		error(4); // "There must be an identifier to follow 'const', 'var', 'char','float' or 'procedure'.",
	}
} // floatdeclaration

//////////////////////////////////////////////////////////////////////
void chardeclaration(void)//char声明的检测
{
	if (sym == SYM_IDENTIFIER)
	{
		enter(ID_CHAR);
		getsym();
	}
	else
	{
		error(4); // "There must be an identifier to follow 'const', 'var', 'char','float' or 'procedure'.",
	}
} // chardeclaration

//////////////////////////////////////////////////////////////////////
void listcode(int from, int to)
{
	int i;

	printf("\n");
	for (i = from; i < to; i++)
	{
		printf("%5d %s\t%d\t%d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
	}
	printf("\n");
} // listcode

//////////////////////////////////////////////////////////////////////
void factor(symset fsys)//因子处理
{
	void expression(symset fsys);
	int i;
	symset set,set1;
	set1=createset(SYM_RPAREN, SYM_NULL);

	//记得在main函数中的facbegsys加入mm和pp这样这一部才不会报错
	test(facbegsys, fsys, 24); // The symbol can not be as the beginning of an expression.

	while (inset(sym, facbegsys))
	{//printf("factor循环执行\n");
		if (sym == SYM_IDENTIFIER)
		{
		    //printf("抓到ident\n");
			if ((i = position(id)) == 0)
			{
				error(11); // Undeclared identifier.
			}
			else
			{
				switch (table[i].kind)
				{
				    //printf("搜索ident类型\n");
					mask* mk;
				case ID_CONSTANT:
					gen(LIT, 0, table[i].value);
					getsym();
					break;
                case ID_VARIABLE:
                    mk = (mask*) &table[i];
                    gen(LOD, level - mk->level, mk->address);
                    getsym();
                    if (sym == SYM_PP)//处理a++形式的运算
                    {
                        gen(LIT, 0, 1); //在栈顶放入1
                        gen(OPR, 0, 2);//栈顶元素加上次栈顶的元素，放在栈顶
                        gen(STO, level - mk->level, mk->address);//将栈顶元素存储回去那个变量
                        /**
                         因为在a++中存在的式子，是用原来的值来操作，所以我就采用如下的方法：
                         先给它加+1存起来
                         然后再取出，再-1，但是不存储，把值留下来给后续操作。
                        */
                        gen(LOD, level - mk->level, mk->address);//再重新取出来一遍取出来变量原本的值放在栈顶
                        gen(LIT, 0, 1); //在栈顶放入1
                        gen(OPR, 0, 3);//然后再把刚刚加的减回去，然后留在栈顶不存储
                        getsym();
                    }
                    else if (sym == SYM_MM)//a--形式
                    {
                        gen(LIT, 0, 1); //在栈顶放入1
                        gen(OPR, 0, 3);//次栈减去栈顶的元素，放在栈顶
                        gen(STO, level - mk->level, mk->address);//将栈顶元素存储回去那个变量
                        /**
                        方法与a++类似
                        */
                        gen(LOD, level - mk->level, mk->address);//再重新取出来一遍取出来变量原本的值放在栈顶
                        gen(LIT, 0, 1); //在栈顶放入1
                        gen(OPR, 0, 2);//然后再把刚刚减的加回去，然后留在栈顶就部存储
                        getsym();
                    }
					break;
				case ID_PROCEDURE:
					error(21); // Procedure identifier can not be in an expression.
					getsym();
					break;
				} // switch
				//printf("跳出factor循环\n");
				break;
			}

		}
		else
        {
            if (sym == SYM_NUMBER)
		    {
			    if (num > MAXADDRESS)
			    {
				   error(25); // The number is too great.
				   num = 0;
			    }
			    gen(LIT, 0, num);
			    getsym();
		    }
		     else if (sym == SYM_LPAREN)//左括号
		    {
			   getsym();
			   set = uniteset(set1, fsys);
			   expression(set);
               destroyset(set);
			   destroyset(set1);
               if (sym == SYM_RPAREN)//右括号
			   {
				   getsym();
			   }
			   else
			   {
				error(22); // Missing ')'.
			   }
		    }
		    //新增在表达式子中关于++a、--a的顺序的检测
		    else if(sym==SYM_PP)
            {
                getsym();
                if (sym == SYM_IDENTIFIER)//如果后面是ident
                {
                    mask* mk;
                    if (! (i = position(id)))
                        error(11); // Undeclared identifier.
		            else if (table[i].kind != ID_VARIABLE)
		            {
		                error(12); // Illegal assignment.
                        i = 0;
                    }
                    getsym();
                    mk = (mask*) &table[i];//在table表中获取指向i的变量指针
                    gen(LOD, level - mk->level, mk->address);//取出变量原本的值放在栈顶
                    gen(LIT,0,1);//在栈顶放入1
                    gen(OPR, 0, 2);//栈顶元素加上次栈顶的元素，放在栈顶
                    if (i)
		            {
		 	            gen(STO, level - mk->level, mk->address);//将栈顶元素存储回去那个变量

                        gen(LOD, level - mk->level, mk->address);//取出变量原本的值放在栈顶，因为后面还要其他运算要用
		            }
                 }
                 else error(35);//缺少ident
		    }
            else if(sym==SYM_MM)
            {
                getsym();
                if (sym == SYM_IDENTIFIER)//如果后面是ident
                {
                    mask* mk;
                    if (! (i = position(id)))
                        error(11); // Undeclared identifier.
		            else if (table[i].kind != ID_VARIABLE)
		            {
		                error(12); // Illegal assignment.
                        i = 0;
                    }
                    getsym();
                    mk = (mask*) &table[i];//在table表中获取指向i的变量指针
                    gen(LOD, level - mk->level, mk->address);//取出变量原本的值放在栈顶
                    gen(LIT,0,1);//在栈顶放入1
                    gen(OPR, 0, 3);//次栈减去栈顶，放在栈顶
                    if (i)
		            {
		 	            gen(STO, level - mk->level, mk->address);//将栈顶元素存储回去那个变量

                        gen(LOD, level - mk->level, mk->address);//取出变量原本的值放在栈顶，因为后面还要其他运算要用
		            }
                 }
                 else error(35);//缺少ident
		    }

        // 这一句原本它放在外层循环的
		 test(fsys, createset(SYM_LPAREN, SYM_NULL), 23);//The symbol can not be followed by a factor.
		}

	} // while
} // factor

//////////////////////////////////////////////////////////////////////
void term(symset fsys)//项处理
{
	int mulop;
	symset set,set1;

	set1=createset(SYM_TIMES, SYM_SLASH, SYM_NULL);
	set = uniteset(fsys, set1);
	factor(set);
	//printf("跳出factor\n");
	while (sym == SYM_TIMES || sym == SYM_SLASH)//新增关于a++、a--的顺序的检测
	{
		mulop = sym;
		getsym();
		factor(set);
		if (mulop == SYM_TIMES)
		{
			gen(OPR, 0, OPR_MUL);
		}
		else if (mulop == SYM_SLASH)
		{
			gen(OPR, 0, OPR_DIV);
		}
	} // while
	destroyset(set);
    destroyset(set1);
} // term

//////////////////////////////////////////////////////////////////////
void expression(symset fsys)//生成计算表达式的目标代码指令
{
	int addop;
	symset set,set1;

    set1 = createset(SYM_PLUS, SYM_MINUS, SYM_NULL);
	set = uniteset(set1, fsys);

	if (sym == SYM_PLUS || sym == SYM_MINUS)//表达式的第一个+-好视为后面的项取反还是去正
	{
		addop = sym;
		getsym();
		term(set);//对项进行计算，如果是计算结果放在栈顶
		if (addop == SYM_MINUS)//如果是-，就取反
		{
			gen(OPR, 0, OPR_NEG);
		}
		//正的不需要做任何处理
	}
	else
	{
		term(set);//如果开始没有+-号就直接当成普通项
	}

	while (sym == SYM_PLUS || sym == SYM_MINUS)//这里开始就是当成加与减号处理
	{
		addop = sym;
		getsym();
		term(set);//刚才已经把第一个项放在栈顶了，这是接下来的项
		if (addop == SYM_PLUS)
		{
			gen(OPR, 0, OPR_ADD);
		}
		else
		{
			gen(OPR, 0, OPR_MIN);
		}
	} // while

	destroyset(set);
	destroyset(set1);
} // expression

//////////////////////////////////////////////////////////////////////
void condition(symset fsys)//解析条件语句，并生成目标代码
{
	int relop;
	symset set;

	if (sym == SYM_ODD)
	{
		getsym();
		expression(fsys);
		gen(OPR, 0, 6);
	}
	else
	{
		set = uniteset(relset, fsys);
		expression(set);
		destroyset(set);
		if (! inset(sym, relset))
		{
			error(20);
		}
		else
		{
			relop = sym;
			getsym();
			expression(fsys);
			switch (relop)
			{
			case SYM_EQU:
				gen(OPR, 0, OPR_EQU);
				break;
			case SYM_NEQ:
				gen(OPR, 0, OPR_NEQ);
				break;
			case SYM_LES:
				gen(OPR, 0, OPR_LES);
				break;
			case SYM_GEQ:
				gen(OPR, 0, OPR_GEQ);
				break;
			case SYM_GTR:
				gen(OPR, 0, OPR_GTR);
				break;
			case SYM_LEQ:
				gen(OPR, 0, OPR_LEQ);
				break;
			} // switch
		} // else
	} // else
} // condition

//////////////////////////////////////////////////////////////////////
void statement(symset fsys)////解析语句，并生成目标代码
{
	int i, cx1, cx2,a;
	char c;
	symset set1, set;

	if (sym == SYM_IDENTIFIER)
	{ // variable assignment
		mask* mk;
		if (! (i = position(id)))
		{
			error(11); // Undeclared identifier.
		}
        else if (table[i].kind == ID_VARIABLE)
        {
            getsym();
            if (sym == SYM_BECOMES)//这里进行了修改，把内容放入if内了
            {
                getsym();
                expression(fsys);
                mk = (mask*) &table[i];
                if (i)
                {
                    gen(STO, level - mk->level, mk->address);//存储
                }
            }
        }
        else if(table[i].kind == ID_CHAR)//处理字符（C语言中字符也是当做数字处理）
        {
            getsym();
            if (sym == SYM_BECOMES)//这里进行了修改，把内容放入if内了
            {
                getsym();
                expression(fsys);
                mk = (mask*) &table[i];
                if (i)
                {
                    gen(STO, level - mk->level, mk->address);//存储
                }
            }
        }
        else if(table[i].kind == ID_FLOAT)//处理浮点
        {
            getsym();
            if (sym == SYM_BECOMES)
            {
                getsym();
                expression(fsys);//计算表达式后面的值放在栈顶，还没写浮点型的计算规则
                mk = (mask*) &table[i];
                if (i)
                {
                    gen(STO, level - mk->level, mk->address);//存储
                }
            }
        }
        else
        {
            error(12); // Illegal assignment.
            i = 0;
        }

		///下面开始是我增加的
      // SYM_TEQ,  // *=
      // SYM_SEQ,  // /=
         if (sym == SYM_TEQ)
		{
			getsym();
		    mk = (mask*) &table[i];///在table表中获取指向i的变量指针
            gen(LOD, level - mk->level, mk->address);//取出变量原本的值放在栈顶
            if (sym == SYM_SEMICOLON)//如果后面是分号
            {
                getsym();
            }
            expression(fsys);//计算表达式后面的值，然后把他放在栈顶
            gen(OPR, 0, 4);//栈顶和次栈顶的元素相乘，放在栈顶
		    if (i)
		    {
		 	gen(STO, level - mk->level, mk->address);//将栈顶元素存储回去那个变量
		    }
		}
        else if (sym == SYM_SEQ)
		{
			getsym();
		    mk = (mask*) &table[i];///在table表中获取指向i的变量指针
            gen(LOD, level - mk->level, mk->address);//取出变量原本的值放在栈顶
            if (sym == SYM_SEMICOLON)//如果后面是分号
            {
                getsym();
            }
            expression(fsys);//计算表达式后面的值，然后把他放在栈顶
            gen(OPR, 0, 5);//栈顶元素除以次栈顶的元素，放在栈顶
		    if (i)
		    {
		 	gen(STO, level - mk->level, mk->address);//将栈顶元素存储回去那个变量
		    }
		}
        else if (sym == SYM_PEQ)
		{
			getsym();
		    mk = (mask*) &table[i];///在table表中获取指向i的变量指针
            gen(LOD, level - mk->level, mk->address);//取出变量原本的值放在栈顶
            if (sym == SYM_SEMICOLON)//如果后面是分号
            {
                getsym();
            }
            expression(fsys);//计算表达式后面的值，然后把他放在栈顶
            gen(OPR, 0, 2);//栈顶元素加上次栈顶的元素，放在栈顶
		    if (i)
		    {
		 	gen(STO, level - mk->level, mk->address);//将栈顶元素存储回去那个变量
		    }
		}
        else if (sym == SYM_MEQ)
		{
			getsym();
		    mk = (mask*) &table[i];///在table表中获取指向i的变量指针
            gen(LOD, level - mk->level, mk->address);//取出变量原本的值放在栈顶
            if (sym == SYM_SEMICOLON)//如果后面是分号
            {
                getsym();
            }
            expression(fsys);//计算表达式后面的值，然后把他放在栈顶
            gen(OPR, 0, 3);//次栈顶元素减去栈顶的元素，放在栈顶
		    if (i)
		    {
		 	gen(STO, level - mk->level, mk->address);//将栈顶元素存储回去那个变量
		    }
		}
        else if (sym == SYM_PP)
        {
            getsym();
            mk = (mask*) &table[i];///在table表中获取指向i的变量指针
            gen(LOD, level - mk->level, mk->address);//取出变量原本的值放在栈顶
            gen(LIT, 0, 1); //在栈顶放入1
            gen(OPR, 0, 2);//栈顶元素加上次栈顶的元素，放在栈顶
            if (i)
            {
                gen(STO, level - mk->level, mk->address);//将栈顶元素存储回去那个变量
            }
        }
        else if (sym == SYM_MM)
		{
            getsym();
            mk = (mask*) &table[i];///在table表中获取指向i的变量指针
            gen(LOD, level - mk->level, mk->address);//取出变量原本的值放在栈顶
            gen(LIT,0,1);//在栈顶放入1
            gen(OPR, 0, 3);//次栈顶减去栈顶，放在栈顶
            if (i)
            {
                gen(STO, level - mk->level, mk->address);//将栈顶元素存储回去那个变量
            }
		}
	}
    else if (sym == SYM_MM)///这是先识别--再识别ident的
    {
        getsym();
        if (sym == SYM_IDENTIFIER)//如果后面是ident
        {
            mask* mk;
            if (! (i = position(id)))
            {
                error(11); // Undeclared identifier.
            }
            else if (table[i].kind != ID_VARIABLE)
            {
                error(12); // Illegal assignment.
                i = 0;
            }
            getsym();
            mk = (mask*) &table[i];//在table表中获取指向i的变量指针
            gen(LOD, level - mk->level, mk->address);//取出变量原本的值放在栈顶
            gen(LIT, 0, 1); //在栈顶放入1
            gen(OPR, 0, 3);//次栈顶减去栈顶，放在栈顶
            if (i)
            {
                gen(STO, level - mk->level, mk->address);//将栈顶元素存储回去那个变量
            }
        }
        else error(35);//缺少ident
    }

    else if (sym == SYM_PP)///这是先识别++再识别ident的
    {
        getsym();
        if (sym == SYM_IDENTIFIER)//如果后面是ident
        {
            mask* mk;
            if (! (i = position(id)))
            {
                error(11); // Undeclared identifier.
            }
            else if (table[i].kind != ID_VARIABLE)
            {
                error(12); // Illegal assignment.
                i = 0;
            }
            getsym();
            mk = (mask*) &table[i];//在table表中获取指向i的变量指针
            gen(LOD, level - mk->level, mk->address);//取出变量原本的值放在栈顶
            gen(LIT, 0, 1); //在栈顶放入1
            gen(OPR, 0, 2);//栈顶元素加上次栈顶的元素，放在栈顶
            if (i)
            {
                gen(STO, level - mk->level, mk->address);//将栈顶元素存储回去那个变量
            }
        }
        else error(35);//缺少ident
    }
	else if (sym == SYM_CALL)
	{ // procedure call
		getsym();
		if (sym != SYM_IDENTIFIER)
		{
			error(14); // There must be an identifier to follow the 'call'.
		}
		else
		{
			if (! (i = position(id)))
			{
				error(11); // Undeclared identifier.
			}
			else if (table[i].kind == ID_PROCEDURE)
			{
				mask* mk;
				mk = (mask*) &table[i];
				gen(CAL, level - mk->level, mk->address);
			}
			else
			{
				error(15); // A constant or variable can not be called.
			}
			getsym();
		}
	}
	else if (sym == SYM_IF)
	{ // if statement
		getsym();

		set1 = createset(SYM_THEN,SYM_DO, SYM_ELSE,SYM_NULL);//不要忘记加上else
		set = uniteset(set1, fsys);
		condition(set);//解析条件语句,如果最高位为0，后面if才跳转
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_THEN)
		{
			getsym();

        //printf("好好好%d\n",sym);
		}
		else
		{
			error(16); // 'then' expected.
		}
        /**
		这里虽然在code加入了(JPC, 0, 0)指令，但其实他并不是要执行(JPC, 0, 0)
		因为他先记录了本条code的地址放到cx1，（等下要修改这条指令）
		后面在指向完statement(fsys)后，得到了子程序的入口（then后面部分）地址放到了cx
		然后把要JPC跳转的地址改为了cx（子程序入口）
		所以他实际上是放入了下面的指令：
		（JPC ，0，子程序入口）
		*/
		cx1 = cx;
		gen(JPC, 0, 0);//这里第二个0想写啥都行，因为等下会改
		statement(fsys);
        getsym();//必须再getsym一便！！
		if(sym == SYM_ELSE)
        {

            cx2=cx;
            //cx1记录了jpc指令的存放地址，cx2记录了jmp指令的存放地址
            gen(JMP,0,0);
            getsym();
            code[cx1].a = cx;
            //执行完之后cx++
		    statement(fsys);
            code[cx2].a=cx;//让jmp的跳转地址改为子程序入口
        }
        else code[cx1].a = cx;
	}
	else if (sym == SYM_BEGIN)
	{ // block
		getsym();
		set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(set);
		while (sym == SYM_SEMICOLON || inset(sym, statbegsys))
		{
			if (sym == SYM_SEMICOLON)
			{
				getsym();
			}
			else
			{
				error(10);
			}
			statement(set);
		} // while
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_END)
		{
			getsym();
		}
		else
		{
			error(17); // ';' or 'end' expected.
		}
	}
	else if (sym == SYM_WHILE)
	{ // while statement
		cx1 = cx;
		getsym();
		set1 = createset(SYM_DO, SYM_NULL);
		set = uniteset(set1, fsys);
		condition(set);
		destroyset(set1);
		destroyset(set);
		cx2 = cx;
		gen(JPC, 0, 0);
		if (sym == SYM_DO)
		{
			getsym();
		}
		else
		{
			error(18); // 'do' expected.
		}
		statement(fsys);
		gen(JMP, 0, cx1);//无条件跳转回循环开始
		code[cx2].a = cx;//回填
	}
     else if (sym == SYM_FOR)
    {
        getsym();
       if (sym == SYM_IDENTIFIER)
	{ // variable assignment
    //这一步份跟上面的赋值的代码大致一样，我直接复制下来
		mask* mk;
		if (! (i = position(id)))
		{
			error(11); // Undeclared identifier.
		}
		else if (table[i].kind != ID_VARIABLE)
		{
			error(12); // Illegal assignment.
			i = 0;
		}
		getsym();
		if (sym == SYM_BECOMES)
		{
			getsym();
		}
		else
		{
			error(13); // ':=' expected.
		}
        set1 = createset(SYM_STEP, SYM_NULL);//要设置到哪结束，这里一直读到step
		set = uniteset(set1, fsys);
        expression(set);
        //处理完后生成的表达式的值会放在栈顶
        ///这里等于(LIT,0,for后值)
        destroyset(set1);
		destroyset(set);
		mk = (mask*) &table[i];//后续都是用mk指针来获取table[i]中数据的，这里准备要记录循环变量
        if (i)
		{
			gen(STO, level - mk->level, mk->address);//将栈顶内容给赋值对象
			//前面以及把处理后的值放栈顶了
			 //上面这一段完成了for和step之间的赋值语句
            cx1 = cx;//保存循环开始位置
            if (sym == SYM_STEP)
            {
                getsym();
                set1 = createset(SYM_UNTIL, SYM_NULL);//一直读到untill
	        	set = uniteset(set1, fsys);
                expression(set);//处理step后表达式,处理完后放在栈顶
                ///这里等于(LIT,0,step后值)
                destroyset(set1);
		        destroyset(set);
            }
            else error(33); //缺少step
            if (sym == SYM_UNTIL)
            {
                getsym();
                gen(LOD, level - mk->level, mk->address);//将循环判断变量取出放到栈顶
                set1 = createset(SYM_DO, SYM_NULL);//一直读到untill
	        	set = uniteset(set1, fsys);
                expression(set);
                ///这里等于(LIT,0,untill后值)
                destroyset(set1);
		        destroyset(set);
                gen(OPR, 0, 12);//比较栈顶元素与变量大小大小
                cx2 = cx;      //保存循环时指令结束位置
                gen(JPC, 0, 0);
                if (sym == SYM_DO)//处理循环体
                {
                    getsym();
                    statement(fsys);///这里是执行循环内容
                    ///下面是实现循环变量每次增加step的操作
                    gen(LOD, level - mk->level, mk->address);//先将循环变量的值取出放在栈顶
                    gen(OPR, 0, 2);  //循环变量加step留在栈顶
                    gen(STO, level - mk->level, mk->address);//栈顶值存入循环变量覆盖原本的值，实现循环变量的更新
                    //上面是更新循环变量
                    gen(JMP, 0, cx1);//无条件跳转到循环开始位置
                    code[cx2].a = cx;//回填循环结束的位置，这里是jmp执行后面，所以不会再循环
                }
            }else error(34);//缺少untill
        }

    }

    }//FOR循环

    else if(sym == SYM_WRITE)//我自己加的写函数，//opr 0 13也是我加的
    {
        getsym();
        if (sym == SYM_IDENTIFIER)//如果后面是ident
        {
            mask* mk;
            if (! (i = position(id)))
            {
                error(11); // Undeclared identifier.
            }
            else if (table[i].kind != ID_VARIABLE)
            {
                error(12); // Illegal assignment.
                i = 0;
            }

            getsym();
            mk = (mask*) &table[i];//在table表中获取指向i的变量指针
            if (i)
            {
                gen(LOD, level - mk->level, mk->address);//取出变量原本的值放在栈顶
                gen(OPR, 0, OPR_WRITE);
            }
        }

    }
    else if(sym == SYM_WRITECHAR)//我自己加的读出字符函数
    {
        getsym();
        if (sym == SYM_IDENTIFIER)//如果后面是ident
        {
            mask* mk;
            if (! (i = position(id)))
            {
                error(11); // Undeclared identifier.
            }
            else if (table[i].kind != ID_CHAR)
            {
                error(36); // 他不是char
                i = 0;
            }

            getsym();
            mk = (mask*) &table[i];//在table表中获取指向i的变量指针
            if (i)
            {
                gen(LOD, level - mk->level, mk->address);//取出变量原本的值放在栈顶
                gen(OPR, 0, OPR_WRITECHAR);
            }
        }

    }
/** 这是废弃的没写完的功能

    else if (sym == SYM_RETURN)
    {
        getsym();
        if (sym == SYM_IDENTIFIER)//如果后面是ident
        {
            mask* mk;
            if (! (i = position(id)))
            {
                error(11); // Undeclared identifier.
            }
            else if (table[i].kind != ID_VARIABLE)
            {
                error(12); // Illegal assignment.
                i = 0;
            }
            getsym();
            mk = (mask*) &table[i];//在table表中获取指向i的变量指针
            gen(LOD, level - mk->level, mk->address);//取出变量原本的值放在栈顶
        }
    }
*/

  test(fsys, phi, 19);
} // statement

//////////////////////////////////////////////////////////////////////
///编译当前块
void block(symset fsys)//块
// fsys 是当前模块后跟符号集合
{
    //他没有带入，需要加上
   // statbegsys = createset(SYM_BEGIN, SYM_CALL, SYM_IF, SYM_WHILE, SYM_NULL);///跳转语句符号的集合（块的跳转）
	int cx0;   // initial code index 初始代码索引
	mask* mk;  //mk可以保留初始的tx
	int block_dx;
	int savedTx;
	symset set1, set;
//dx是变量在运行栈中相对本过程基地址的偏移量
	dx = 3; //前面3个要空出来
	block_dx = dx;
	mk = (mask*) &table[tx];//记录本层名字的初始位置
	mk->address = cx;
	gen(JMP, 0, 0);  //无条件跳转到0地址
	if (level > MAXLEVEL)
	{
		error(32); // There are too many levels.
	}
	///下面这一个循环是检查声明语句的
	do
	{
		if (sym == SYM_CONST)///检查是不是一个正确的常数声明语句
		{ // constant declarations 声明常数
			getsym();
			do
			{
				constdeclaration();//对赋值语句的检测
				while (sym == SYM_COMMA) //逗号,
				{
					getsym();
					constdeclaration();
				}
				if (sym == SYM_SEMICOLON)//分号;
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			}
			while (sym == SYM_IDENTIFIER);//标识符
		} // if

		if (sym == SYM_VAR)///检查是不是一个正确的变量声明语句
		{ // variable declarations 声明变量
			getsym();
			do
			{
				vardeclaration();
				while (sym == SYM_COMMA)//逗号,
				{
					getsym();
					vardeclaration();
				}
				if (sym == SYM_SEMICOLON)//分号;
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			}
			while (sym == SYM_IDENTIFIER);//标识符
//block = dx;
		} // if

		if (sym == SYM_CHAR)///检查是不是一个正确的字符声明语句
		{
			getsym();
			do
			{
				chardeclaration();//对赋值语句的检测
				while (sym == SYM_COMMA) //逗号,
				{
					getsym();
					chardeclaration();
				}
				if (sym == SYM_SEMICOLON)//分号;
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			}
			while (sym == SYM_CHAR);//标识符
		} // if

		if (sym == SYM_FLOAT)///检查是不是一个正确的浮点型数据声明语句
		{
			getsym();
			do
			{
				floatdeclaration();//对赋值语句的检测
				while (sym == SYM_COMMA) //逗号,
				{
					getsym();
					floatdeclaration();
				}
				if (sym == SYM_SEMICOLON)//分号;
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			}
			while (sym == SYM_FLOAT);//标识符
		} // if

		while (sym == SYM_PROCEDURE)///检查是不是一个正确的过程声明语句
		{ // procedure declarations
			getsym();
			if (sym == SYM_IDENTIFIER)
			{
				enter(ID_PROCEDURE);
				getsym();
			}
			else
			{
				error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
			}


			if (sym == SYM_SEMICOLON)
			{
				getsym();
			}
			else
			{
				error(5); // Missing ',' or ';'.
			}

			level++;      //如果当前是一个正确的过程声明语句，那么它所在的层次肯定是上一个的层次加一
			savedTx = tx; ///记录下当前的tatble中的末位指针的位置，因为执行后面block后它会改变
			set1 = createset(SYM_SEMICOLON, SYM_NULL);
			set = uniteset(set1, fsys);
			block(set); //再对新的程序块执行block
			destroyset(set1);
			destroyset(set);
			tx = savedTx;///执行完上面的过程后因为子过程的过程量就不再需要了，就让table指针回到执行前的位置，后面可以直接覆盖
			level--;//回到当前过程层次

			if (sym == SYM_SEMICOLON)
			{
				getsym();
				set1 = createset(SYM_IDENTIFIER, SYM_PROCEDURE, SYM_NULL);
				set = uniteset(statbegsys, set1);
				test(set, fsys, 6);
				destroyset(set1);
				destroyset(set);
			}
			else
			{
				error(5); // Missing ',' or ';'.
			}

		} // while==procedure

		set1 = createset(SYM_IDENTIFIER,SYM_NULL);
		set = uniteset(statbegsys, set1);//statbegsys里面包含了所有的sta语句符号
		test(set, declbegsys, 7);//declbegsys里面包含了所有的声明语句符号
		///所以它是测试下一个语句是不是station语句，前面依次检测了他是不是各种声明语句，如果是过程声明，会在whlie中一直循环得
		///所以如果不是声明语句，就只能是sta语句
		destroyset(set1);
		destroyset(set);
	}
	while (inset(sym, declbegsys));//inset是检查sym这个节点是否在declbegsys这个链表内

///上面这一个循环是检查声明语句的，从前面的do开始，一直到这里

	code[mk->address].a = cx;
	mk->address = cx;
	cx0 = cx;
	gen(INT, 0, block_dx);//在栈中开辟a个单元的数区
	set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);//读到分号、end会停
	set = uniteset(set1, fsys);
	statement(set);
	destroyset(set1);
	destroyset(set);
	gen(OPR, 0, OPR_RET); // return
	test(fsys, phi, 8); // test for error: Follow the statement is an incorrect symbol.
	listcode(cx0, cx);
} // block

//////////////////////////////////////////////////////////////////////
int base(int stack[], int currentLevel, int levelDiff)
{
	int b = currentLevel;

	while (levelDiff--)
		b = stack[b];
	return b;
} // base

//////////////////////////////////////////////////////////////////////
// interprets and executes codes.
//执行生成的汇编语言指令代码，（之前生成的代码已经存放在code[]中）
void interpret()
{
	int pc;        // program counter程序计数器
	int stack[STACKSIZE];
	int top;       // top of stack
	int b;         // program, base, and top-stack register
	char a; //等下用来输出char类型数据
	instruction i; // instruction register

	printf("Begin executing PL/0 program.\n");

	pc = 0;
	b = 1;
	top = 3;
	stack[1] = stack[2] = stack[3] = 0;

	do
	{
	    printf("\n现在在读第%d条指令\n",pc);
		i = code[pc++];//读取指令
		switch (i.f)//switch指令类型
		{
		case LIT:
			stack[++top] = i.a;
            printf("栈顶插入新元素%d\n",stack[top]);
			break;
		case OPR:
			switch (i.a) // operator
			{
			case OPR_RET://0
				top = b - 1;
				pc = stack[top + 3];
				b = stack[top + 2];
				break;
			case OPR_NEG://1
				stack[top] = -stack[top];
				break;
			case OPR_ADD://2
			    printf("当前准备执行加法命令，栈顶元素为%d\n",stack[top]);
                printf("次栈顶元素为%d\n",stack[--top]);
				stack[top] += stack[top + 1];
				break;
			case OPR_MIN://3
				top--;
				stack[top] -= stack[top + 1];
				break;
			case OPR_MUL://4
				top--;
				stack[top] *= stack[top + 1];
				break;
			case OPR_DIV://5
				top--;
				if (stack[top + 1] == 0)
				{
					fprintf(stderr, "Runtime Error: Divided by zero.\n");
					fprintf(stderr, "Program terminated.\n");
					continue;
				}
				stack[top] /= stack[top + 1];
				break;
			case OPR_ODD://6
			    printf("栈顶%=2操作",stack[top]);
				stack[top] %= 2;
				break;
			case OPR_EQU://7
			    printf("当前准备执行是否等于比较操作，栈顶元素为%d\n",stack[top]);
                printf("次栈顶元素为%d\n",stack[--top]);
				stack[top] = stack[top] == stack[top + 1];
				break;
			case OPR_NEQ://8
			    printf("当前准备执行是否不等于比较操作，栈顶元素为%d\n",stack[top]);
                printf("次栈顶元素为%d\n",stack[--top]);
				stack[top] = stack[top] != stack[top + 1];
				break;
			case OPR_LES://9
			    printf("当前准备执行是否小于比较操作，栈顶元素为%d\n",stack[top]);
                printf("次栈顶元素为%d\n",stack[--top]);
				stack[top] = stack[top] < stack[top + 1];
				break;
			case OPR_GEQ://10
			    printf("次栈顶是否大于等于栈顶，栈顶元素为%d\n",stack[top]);
                printf("次栈顶元素为%d\n",stack[--top]);
                if( stack[top] >= stack[top + 1])
				stack[top] = 1;
				else stack[top] = 0;
				break;
			case OPR_GTR://11
                printf("次栈顶是否大于栈顶，栈顶元素为%d\n",stack[top]);
                printf("次栈顶元素为%d\n",stack[--top]);
                if( stack[top] > stack[top + 1])
				stack[top] = 1;
				else stack[top] = 0;
				break;
			case OPR_LEQ://12
                printf("次栈顶是否小于等于栈顶，栈顶元素为%d\n",stack[top]);
                printf("次栈顶元素为%d\n",stack[--top]);
                if( stack[top] <= stack[top + 1])
				stack[top] = 1;
				else stack[top] = 0;
                printf("比较结果为%d\n",stack[top]);
                break;
            case OPR_WRITE://13 这个是我自己加的
                printf("调用OPR_WRITE读出栈顶元素，读出的结果为%d\n",stack[top]);
                top--;
                break;
            case OPR_WRITECHAR://
                a=stack[top];
                printf("目前栈顶元素的值为%d\n",stack[top]);
                printf("调用OPR_WRITECHAR读出栈顶字符元素，读出的结果为：%c\n",a);
                top--;
                break;

			} // switch
			break;
		case LOD:
            printf("执行取出操作，即将取出的值为%d\n", stack[base(stack, b, i.l) + i.a]);
			stack[++top] = stack[base(stack, b, i.l) + i.a];
			break;
		case STO:
			stack[base(stack, b, i.l) + i.a] = stack[top];
			printf("当前准备执行存储操作，目前栈顶为%d\n", stack[top]);
			top--;
			break;
		case CAL:
			stack[top + 1] = base(stack, b, i.l);
			// generate new block mark
			stack[top + 2] = b;
			stack[top + 3] = pc;
			b = top + 1;
			pc = i.a;
			break;
		case INT:
			top += i.a;
			break;
		case JMP:
            printf("即将执行JMP跳转，入口%d\n",i.a);
			pc = i.a;
			break;
		case JPC:
            printf("即将检测JPC跳转，入口%d，目前栈顶%d\n",i.a,stack[top]);
			if (stack[top] == 0)
				pc = i.a;
			top--;
			break;
		} // switch
	}
	while (pc);

	printf("End executing PL/0 program.\n");
} // interpret

//////////////////////////////////////////////////////////////////////
int main ()
{
    printf("信息安全1班\n");
    printf("姓名：杨宗兴\n");
    printf("学号：3121004716\n");
    printf("开始实验日期：13周星期一\n");
    printf("完成时间：6月28日\n");
	FILE* hbin;
	char s[80];
	int i;
	symset set, set1, set2;

	printf("Please input source file name: "); // get file name to be compiled
	scanf("%s", s);
	if ((infile = fopen(s, "r")) == NULL)
	{
		printf("File %s can't be opened.\n", s);
		exit(1);
	}

	phi = createset(SYM_NULL);
	relset = createset(SYM_EQU, SYM_NEQ, SYM_LES, SYM_LEQ, SYM_GTR, SYM_GEQ, SYM_NULL);//创造集合

	// create begin symbol sets
	declbegsys = createset(SYM_CONST, SYM_VAR, SYM_PROCEDURE, SYM_NULL);///声明语句符号的集合
	statbegsys = createset(SYM_BEGIN, SYM_CALL, SYM_IF, SYM_WHILE, SYM_NULL);///其他陈述语句符号的集合

	facbegsys = createset(SYM_IDENTIFIER, SYM_NUMBER, SYM_LPAREN, SYM_PP,SYM_MM,SYM_NULL);//记得再这里加入mm和pp

	err = cc = cx = ll = 0; // initialize global variables
	ch = ' ';
	kk = MAXIDLEN;

	getsym();

	set1 = createset(SYM_PERIOD, SYM_NULL);
	set2 = uniteset(declbegsys, statbegsys);
	set = uniteset(set1, set2);
	block(set);
	destroyset(set1);
	destroyset(set2);
	destroyset(set);
	destroyset(phi);
	destroyset(relset);
	destroyset(declbegsys);
	destroyset(statbegsys);
	destroyset(facbegsys);

	if (sym != SYM_PERIOD)
		error(9); // '.' expected.
	if (err == 0)
	{
		hbin = fopen("hbin.txt", "w");
		for (i = 0; i < cx; i++)
			fwrite(&code[i], sizeof(instruction), 1, hbin);
		fclose(hbin);
	}
	if (err == 0)
		interpret();
	else
		printf("There are %d error(s) in PL/0 program.\n", err);
	listcode(0, cx);
	return 0;
} // main

//////////////////////////////////////////////////////////////////////
// eof pl0.c
