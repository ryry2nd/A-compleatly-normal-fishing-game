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

class Bigint
{
public:
    static constexpr uint64_t BASE = UINT64_MAX;
    std::vector<uint64_t> data; // least significant chunk first
    double decimalPart = 0.0;
    bool isNegative = false;

    Bigint(const std::string &str)
    {
        std::string s = str;
        // Remove leading/trailing whsstreamitespace
        s.erase(0, s.find_first_not_of(" \t\n\r"));
        s.erase(s.find_last_not_of(" \t\n\r") + 1);

        if (s.empty())
            throw std::invalid_argument("Empty string");

        size_t dotPos = s.find('.');
        std::string intPartStr = (dotPos == std::string::npos) ? s : s.substr(0, dotPos);
        std::string fracPartStr = (dotPos == std::string::npos) ? "" : s.substr(dotPos + 1);

        // Handle sign
        isNegative = false;
        if (!intPartStr.empty() && intPartStr[0] == '-')
        {
            isNegative = true;
            intPartStr = intPartStr.substr(1);
        }
        else if (!intPartStr.empty() && intPartStr[0] == '+')
        {
            intPartStr = intPartStr.substr(1);
        }

        // Remove leading zeros
        intPartStr.erase(0, intPartStr.find_first_not_of('0'));
        if (intPartStr.empty())
            intPartStr = "0";

        // Parse integer part in reverse chunks
        data.clear();
        int chunkSize = 19; // Max 19 digits fit in uint64_t
        for (int i = (int)intPartStr.length(); i > 0; i -= chunkSize)
        {
            int start = std::max(0, i - chunkSize);
            std::string chunk = intPartStr.substr(start, i - start);
            data.push_back(std::stoull(chunk));
        }

        // Parse fractional part (keep only up to 5 decimal digits)
        while (fracPartStr.length() < 5)
            fracPartStr += "0";
        fracPartStr = fracPartStr.substr(0, 5);
        decimalPart = std::stod("0." + fracPartStr);
    }

    Bigint(double n = 0.0)
    {
        if (std::isnan(n) || std::isinf(n))
            throw std::invalid_argument("NaN or Inf not supported");

        isNegative = n < 0;
        double absVal = std::abs(n);
        double intPart;
        decimalPart = std::modf(absVal, &intPart);

        if (intPart > static_cast<double>(UINT64_MAX))
            throw std::overflow_error("Integer part too large");

        data.push_back(static_cast<uint64_t>(intPart));
    }

    void print() const
    {
        if (isNegative)
            std::cout << "-";
        for (auto it = data.rbegin(); it != data.rend(); ++it)
        {
            if (it == data.rbegin())
                std::cout << *it;
            else
                std::cout << std::setw(20) << std::setfill('0') << *it;
        }
        std::cout << "." << std::setw(5) << std::setfill('0') << static_cast<int>(decimalPart * 100000) << "\n";
    }

    Bigint operator+(const Bigint &other) const
    {
        if (isNegative == other.isNegative)
        {
            Bigint result;
            result.isNegative = isNegative;
            double decSum = decimalPart + other.decimalPart;
            uint64_t carry = 0;
            if (decSum >= 1.0)
            {
                result.decimalPart = decSum - 1.0;
                carry = 1;
            }
            else
                result.decimalPart = decSum;

            result.data = addVectors(data, other.data, carry);
            return result;
        }
        else
        {
            // Convert to subtraction
            if (compareAbs(*this, other) >= 0)
            {
                Bigint result = subtractAbs(*this, other);
                result.isNegative = isNegative;
                return result;
            }
            else
            {
                Bigint result = subtractAbs(other, *this);
                result.isNegative = other.isNegative;
                return result;
            }
        }
    }

    Bigint operator-(const Bigint &other) const
    {
        if (isNegative != other.isNegative)
        {
            Bigint result = addAbs(*this, other);
            result.isNegative = isNegative;
            return result;
        }
        else
        {
            if (compareAbs(*this, other) >= 0)
            {
                Bigint result = subtractAbs(*this, other);
                result.isNegative = isNegative;
                return result;
            }
            else
            {
                Bigint result = subtractAbs(other, *this);
                result.isNegative = !other.isNegative;
                return result;
            }
        }
    }

