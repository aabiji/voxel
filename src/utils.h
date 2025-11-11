#pragma once

#include <format>
#include <string>
#include <iostream>

// Error types
class Result
{
public:
    Result() : m_is_err(false), m_msg("") {}

    template <typename... Args>
    Result(std::string_view fmt, Args&&... args)
        : m_is_err(true), m_msg(std::vformat(fmt, std::make_format_args(args...))) {}

    bool is_err() const { return m_is_err; }
    std::string error() const { return m_msg; }

private:
    bool m_is_err;
    std::string m_msg;
};

template <typename T>
class ResultOr
{
public:
    // implicit conversion from value type
    ResultOr(T& value) : m_value(value) {}
    ResultOr(const T& value) : m_value(value) {}

    // implicit conversion from error type
    ResultOr(const Result& err) : m_result(err) {}
    ResultOr(Result& err) : m_result(err) {}

    T value() const { return m_value; }
    Result error() const { return m_result; }
    bool is_err() const { return m_result.is_err(); }

private:
    T m_value;
    Result m_result;
};

// Logging
enum class Level { info, warning, error, fatal };

template <typename ...Args>
void log(std::string_view fmt, Args&&... args)
{
    std::cout << std::vformat(fmt, std::make_format_args(args...)) << "\n";
}

template <typename ...Args>
void log(Level level, std::string_view fmt, Args&&... args)
{
    std::string red = "\x1b[31m";
    std::string yellow = "\x1b[33m";
    std::string cyan = "\x1b[36m";
    std::string reset = "\x1b[0m";
    std::string prefix =
        level == Level::info ? cyan : level == Level::warning ? yellow : red;
    std::cout << prefix;
    log(fmt, args...);
    std::cout << reset << "\n";
    if (level == Level::fatal)
        exit(EXIT_FAILURE);
}