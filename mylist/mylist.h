// Headers
#include "../basic_lib.h"
#include "../entry/entry.h"

// My linked list header
#ifndef MYLL_H
#define MYLL_H
// My linked list
class MyLL{
	public:
		MyLL(void);
		bool empty(void);
		LLNODE *push_back(Entry);
		LLNODE *push_front(Entry);
		void pop_front(void);
		void pop_back(void);
		void remove(LLNODE *);
		LLNODE *pop_value();
		Entry front(void);
		Entry back(void);
		int size(void);
	private:
		int numberOfElement;
		LLNODE *head;
		LLNODE *tail;
		void push_back(LLNODE *);
		void push_front(LLNODE *);
};
#endif
