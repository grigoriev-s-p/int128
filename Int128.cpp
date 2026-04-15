#include"Int128.hpp"
#include<algorithm>
#include<cmath>

Int128::Int128() {
    lower = 0;
    higher = 0;
}

Int128::Int128(int64_t x) {
    lower = (uint64_t)x;
    higher = (uint64_t)(x >> 63);
}

Int128::Int128(std::string_view x) {
    lower = 0;
    higher = 0;
    bool negative = false;
    size_t i = 0;
    if (!x.empty() && x[0] == '-') {
        negative = true;
        i = 1;
    }
    else if (!x.empty() && x[0] == '+') {
        i = 1;
    }
    for (; i < x.size(); ++i) {
        *this *= 10;
        *this += (int64_t)(x[i] - '0');
    }
    if (negative) {
        *this = -*this;
    }
}
Int128::operator int64_t() const {
    return (int64_t)lower;
}

Int128::operator double() const {
    if ((int64_t)higher < 0) {
        uint64_t l = ~lower + 1;
        uint64_t h = ~higher;
        if (l == 0) {
            h += 1;
        }
        return -(double(h) * pow(2, 64) + double(l));
    }
    return double(higher) * pow(2, 64) + double(lower);
}

std::string Int128::str() const {
    if (*this == 0) {
        return "0";
    }
    uint64_t cur_lower = lower;
    uint64_t cur_higher = higher;
    bool negative = false;
    if ((int64_t)cur_higher < 0) {
        negative = true;
        cur_lower = ~cur_lower + 1;
        cur_higher = ~cur_higher;
        if (cur_lower == 0) {
            cur_higher += 1;
        }
    }
    std::string s = "";
    while (cur_lower != 0 || cur_higher != 0) {
        uint64_t q_lower = 0, q_higher = 0, r_lower = 0, r_higher = 0;
        for (int i = 127; i >= 0; --i) {
            r_higher = (r_higher << 1) | (r_lower >> 63);
            r_lower = (r_lower << 1);

            uint64_t bit;
            if (i < 64) {
                bit = (cur_lower >> i) & 1;
            }
            else {
                bit = (cur_higher >> (i - 64)) & 1;
            }

            r_lower |= bit;

            if (r_higher > 0 || r_lower >= 10) {
                uint64_t old = r_lower;
                r_lower -= 10;
                if (old < 10) {
                    r_higher -= 1;
                }
                if (i < 64) {
                    q_lower |= (1ULL << i);
                }
                else {
                    q_higher |= (1ULL << (i - 64));
                }
            }
        }
        s += (char)('0' + r_lower);
        cur_lower = q_lower;
        cur_higher = q_higher;
    }
    if (negative) {
        s += '-';
    }
    std::reverse(s.begin(), s.end());
    return s;
}

Int128& Int128::operator+=(const Int128& other) {
    uint64_t old = lower;
    lower += other.lower;
    higher += other.higher;
    if (lower < old) {
        higher += 1;
    }
    return *this;
}

Int128& Int128::operator-=(const Int128& other) {
    return *this += (-other);
}

Int128& Int128::operator*=(const Int128& other) {
    bool negative = ((int64_t)higher < 0) != ((int64_t)other.higher < 0); //если знаки несовпадают - рез отриц иначе положит
    Int128 a = *this;
    if ((int64_t)a.higher < 0) {
        a = -a;
    }
    Int128 b = other;
    if ((int64_t)b.higher < 0) {
        b = -b;
    }

    uint64_t a0, a1, b0, b1;
    a0 = a.lower & 0xFFFFFFFF; // левые 32 бита .lower
    a1 = a.lower >> 32; //правые
    b0 = b.lower & 0xFFFFFFFF;
    b1 = b.lower >> 32;
    //каждое число вида x1*2^32 + x0
    uint64_t r0 = a0 * b0;
    uint64_t r1 = a0 * b1 + (r0 >> 32);
    uint64_t r2 = a1 * b0 + (r1 & 0xFFFFFFFF);
    // эта фигня считает разделив каждое число на 2 32битных, чтобы все посчиталось правильно, вроде
    lower = (r2 << 32) | (r0 & 0xFFFFFFFF);
    higher = (a.lower * b.higher) + (a.higher * b.lower) + (a1 * b1) + (r1 >> 32) + (r2 >> 32);
    if (negative) {
        *this = -*this;
    }
    return *this;
}

