#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "set.h"

//��ʹ�������������ϣ�����������sym�����ݣ�set.h

symset uniteset(symset s1, symset s2)
//�����������������ǣ���s1��s2�е�enumȫ�����ϵ�p�У����Ұ��մ�С��������
//��������ݶ��ǱȽϴ�С�����뵽p�С�Ȼ��ʣ�µ�ȫ������p��
{
	symset s;
	snode* p;
//snode�����������һ���ڵ㣬symset����������snode����洢����enum��ö�٣���
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

void setinsert(symset s, int elem)//��symset s�в���ָ��elem
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

symset createset(int elem, .../* SYM_NULL */) //���켯��
{
	va_list list;
	symset s;

	s = (snode*) malloc(sizeof(snode));
	s->next = NULL;
//����ʱ�����ʱ����ֱ������һ���ڵ����ģ�����ͷ�ڵ������յ�
	va_start(list, elem);
	while (elem)
	{
		setinsert(s, elem);
		elem = va_arg(list, int);
	}
	va_end(list);
	return s;
} // createset

void destroyset(symset s) //���ټ���
{
	snode* p;

	while (s) //�����ϲ�Ϊ��
	{
		p = s;
		s = s->next;
		free(p);
	}
} // destroyset

//ԭ���ô�����ˣ����Ҵ��˺ܶ�ط�
int inset(int elem, symset s)//���elem�Ƿ���symset��,�ڵĻ�����1
{
	///�������޸ĵ�
	while (s->next != NULL){

		s = s->next;//�����ͷ�ڵ�����
	    if (s->elem == elem)
		   return 1;
	}
    return 0;

/**ԭ����
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
    //     printf("sysset��Ϊ%d\n",&s->elem);
