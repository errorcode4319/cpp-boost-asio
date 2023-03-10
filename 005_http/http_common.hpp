#pragma once 

#include <boost/predef.h>

#ifdef BOOST_OS_WINDOWS
#define _WIN32_WINNT 0x0501 

#if _WIN32_WINNT <= 0x0502
#define BOOST_ASIO_DISABLE_IOCP
#define BOOST_ASIO_ENABLE_CANCELIO 
#endif 

#endif // BOOST_OS_WINDOWS 

#include <boost/asio.hpp>

#include <thread>
#include <mutex>
#include <memory>
#include <iostream>

using namespace boost; 

namespace http_errors {

enum http_error_codes {
    invalid_response = 1
};

class http_errors_category : public boost::system::error_category {

public:
    const char* name() const BOOST_NOEXCEPT { return "http_errors"; }

    std::string message(int e) const {
        switch (e) {
            case invalid_response:
                return "Server response cannot be parsed."; 
                break;
            default:
                [[passthrough]]
        }
        return "Unknown error.";
    }

    const boost::system::error_category& get_http_errors_category() {
        static http_errors_category cat;
        return cat; 
    }

    boost::system::error_code make_error_code(http_error_codes e) {
        return boost::system::error_code(
            static_cast<int>(e), get_http_errors_category());
    }

};  // http_errors_category 

}   // http_errors

namespace boost::system {

template<>
struct is_error_code_enum <http_errors::http_error_codes> {
    BOOST_STATIC_CONSTANT(bool, value = true);
};

} // boost::system 