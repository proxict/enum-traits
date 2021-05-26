#ifndef ENUM_TRAITS_ENUM_TRAITS_HPP_
#define ENUM_TRAITS_ENUM_TRAITS_HPP_

#include <algorithm>
#include <array>
#include <limits>
#include <type_traits>

#if defined(__clang__)
#if __clang_major__ < 4
#error "This version of clang is not supported. Please, use version 4 or higher"
#endif
#elif defined(__GNUC__) || defined(__GNUG__)
#if __GNUC__ < 9
#error "This version of gcc is not supported. Please, use version 9 or higher"
#endif
#elif defined(_MSC_VER)
#if _MSC_VER < 1910
#error "This version of MSVC is not supported. Please, use version 19.10 or higher"
#endif
#else
#error "Unknown compiler - not supported"
#endif

#ifndef ENUM_TRAITS_MAX_NAME_LENGTH
#define ENUM_TRAITS_MAX_NAME_LENGTH 64
#endif

#ifndef ENUM_TRAITS_MIN_ENUM_VALUE
#define ENUM_TRAITS_MIN_ENUM_VALUE std::numeric_limits<int8_t>::min()
#endif

#ifndef ENUM_TRAITS_MAX_ENUM_VALUE
#define ENUM_TRAITS_MAX_ENUM_VALUE std::numeric_limits<int8_t>::max()
#endif

namespace etraits {

namespace detail {
    constexpr bool cstreq(const char* a, const char* b) noexcept {
        return *a == *b && (*a == 0 || cstreq(a + 1, b + 1));
    }
} // namespace detail

template <typename T, std::size_t TMaxSize>
class SizedString {
public:
    template <std::size_t TSize,
              std::size_t... I,
              typename std::enable_if<(TSize < TMaxSize), bool>::type = true>
    constexpr std::array<T, TMaxSize> toArray(T const (&chars)[TSize], std::index_sequence<I...>) noexcept {
        return { { chars[I]..., 0 } };
    }

    template <std::size_t TSize,
              std::size_t... I,
              typename std::enable_if<(TSize >= TMaxSize), bool>::type = false>
    constexpr std::array<T, TMaxSize> toArray(T const (&)[TSize], std::index_sequence<I...>) noexcept {
        static_assert(TSize < TMaxSize,
                      "The default maximum length of enum name is 64 characters. To increase the length, "
                      "define ENUM_TRAITS_MAX_NAME_LENGTH to a higher value");
        return {};
    }

    template <std::size_t TSize>
    constexpr SizedString(T const (&chars)[TSize]) noexcept
        : mData(toArray(chars, std::make_index_sequence<TSize>{})) {}

    constexpr SizedString(std::array<T, TMaxSize> other) noexcept
        : mData(other) {}

    constexpr const T* c_str() const noexcept { return mData.data(); }

private:
    std::array<T, TMaxSize> mData;
};

template <typename T, std::size_t TSize>
constexpr bool operator==(const SizedString<T, TSize>& lhs, const char* rhs) noexcept {
    return detail::cstreq(lhs.c_str(), rhs);
}

template <typename T, std::size_t TSize>
constexpr bool operator!=(const SizedString<T, TSize>& lhs, const char* rhs) noexcept {
    return !detail::cstreq(lhs.c_str(), rhs);
}

template <typename T, std::size_t TSize>
constexpr bool operator==(const char* lhs, const SizedString<T, TSize>& rhs) noexcept {
    return rhs == lhs;
}

template <typename T, std::size_t TSize>
constexpr bool operator!=(const char* lhs, const SizedString<T, TSize>& rhs) noexcept {
    return rhs != lhs;
}

template <typename T, std::size_t TSize1, std::size_t TSize2>
constexpr bool operator==(const SizedString<T, TSize1>& lhs, const SizedString<T, TSize2>& rhs) noexcept {
    return detail::cstreq(lhs.c_str(), rhs.c_str());
}

template <typename T, std::size_t TSize1, std::size_t TSize2>
constexpr bool operator!=(const SizedString<T, TSize1>& lhs, const SizedString<T, TSize2>& rhs) noexcept {
    return !(lhs == rhs);
}

using StaticString = SizedString<char, ENUM_TRAITS_MAX_NAME_LENGTH>;

namespace detail {
    static constexpr std::size_t StringNpos = 0xFFFFFFFF;