Int128& Int128::operator/=(const Int128& other) {
    bool negative = ((int64_t)higher < 0) != ((int64_t)other.higher < 0);

    uint64_t a_lower = lower;
    uint64_t a_higher = higher;
    if ((int64_t)a_higher < 0) {
        a_lower = ~a_lower + 1;
        a_higher = ~a_higher;
        if (a_lower == 0) {
            a_higher += 1;
        }
    }

    uint64_t b_lower = other.lower;
    uint64_t b_higher = other.higher;
    if ((int64_t)b_higher < 0) {
        b_lower = ~b_lower + 1;
        b_higher = ~b_higher;
        if (b_lower == 0) {
            b_higher += 1;
        }
    }

    uint64_t q_lower = 0, q_higher = 0, r_lower = 0, r_higher = 0;
    for (int i = 127; i >= 0; --i) {
        r_higher = (r_higher << 1) | (r_lower >> 63);
        r_lower = (r_lower << 1);

        uint64_t bit; //крайний бит
        if (i < 64) {
            bit = (a_lower >> i) & 1;
        }
        else {
            bit = (a_higher >> (i - 64)) & 1;
        }

        r_lower |= bit;

        if (r_higher > b_higher || (r_higher == b_higher && r_lower >= b_lower)) {
            uint64_t old = r_lower;
            r_lower -= b_lower;
            r_higher -= b_higher;
            if (old < b_lower) {
                r_higher -= 1;
            }
            if (i < 64) {
                q_lower |= (1ULL << i);
            }
            else {
                q_higher |= (1ULL << (i - 64));
            }
        }
    }
    lower = q_lower;
    higher = q_higher;
    if (negative) {
        *this = -*this;
    }
    return *this;
}

Int128& Int128::operator%=(const Int128& other) {
    bool negative = ((int64_t)higher < 0);

    uint64_t a_lower = lower;
    uint64_t a_higher = higher;
    if ((int64_t)a_higher < 0) {
        a_lower = ~a_lower + 1;
        a_higher = ~a_higher;
        if (a_lower == 0) {
            a_higher += 1;
        }
    }

    uint64_t b_lower = other.lower;
    uint64_t b_higher = other.higher;
    if ((int64_t)b_higher < 0) {
        b_lower = ~b_lower + 1;
        b_higher = ~b_higher;
        if (b_lower == 0) {
            b_higher += 1;
        }
    }

    uint64_t r_lower = 0, r_higher = 0;
    for (int i = 127; i >= 0; --i) {
        r_higher = (r_higher << 1) | (r_lower >> 63);
        r_lower = (r_lower << 1);

        if (i < 64) {
            r_lower |= (a_lower >> i) & 1;
        }
        else {
            r_lower |= (a_higher >> (i - 64)) & 1;
        }

        if (r_higher > b_higher || (r_higher == b_higher && r_lower >= b_lower)) {
            uint64_t old = r_lower;
            r_lower -= b_lower;
            r_higher -= b_higher;
            if (old < b_lower) {
                r_higher -= 1;
            }
        }
    }
    lower = r_lower;
    higher = r_higher;
    if (negative) {
        *this = -*this;
    }
    return *this;
}

Int128 Int128::operator+(const Int128& other) const {
    return Int128(*this) += other;
}
Int128 Int128::operator-(const Int128& other) const {
    return Int128(*this) -= other;
}
Int128 Int128::operator*(const Int128& other) const {
    return Int128(*this) *= other;
}
Int128 Int128::operator/(const Int128& other) const {
    return Int128(*this) /= other;
}
Int128 Int128::operator%(const Int128& other) const {
    return Int128(*this) %= other;
}

Int128 Int128::operator-() const {
    Int128 res;
    res.lower = ~lower + 1;
    res.higher = ~higher;
    if (res.lower == 0) {
        res.higher += 1;
    }
    return res;
}

bool Int128::operator==(const Int128& other) const {
    return lower == other.lower && higher == other.higher;
}
bool Int128::operator!=(const Int128& other) const {
    return !(*this == other);
}
bool Int128::operator<(const Int128& other) const {
    if (higher != other.higher) {
        return (int64_t)higher < (int64_t)other.higher;
    }
    return lower < other.lower;
}
bool Int128::operator>(const Int128& other) const {
    return other < *this;
}
bool Int128::operator<=(const Int128& other) const {
    return !(*this > other);
}
bool Int128::operator>=(const Int128& other) const {
    return !(*this < other);
}

std::ostream& operator<<(std::ostream& os, const Int128& val) {
    return os << val.str();
}