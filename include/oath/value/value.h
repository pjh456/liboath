#pragma once

#include <type_traits>
#include <concepts>

namespace oath::value
{
	template<typename T>
		requires std::is_copy_constructible_v<T>
	class Value
	{
	private:
		T val;

	public:
		explicit Value(const T& t) : val(t) {}
		explicit Value(T&& t) noexcept : val(std::move(t)) {}

		Value(const Value& other) : val(other.val) {}
		Value(Value&& other) noexcept : val(std::move(other.val)) {}

	public:
		const T& get() const { return val; }
		T& get_mut() { return val; }

		operator const T& () const { return val; }
		operator T && () { return std::move(val); }

	public:

		template<typename F>
			requires std::invocable<F, T>
		auto map(F&& func) const&
		{
			using InvokeResult = std::invoke_result_t<F, const T&>;
			return Value<InvokeResult>{ std::invoke(std::forward<F>(func), val) };
		}

		template<typename F> 
			requires std::invocable<F, T>
		auto map(F&& func) && noexcept
		{
			using InvokeResult = std::invoke_result_t<F, T>;
			return Value<InvokeResult>{ std::invoke(std::forward<F>(func), std::move(val)) };
		}
	};
}