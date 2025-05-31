#pragma once

#include "config.h"
#include "handle.h"

namespace oath
{
	template<typename T>
	class Owned
	{
	private:
		Handle<T>* handle;

		explicit Owned(const T& value) : handle(new Handle<T>(value)) {}

	public:
		explicit Owned(Handle<T>* _handle) : handle(_handle) {}
		explicit Owned(Owned<T>&& other) noexcept : handle(other.handle) { other.handle = nullptr; }

		Owned(const Owned<T>&) = delete;
		Owned& operator=(const Owned<T>&) = delete;

		~Owned() { delete handle; }

		static Owned make_owned(T _data) { return { _data }; }

	public:
		T* get() OATH_NOEXCEPT { OATH_CHECK(handle != nullptr); return handle->get(); }
		const T* get() const OATH_NOEXCEPT { OATH_CHECK(handle != nullptr); return handle->get(); }

		T* operator->() OATH_NOEXCEPT { return get(); }
		const T* operator->() const OATH_NOEXCEPT { return get(); }

		Handle<T>* borrow() OATH_NOEXCEPT { OATH_CHECK(handle != nullptr); return handle->borrow_from(this); }

		friend Handle<T>;
	};
}
