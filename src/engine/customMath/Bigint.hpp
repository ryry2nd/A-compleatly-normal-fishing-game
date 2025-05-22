#pragma once

#include <vector>
#include <iostream>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

class Bigint
{
public:
    boost::multiprecision::cpp_int MAX_DOUBLE = boost::multiprecision::cpp_int(std::numeric_limits<double>::max());
    boost::multiprecision::cpp_int MAX_FLOAT = boost::multiprecision::cpp_int(std::numeric_limits<float>::max());

    static constexpr int SCALE = 100000; // fixed scale = 5 digits
    boost::multiprecision::cpp_int value;

    Bigint()
    {
        value = boost::multiprecision::cpp_int();
    }

    Bigint(double d)
    {
        value = boost::multiprecision::cpp_int(static_cast<long long>(d * SCALE));
    }

    Bigint(float f)
    {
        value = boost::multiprecision::cpp_int(static_cast<long long>(f * SCALE));
    }

    Bigint(int i)
    {
        value = boost::multiprecision::cpp_int(static_cast<long long>(i * SCALE));
    }

    Bigint(const std::string &s)
    {
        size_t dot = s.find('.');
        std::string intPart = (dot == std::string::npos) ? s : s.substr(0, dot);
        std::string fracPart = (dot == std::string::npos) ? "" : s.substr(dot + 1);

        while (fracPart.length() < 5)
            fracPart += '0';
        if (fracPart.length() > 5)
            fracPart = fracPart.substr(0, 5);

        value = boost::multiprecision::cpp_int(intPart + fracPart);
    }

    Bigint operator+(const Bigint &other) const
    {
        Bigint result;
        result.value = value + other.value;
        return result;
    }

    Bigint operator-(const Bigint &other) const
    {
        Bigint result;
        result.value = value - other.value;
        return result;
    }

    Bigint operator*(const Bigint &other) const
    {
        Bigint result;
        result.value = (value * other.value) / SCALE;
        return result;
    }

    Bigint operator/(const Bigint &other) const
    {
        Bigint result;
        result.value = (value * SCALE) / other.value;
        return result;
    }

    void operator+=(const Bigint &other)
    {
        *this = *this + other;
    }

    void operator-=(const Bigint &other)
    {
        *this = *this - other;
    }

    void operator*=(const Bigint &other)
    {
        *this = *this * other;
    }

    void operator/=(const Bigint &other)
    {
        *this = *this / other;
    }

    bool isZero() const
    {
        return value.is_zero();
    }

    bool operator<(const Bigint &other)
    {
        return value < other.value;
    }

    bool operator>(const Bigint &other)
    {
        return value < other.value;
    }

    bool operator==(const Bigint &other)
    {
        return value == other.value;
    }

    bool operator!=(const Bigint &other)
    {
        return value != other.value;
    }

    bool operator<=(const Bigint &other)
    {
        return value <= other.value;
    }

    bool operator>=(const Bigint &other)
    {
        return value >= other.value;
    }

    std::string toString() const
    {
        boost::multiprecision::cpp_int intPart = value / SCALE;
        boost::multiprecision::cpp_int fracPart = abs(value % SCALE);

        std::ostringstream out;
        out << intPart << "." << std::setw(5) << std::setfill('0') << fracPart;
        return out.str();
    }

    float toFloat() const
    {
        if (value > MAX_FLOAT)
            return INFINITY;
        return static_cast<float>(value) / 100000.0f;
    }

    double toDouble() const
    {
        if (value > MAX_DOUBLE)
            return INFINITY;
        return static_cast<double>(value) / 100000.0;
    }
};