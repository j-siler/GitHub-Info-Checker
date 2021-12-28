#ifndef INFOBEAMER_API_TYPES_HPP
#define INFOBEAMER_API_TYPES_HPP

#include <exception>
#include <string>

namespace InfoBeamer {

enum class IBErrCode
{
    OK=0,
    BAD_JSON
};

template<class C>
class IBException: public std::exception
{
public:
    IBException(const std::string &m, IBErrCode e) : _msg(m), _err(e){}
    const std::string &msg() const {return _msg;};
    IBErrCode err() const {return _err;}
    const char* what() const _NOEXCEPT {return _msg.c_str();}
private:
    std::string _msg;
    IBErrCode _err;
};
}

#endif // INFOBEAMER_API_TYPES_HPP
