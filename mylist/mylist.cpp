// Headers
#include "../basic_lib.h"
#include "mylist.h"
#include "../entry/entry.h"

// My linked list definition
MyLL::MyLL(void){
	head = tail = NULL;
	numberOfElement = 0;
}
bool MyLL::empty(void){
	return (head == NULL);
}
void MyLL::push_back(LLNODE *ptr){
	if(head == NULL){
		ptr->prev = ptr->next = NULL;
		head = ptr;
		tail = ptr;
	}
	else{
		ptr->prev = tail;
		ptr->prev->next = ptr;
		ptr->next = NULL;
		tail = ptr;
	}
	numberOfElement ++;
}
LLNODE *MyLL::push_back(Entry ent){
	LLNODE *ptr;
	ptr = (LLNODE *)malloc(sizeof(LLNODE));
	ptr->ent = ent;
	push_back(ptr);
	return ptr;
}
void MyLL::pop_front(void){
	if(!empty()){
		if(head->next != NULL){
			head = head->next;
			free(head->prev);
			head->prev = NULL;
		}
		else{
			free(head);
			head = tail = NULL;
		}
		numberOfElement --;
	}
}
void MyLL::remove(LLNODE *ptr){
	if(head == ptr) head = head->next;
	if(tail == ptr) tail = tail->prev;
	if(ptr->prev != NULL)
		ptr->prev->next = ptr->next;
	if(ptr->next != NULL)
		ptr->next->prev = ptr->prev;
	free(ptr);
	numberOfElement --;
}
Entry MyLL::front(void){
	if(!empty()) return head->ent;
	fprintf(stderr, "Error: linked list is empty!!\n");
	return 0;
}
LLNODE *MyLL::pop_value(){
	LLNODE *ptr;
	bool test = false;
	ptr = head;
	while(ptr != NULL)
	{
		if(ptr->ent.getValue() < Long_lived_THR)
		{
			test = true;
			//printf("Find Short-lived Entry: %f \n",ptr->ent.getValue());
			break;
		}
		else 
			ptr = ptr->next;
	}
	if(test)
		return ptr;
	else
	{
		//printf("No Find Short-lived Entry: %f \n", head->ent.getValue());
		//printf("No Find Short-lived Entry: %f \n", head->next->ent.getValue());
		return head;
	}
	return tail;

}
Entry MyLL::back(void){
	if(!empty()) return tail->ent;
	fprintf(stderr, "Error: linked list is empty!!\n");
	return 0;
}
int MyLL::size(void){
	return numberOfElement;
}
