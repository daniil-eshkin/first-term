#include "big_integer.h"
#include <cmath>
#include <algorithm>
#include <iostream>

void trim(big_integer &a, size_t s) {
    while (a.size() < s) {
        a.digits.push_back(a.further);
    }
}

void shrink(big_integer &a) {
    while (a.size() > 1 && a.digits.back() == a.further) {
        a.digits.pop_back();
    }
}

uint32_t big_integer::operator[](size_t index) const {
    return (index < digits.size() ? digits[index] : further);
}

size_t big_integer::size() const {
    return digits.size();
}

static uint32_t adc(uint32_t a, uint32_t b, uint32_t &d) {
    uint32_t c = a + b + d;
    d = (c < a || (d == 1 && c <= a));
    return c;
}

static uint32_t sbb(uint32_t a, uint32_t b, uint32_t &d) {
    return adc(a, ~b, d);
}

big_integer &evaluate(big_integer &a, const big_integer& b, uint32_t d,
    uint32_t (*eval)(uint32_t, uint32_t, uint32_t&)) {
    trim(a, b.size());
    a.digits.push_back(a.further);
    for (size_t i = 0; i < a.size(); i++) {
        a.digits[i] = eval(a[i], b[i], d);
    }
    a.further = (a.digits.back() & 2 ? ~0 : 0);
    shrink(a);
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

bool is_zero(const big_integer &a) {
    return (a.further == 0 && a.size() == 1 && a.digits[0] == 0);
}

big_integer::big_integer() {
    digits.push_back(0);
    further = 0;
}

big_integer::big_integer(int a) {
    digits.push_back(a);
    further = (a < 0 ? ~0 : 0);
}

big_integer::big_integer(uint32_t a) {
    digits.push_back(a);
    further = 0;
}

big_integer::big_integer(uint64_t a) {
    digits.push_back((uint32_t)a);
    digits.push_back((uint32_t)(a >> 32));
    further = 0;
}

big_integer::big_integer(const big_integer &a) {
    digits = a.digits;
    further = a.further;
}

big_integer::big_integer(const std::string &s) {
    digits.push_back(0);
    further = 0;
    if (s.empty()) {
        return;
    }
    size_t i = 0;
    int sign = 1;
    if (s[0] == '-') {
        i++;
        sign = -1;
    }
    for (; i < s.size(); i++) {
        if (!isalnum(s[i])) {
            throw std::runtime_error("Incorrect string format");
        }
        *this *= 10;
        *this += sign * (s[i] - '0');
    }
}

std::string to_string(big_integer a) {
    if (is_zero(a)) {
        return "0";
    }
    std::string s;
    int sign = 1;
    if (a.further != 0) {
        sign = -1;
        a = -a;
    }
    while (!is_zero(a)) {
        s.push_back((a % 10)[0] + '0');
        a /= 10;
    }
    if (sign == -1) {
        s.push_back('-');
    }
    std::reverse(s.begin(), s.end());
    return s;
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
        return unsigned_cmp(a, b) == 1;
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
    shrink(a);
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
    shrink(a);
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
    shrink(a);
    return a;
}

big_integer operator^(big_integer a, const big_integer &b) {
    return a ^= b;
}

big_integer &operator<<=(big_integer &a, int s) {
    if (s == 0 || is_zero(a)) {
        return a;
    } else if (s < 0) {
        return a >>= -s;
    } else {
        uint32_t s1 = s / 32;
        trim(a, a.size() + s1);
        for (size_t i = a.size() - 1;; i--) {
            a.digits[i] = (i >= s1 ? a[i - s1] : 0);
            if (i == 0) {
                break;
            }
        }
        s %= 32;
        if (s != 0) {
            a *= ((uint32_t)1 << s);
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
        uint32_t s1 = s / 32;
        if (s1 >= a.size()) {
            a.digits.clear();
            a.digits.push_back(a.further);
            return a;
        }
        for (size_t i = 0; i < a.size() - s1; i++) {
            a.digits[i] = a[i + s1];
        }
        for (size_t i = 0; i < s1; i++) {
            a.digits.pop_back();
        }
        s %= 32;
        if (s != 0) {
            if (a.further != 0) {
               a -= ((uint32_t)1 << s) - 1;
            }
            a /= ((uint32_t)1 << s);
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

big_integer &operator*=(big_integer &a, const big_integer &b) {
    return a = a * b;
}

big_integer operator*(big_integer a, big_integer b) {
    if (is_zero(a)) {
        return a;
    } else if (is_zero(b)) {
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
        uint32_t d = 0;
        for (size_t j = 0; j < b.size() || d; j++) {
            uint64_t cur = (uint64_t)c[i + j]
                    + (uint64_t)a[i] * b[j] + d;
            c.digits[i + j] = (uint32_t)cur;
            d = (uint32_t)(cur >> 32);
        }
    }
    shrink(c);
    if (sign == -1) {
        c = -c;
    }
    return c;
}

big_integer div_long_short(big_integer a, uint32_t b) {
    if (is_zero(a)) {
        return a;
    } else if (is_zero(b)) {
        throw std::runtime_error("Division by zero");
    }
    int sign = (a.further == 0 ? 1 : -1);
    if (a.further > 0) {
        a = -a;
    }
    uint32_t d = 0;
    for (size_t i = a.size() - 1;; i--) {
        uint64_t cur = a[i] +
                (((uint64_t)d) << 32);
        a.digits[i] = (uint32_t)(cur / b);
        d = (uint32_t)(cur % b);
        if (i == 0) {
            break;
        }
    }
    shrink(a);
    if (sign == -1) {
        a = -a;
    }
    return a;
}

big_integer suffix(const big_integer &a, size_t len) {
    big_integer b;
    b.digits.pop_back();
    for (size_t i = 0; i < len; i++) {
        b.digits.push_back(a[a.size() - len + i]);
    }
    return b;
}

void unsigned_div(big_integer &a, big_integer &b, big_integer &q, big_integer &r) {
    if (a < b) {
        q = 0;
        r = a;
        return;
    }
    uint32_t q1;
    if (a.size() == b.size()) {
        unsigned_div_small_ans(a, b, q1, r);
        q = q1;
        return;
    }
    r = suffix(a, b.size());
    unsigned_div_small_ans(r, b, q1, r);
    q = q1;
    for (size_t i = 0; i < a.size() - b.size(); i++) {
        q <<= 32;
        r <<= 32;
        r.digits[0] = a[a.size() - b.size() - i - 1];
        if (r >= b) {
            unsigned_div_small_ans(r, b, q1, r);
            q.digits[0] = q1;
        }
    }
}

uint32_t div_3_2(uint32_t a2, uint32_t a1, uint32_t a0, uint32_t b1, uint32_t b0) {
    uint64_t q = (((uint64_t)a2 << 32) + a1) / b1;
    while (true) {
        uint32_t c2, c1, c0;
        uint64_t d = q * b0;
        c0 = (uint32_t)d;
        c1 = (uint32_t)(d >> 32);
        d = c1 + q * b1;
        c1 = (uint32_t)d;
        c2 = (uint32_t)(d >> 32);
        if (c2 > a2 || (c2 == a2 && c1 > a1) || (c2 == a2 && c1 == a1 && c0 > a0)) {
            q--;
        } else {
            break;
        }
    }
    return (uint32_t)q;
}

void unsigned_div_small_ans(big_integer &a, big_integer &b, uint32_t &q, big_integer &r) {
    if (a < b) {
        q = 0;
        r = a;
        return;
    }
    uint32_t x = ((uint32_t)1 << 31), t = 0;
    while ((b[b.size() - 1] & x) == 0) {
        x >>= 1;
        t++;
    }
    a <<= t;
    b <<= t;
    if (a.size() == b.size()) {
        q = 1;
    } else {
        q = div_3_2(a[a.size() - 1], a[a.size() - 2], a[a.size() - 3], b[b.size() - 1], b[b.size() - 2]);
    }
    a >>= t;
    b >>= t;
    r = a - q * b;
    if (r.further > 0) {
        q--;
        r += b;
    }
}

big_integer operator/(big_integer a, big_integer b) {
    if (b.size() == 1) {
        if (b.further > 0) {
            a = -a;
            b = -b;
        }
        return div_long_short(a, b[0]);
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
