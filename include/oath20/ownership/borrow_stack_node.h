#pragma once
#include "config.h"

#ifdef OATH_DEBUG
#include <iostream>
#endif // OATH_DEBUG


namespace oath
{
	struct BorrowStackNode
	{
		void* origin;
		BorrowStackNode* parent;

		BorrowStackNode(void* o, BorrowStackNode* p) : origin(o), parent(p) {}

#ifdef OATH_DEBUG
		~BorrowStackNode()
		{
			std::cout
				<< "[Debug] Releasing borrow from "
				<< (parent == nullptr ? "Owned" : "Borrowed")
				<< "@" << origin
				<< "\n";
		}
#else
		~BorrowStackNode() = default;
#endif // OATH_DEBUG

	};
}