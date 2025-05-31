#pragma once

#include <variant>
#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <concepts>

#include "utils/concept_utils.h"

namespace oath
{
	template<typename T, typename E>
		requires (!std::is_same_v<T, E>)
	class Result
	{
	public:
		using OkType = T;
		using ErrType = E;

	private:
		std::variant<OkType, ErrType> data;

	public:

		static Result<OkType, ErrType> ok(OkType&& val) { return Result<OkType, ErrType>(std::move(val)); }
		static Result<OkType, ErrType> err(ErrType&& err) { return Result<OkType, ErrType>(std::move(err)); }

	private:
		Result<OkType, ErrType>(std::variant<OkType, ErrType> val) : data(std::move(val)) {}
		explicit Result<OkType, ErrType>(OkType&& val) noexcept : data(std::move(val)) {}
		explicit Result<OkType, ErrType>(ErrType&& err) noexcept : data(std::move(err)) {}

		const OkType& get_ok() const& { return std::get<OkType>(data); }
		const ErrType& get_err() const& { return std::get<ErrType>(data); }

	public:
		bool is_ok() const noexcept { return std::holds_alternative<T>(data); }
		bool is_err() const noexcept { return std::holds_alternative<E>(data); }

		const OkType& unwrap() const& noexcept { assert(is_ok());  return get_ok(); }
		OkType unwrap() && noexcept { assert(is_ok());  return std::move(std::get<OkType>(std::move(data))); }

		const ErrType& unwrap_err() const& noexcept { assert(is_err()); return get_err(); }
		ErrType unwrap_err() && noexcept { assert(is_err()); return std::move(std::get<ErrType>(std::move(data))); }

		const OkType& unwrap_or(const OkType& default_val) const& noexcept { return is_ok() ? get_ok() : default_val; }

		const ErrType& unwrap_err_or(const ErrType& default_err) const& noexcept { return is_err() ? get_err() : default_err; }

		operator bool() const { return is_ok(); }

		void expect(const char* msg) const { if (is_err()) throw std::runtime_error(msg); }

	public: // map
		template<typename Fn>
			requires std::invocable<Fn, const OkType&>
		auto map(Fn&& func) const& -> Result<decltype(func(std::declval<const OkType&>())), ErrType>
		{
			using U = decltype(func(std::declval<const OkType&>()));
			using Ret = Result<U, ErrType>;
			if (is_ok()) return Ret::ok(func(get_ok()));
			else return Ret::err(get_err());
		}

		template<typename Fn>
			requires std::invocable<Fn, OkType&&>
		auto map(Fn&& func) && -> Result<decltype(func(std::declval<OkType&&>())), ErrType>
		{
			using U = decltype(func(std::declval<OkType&&>()));
			using Ret = Result<U, ErrType>;
			if (is_ok()) return Ret::ok(func(std::move(std::get<OkType>(std::move(data)))));
			else return Ret::err(std::move(std::get<ErrType>(std::move(data))));
		}

	public: // map_err
		template<typename Fn>
			requires std::invocable<Fn, const ErrType&>
		auto map_err(Fn&& func) const& -> Result<OkType, decltype(func(std::declval<const ErrType&>()))>
		{
			using U = decltype(func(std::declval<const ErrType&>()));
			using Ret = Result<OkType, U>;
			if (is_ok()) return Ret::ok(get_ok());
			else return Ret::err(func(get_err()));
		}

		template<typename Fn>
			requires std::invocable<Fn, ErrType&&>
		auto map_err(Fn&& func) && -> Result<OkType, decltype(func(std::declval<ErrType&&>()))>
		{
			using U = decltype(func(std::declval<ErrType&&>()));
			using Ret = Result<OkType, U>;
			if (is_ok()) return Ret::ok(std::move(std::get<OkType>(std::move(data))));
			else return Ret::err(func(std::move(std::get<ErrType>(std::move(data)))));
		}

	public: // and_then
		template<typename Fn>
			requires requires
		{
			typename std::invoke_result_t<Fn, const OkType&>::OkType;
			typename std::invoke_result_t<Fn, const OkType&>::ErrType;
			requires std::is_same_v<typename std::invoke_result_t<Fn, const T&>::ErrType, ErrType>;
		}
		auto and_then(Fn&& func) const& -> std::invoke_result_t<Fn, const OkType&>
		{
			using Ret = std::invoke_result_t<Fn, const OkType&>;
			if (is_ok()) return std::invoke(std::forward<Fn>(func), get_ok());
			else return Ret::err(get_err());
		}

		template<typename Fn>
			requires requires
		{
			typename std::invoke_result_t<Fn, OkType&&>::OkType;
			typename std::invoke_result_t<Fn, OkType&&>::ErrType;
			requires std::is_same_v<typename std::invoke_result_t<Fn, T&&>::ErrType, ErrType>;
		}
		auto and_then(Fn&& func) && -> std::invoke_result_t<Fn, OkType&&>
		{
			using Ret = std::invoke_result_t<Fn, OkType&&>;
			if (is_ok()) return std::invoke(std::forward<Fn>(func), std::move(std::get<OkType>(std::move(data))));
			else return Ret::err(std::move(std::get<ErrType>(std::move(data))));
		}

	public: // match
		template<typename OkFn, typename ErrFn>
			requires requires
		{
			std::invocable<OkFn, const OkType&>;
			std::invocable<ErrFn, const ErrType&>;
			std::is_same_v<std::invoke_result_t<OkFn, const OkType&>, std::invoke_result_t<ErrFn, const ErrType&>>;
		}
		decltype(auto) match(OkFn&& ok_func, ErrFn&& err_func) const&
		{
			if (is_ok()) return std::invoke(std::forward<OkFn>(ok_func), get_ok());
			else return std::invoke(std::forward<ErrFn>(err_func), get_err());
		}

		template<typename OkFn, typename ErrFn>
			requires requires
		{
			std::invocable<OkFn, OkType&&>;
			std::invocable<ErrFn, ErrType&&>;
			std::is_same_v<std::invoke_result_t<OkFn, OkType&&>, std::invoke_result_t<ErrFn, ErrType&&>>;
		}
		decltype(auto) match(OkFn&& ok_func, ErrFn&& err_func)&&
		{
			if (is_ok()) return std::invoke(std::forward<OkFn>(ok_func), std::move(std::get<OkType>(std::move(data))));
			else return std::invoke(std::forward<ErrFn>(err_func), std::move(std::get<ErrType>(std::move(data))));
		}
	};
}