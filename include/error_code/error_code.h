#ifndef INCLUDE_ERROR_CODE_H_
#define INCLUDE_ERROR_CODE_H_

#include <type_traits>
#include <system_error>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

namespace ec {

template <typename T>
struct error_code_traits;

template <typename ErrorCode, typename Traits = error_code_traits<ErrorCode>>
class basic_error_code;

template <typename Base, typename ErrorCode>
class basic_system_error;

namespace detail {

template <typename T>
struct get_system_error;

template <typename T>
struct get_ctor_enabler;

} // namespace detail

template <typename T>
struct error_code_traits {
    using error_code = typename std::decay<T>::type;
    using error_category = typename std::decay<
                decltype(std::declval<error_code>().category())>::type;
    using error_condition = typename std::decay<
                decltype(std::declval<error_code>().default_error_condition())>::type;
    using system_error = typename detail::get_system_error<T>::type;
    using ctor_enabler = typename detail::get_ctor_enabler<T>::type;
};

namespace detail {

namespace sys = ::boost::system;

template <>
struct get_system_error<sys::error_code> {
    using type = basic_system_error<sys::system_error, basic_error_code<sys::error_code>>;
};

template <>
struct get_system_error<std::error_code> {
    using type = basic_system_error<std::system_error, basic_error_code<std::error_code>>;
};

template <>
struct get_ctor_enabler<sys::error_code> {
    struct type {
        template <typename ErrorConditionEnum, typename T = void>
        using enable_for = typename std::enable_if<
                sys::is_error_condition_enum<ErrorConditionEnum>, T>::type;
    };
};

template <>
struct get_ctor_enabler<std::error_code> {
    struct type {
        template <typename ErrorConditionEnum, typename T = void>
        using enable_for = typename std::enable_if<
                std::is_error_condition_enum<ErrorConditionEnum>, T>::type;
    };
};

} // namespace detail

template <typename ErrorCode, typename Traits>
class basic_error_code {
public:
    using base_type = ErrorCode;
    using error_category = typename Traits::error_category;
    using error_condition = typename Traits::error_condition;
    using system_error = typename Traits::system_error;
    using ce = typename Traits::ctor_enabler;

    basic_error_code() noexcept = default;

    basic_error_code(base_type e) noexcept : base_(std::move(e)) {}

    basic_error_code(base_type e, std::string what)
    noexcept(std::is_nothrow_move_constructible<std::string>::value)
    : base_(std::move(e)), what_(std::move(what)) {}

    basic_error_code(int val, const error_category& cat ) noexcept
    : basic_error_code(base_type(val, cat)) {}

    basic_error_code(int val, const error_category& cat, std::string what)
    noexcept(std::is_nothrow_move_constructible<std::string>::value)
    : basic_error_code(base_type(val, cat), std::move(what)) {}

    template <typename ErrorCodeEnum>
    explicit basic_error_code(ErrorCodeEnum e, ce::enable_for<ErrorCodeEnum>* = nullptr) noexcept
    : base_(make_error_code(e)) {}

    template <typename ErrorCodeEnum>
    explicit basic_error_code(ErrorCodeEnum e, std::string what, ce::enable_for<ErrorCodeEnum>* = nullptr)
    noexcept(std::is_nothrow_move_constructible<std::string>::value)
    : base_(make_error_code(e)), what_(std::move(what)) {}

    void assign(int val, const error_category& cat) noexcept {
        base_.assign(val, cat);
    }

    void assign(int val, const error_category& cat, std::string msg) {
        base_.assign(val, cat);
        what_ = std::move(msg);
    }

    template<typename ErrorCodeEnum>
    ce::enable_for<ErrorCodeEnum, basic_error_code>& operator=(ErrorCodeEnum val) noexcept {
        base_ = make_error_code(val);
        return *this;
    }

    void clear() noexcept {
        base_.clear();
        what_.clear();
    }

    error_condition default_error_condition() const noexcept {
        return base().default_error_condition();
    }

    std::string message() const {
        return what().empty() ? base().message() : what();
    }

    const std::string& what() const noexcept { return what_; }

    int value() const noexcept { return base().value(); }

