#pragma once

#include "config.h"
#include "borrow_stack_node.h"

namespace oath
{
	template<typename T>
	class Owned;
	template<typename T>
	class Borrowed;

	template<typename T>
	class Handle
	{
	private:
		T* data;
		BorrowStackNode* borrow_source;

		explicit Handle(T* ptr) : data(ptr) {}
		~Handle() { delete data; }

		// No copy
		Handle(const Handle<T>&) = delete;
		Handle<T>& operator=(const Handle<T>&) = delete;

		Handle(Handle<T>&& other) noexcept : data(other.data) { other.data = nullptr; }

		friend Owned<T>;
		friend Borrowed<T>;

	public:
		T* get() OATH_NOEXCEPT { OATH_CHECK(data != nullptr); return data; }
		const T* get() OATH_NOEXCEPT { OATH_CHECK(data != nullptr); return data; }

		T* operator->() OATH_NOEXCEPT { return get(); }
		const T* operator->() const OATH_NOEXCEPT { return get(); }

		T& operator*() OATH_NOEXCEPT { return *(get()); }
		const T& operator*() const OATH_NOEXCEPT { return *(get()); }

	public:
		Handle<T>* borrow_from(void* origin) OATH_NOEXCEPT
		{
			borrow_source = new BorrowStackNode(origin, borrow_source);
			return this;
		}

		void return_handle() OATH_NOEXCEPT
		{
			auto parent_node = borrow_source->parent;
			OATH_CHECK(parent_node != nullptr);
			delete borrow_source;
			borrow_source = parent_node;
		}

	};
}