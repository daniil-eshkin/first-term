#include "big_integer.h"
#include <cmath>
#include <algorithm>
#include <iostream>

void trim(big_integer& a, size_t s) {
    while (a.size() < s) {
        a.digits.push_back(a.further);
    }
}

unsigned int big_integer::operator[](size_t index) const {
    return (index < digits.size() ? digits[index] : further);
}

size_t big_integer::size() const {
    return digits.size();
}

static unsigned int adc(unsigned int a, unsigned int b, unsigned int &d) {
    unsigned int c = a + b + d;
    d = (c < a || (d == 1 && c <= a));
    return c;
}

static unsigned int sbb(unsigned int a, unsigned int b, unsigned int &d) {
    return adc(a, ~b, d);
}

big_integer &evaluate(big_integer &a, const big_integer& b, unsigned int d,
    unsigned int (*eval)(unsigned int, unsigned int, unsigned int&)) {
    trim(a, b.size());
    a.digits.push_back(a.further);
    for (size_t i = 0; i < a.size(); i++) {
        a.digits[i] = eval(a[i], b[i], d);
    }
    a.further = (a.digits.back() & 2 ? ~0 : 0);
    while (a.size() > 1 && a.digits.back() == a.further) {
        a.digits.pop_back();
    }
    return a;
}

int unsigned_cmp(const big_integer &a, const big_integer &b) {
    for (size_t i = std::max(a.size(), b.size()) - 1;; i--) {
        if (a[i] > b[i]) {
            return 1;
        } else if (b[i] > a[i]) {
            return -1;
        }
        if (i == 0) {
            break;
        }
    }
    return 0;
}

big_integer::big_integer() {
    digits.assign(1, 0);
    further = 0;
}

big_integer::big_integer(int a) {
    digits.assign(1, a);
    further = (a < 0 ? ~0 : 0);
}

big_integer::big_integer(const big_integer &a) {
    digits = a.digits;
    further = a.further;
}

big_integer &big_integer::operator=(const big_integer &a) {
    if (this != &a) {
        digits = a.digits;
        further = a.further;
    }
    return *this;
}

bool operator==(const big_integer &a, const big_integer &b) {
    return a.digits == b.digits && a.further == b.further;
}

bool operator!=(const big_integer &a, const big_integer &b) {
    return !(a == b);
}

bool operator>(const big_integer &a, const big_integer &b) {
    if (a.further < b.further) {
        return true;
    } else if (a.further > b.further) {
        return false;
    } else {
        return unsigned_cmp(a, b) == (a.further == 0 ? 1 : -1);
    }
}

bool operator<(const big_integer &a, const big_integer &b) {
    return b > a;
}

bool operator>=(const big_integer &a, const big_integer &b) {
    return !(b > a);
}

bool operator<=(const big_integer &a, const big_integer &b) {
    return !(a > b);
}

big_integer operator~(big_integer a) {
    for (size_t i = 0; i < a.size(); i++) {
        a.digits[i] = ~a.digits[i];
    }
    a.further = ~a.further;
    return a;
}

big_integer &operator&=(big_integer &a, const big_integer &b) {
    trim(a, b.size());
    for (size_t i = 0; i < a.size(); i++) {
        a.digits[i] &= b[i];
    }
    a.further &= b.further;
    return a;
}

big_integer operator&(big_integer a, const big_integer &b) {
    return a &= b;
}

big_integer &operator|=(big_integer &a, const big_integer &b) {
    trim(a, b.size());
    for (size_t i = 0; i < a.size(); i++) {
        a.digits[i] |= b[i];
    }
    a.further |= b.further;
    return a;
}

big_integer operator|(big_integer a, const big_integer &b) {
    return a |= b;
}

big_integer &operator^=(big_integer &a, const big_integer &b) {
    trim(a, b.size());
    for (size_t i = 0; i < a.size(); i++) {
        a.digits[i] ^= b[i];
    }
    a.further ^= b.further;
    return a;
}