    const error_category& category() const noexcept { return base().category(); }

    explicit operator bool() const noexcept { return base();}

    bool operator!() const noexcept { return !(*this);}

    const base_type& base() const noexcept { return base_;}

private:
    base_type base_;
    std::string what_;
};

template <typename T>
using basic_error_condition = typename basic_error_code<T>::error_condition;
template <typename T>
using basic_error_category = typename basic_error_code<T>::error_category;

template <typename T>
inline bool operator==( const basic_error_code<T>& lhs,
        const basic_error_code<T>& rhs ) noexcept {
    return lhs.base() == rhs.base();
}

template <typename T>
inline bool operator==( const basic_error_code<T>& lhs,
        const basic_error_condition<T>& rhs ) noexcept {
    return lhs.base() == rhs;
}

template <typename T>
inline bool operator==( const basic_error_code<T>& lhs,
        const typename basic_error_code<T>::base_type& rhs ) noexcept {
    return lhs.base() == rhs;
}

template <typename T>
inline bool operator==( const basic_error_condition<T>& lhs,
        const basic_error_code<T>& rhs ) noexcept {
    return lhs == rhs.base();
}

template <typename T>
inline bool operator==( const typename basic_error_code<T>::base_type& lhs,
        const basic_error_code<T>& rhs ) noexcept {
    return lhs == rhs.base();
}

template <typename T>
inline bool operator!=( const basic_error_code<T>& lhs,
        const basic_error_code<T>& rhs ) noexcept {
    return !(lhs == rhs);
}

template <typename T>
inline bool operator!=( const basic_error_code<T>& lhs,
        const basic_error_condition<T>& rhs ) noexcept {
    return !(lhs == rhs);
}

template <typename T>
inline bool operator!=( const basic_error_code<T>& lhs,
        const typename basic_error_code<T>::base_type& rhs ) noexcept {
    return !(lhs == rhs);
}

template <typename T>
inline bool operator!=( const basic_error_condition<T>& lhs,
        const basic_error_code<T>& rhs ) noexcept {
    return !(lhs == rhs);
}

template <typename T>
inline bool operator!=( const basic_error_code<T>::base_type& lhs,
        const basic_error_code<T>& rhs ) noexcept {
    return !(lhs == rhs);
}

template <typename T>
inline bool operator<( const basic_error_code<T>& lhs,
        const basic_error_code<T>& rhs ) noexcept {
    return lhs.base() < rhs.base();
}

template <typename T>
inline std::size_t hash_value( const basic_error_code<T>& ec ) {
  return hash_value(ec.base());
}

template <typename Base, typename ErrorCode>
class basic_system_error : public Base {
public:
    using error_code = ErrorCode;
    using base = Base;
    basic_system_error(error_code ec) : base(ec.base(), ec.what()) {}
    basic_system_error(error_code ec, std::string msg) : base(ec.base(), msg + ec.what()) {}
};

namespace impl {

namespace boost {

using error_code = basic_error_code<::boost::system::error_code>;
using error_category = error_code::error_category;
using error_condition = error_code::error_condition;
using system_error = error_code::system_error;

} // namespace boost

namespace std {

using error_code = basic_error_code<::std::error_code>;
using error_category = error_code::error_category;
using error_condition = error_code::error_condition;
using system_error = error_code::system_error;

} // namespace std

} // namsepace impl

#ifndef ERROR_CODE_USE_STD_NS_DEFAULT
#define ERROR_CODE_ENUM_NS boost
using namespace impl::boost;
#else
#define ERROR_CODE_ENUM_NS std
using namespace impl::std;
#endif

} // namespace ec

#define ERROR_CODE_DECLARE_ERROR_CONDITION_ENUM(Enum)\
    namespase ERROR_CODE_ENUM_NS {\
    template <> struct is_error_condition_enum<Enum> : true_type {};\
    }

#define ERROR_CODE_DECLARE_ERROR_CODE_ENUM(Enum)\
    namespase ERROR_CODE_ENUM_NS {\
    template <> struct is_error_code_enum<Enum> : true_type {};\
    }

#endif /* INCLUDE_ERROR_CODE_H_ */
