// pl0 compiler source code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pl0.h"  //�����ܶ��������
#include "set.c"
///sym��ȫ�ֱ�������������ȡ�ĵ��ʻ���ţ���pl0.h��118�����������ˡ�
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
//��pl0.h��ÿһ��ERROR��ԭ������

//////////////////////////////////////////////////////////////////////
void getch(void)///��ȡ��һ���ַ���
{
	if (cc == ll)  //cc�Ƕ�ȡ���ڼ����ַ���ll���г��ȣ����cc=ll˵������һ����
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
void getsym(void)///��ȡ��һ�����ţ�getch�Ƕ�ȡ��һ���ַ�
{
	int i, k;
	char f;//f��ע�ͷ��ż�¼ǰ��һ��ֵ��
	char a[MAXIDLEN + 1];//�����洢һ��������

	while (ch == ' ')//���Ϊ�գ�
		getch();//��ȡ��һ���ַ�

	if (isalpha(ch))///�����������һ��Ӣ����ĸ
	{ // symbol is a reserved word or an identifier.
		k = 0;
		do
		{
			if (k < MAXIDLEN)
				a[k++] = ch;
			getch();
		}
		while (isalpha(ch) || isdigit(ch));//�������ѭ�����������ʷ���a��
		a[k] = 0;//ΪʲôҪ�����һλ����0����������
		strcpy(id, a);//��a��������Ϊid���ַ�����id��pl0�������ˣ�Ҳ��һ���ַ�����
		word[0] = id;//���µ��ʷŵ�word�ĵ�0�ţ����Ը���֮ǰ�ġ�
		i = NRW;//17
		///����ԭ���Ĵ�����ܳ������Ҽǵõķ�ʽ���ˣ�
		while ((strcmp(id, word[i--]))!=0);
		// ԭ�����룺
		// while (strcmp(id, word[i--])); //strcmp�����Ա������ַ�����
		/**strcmp�����Ա������ַ�����
		//��str1=str2���򷵻��㣻��str1<str2���򷵻ظ�������str1>str2���򷵻�����
		�������һֱ�ȵ���str1>str2���򷵻�����Ϊֹ��
		*/
		if (++i)//���++i����1����Ϊ�棬˵����������0��λ�ҵ��ģ�����word���汾�����С�˵���Ǳ����ؼ��֡�
           {
               //��Ϊ����i--���������0���ҵ���i����-1������++i����0.
			sym = wsym[i]; // symbol is a reserved word ������
			//��������word�е��ţ���Ӧ����wsym�е�����

			/**
			��word�а����Ķ��ᱻ�����Ǳ����ؼ��֣�
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
	///���涼��������Ƕ����������
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
		/**��������ԭ���Ĳ��Ⱥ�
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
	//���������Լ����ӵĲ��Ⱥ�
		else if (ch == '!')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_NEQ;     // !=
			getch();
		}
	}
	//�������Ҽӵ�
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
        else if (ch == '/') // ע�ͷ���//
		{
            while (cc != ll) {//û�ж�����һ��֮ǰ��һֱgetch������
            getch();
        }
            getsym();
		}
        else if (ch == '*')  // ע�ͷ���/*
        {
            f = ch;
            getch();
            while (f != '*' || ch != '/')  //f��ǰ��һ���ַ���ch�Ǻ���һ��
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
			sym = SYM_PP;     // ��������++
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
			sym = SYM_MM;     // ��������--
			getch();
		}
		else sym=SYM_MINUS;
	}

	else
	{ // other tokens
	    //�������еĵ����ַ���ɵķ��ţ���������ʶ��
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
// generates (assembles) an instruction. ���ɻ��ָ��
/**
gen����������ʵ�ִ������ɵģ����ɻ�����Դ���
gen�������������ֱ����Ŀ�����Ĺ����룬����λ������
���ɵĴ���˳���������code
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
/**�������Ƿ���s1�У�������ھͱ���
�����������������һ�������Ƿ�Ϸ���s1���ǺϷ��ģ�������Ҫ��
*/
//����֮����ʣ�µ�����s1��s2�е�symȫ������
void test(symset s1, symset s2, int n)
{
	symset s;

	if (! inset(sym, s1))//�ڵĻ�����1�������ϷǾ����ڵĻ���ִ�У�
        ///���Ծ������sym����s1�оͱ���
	{
		error(n);
		s = uniteset(s1, s2);
		//Ӧ����Ҫȡȫ����Ȼ����ꡣ
		while(! inset(sym, s))
			getsym();
		destroyset(s);
	}
} // test

//////////////////////////////////////////////////////////////////////
int dx;  // data allocation index ���ݷ�������

// enter object(constant, variable or procedre) into table.
//��table��������󣨳�������������̣���
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
    case ID_CHAR://���������ӵ�
		mk = (mask*) &table[tx];
		mk->level = level;
		mk->address = dx++;
		break;
    case ID_FLOAT://���������ӵ�
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
//�ڷ��ű��в��ұ�ʶ����
int position(char* id)
{
	int i;
	strcpy(table[0].name, id);
	i = tx + 1;
	while (strcmp(table[--i].name, id) != 0);
	return i;
} // position

//////////////////////////////////////////////////////////////////////
void constdeclaration()  //const��������
{
	if (sym == SYM_IDENTIFIER)
	{
		getsym();
		if (sym == SYM_EQU || sym == SYM_BECOMES)
		{
			if (sym == SYM_BECOMES)
				error(1); // Found ':=' when expecting '='.
			getsym();
			if (sym == SYM_NUMBER) //�����һ�����������֣���ô����:=number
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
void vardeclaration(void)//���������ļ��
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
void floatdeclaration(void)//���������ļ��
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
void chardeclaration(void)//char�����ļ��
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
void factor(symset fsys)//���Ӵ���
{
	void expression(symset fsys);
	int i;
	symset set,set1;
	set1=createset(SYM_RPAREN, SYM_NULL);

	//�ǵ���main�����е�facbegsys����mm��pp������һ���Ų��ᱨ��
	test(facbegsys, fsys, 24); // The symbol can not be as the beginning of an expression.

	while (inset(sym, facbegsys))
	{//printf("factorѭ��ִ��\n");
		if (sym == SYM_IDENTIFIER)
		{
		    //printf("ץ��ident\n");
			if ((i = position(id)) == 0)
			{
				error(11); // Undeclared identifier.
			}
			else
			{
				switch (table[i].kind)
				{
				    //printf("����ident����\n");
					mask* mk;
				case ID_CONSTANT:
					gen(LIT, 0, table[i].value);
					getsym();
					break;
                case ID_VARIABLE:
                    mk = (mask*) &table[i];
                    gen(LOD, level - mk->level, mk->address);
                    getsym();
                    if (sym == SYM_PP)//����a++��ʽ������
                    {
                        gen(LIT, 0, 1); //��ջ������1
                        gen(OPR, 0, 2);//ջ��Ԫ�ؼ��ϴ�ջ����Ԫ�أ�����ջ��
                        gen(STO, level - mk->level, mk->address);//��ջ��Ԫ�ش洢��ȥ�Ǹ�����
                        /**
                         ��Ϊ��a++�д��ڵ�ʽ�ӣ�����ԭ����ֵ�������������ҾͲ������µķ�����
                         �ȸ�����+1������
                         Ȼ����ȡ������-1�����ǲ��洢����ֵ������������������
                        */
                        gen(LOD, level - mk->level, mk->address);//������ȡ����һ��ȡ��������ԭ����ֵ����ջ��
                        gen(LIT, 0, 1); //��ջ������1
                        gen(OPR, 0, 3);//Ȼ���ٰѸոռӵļ���ȥ��Ȼ������ջ�����洢
                        getsym();
                    }
                    else if (sym == SYM_MM)//a--��ʽ
                    {
                        gen(LIT, 0, 1); //��ջ������1
                        gen(OPR, 0, 3);//��ջ��ȥջ����Ԫ�أ�����ջ��
                        gen(STO, level - mk->level, mk->address);//��ջ��Ԫ�ش洢��ȥ�Ǹ�����
                        /**
                        ������a++����
                        */
                        gen(LOD, level - mk->level, mk->address);//������ȡ����һ��ȡ��������ԭ����ֵ����ջ��
                        gen(LIT, 0, 1); //��ջ������1
                        gen(OPR, 0, 2);//Ȼ���ٰѸոռ��ļӻ�ȥ��Ȼ������ջ���Ͳ��洢
                        getsym();
                    }
					break;
				case ID_PROCEDURE:
					error(21); // Procedure identifier can not be in an expression.
					getsym();
					break;
				} // switch
				//printf("����factorѭ��\n");
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
		     else if (sym == SYM_LPAREN)//������
		    {
			   getsym();
			   set = uniteset(set1, fsys);
			   expression(set);
               destroyset(set);
			   destroyset(set1);
               if (sym == SYM_RPAREN)//������
			   {
				   getsym();
			   }
			   else
			   {
				error(22); // Missing ')'.
			   }
		    }
		    //�����ڱ��ʽ���й���++a��--a��˳��ļ��
		    else if(sym==SYM_PP)
            {
                getsym();
                if (sym == SYM_IDENTIFIER)//���������ident
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
                    mk = (mask*) &table[i];//��table���л�ȡָ��i�ı���ָ��
                    gen(LOD, level - mk->level, mk->address);//ȡ������ԭ����ֵ����ջ��
                    gen(LIT,0,1);//��ջ������1
                    gen(OPR, 0, 2);//ջ��Ԫ�ؼ��ϴ�ջ����Ԫ�أ�����ջ��
                    if (i)
		            {
		 	            gen(STO, level - mk->level, mk->address);//��ջ��Ԫ�ش洢��ȥ�Ǹ�����

                        gen(LOD, level - mk->level, mk->address);//ȡ������ԭ����ֵ����ջ������Ϊ���滹Ҫ��������Ҫ��
		            }
                 }
                 else error(35);//ȱ��ident
		    }
            else if(sym==SYM_MM)
            {
                getsym();
                if (sym == SYM_IDENTIFIER)//���������ident
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
                    mk = (mask*) &table[i];//��table���л�ȡָ��i�ı���ָ��
                    gen(LOD, level - mk->level, mk->address);//ȡ������ԭ����ֵ����ջ��
                    gen(LIT,0,1);//��ջ������1
                    gen(OPR, 0, 3);//��ջ��ȥջ��������ջ��
                    if (i)
		            {
		 	            gen(STO, level - mk->level, mk->address);//��ջ��Ԫ�ش洢��ȥ�Ǹ�����

                        gen(LOD, level - mk->level, mk->address);//ȡ������ԭ����ֵ����ջ������Ϊ���滹Ҫ��������Ҫ��
		            }
                 }
                 else error(35);//ȱ��ident
		    }

        // ��һ��ԭ�����������ѭ����
		 test(fsys, createset(SYM_LPAREN, SYM_NULL), 23);//The symbol can not be followed by a factor.
		}

	} // while
} // factor

//////////////////////////////////////////////////////////////////////
void term(symset fsys)//���
{
	int mulop;
	symset set,set1;

	set1=createset(SYM_TIMES, SYM_SLASH, SYM_NULL);
	set = uniteset(fsys, set1);
	factor(set);
	//printf("����factor\n");
	while (sym == SYM_TIMES || sym == SYM_SLASH)//��������a++��a--��˳��ļ��
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
void expression(symset fsys)//���ɼ�����ʽ��Ŀ�����ָ��
{
	int addop;
	symset set,set1;

    set1 = createset(SYM_PLUS, SYM_MINUS, SYM_NULL);
	set = uniteset(set1, fsys);

	if (sym == SYM_PLUS || sym == SYM_MINUS)//���ʽ�ĵ�һ��+-����Ϊ�������ȡ������ȥ��
	{
		addop = sym;
		getsym();
		term(set);//������м��㣬����Ǽ���������ջ��
		if (addop == SYM_MINUS)//�����-����ȡ��
		{
			gen(OPR, 0, OPR_NEG);
		}
		//���Ĳ���Ҫ���κδ���
	}
	else
	{
		term(set);//�����ʼû��+-�ž�ֱ�ӵ�����ͨ��
	}

	while (sym == SYM_PLUS || sym == SYM_MINUS)//���￪ʼ���ǵ��ɼ�����Ŵ���
	{
		addop = sym;
		getsym();
		term(set);//�ղ��Ѿ��ѵ�һ�������ջ���ˣ����ǽ���������
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
void condition(symset fsys)//����������䣬������Ŀ�����
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
void statement(symset fsys)////������䣬������Ŀ�����
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
            if (sym == SYM_BECOMES)//����������޸ģ������ݷ���if����
            {
                getsym();
                expression(fsys);
                mk = (mask*) &table[i];
                if (i)
                {
                    gen(STO, level - mk->level, mk->address);//�洢
                }
            }
        }
        else if(table[i].kind == ID_CHAR)//�����ַ���C�������ַ�Ҳ�ǵ������ִ���
        {
            getsym();
            if (sym == SYM_BECOMES)//����������޸ģ������ݷ���if����
            {
                getsym();
                expression(fsys);
                mk = (mask*) &table[i];
                if (i)
                {
                    gen(STO, level - mk->level, mk->address);//�洢
                }
            }
        }
        else if(table[i].kind == ID_FLOAT)//������
        {
            getsym();
            if (sym == SYM_BECOMES)
            {
                getsym();
                expression(fsys);//������ʽ�����ֵ����ջ������ûд�����͵ļ������
                mk = (mask*) &table[i];
                if (i)
                {
                    gen(STO, level - mk->level, mk->address);//�洢
                }
            }
        }
        else
        {
            error(12); // Illegal assignment.
            i = 0;
        }

		///���濪ʼ�������ӵ�
      // SYM_TEQ,  // *=
      // SYM_SEQ,  // /=
         if (sym == SYM_TEQ)
		{
			getsym();
		    mk = (mask*) &table[i];///��table���л�ȡָ��i�ı���ָ��
            gen(LOD, level - mk->level, mk->address);//ȡ������ԭ����ֵ����ջ��
            if (sym == SYM_SEMICOLON)//��������Ƿֺ�
            {
                getsym();
            }
            expression(fsys);//������ʽ�����ֵ��Ȼ���������ջ��
            gen(OPR, 0, 4);//ջ���ʹ�ջ����Ԫ����ˣ�����ջ��
		    if (i)
		    {
		 	gen(STO, level - mk->level, mk->address);//��ջ��Ԫ�ش洢��ȥ�Ǹ�����
		    }
		}
        else if (sym == SYM_SEQ)
		{
			getsym();
		    mk = (mask*) &table[i];///��table���л�ȡָ��i�ı���ָ��
            gen(LOD, level - mk->level, mk->address);//ȡ������ԭ����ֵ����ջ��
            if (sym == SYM_SEMICOLON)//��������Ƿֺ�
            {
                getsym();
            }
            expression(fsys);//������ʽ�����ֵ��Ȼ���������ջ��
            gen(OPR, 0, 5);//ջ��Ԫ�س��Դ�ջ����Ԫ�أ�����ջ��
		    if (i)
		    {
		 	gen(STO, level - mk->level, mk->address);//��ջ��Ԫ�ش洢��ȥ�Ǹ�����
		    }
		}
        else if (sym == SYM_PEQ)
		{
			getsym();
		    mk = (mask*) &table[i];///��table���л�ȡָ��i�ı���ָ��
            gen(LOD, level - mk->level, mk->address);//ȡ������ԭ����ֵ����ջ��
            if (sym == SYM_SEMICOLON)//��������Ƿֺ�
            {
                getsym();
            }
            expression(fsys);//������ʽ�����ֵ��Ȼ���������ջ��
            gen(OPR, 0, 2);//ջ��Ԫ�ؼ��ϴ�ջ����Ԫ�أ�����ջ��
		    if (i)
		    {
		 	gen(STO, level - mk->level, mk->address);//��ջ��Ԫ�ش洢��ȥ�Ǹ�����
		    }
		}
        else if (sym == SYM_MEQ)
		{
			getsym();
		    mk = (mask*) &table[i];///��table���л�ȡָ��i�ı���ָ��
            gen(LOD, level - mk->level, mk->address);//ȡ������ԭ����ֵ����ջ��
            if (sym == SYM_SEMICOLON)//��������Ƿֺ�
            {
                getsym();
            }
            expression(fsys);//������ʽ�����ֵ��Ȼ���������ջ��
            gen(OPR, 0, 3);//��ջ��Ԫ�ؼ�ȥջ����Ԫ�أ�����ջ��
		    if (i)
		    {
		 	gen(STO, level - mk->level, mk->address);//��ջ��Ԫ�ش洢��ȥ�Ǹ�����
		    }
		}
        else if (sym == SYM_PP)
        {
            getsym();
            mk = (mask*) &table[i];///��table���л�ȡָ��i�ı���ָ��
            gen(LOD, level - mk->level, mk->address);//ȡ������ԭ����ֵ����ջ��
            gen(LIT, 0, 1); //��ջ������1
            gen(OPR, 0, 2);//ջ��Ԫ�ؼ��ϴ�ջ����Ԫ�أ�����ջ��
            if (i)
            {
                gen(STO, level - mk->level, mk->address);//��ջ��Ԫ�ش洢��ȥ�Ǹ�����
            }
        }
        else if (sym == SYM_MM)
		{
            getsym();
            mk = (mask*) &table[i];///��table���л�ȡָ��i�ı���ָ��
            gen(LOD, level - mk->level, mk->address);//ȡ������ԭ����ֵ����ջ��
            gen(LIT,0,1);//��ջ������1
            gen(OPR, 0, 3);//��ջ����ȥջ��������ջ��
            if (i)
            {
                gen(STO, level - mk->level, mk->address);//��ջ��Ԫ�ش洢��ȥ�Ǹ�����
            }
		}
	}
    else if (sym == SYM_MM)///������ʶ��--��ʶ��ident��
    {
        getsym();
        if (sym == SYM_IDENTIFIER)//���������ident
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
            mk = (mask*) &table[i];//��table���л�ȡָ��i�ı���ָ��
            gen(LOD, level - mk->level, mk->address);//ȡ������ԭ����ֵ����ջ��
            gen(LIT, 0, 1); //��ջ������1
            gen(OPR, 0, 3);//��ջ����ȥջ��������ջ��
            if (i)
            {
                gen(STO, level - mk->level, mk->address);//��ջ��Ԫ�ش洢��ȥ�Ǹ�����
            }
        }
        else error(35);//ȱ��ident
    }

    else if (sym == SYM_PP)///������ʶ��++��ʶ��ident��
    {
        getsym();
        if (sym == SYM_IDENTIFIER)//���������ident
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
            mk = (mask*) &table[i];//��table���л�ȡָ��i�ı���ָ��
            gen(LOD, level - mk->level, mk->address);//ȡ������ԭ����ֵ����ջ��
            gen(LIT, 0, 1); //��ջ������1
            gen(OPR, 0, 2);//ջ��Ԫ�ؼ��ϴ�ջ����Ԫ�أ�����ջ��
            if (i)
            {
                gen(STO, level - mk->level, mk->address);//��ջ��Ԫ�ش洢��ȥ�Ǹ�����
            }
        }
        else error(35);//ȱ��ident
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

		set1 = createset(SYM_THEN,SYM_DO, SYM_ELSE,SYM_NULL);//��Ҫ���Ǽ���else
		set = uniteset(set1, fsys);
		condition(set);//�����������,������λΪ0������if����ת
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_THEN)
		{
			getsym();

        //printf("�úú�%d\n",sym);
		}
		else
		{
			error(16); // 'then' expected.
		}
        /**
		������Ȼ��code������(JPC, 0, 0)ָ�����ʵ��������Ҫִ��(JPC, 0, 0)
		��Ϊ���ȼ�¼�˱���code�ĵ�ַ�ŵ�cx1��������Ҫ�޸�����ָ�
		������ָ����statement(fsys)�󣬵õ����ӳ������ڣ�then���沿�֣���ַ�ŵ���cx
		Ȼ���ҪJPC��ת�ĵ�ַ��Ϊ��cx���ӳ�����ڣ�
		������ʵ�����Ƿ����������ָ�
		��JPC ��0���ӳ�����ڣ�
		*/
		cx1 = cx;
		gen(JPC, 0, 0);//����ڶ���0��дɶ���У���Ϊ���»��
		statement(fsys);
        getsym();//������getsymһ�㣡��
		if(sym == SYM_ELSE)
        {

            cx2=cx;
            //cx1��¼��jpcָ��Ĵ�ŵ�ַ��cx2��¼��jmpָ��Ĵ�ŵ�ַ
            gen(JMP,0,0);
            getsym();
            code[cx1].a = cx;
            //ִ����֮��cx++
		    statement(fsys);
            code[cx2].a=cx;//��jmp����ת��ַ��Ϊ�ӳ������
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
		gen(JMP, 0, cx1);//��������ת��ѭ����ʼ
		code[cx2].a = cx;//����
	}
     else if (sym == SYM_FOR)
    {
        getsym();
       if (sym == SYM_IDENTIFIER)
	{ // variable assignment
    //��һ���ݸ�����ĸ�ֵ�Ĵ������һ������ֱ�Ӹ�������
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
        set1 = createset(SYM_STEP, SYM_NULL);//Ҫ���õ��Ľ���������һֱ����step
		set = uniteset(set1, fsys);
        expression(set);
        //����������ɵı��ʽ��ֵ�����ջ��
        ///�������(LIT,0,for��ֵ)
        destroyset(set1);
		destroyset(set);
		mk = (mask*) &table[i];//����������mkָ������ȡtable[i]�����ݵģ�����׼��Ҫ��¼ѭ������
        if (i)
		{
			gen(STO, level - mk->level, mk->address);//��ջ�����ݸ���ֵ����
			//ǰ���Լ��Ѵ�����ֵ��ջ����
			 //������һ�������for��step֮��ĸ�ֵ���
            cx1 = cx;//����ѭ����ʼλ��
            if (sym == SYM_STEP)
            {
                getsym();
                set1 = createset(SYM_UNTIL, SYM_NULL);//һֱ����untill
	        	set = uniteset(set1, fsys);
                expression(set);//����step����ʽ,����������ջ��
                ///�������(LIT,0,step��ֵ)
                destroyset(set1);
		        destroyset(set);
            }
            else error(33); //ȱ��step
            if (sym == SYM_UNTIL)
            {
                getsym();
                gen(LOD, level - mk->level, mk->address);//��ѭ���жϱ���ȡ���ŵ�ջ��
                set1 = createset(SYM_DO, SYM_NULL);//һֱ����untill
	        	set = uniteset(set1, fsys);
                expression(set);
                ///�������(LIT,0,untill��ֵ)
                destroyset(set1);
		        destroyset(set);
                gen(OPR, 0, 12);//�Ƚ�ջ��Ԫ���������С��С
                cx2 = cx;      //����ѭ��ʱָ�����λ��
                gen(JPC, 0, 0);
                if (sym == SYM_DO)//����ѭ����
                {
                    getsym();
                    statement(fsys);///������ִ��ѭ������
                    ///������ʵ��ѭ������ÿ������step�Ĳ���
                    gen(LOD, level - mk->level, mk->address);//�Ƚ�ѭ��������ֵȡ������ջ��
                    gen(OPR, 0, 2);  //ѭ��������step����ջ��
                    gen(STO, level - mk->level, mk->address);//ջ��ֵ����ѭ����������ԭ����ֵ��ʵ��ѭ�������ĸ���
                    //�����Ǹ���ѭ������
                    gen(JMP, 0, cx1);//��������ת��ѭ����ʼλ��
                    code[cx2].a = cx;//����ѭ��������λ�ã�������jmpִ�к��棬���Բ�����ѭ��
                }
            }else error(34);//ȱ��untill
        }

    }

    }//FORѭ��

    else if(sym == SYM_WRITE)//���Լ��ӵ�д������//opr 0 13Ҳ���Ҽӵ�
    {
        getsym();
        if (sym == SYM_IDENTIFIER)//���������ident
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
            mk = (mask*) &table[i];//��table���л�ȡָ��i�ı���ָ��
            if (i)
            {
                gen(LOD, level - mk->level, mk->address);//ȡ������ԭ����ֵ����ջ��
                gen(OPR, 0, OPR_WRITE);
            }
        }

    }
    else if(sym == SYM_WRITECHAR)//���Լ��ӵĶ����ַ�����
    {
        getsym();
        if (sym == SYM_IDENTIFIER)//���������ident
        {
            mask* mk;
            if (! (i = position(id)))
            {
                error(11); // Undeclared identifier.
            }
            else if (table[i].kind != ID_CHAR)
            {
                error(36); // ������char
                i = 0;
            }

            getsym();
            mk = (mask*) &table[i];//��table���л�ȡָ��i�ı���ָ��
            if (i)
            {
                gen(LOD, level - mk->level, mk->address);//ȡ������ԭ����ֵ����ջ��
                gen(OPR, 0, OPR_WRITECHAR);
            }
        }

    }
/** ���Ƿ�����ûд��Ĺ���

    else if (sym == SYM_RETURN)
    {
        getsym();
        if (sym == SYM_IDENTIFIER)//���������ident
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
            mk = (mask*) &table[i];//��table���л�ȡָ��i�ı���ָ��
            gen(LOD, level - mk->level, mk->address);//ȡ������ԭ����ֵ����ջ��
        }
    }
*/

  test(fsys, phi, 19);
} // statement

//////////////////////////////////////////////////////////////////////
///���뵱ǰ��
void block(symset fsys)//��
// fsys �ǵ�ǰģ�������ż���
{
    //��û�д��룬��Ҫ����
   // statbegsys = createset(SYM_BEGIN, SYM_CALL, SYM_IF, SYM_WHILE, SYM_NULL);///��ת�����ŵļ��ϣ������ת��
	int cx0;   // initial code index ��ʼ��������
	mask* mk;  //mk���Ա�����ʼ��tx
	int block_dx;
	int savedTx;
	symset set1, set;
//dx�Ǳ���������ջ����Ա����̻���ַ��ƫ����
	dx = 3; //ǰ��3��Ҫ�ճ���
	block_dx = dx;
	mk = (mask*) &table[tx];//��¼�������ֵĳ�ʼλ��
	mk->address = cx;
	gen(JMP, 0, 0);  //��������ת��0��ַ
	if (level > MAXLEVEL)
	{
		error(32); // There are too many levels.
	}
	///������һ��ѭ���Ǽ����������
	do
	{
		if (sym == SYM_CONST)///����ǲ���һ����ȷ�ĳ����������
		{ // constant declarations ��������
			getsym();
			do
			{
				constdeclaration();//�Ը�ֵ���ļ��
				while (sym == SYM_COMMA) //����,
				{
					getsym();
					constdeclaration();
				}
				if (sym == SYM_SEMICOLON)//�ֺ�;
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			}
			while (sym == SYM_IDENTIFIER);//��ʶ��
		} // if

		if (sym == SYM_VAR)///����ǲ���һ����ȷ�ı����������
		{ // variable declarations ��������
			getsym();
			do
			{
				vardeclaration();
				while (sym == SYM_COMMA)//����,
				{
					getsym();
					vardeclaration();
				}
				if (sym == SYM_SEMICOLON)//�ֺ�;
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			}
			while (sym == SYM_IDENTIFIER);//��ʶ��
//block = dx;
		} // if

		if (sym == SYM_CHAR)///����ǲ���һ����ȷ���ַ��������
		{
			getsym();
			do
			{
				chardeclaration();//�Ը�ֵ���ļ��
				while (sym == SYM_COMMA) //����,
				{
					getsym();
					chardeclaration();
				}
				if (sym == SYM_SEMICOLON)//�ֺ�;
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			}
			while (sym == SYM_CHAR);//��ʶ��
		} // if

		if (sym == SYM_FLOAT)///����ǲ���һ����ȷ�ĸ����������������
		{
			getsym();
			do
			{
				floatdeclaration();//�Ը�ֵ���ļ��
				while (sym == SYM_COMMA) //����,
				{
					getsym();
					floatdeclaration();
				}
				if (sym == SYM_SEMICOLON)//�ֺ�;
				{
					getsym();
				}
				else
				{
					error(5); // Missing ',' or ';'.
				}
			}
			while (sym == SYM_FLOAT);//��ʶ��
		} // if

		while (sym == SYM_PROCEDURE)///����ǲ���һ����ȷ�Ĺ����������
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

			level++;      //�����ǰ��һ����ȷ�Ĺ���������䣬��ô�����ڵĲ�ο϶�����һ���Ĳ�μ�һ
			savedTx = tx; ///��¼�µ�ǰ��tatble�е�ĩλָ���λ�ã���Ϊִ�к���block������ı�
			set1 = createset(SYM_SEMICOLON, SYM_NULL);
			set = uniteset(set1, fsys);
			block(set); //�ٶ��µĳ����ִ��block
			destroyset(set1);
			destroyset(set);
			tx = savedTx;///ִ��������Ĺ��̺���Ϊ�ӹ��̵Ĺ������Ͳ�����Ҫ�ˣ�����tableָ��ص�ִ��ǰ��λ�ã��������ֱ�Ӹ���
			level--;//�ص���ǰ���̲��

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
		set = uniteset(statbegsys, set1);//statbegsys������������е�sta������
		test(set, declbegsys, 7);//declbegsys������������е�����������
		///�������ǲ�����һ������ǲ���station��䣬ǰ�����μ�������ǲ��Ǹ���������䣬����ǹ�������������whlie��һֱѭ����
		///�����������������䣬��ֻ����sta���
		destroyset(set1);
		destroyset(set);
	}
	while (inset(sym, declbegsys));//inset�Ǽ��sym����ڵ��Ƿ���declbegsys���������

///������һ��ѭ���Ǽ���������ģ���ǰ���do��ʼ��һֱ������

	code[mk->address].a = cx;
	mk->address = cx;
	cx0 = cx;
	gen(INT, 0, block_dx);//��ջ�п���a����Ԫ������
	set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);//�����ֺš�end��ͣ
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
//ִ�����ɵĻ������ָ����룬��֮ǰ���ɵĴ����Ѿ������code[]�У�
void interpret()
{
	int pc;        // program counter���������
	int stack[STACKSIZE];
	int top;       // top of stack
	int b;         // program, base, and top-stack register
	char a; //�����������char��������
	instruction i; // instruction register

	printf("Begin executing PL/0 program.\n");

	pc = 0;
	b = 1;
	top = 3;
	stack[1] = stack[2] = stack[3] = 0;

	do
	{
	    printf("\n�����ڶ���%d��ָ��\n",pc);
		i = code[pc++];//��ȡָ��
		switch (i.f)//switchָ������
		{
		case LIT:
			stack[++top] = i.a;
            printf("ջ��������Ԫ��%d\n",stack[top]);
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
			    printf("��ǰ׼��ִ�мӷ����ջ��Ԫ��Ϊ%d\n",stack[top]);
                printf("��ջ��Ԫ��Ϊ%d\n",stack[--top]);
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
			    printf("ջ��%=2����",stack[top]);
				stack[top] %= 2;
				break;
			case OPR_EQU://7
			    printf("��ǰ׼��ִ���Ƿ���ڱȽϲ�����ջ��Ԫ��Ϊ%d\n",stack[top]);
                printf("��ջ��Ԫ��Ϊ%d\n",stack[--top]);
				stack[top] = stack[top] == stack[top + 1];
				break;
			case OPR_NEQ://8
			    printf("��ǰ׼��ִ���Ƿ񲻵��ڱȽϲ�����ջ��Ԫ��Ϊ%d\n",stack[top]);
                printf("��ջ��Ԫ��Ϊ%d\n",stack[--top]);
				stack[top] = stack[top] != stack[top + 1];
				break;
			case OPR_LES://9
			    printf("��ǰ׼��ִ���Ƿ�С�ڱȽϲ�����ջ��Ԫ��Ϊ%d\n",stack[top]);
                printf("��ջ��Ԫ��Ϊ%d\n",stack[--top]);
				stack[top] = stack[top] < stack[top + 1];
				break;
			case OPR_GEQ://10
			    printf("��ջ���Ƿ���ڵ���ջ����ջ��Ԫ��Ϊ%d\n",stack[top]);
                printf("��ջ��Ԫ��Ϊ%d\n",stack[--top]);
                if( stack[top] >= stack[top + 1])
				stack[top] = 1;
				else stack[top] = 0;
				break;
			case OPR_GTR://11
                printf("��ջ���Ƿ����ջ����ջ��Ԫ��Ϊ%d\n",stack[top]);
                printf("��ջ��Ԫ��Ϊ%d\n",stack[--top]);
                if( stack[top] > stack[top + 1])
				stack[top] = 1;
				else stack[top] = 0;
				break;
			case OPR_LEQ://12
                printf("��ջ���Ƿ�С�ڵ���ջ����ջ��Ԫ��Ϊ%d\n",stack[top]);
                printf("��ջ��Ԫ��Ϊ%d\n",stack[--top]);
                if( stack[top] <= stack[top + 1])
				stack[top] = 1;
				else stack[top] = 0;
                printf("�ȽϽ��Ϊ%d\n",stack[top]);
                break;
            case OPR_WRITE://13 ��������Լ��ӵ�
                printf("����OPR_WRITE����ջ��Ԫ�أ������Ľ��Ϊ%d\n",stack[top]);
                top--;
                break;
            case OPR_WRITECHAR://
                a=stack[top];
                printf("Ŀǰջ��Ԫ�ص�ֵΪ%d\n",stack[top]);
                printf("����OPR_WRITECHAR����ջ���ַ�Ԫ�أ������Ľ��Ϊ��%c\n",a);
                top--;
                break;

			} // switch
			break;
		case LOD:
            printf("ִ��ȡ������������ȡ����ֵΪ%d\n", stack[base(stack, b, i.l) + i.a]);
			stack[++top] = stack[base(stack, b, i.l) + i.a];
			break;
		case STO:
			stack[base(stack, b, i.l) + i.a] = stack[top];
			printf("��ǰ׼��ִ�д洢������Ŀǰջ��Ϊ%d\n", stack[top]);
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
            printf("����ִ��JMP��ת�����%d\n",i.a);
			pc = i.a;
			break;
		case JPC:
            printf("�������JPC��ת�����%d��Ŀǰջ��%d\n",i.a,stack[top]);
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
    printf("��Ϣ��ȫ1��\n");
    printf("������������\n");
    printf("ѧ�ţ�3121004716\n");
    printf("��ʼʵ�����ڣ�13������һ\n");
    printf("���ʱ�䣺6��28��\n");
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
	relset = createset(SYM_EQU, SYM_NEQ, SYM_LES, SYM_LEQ, SYM_GTR, SYM_GEQ, SYM_NULL);//���켯��

	// create begin symbol sets
	declbegsys = createset(SYM_CONST, SYM_VAR, SYM_PROCEDURE, SYM_NULL);///���������ŵļ���
	statbegsys = createset(SYM_BEGIN, SYM_CALL, SYM_IF, SYM_WHILE, SYM_NULL);///�������������ŵļ���

	facbegsys = createset(SYM_IDENTIFIER, SYM_NUMBER, SYM_LPAREN, SYM_PP,SYM_MM,SYM_NULL);//�ǵ����������mm��pp

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