    template <std::size_t TSize>
    constexpr std::size_t find(char const (&str)[TSize], const char c, const std::size_t pos = 0) noexcept {
        return pos < TSize ? (str[pos] == c ? pos : find(str, c, pos + 1)) : StringNpos;
    }

    template <std::size_t TSize>
    constexpr std::size_t
    rfind(char const (&str)[TSize], const char c, const std::size_t pos = StringNpos) noexcept {
        // clang-format off
        return pos == 0 ? (str[std::min(pos, TSize - 1)] == c ? pos : StringNpos)
                        : str[std::min(pos, TSize - 1)] == c ? std::min(pos, TSize - 1) : rfind(str, c, std::min(pos, TSize - 1) - 1);
        // clang-format on
    }

    template <std::size_t TSize, std::size_t TOtherSize>
    constexpr bool findSubstr(char const (&str)[TSize],
                              std::size_t index,
                              std::size_t index2,
                              char const (&substr)[TOtherSize]) noexcept {
        // clang-format off
        return index2 == TOtherSize - 1 ? true :
            (index < TSize ? (str[index] == substr[index2] ? findSubstr(str, index + 1, index2 + 1, substr) : false) : false);
        // clang-format on
    }

    template <std::size_t TSize,
              std::size_t TOtherSize,
              typename std::enable_if<(TOtherSize - 1 <= TSize), bool>::type = true>
    constexpr std::size_t
    find(char const (&str)[TSize], char const (&substr)[TOtherSize], const std::size_t pos = 0) noexcept {
        return pos < TSize ? (findSubstr(str, pos, 0, substr) ? pos : find(str, substr, pos + 1))
                           : StringNpos;
    }

    template <typename T, T TValue>
    constexpr bool validateEnum() noexcept {
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
        constexpr std::size_t start = find(__PRETTY_FUNCTION__, "TValue = ") + 9;
        static_assert(start != StringNpos, "Unexpected __PRETTY_FUNCTION__ output");
#endif
#if defined(__clang__)
        constexpr char valueStart = __PRETTY_FUNCTION__[start];
        return (valueStart != '-' && !(valueStart >= '0' && valueStart <= '9'));
#elif defined(__GNUC__) || defined(__GNUG__)
        return find(__PRETTY_FUNCTION__, '(', start) == StringNpos;
#elif defined(_MSC_VER)
        constexpr std::size_t end = sizeof(__FUNCSIG__) - 17;
        return rfind(__FUNCSIG__, ')', end) == StringNpos;
#else
        return false;
#endif
    }

    template <typename T, T TValue>
    struct EnumValid {
        static constexpr bool value = validateEnum<T, TValue>();
    };

    template <typename T, typename std::underlying_type<T>::type TValue>
    struct EnumValidIndex {
        static constexpr bool value = EnumValid<T, static_cast<T>(TValue)>::value;
    };

    template <typename T>
    struct EnumLimits {
        using type = typename std::underlying_type<T>::type;
        template <typename...,
                  typename TType = type,
                  typename std::enable_if<std::is_unsigned<TType>::value, bool>::type = true>
        static constexpr type min() {
            return 0;
        }

        template <typename...,
                  typename TType = type,
                  typename std::enable_if<!std::is_unsigned<TType>::value, bool>::type = true>
        static constexpr type min() {
            return static_cast<type>(ENUM_TRAITS_MIN_ENUM_VALUE);
        }

        static constexpr type max() { return static_cast<type>(ENUM_TRAITS_MAX_ENUM_VALUE); }
    };

    template <typename T>
    using EnumType = typename EnumLimits<T>::type;

    /// min
    template <typename T, short TIndexStop, short TIndex, bool TValid>
    struct EnumMinImpl;

