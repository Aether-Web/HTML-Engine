#pragma once
#include <variant>
#include <stdexcept>

namespace VEOEngine {

template<typename T, typename E>
class Result {
public:
    static Result Ok(T val) { Result r; r.data_ = std::move(val); return r; }
    static Result Err(E err) { Result r; r.data_ = std::move(err); return r; }
    
    bool isOk() const { return std::holds_alternative<T>(data_); }
    bool isErr() const { return std::holds_alternative<E>(data_); }
    
    const T& value() const { return std::get<T>(data_); }
    T& value() { return std::get<T>(data_); }
    const E& error() const { return std::get<E>(data_); }
    
private:
    std::variant<T, E> data_;
};

} // namespace VEOEngine
