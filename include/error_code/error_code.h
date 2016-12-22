#ifndef INCLUDE_ERROR_CODE_H_
#define INCLUDE_ERROR_CODE_H_

#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

namespace ec {

template <typename ErrorConditionEnum, typename T>
using enable_if_error_enum = typename boost::enable_if<
        boost::system::is_error_condition_enum<ErrorConditionEnum>, T>::type;

class error_code {
public:
    using base_type = ::boost::system::error_code;
    using error_category = ::boost::system::error_category;
    using error_condition = decltype(base_type().default_error_condition());

    error_code() = default;

    error_code(int val, const error_category & cat ) : base_(val, cat) {}

    error_code(int val, const error_category & cat, std::string what)
    : base_(val, cat), what_(std::move(what)) {}

    error_code(base_type e) : base_(std::move(e)) {}

    error_code(base_type e, std::string what)
    : base_(std::move(e)), what_(std::move(what)) {}

    template <class ErrorConditionEnum>
    explicit error_code(ErrorConditionEnum e,
            enable_if_error_enum<ErrorConditionEnum, void>* = nullptr)
        : base_(make_error_code(e)) {}

    template <class ErrorConditionEnum>
    explicit error_code(ErrorConditionEnum e, std::string what,
            enable_if_error_enum<ErrorConditionEnum, void>* = nullptr)
        : base_(make_error_code(e)), what_(std::move(what)) {}

    error_condition default_error_condition() const noexcept {
        return base().default_error_condition();
    }

    std::string message() const {
        return what().empty() ? base().message() : what();
    }

    const std::string& what() const noexcept { return what_; }

    int value() const noexcept { return base().value(); }

    const error_category& category() const noexcept { return base().category(); }

    operator base_type::unspecified_bool_type() const noexcept { return base();}

    bool operator!() const noexcept { return !base();}

    const base_type& base() const noexcept { return base_;}

private:
    base_type base_;
    std::string what_;
};

using error_condition = error_code::error_condition;
using error_category = error_code::error_category;

inline bool operator==( const error_code& lhs, const error_code& rhs ) noexcept {
    return lhs.base() == rhs.base();
}

inline bool operator==( const error_code& lhs, const error_condition& rhs ) noexcept {
    return lhs.base() == rhs;
}

inline bool operator==( const error_code& lhs, const error_code::base_type& rhs ) noexcept {
    return lhs.base() == rhs;
}

inline bool operator==( const error_condition& lhs, const error_code& rhs ) noexcept {
    return lhs == rhs.base();
}

inline bool operator==( const error_code::base_type& lhs, const error_code& rhs ) noexcept {
    return lhs == rhs.base();
}

inline bool operator!=( const error_code& lhs, const error_code& rhs ) noexcept {
    return !(lhs == rhs);
}

inline bool operator!=( const error_code& lhs, const error_condition& rhs ) noexcept {
    return !(lhs == rhs);
}

inline bool operator!=( const error_code& lhs, const error_code::base_type& rhs ) noexcept {
    return !(lhs == rhs);
}

inline bool operator!=( const error_condition& lhs, const error_code& rhs ) noexcept {
    return !(lhs == rhs);
}

inline bool operator!=( const error_code::base_type& lhs, const error_code& rhs ) noexcept {
    return !(lhs == rhs);
}

inline bool operator<( const error_code& lhs, const error_code& rhs ) noexcept {
    return lhs.base() < rhs.base();
}

inline std::size_t hash_value( const error_code& ec ) {
  return hash_value(ec.base());
}

class system_error : public ::boost::system::system_error {
    using base = ::boost::system::system_error;
public:
    system_error(error_code ec) : base(ec.base(), ec.what()) {}
    system_error(error_code ec, std::string msg) : base(ec.base(), msg + ec.what()) {}
};

} // namespace ec

#endif /* INCLUDE_ERROR_CODE_H_ */