big_integer operator^(big_integer a, const big_integer &b) {
    return a ^= b;
}

big_integer &operator<<=(big_integer &a, int s) {
    if (s == 0) {
        return a;
    } else if (s < 0) {
        return a >>= -s;
    } else {
        unsigned int s1 = s / (sizeof(unsigned int) << 3);
        a.digits.resize(a.size() + s1);
        for (size_t i = a.size() - 1;; i--) {
            a.digits[i] = (i >= s1 ? a[i - s1] : 0);
            if (i == 0) {
                break;
            }
        }
        s %= sizeof(unsigned int) << 3;
        unsigned int t = (sizeof(unsigned int) << 3) - s;
        unsigned int d = 0;
        for (size_t i = s1; i < a.size(); i++) {
            unsigned int d1 = (a[i] >> t);
            a.digits[i] = (a[i] << s) | d;
            d = d1;
        }
        if (d != (a.further >> t)) {
            a.digits.push_back(d);
        }
        return a;
    }
}

big_integer operator<<(big_integer a, int s) {
    return a <<= s;
}

big_integer &operator>>=(big_integer &a, int s) {
    if (s == 0) {
        return a;
    } else if (s < 0) {
        return a <<= -s;
    } else {
        unsigned int s1 = s / (sizeof(unsigned int) << 3);
        if (s1 >= a.size()) {
            a.digits.assign(1, a.further);
            return a;
        }
        for (size_t i = 0; i < a.size() - s1; i++) {
            a.digits[i] = a[i + s1];
        }
        s %= sizeof(unsigned int) << 3;
        unsigned int t = (sizeof(unsigned int) << 3) - s;
        for (size_t i = s1; i < a.size(); i++) {
            a.digits[i] = (a[i] >> s) | ((a[i + 1] % (1 << s)) << t);
        }
        while (a.size() > 1 && a.digits.back() == a.further) {
            a.digits.pop_back();
        }
        return a;
    }
}

big_integer operator>>(big_integer a, int s) {
    return a >>= s;
}

big_integer operator+(const big_integer &a) {
    return big_integer(a);
}

big_integer &operator+=(big_integer &a, const big_integer &b) {
    return evaluate(a, b, 0, adc);
}

big_integer operator+(big_integer a, const big_integer &b) {
    return a += b;
}

big_integer& operator++(big_integer &a) {
    a += 1;
    return a;
}

big_integer operator++(big_integer &a, int) {
    big_integer old = a;
    a += 1;
    return old;
}

big_integer operator-(big_integer a) {
    return ~a + 1;
}

big_integer &operator-=(big_integer &a, const big_integer &b) {
    return evaluate(a, b, 1, sbb);
}

big_integer operator-(big_integer a, const big_integer &b) {
    return a -= b;
}

big_integer& operator--(big_integer &a) {
    a -= 1;
    return a;
}

big_integer operator--(big_integer &a, int) {
    big_integer old = a;
    a -= 1;
    return old;
}

big_integer &operator*=(big_integer &a, int b) {
    if (a == 0) {
        return a;
    } else if (b == 0) {
        return a = 0;
    }
    int sign = (a.further == 0 ? 1 : -1) * (b > 0 ? 1 : -1);
    if (b < 0) {
        b = -b;
    }
    if (a.further > 0) {
        a = -a;
    }
    unsigned int d = 0;
    for (size_t i = 0; i < a.size(); i++) {
        unsigned long long x = (unsigned long long)a[i] * b + d;
        a.digits[i] = x;
        d = x >> (sizeof(unsigned int) << 3);
    }
    if (d) {
        a.digits.push_back(d);
    }
    if (sign == -1) {
        a = -a;
    }
    return a;
}

big_integer operator*(big_integer a, int b) {
    return a *= b;
}

big_integer::big_integer(const std::string &s) {
    digits.assign(1, 0);
    further = 0;
    size_t i = 0;
    int sign = 1;
    if (s[0] == '-') {
        i++;
        sign = -1;
    }
    for (; i < s.size(); i++) {
        *this *= 10;
        *this += sign * (s[i] - '0');
    }
}

