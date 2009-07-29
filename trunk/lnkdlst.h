#ifndef ZEE_LNKDLST
#define ZEE_LNKDLST

#include <stdlib.h>
#define BOOL int
#define TRUE 1
#define FALSE 0

struct ListNode
{
	char value;
	struct ListNode* prev;
	struct ListNode* next;
};

struct ListNode* insertNode(struct ListNode* insAt, struct ListNode* newNode, char insVal)
{
	if(newNode == NULL) return NULL;
	newNode->value = insVal;
	newNode->prev = insAt;
	newNode->next = insAt->next;
	if(insAt->next != NULL) insAt->next->prev = newNode;
	insAt->next = newNode;
	return newNode;
}

struct ListNode* initNode(struct ListNode* theNode)
{
	theNode->prev = NULL;
	theNode->next = NULL;
}

struct ListNode* insertNodeBef(struct ListNode* insAt, struct ListNode* newNode, char insVal)
{
	if(newNode == NULL) return NULL;
	if(insAt->prev != NULL) insAt->prev->next = newNode;
	newNode->prev = insAt->prev;
	newNode->next = insAt;
	insAt->prev = newNode;
	newNode->value = insVal;
	return newNode;
}

char getVal(struct ListNode* node)
{
	return node->value;
}

#endif
