#pragma once

#include "config.h"
#include "handle.h"

namespace oath
{
	template<typename T>
	class Borrowed
	{
	private:
		Handle<T>* handle;

	public:
		Borrowed(Handle<T>* h) : handle(h) {}

		~Borrowed() { if (handle) handle.return_handle(); }

	public:
		T* get() OATH_NOEXCEPT { OATH_CHECK(handle != nullptr); return handle->get(); }
		const T* get() const OATH_NOEXCEPT { OATH_CHECK(handle != nullptr); return handle->get(); }

		T* operator->() OATH_NOEXCEPT { return get(); }
		const T* operator->() const OATH_NOEXCEPT { return get(); }

		Handle<T>* borrow() OATH_NOEXCEPT { OATH_CHECK(handle != nullptr); return handle->borrow_from(this); }

		friend Handle<T>;

	};
}