    bool operator<(const Bigint &other) const
    {
        if (isNegative != other.isNegative)
            return isNegative;

        int cmp = compareAbs(*this, other);
        return isNegative ? cmp > 0 : cmp < 0;
    }

    bool operator>(const Bigint &other) const
    {
        if (isNegative != other.isNegative)
            return other.isNegative;

        int cmp = compareAbs(*this, other);
        return isNegative ? cmp < 0 : cmp > 0;
    }

    bool operator==(const Bigint &other) const
    {
        return (isNegative == other.isNegative) && (data == other.data) && (decimalPart == other.decimalPart);
    }

    bool operator>=(const Bigint &other) const
    {
        return (*this > other) || (*this == other);
    }

    bool operator<=(const Bigint &other) const
    {
        return (*this < other) || (*this == other);
    }

    void operator+=(const Bigint &other)
    {
        *this = *this + other;
    }
    void operator-=(const Bigint &other) { *this = *this - other; }

    float toFloat() const
    {
        float base = (data.empty() ? 0.0f : static_cast<float>(data[0]));
        return (isNegative ? -1.0f : 1.0f) * (base + static_cast<float>(decimalPart));
    }

    double toDouble() const
    {
        double base = data.empty() ? 0.0 : static_cast<double>(data.back());
        return (isNegative ? -1.0 : 1.0) * (base + decimalPart);
    }

private:
    static std::vector<uint64_t> addVectors(const std::vector<uint64_t> &a, const std::vector<uint64_t> &b, uint64_t carryIn)
    {
        std::vector<uint64_t> res;
        size_t n = std::max(a.size(), b.size());
        uint64_t carry = carryIn;

        for (size_t i = 0; i < n; ++i)
        {
            uint64_t aval = (i < a.size()) ? a[i] : 0;
            uint64_t bval = (i < b.size()) ? b[i] : 0;
            uint64_t sum = aval + bval + carry;
            carry = (sum < aval || (carry && sum == aval)) ? 1 : 0;
            res.push_back(sum);
        }
        if (carry)
            res.push_back(carry);
        return res;
    }

    static std::vector<uint64_t> subtractVectors(const std::vector<uint64_t> &a, const std::vector<uint64_t> &b, int borrowIn)
    {
        std::vector<uint64_t> res;
        size_t n = a.size();
        int64_t borrow = borrowIn;

        for (size_t i = 0; i < n; ++i)
        {
            uint64_t aval = a[i];
            uint64_t bval = (i < b.size()) ? b[i] : 0;

            uint64_t sub = aval - bval - borrow;

            if (aval < bval + borrow)
                borrow = 1;
            else
                borrow = 0;

            res.push_back(sub);
        }

        while (res.size() > 1 && res.back() == 0)
            res.pop_back();

        return res;
    }

    static Bigint subtractAbs(const Bigint &a, const Bigint &b)
    {
        Bigint result;
        double decDiff = a.decimalPart - b.decimalPart;
        int borrow = 0;
        if (decDiff < 0)
        {
            decDiff += 1.0;
            borrow = 1;
        }
        result.decimalPart = decDiff;
        result.data = subtractVectors(a.data, b.data, borrow);
        return result;
    }

    static Bigint addAbs(const Bigint &a, const Bigint &b)
    {
        Bigint result;
        double decSum = a.decimalPart + b.decimalPart;
        uint64_t carry = 0;
        if (decSum >= 1.0)
        {
            result.decimalPart = decSum - 1.0;
            carry = 1;
        }
        else
            result.decimalPart = decSum;
        result.data = addVectors(a.data, b.data, carry);
        return result;
    }

    static int compareAbs(const Bigint &a, const Bigint &b)
    {
        if (a.data.size() > b.data.size())
            return 1;
        if (a.data.size() < b.data.size())
            return -1;
        for (int i = (int)a.data.size() - 1; i >= 0; --i)
        {
            if (a.data[i] > b.data[i])
                return 1;
            if (a.data[i] < b.data[i])
                return -1;
        }
        if (a.decimalPart > b.decimalPart)
            return 1;
        if (a.decimalPart < b.decimalPart)
            return -1;
        return 0;
    }
};