    template <typename T, short TIndexStop, short TIndex>
    struct EnumMinImpl<T, TIndexStop, TIndex, false> {
        static_assert(TIndex < 128 && TIndex > -129, "");
        static constexpr short value =
            EnumMinImpl<T, TIndexStop, TIndex + 1, EnumValidIndex<T, TIndex + 1>::value>::value;
    };

    template <typename T, short TIndexStop, short TIndex>
    struct EnumMinImpl<T, TIndexStop, TIndex, true> {
        static_assert(TIndex < 128 && TIndex > -129, "");
        static constexpr short value = TIndex;
    };

    template <typename T, short TIndexStop>
    struct EnumMinImpl<T, TIndexStop, TIndexStop, false> {
        static constexpr short value = TIndexStop;
    };

    template <typename T, short TIndexStop>
    struct EnumMinImpl<T, TIndexStop, TIndexStop, true> {
        static constexpr short value = TIndexStop;
    };

    template <typename T>
    struct EnumMin {
        static constexpr auto value =
            static_cast<EnumType<T>>(EnumMinImpl<T,
                                                 EnumLimits<T>::max(), // stop at max
                                                 EnumLimits<T>::min(), // start at min
                                                 EnumValidIndex<T, EnumLimits<T>::min()>::value>::value);
    };

    /// max
    template <typename T, short TIndexStop, short TIndex, bool TValid>
    struct EnumMaxImpl;

    template <typename T, short TIndexStop, short TIndex>
    struct EnumMaxImpl<T, TIndexStop, TIndex, false> {
        static_assert(TIndex < 128 && TIndex > -129, "");
        static constexpr short value =
            EnumMaxImpl<T, TIndexStop, TIndex - 1, EnumValidIndex<T, TIndex - 1>::value>::value;
    };

    template <typename T, short TIndexStop, short TIndex>
    struct EnumMaxImpl<T, TIndexStop, TIndex, true> {
        static_assert(TIndex < 128 && TIndex > -129, "");
        static constexpr short value = TIndex;
    };

    template <typename T, short TIndexStop>
    struct EnumMaxImpl<T, TIndexStop, TIndexStop, false> {
        static constexpr short value = TIndexStop;
    };

    template <typename T, short TIndexStop>
    struct EnumMaxImpl<T, TIndexStop, TIndexStop, true> {
        static constexpr short value = TIndexStop;
    };

    template <typename T>
    struct EnumMax {
        static constexpr auto value =
            static_cast<EnumType<T>>(EnumMaxImpl<T,
                                                 EnumLimits<T>::min(), // stop at min
                                                 EnumLimits<T>::max(), // start at max
                                                 EnumValidIndex<T, EnumLimits<T>::max()>::value>::value);
    };

    template <typename T, std::size_t TIndex>
    struct EnumValue {
        using type = typename std::underlying_type<T>::type;
        static constexpr type value = static_cast<type>(TIndex) + EnumMin<T>::value;
    };

    template <typename T>
    struct EnumRange {
        static constexpr std::size_t range() noexcept {
            return static_cast<std::size_t>(EnumMax<T>::value - EnumMin<T>::value + 1);
        }
        static constexpr std::size_t value =
            range() <= (ENUM_TRAITS_MAX_ENUM_VALUE - ENUM_TRAITS_MIN_ENUM_VALUE + 1) ? range() : 0ULL;
    };

    template <typename T, T... TArgs>
    struct Accumulate {
        static constexpr std::size_t accumulate() noexcept { return 0; }

        template <typename... TRest>
        static constexpr std::size_t accumulate(T element, TRest... rest) noexcept {
            return element + accumulate(rest...);
        }

        static constexpr std::size_t value = accumulate(TArgs...);
    };

    template <typename TEnum>
    struct EnumCount {
        template <std::size_t... I>
        static constexpr std::size_t count(std::index_sequence<I...>) noexcept {
            return Accumulate<bool, EnumValidIndex<TEnum, EnumValue<TEnum, I>::value>::value...>::value;
        }

