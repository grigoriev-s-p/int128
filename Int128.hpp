#pragma once

#include<iostream>
#include<string>
#include<string_view>
#include<cstdint>

class Int128 {
private:
    uint64_t lower;
    uint64_t higher;

public:
    Int128();
    Int128(int64_t x);
    Int128(std::string_view x);

    explicit operator int64_t() const;
    explicit operator double() const;

    std::string str() const;

    Int128& operator+=(const Int128& other);
    Int128& operator-=(const Int128& other);
    Int128& operator*=(const Int128& other);
    Int128& operator/=(const Int128& other);
    Int128& operator%=(const Int128& other);

    Int128 operator+(const Int128& other) const;
    Int128 operator-(const Int128& other) const;
    Int128 operator*(const Int128& other) const;
    Int128 operator/(const Int128& other) const;
    Int128 operator%(const Int128& other) const;
    Int128 operator-() const;

    bool operator==(const Int128& other) const;
    bool operator!=(const Int128& other) const;
    bool operator<(const Int128& other) const;
    bool operator>(const Int128& other) const;
    bool operator<=(const Int128& other) const;
    bool operator>=(const Int128& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Int128& val);
};