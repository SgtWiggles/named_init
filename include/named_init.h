#pragma once
#include <tuple>
#include <type_traits>
#include <utility>

#ifndef AO_MAX_FIELDS
#define AO_MAX_FIELDS 32
#endif

namespace detail {
static constexpr int MAX_RANK = AO_MAX_FIELDS;
template <int Idx> struct Rank : Rank<Idx - 1> {};
template <> struct Rank<0> {};

struct Empty {};
template <class T, class = void> struct HasFieldPtr : std::false_type {};
template <class T>
struct HasFieldPtr<T, std::void_t<decltype(T::field_ptr)>> : std::true_type {};

template <class T, class = void> struct HasEnabledBraced : std::false_type {};
template <class T>
struct HasEnabledBraced<T, std::void_t<typename T::ZIMPL_HAS_ENABLED_BRACED>>
    : std::true_type {};

template <class... Args> struct BaseFieldTraits {
	static constexpr bool allof = (HasFieldPtr<Args>::value && ... && true);
	static constexpr bool oneof = (HasFieldPtr<Args>::value || ... || false);
};

template <int Idx, class T> void orderedConstruct(T &) {
}
template <int Idx, class T, class Head, class... Args>
void orderedConstruct(T &out, Head &&head, Args &&... args) {
	static constexpr auto field_ptr =
	    std::tuple_element_t<Idx, typename T::ZIMPL_FIELDS>::field_ptr;
	out.*field_ptr = std::move(head);
	orderedConstruct<Idx + 1>(out, std::forward<Args>(args)...);
}

template <class T, class... Args> void construct(T &out, Args &&... args) {
	using traits = BaseFieldTraits<Args...>;
	static_assert(traits::allof || !traits::oneof,
	              "Cannot mix named with ordered initialization");
	static_assert(sizeof...(args) <= std::tuple_size_v<typename T::ZIMPL_FIELDS>,
	              "Too many arguments to initialize type");

	if constexpr (traits::allof)
		(args.construct(out), ...);
	else if constexpr (!traits::oneof) {

		using braced = HasEnabledBraced<T>;
		if constexpr (braced::value)
			orderedConstruct<0>(out, std::forward<Args>(args)...);
		else
			out = T{std::forward<Args>(args)...};
	}
}

} // namespace detail
template <class T, class... Args> T construct(Args &&... args) {
	T ret;
	detail::construct(ret, std::forward<Args>(args)...);
	return ret;
}

#define IMPL_FIELD(NAME)                                                       \
	struct NAME##_ {                                                             \
		void construct(ZIMPL_TYPE_NAME &type) {                                    \
			type.*field_ptr = std::move(val);                                        \
		}                                                                          \
		decltype(NAME) val;                                                        \
		static constexpr decltype(NAME) ZIMPL_TYPE_NAME::*field_ptr =              \
		    &ZIMPL_TYPE_NAME::NAME;                                                \
	};                                                                           \
	static decltype(std::tuple_cat(std::declval<GET_REGISTERED_TYPES()>(),       \
	                               std::declval<std::tuple<NAME##_>>()))         \
	    ZIMPL_TYPE_LIST(                                                         \
	        detail::Rank<std::tuple_size<GET_REGISTERED_TYPES()>::value + 1>);

#define IMPL_FIELD1(NAME) IMPL_FIELD(NAME)
#define IMPL_FIELD2(TYPE, NAME)                                                \
	TYPE NAME;                                                                   \
	IMPL_FIELD(NAME)
#define IMPL_FIELD3(TYPE, NAME, DEFAULT)                                       \
	TYPE NAME = DEFAULT;                                                         \
	IMPL_FIELD(NAME)

#define IMPL_FIELD_GET_MACRO3(_1, _2, _3, NAME, ...) NAME
#define FIELD(...)                                                             \
	IMPL_FIELD_GET_MACRO3(__VA_ARGS__, IMPL_FIELD3, IMPL_FIELD2, IMPL_FIELD1)    \
	(__VA_ARGS__)

#define GET_REGISTERED_TYPES()                                                 \
	decltype(ZIMPL_TYPE_LIST(detail::Rank<detail::MAX_RANK>{}))

#define BEGIN_REFLECT(NAME)                                                    \
	using ZIMPL_TYPE_NAME = NAME;                                                \
	static std::tuple<> ZIMPL_TYPE_LIST(detail::Rank<0>);

#define ENABLE_BRACED_INIT(NAME)                                               \
	using ZIMPL_HAS_ENABLED_BRACED = void;                                       \
	NAME() = default;                                                            \
	template <class... Args> NAME(Args &&... args) {                             \
		detail::construct<NAME>(*this, std::forward<Args>(args)...);               \
	}

#define END_REFLECT() using ZIMPL_FIELDS = GET_REGISTERED_TYPES();

#undef AO_MAX_FIELDS