        static constexpr std::size_t value = count(std::make_index_sequence<EnumRange<TEnum>::value>{});
    };

    template <typename TEnum>
    struct EnumValidFlags {
        template <std::size_t... I>
        static constexpr auto getValidIndices(std::index_sequence<I...>) noexcept {
            return std::integer_sequence<bool, EnumValidIndex<TEnum, EnumValue<TEnum, I>::value>::value...>{};
        }

        template <bool... I>
        static constexpr std::array<bool, sizeof...(I)> toArray(std::integer_sequence<bool, I...>) noexcept {
            return { (static_cast<bool>(I))... };
        }

        static constexpr auto pack = getValidIndices(std::make_index_sequence<EnumRange<TEnum>::value>{});

        static constexpr std::array<bool, EnumRange<TEnum>::value> value = toArray(pack);
    };

    template <typename TEnum>
    struct EnumValues {
        static constexpr std::size_t size = EnumCount<TEnum>::value;

        template <std::size_t TSize, std::size_t... I>
        static constexpr std::array<TEnum, size> toArray(TEnum (&enums)[TSize],
                                                         std::index_sequence<I...>) noexcept {
            return { { enums[I]... } };
        }

        template <std::size_t... I>
        static constexpr auto getValues(std::index_sequence<I...>) noexcept {
            constexpr auto validFlags = EnumValidFlags<TEnum>::value;
            TEnum values[size] = {};
            for (std::size_t i = 0, k = 0; i < validFlags.size(); ++i) {
                if (validFlags[i]) {
                    values[k++] = static_cast<TEnum>(
                        static_cast<typename std::underlying_type<TEnum>::type>(i) + EnumMin<TEnum>::value);
                }
            }
            return toArray(values, std::make_index_sequence<size>{});
        }

        static constexpr auto value = getValues(std::make_index_sequence<size>{});
    };

    template <typename T, std::size_t TIndex>
    struct EnumValueFromIndex {
        using type = typename std::underlying_type<T>::type;
        static constexpr type value = static_cast<type>(EnumValues<T>::value[TIndex]);
    };

    template <typename T, T TValue>
    constexpr auto getNameOfValidEnum() noexcept {
        static_assert(EnumValid<T, TValue>::value, "This function only works with valid enum values");

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
        constexpr std::size_t end = sizeof(__PRETTY_FUNCTION__) - 2;
        constexpr std::size_t colonPos = rfind(__PRETTY_FUNCTION__, ':', end);
        static_assert(colonPos != StringNpos, "Unexpected __PRETTY_FUNCTION__ output");
        constexpr std::size_t begin = colonPos + 1;
#elif defined(_MSC_VER)
        constexpr std::size_t end = sizeof(__FUNCSIG__) - 17;
        constexpr std::size_t colonPos = rfind(__FUNCSIG__, ':', end);
        static_assert(colonPos != StringNpos, "Unexpected __FUNCSIG__ output");
        constexpr std::size_t begin = colonPos + 1;
#else
#error "Unknown compiler - not supported"
#endif

        static_assert(begin < end, "Unexpected output: begin must be lower than end");
        constexpr std::size_t size = end - begin;
        char name[size] = {};
        for (std::size_t i = 0; i < size; ++i) {
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
            name[i] = __PRETTY_FUNCTION__[begin + i];
#elif defined(_MSC_VER)
            name[i] = __FUNCSIG__[begin + i];
#else
#error "Unknown compiler - not supported"
#endif
        }
        return StaticString(name);
    }

