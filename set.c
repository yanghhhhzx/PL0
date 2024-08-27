#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "set.h"

//他使用链表来代表集合，集合里面是sym，内容：set.h

symset uniteset(symset s1, symset s2)
//这整个函数的作用是，把s1与s2中的enum全部集合到p中，并且按照从小到大排序
//下面的内容都是比较大小、插入到p中。然后剩下的全部插入p中
{
	symset s;
	snode* p;
//snode是链表里面的一个节点，symset是整个链表，snode里面存储的是enum（枚举）。
	s = p = (snode*) malloc(sizeof(snode));
	while (s1 && s2)
	{
		p->next = (snode*) malloc(sizeof(snode));
		p = p->next;
		if (s1->elem < s2->elem)
		{
			p->elem = s1->elem;
			s1 = s1->next;
		}
		else
		{
			p->elem = s2->elem;
			s2 = s2->next;
		}
	}

	while (s1)
	{
		p->next = (snode*) malloc(sizeof(snode));
		p = p->next;
		p->elem = s1->elem;
		s1 = s1->next;

	}

	while (s2)
	{
		p->next = (snode*) malloc(sizeof(snode));
		p = p->next;
		p->elem = s2->elem;
		s2 = s2->next;
	}

	p->next = NULL;

	return s;
} // uniteset

void setinsert(symset s, int elem)//在symset s中插入指定elem
{
	snode* p = s;
	snode* q;

	while (p->next && p->next->elem < elem)
	{
		p = p->next;
	}

	q = (snode*) malloc(sizeof(snode));
	q->elem = elem;
	q->next = p->next;
	p->next = q;
} // setinsert

symset createset(int elem, .../* SYM_NULL */) //创造集合
{
	va_list list;
	symset s;

	s = (snode*) malloc(sizeof(snode));
	s->next = NULL;
//创建时插入的时候是直接在下一个节点插入的，所以头节点是留空的
	va_start(list, elem);
	while (elem)
	{
		setinsert(s, elem);
		elem = va_arg(list, int);
	}
	va_end(list);
	return s;
} // createset

void destroyset(symset s) //销毁集合
{
	snode* p;

	while (s) //当集合不为空
	{
		p = s;
		s = s->next;
		free(p);
	}
} // destroyset

//原本得代码错了，而且错了很多地方
int inset(int elem, symset s)//检查elem是否在symset中,在的话返回1
{
	///下面是修改的
	while (s->next != NULL){

		s = s->next;//链表的头节点留空
	    if (s->elem == elem)
		   return 1;
	}
    return 0;

/**原本是
    while (s && s->elem < elem)
		s = s->next;
    if (s && s->elem == elem)
        return 1;
    else
        return 0;
*/
} // inset

// EOF set.c
    //  printf("%d\n",&elem);
    //     printf("sysset的为%d\n",&s->elem);
