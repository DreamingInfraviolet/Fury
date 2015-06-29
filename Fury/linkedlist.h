#pragma once
#include <cstdlib>
#include <initializer_list>
#include <list>

template<class T, class Allocator = std::malloc, class Deallocator = std::free>
class DoublyLinkedList
{
public:
	struct Node
	{
	public:
		T cargo;
		Node* next;
		Node* previous;

		Node()
		{

			next = previous = NULL;
		}

		Node(T& cargo_, Node* previous_, Node* next_ = NULL)
		{
			cargo = cargo_;
			previous = previous_;
			next = next_;
		}

		~Node()
		{
			if (next)
			{
				//next->~Node(); //Is this needed?
				Deallocator(next);
			}
		}
	};

private:
	Node* mHead;
	Node* mTail;
	size_t mSize;
public:

	DoublyLinkedList()
	{
		mHead = NULL;
		mTail = NULL;
		mSize = 0;
	}

	DoublyLinkedList(size_t size, const T& value = T())
	{
		if (size == 0)
			return;

		mHead = Allocator(sizeof(value));
		size_t index = 1;
		mSize = size;

		Node* current = mHead;

		for (; index < size; ++index)
		{
			current->next = Allocator(sizeof(Node));
			current->next->previous = current;
			current->next->cargo = value;
			current = current->next;
		}

		mTail = current;
	}

	DoublyLinkedList(std::initializer_list list)
	{
		//Test above first
	}

	~DoublyLinkedList();
	{
		if (mHead)
			Deallocator(mHead);
	}

	void push_back(const T& value)
	{
		if (mTail)
		{
			mTail->next = Allocator(sizeof(Node));
			mTail->next->previous = mTail;
			mTail = mTail->next;
			mTail->cargo = value;
		}
		else //If empty
		{
			mHead = Allocator(sizeof(Node));
			mTail = mHead;
			mHead.cargo = value;
		}
	}

	void pop_back()
	{
		if (mTail->previous == NULL)
		{
			mHead = NULL;

		}
	}

	void push_front();
	void pop_front();
	T& front();
	T& back();
	size_t size();
	size_t length();
	bool isEmpty();
	bool isFull();
};