    template <typename T, T TValue>
    constexpr auto getNameOfInvalidEnum() noexcept {
        static_assert(!EnumValid<T, TValue>::value, "This function only works with invalid enum values");

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
        constexpr std::size_t end = sizeof(__PRETTY_FUNCTION__) - 2;
        constexpr std::size_t spacePos = rfind(__PRETTY_FUNCTION__, ' ');
        static_assert(spacePos != StringNpos, "Unexpected __PRETTY_FUNCTION__ output");
        constexpr std::size_t begin = spacePos + 1;
#elif defined(_MSC_VER)
        constexpr std::size_t end = sizeof(__FUNCSIG__) - 17;
        constexpr std::size_t commaPos = rfind(__FUNCSIG__, ',', end);
        static_assert(commaPos != StringNpos, "Unexpected __FUNCSIG__ output");
        constexpr std::size_t begin = commaPos + 1;
#else
#error "Unknown compiler - not supported"
#endif
        static_assert(begin < end, "Unexpected __PRETTY_FUNCTION__ output: begin must be lower than end");
        constexpr std::size_t size = end - begin;
        char name[size];
        for (std::size_t i = 0; i < size; ++i) {
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
            name[i] = __PRETTY_FUNCTION__[begin + i];
#elif defined(_MSC_VER)
            name[i] = __FUNCSIG__[begin + i];
#else
#error "Unknown compiler - not supported"
#endif
        }
        return StaticString(name);
    }

    template <typename T, T TValue, typename std::enable_if<EnumValid<T, TValue>::value, bool>::type = true>
    constexpr auto getName() noexcept {
        return getNameOfValidEnum<T, TValue>();
    }

    template <typename T, T TValue, typename std::enable_if<!EnumValid<T, TValue>::value, bool>::type = false>
    constexpr auto getName() noexcept {
        return getNameOfInvalidEnum<T, TValue>();
    }

    template <typename TEnum, std::size_t... I>
    constexpr auto getNamesImpl(std::index_sequence<I...>) noexcept {
        return std::array<StaticString, sizeof...(I)>{
            getName<TEnum, static_cast<TEnum>(EnumValueFromIndex<TEnum, I>::value)>()...
        };
    }

    template <typename TEnum>
    constexpr auto getNames() noexcept {
        return getNamesImpl<TEnum>(std::make_index_sequence<EnumCount<TEnum>::value>{});
    }

} // namespace detail

template <typename T>
struct EnumTraits {
    using UnderlyingType = typename std::underlying_type<T>::type;
    using ValueType = T;

    static constexpr ValueType Invalid = static_cast<T>(std::numeric_limits<UnderlyingType>::max());

    static constexpr UnderlyingType min() noexcept { return detail::EnumMin<T>::value; }

    static constexpr UnderlyingType max() noexcept { return detail::EnumMax<T>::value; }

    template <typename...,
              std::size_t TSize = detail::EnumRange<T>::value,
              typename std::enable_if<TSize != 0, bool>::type = true>
    static constexpr auto values() noexcept {
        constexpr auto values = detail::EnumValues<T>::value;
        return values;
    }

    template <typename...,
              std::size_t TSize = detail::EnumRange<T>::value,
              typename std::enable_if<TSize != 0, bool>::type = true>
    static constexpr auto names() noexcept {
        return detail::getNames<T>();
    }

    template <typename...,
              std::size_t TSize = detail::EnumRange<T>::value,
              typename std::enable_if<TSize != 0, bool>::type = true>
    static constexpr StaticString name(const T value) noexcept {
        constexpr auto values = detail::EnumValues<T>::value;
        const auto it = std::find(std::begin(values), std::end(values), value);
        if (it == std::end(values)) {
            return "";
        }
        return detail::getNames<T>()[std::distance(std::begin(values), it)];
    }

    template <typename...,
              std::size_t TSize = detail::EnumRange<T>::value,
              typename std::enable_if<TSize != 0, bool>::type = true>
    static constexpr T fromStr(const char* name) noexcept {
        constexpr auto names = detail::getNames<T>();
        constexpr auto values = detail::EnumValues<T>::value;
        for (std::size_t i = 0; i < names.size(); ++i) {
            if (name == names[i]) {
                return values[i];
            }
        }
        return static_cast<T>(Invalid);
    }
};

// needed even for unnamed enums - GCC bug?
template <typename T>
constexpr typename EnumTraits<T>::ValueType EnumTraits<T>::Invalid;

} // namespace etraits

#endif // ENUM_TRAITS_ENUM_TRAITS_HPP_