big_integer &operator*=(big_integer &a, const big_integer &b) {
    return a = a * b;
}

big_integer operator*(big_integer a, big_integer b) {
    if (a == 0) {
        return a;
    } else if (b == 0) {
        return a = 0;
    }
    int sign = 1;
    if (a.further != 0) {
        sign = -sign;
        a = -a;
    }
    if (b.further != 0) {
        sign = -sign;
        b = -b;
    }
    big_integer c;
    trim(c, a.size() + b.size());
    for (size_t i = 0; i < a.size(); i++) {
        unsigned int d = 0;
        for (size_t j = 0; j < b.size() || d; j++) {
            unsigned long long cur = (unsigned long long)c[i + j]
                    + (unsigned long long)a[i] * b[j] + d;
            c.digits[i + j] = (unsigned int)cur;
            d = (unsigned int)(cur >> (sizeof(unsigned int) << 3));
        }
    }
    while (c.size() > 1 && c.digits.back() == c.further)
        c.digits.pop_back();
    if (sign == -1) {
        c = -c;
    }
    return c;
}

big_integer &operator/=(big_integer &a, int b) {
    if (a == 0) {
        return a;
    } else if (b == 0) {
        return a = 0;
    } else if (b == INT32_MIN) {
        return a /= big_integer(b);
    }
    int sign = (a.further == 0 ? 1 : -1) * (b > 0 ? 1 : -1);
    if (b < 0) {
        b = -b;
    }
    if (a.further > 0) {
        a = -a;
    }
    unsigned int d = 0;
    for (size_t i = a.size() - 1;; i--) {
        unsigned long long cur = a[i] +
                (((unsigned long long)d) << (sizeof(unsigned int) << 3));
        a.digits[i] = (unsigned int)(cur / b);
        d = (unsigned int)(cur % b);
        if (i == 0) {
            break;
        }
    }
    while (a.size() > 1 && a.digits.back() == 0) {
        a.digits.pop_back();
    }
    if (sign == -1) {
        a = -a;
    }
    return a;
}

big_integer operator/(big_integer a, int b) {
    return a /= b;
}

big_integer &operator%=(big_integer &a, int b) {
    return a -= (a / b) * b;
}

int operator%(big_integer a, int b) {
    a %= b;
    return a[0];
}

void unsigned_div(big_integer &a, const big_integer &b, big_integer &q, big_integer &r) {
    if (a == 0) {
        q = 0;
        r = 0;
        return;
    }
    bool even = a[0] % 2;
    a >>= 1;
    unsigned_div(a, b, q, r);
    q <<= 1;
    r <<= 1;
    if (even) {
        r++;
    }
    if (r >= b) {
        q++;
        r -= b;
    }
}

big_integer operator/(big_integer a, big_integer b) {
    if (a == 0) {
        return a;
    }
    if (b == 0) {
        return b;
    }
    int sign = 1;
    if (a.further > 0) {
        sign = -sign;
        a = -a;
    }
    if (b.further > 0) {
        sign = -sign;
        b = -b;
    }
    if (a < b) {
        return 0;
    }
    big_integer q, r;
    unsigned_div(a, b, q, r);
    if (sign == -1) {
        q = -q;
    }

    return q;
}

big_integer &operator/=(big_integer &a, const big_integer &b) {
    return a = a / b;
}

big_integer &operator%=(big_integer &a, const big_integer &b) {
    return a -= (a / b) * b;
}

big_integer operator%(big_integer a, const big_integer &b) {
    return a %= b;
}

std::string to_string(big_integer a) {
    if (a == 0) {
        return "0";
    }
    std::string s;
    int sign = 1;
    if (a.further != 0) {
        sign = -1;
        a = -a;
    }
    while (a != 0) {
        s.push_back(a % 10 + '0');
        a /= 10;
    }
    if (sign == -1) {
        s.push_back('-');
    }
    std::reverse(s.begin(), s.end());
    return s;
}

