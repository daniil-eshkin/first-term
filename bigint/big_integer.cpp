#include "big_integer.h"
#include <cmath>
#include <algorithm>
#include <stdexcept>

void trim(big_integer &a, size_t s) {
    while (a.digits.size() < s) {
        a.digits.push_back(a.further);
    }
}

/*void shrink(big_integer &a) {
    while (a.digits.size() > 1 && a.digits.back() == a.further) {
        a.digits.pop_back();
    }
}*/

uint32_t big_integer::operator[](size_t index) const {
    return (index < digits.size() ? digits[index] : further);
}

static std::function<uint32_t(uint32_t, uint32_t, uint32_t&)> adc =
        [](uint32_t a, uint32_t b, uint32_t &d) {
    uint32_t c = a + b + d;
    d = (c < a || (d == 1 && c <= a));
    return c;
};

static std::function<uint32_t(uint32_t, uint32_t, uint32_t&)> sbb =
        [](uint32_t a, uint32_t b, uint32_t &d) { return adc(a, ~b, d); };

static std::function<uint32_t(uint32_t, uint32_t)> and_ =
        [](uint32_t a, uint32_t b) { return a & b; };

static std::function<uint32_t(uint32_t, uint32_t)> or_ =
        [](uint32_t a, uint32_t b) { return a | b; };

static std::function<uint32_t(uint32_t, uint32_t)> xor_ =
        [](uint32_t a, uint32_t b) { return a ^ b; };

big_integer &evaluate(big_integer &a, const big_integer &b, uint32_t d,
    std::function<uint32_t(uint32_t, uint32_t, uint32_t&)> &eval) {
    trim(a, b.digits.size());
    a.digits.push_back(a.further);
    for (size_t i = 0; i < a.digits.size(); ++i) {
        a.digits[i] = eval(a.digits[i], b[i], d);
    }
    a.further = (a.digits.back() & 2 ? ~0 : 0);
    //shrink(a);
    while (a.digits.size() > 1 && a.digits.back() == a.further) {
        a.digits.pop_back();
    }

    return a;
}

big_integer &bitwise(big_integer &a, const big_integer &b,
    std::function<uint32_t(uint32_t, uint32_t)> &eval) {
    trim(a, b.digits.size());
    for (size_t i = 0; i < a.digits.size(); ++i) {
        a.digits[i] = eval(a.digits[i], b[i]);
    }
    a.further = eval(a.further, b.further);
    //shrink(a);
    while (a.digits.size() > 1 && a.digits.back() == a.further) {
        a.digits.pop_back();
    }

    return a;
}

int unsigned_cmp(const big_integer &a, const big_integer &b) {
    for (size_t i = std::max(a.digits.size(), b.digits.size()) - 1;; --i) {
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

big_integer::big_integer()
    : further(0) {
    digits.push_back(0);
}

big_integer::big_integer(int a)
    : further(a < 0 ? ~0 : 0) {
    digits.push_back(a);
}

big_integer::big_integer(uint32_t a)
    : further(0) {
    digits.push_back(a);
}

big_integer::big_integer(uint64_t a)
    : further(0) {
    digits.push_back(static_cast<uint32_t>(a & UINT32_MAX));
    digits.push_back(static_cast<uint32_t>((a >> 32) & UINT32_MAX));
}

big_integer::big_integer(const big_integer &a)
    : further(a.further)
    , digits(a.digits) {
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
    bool sign = false;
    if (s[0] == '-') {
        i++;
        sign = true;
    }
    for (; i < s.size(); ++i) {
        *this *= 10;
        *this += (s[i] - '0');
    }
    if (sign) {
        negate(*this);
    }
}

std::string to_string(big_integer a) {
    if (a.further == 0 && a.digits.size() == 1 && a.digits[0] == 0) {
        return "0";
    }
    std::string s;
    int sign = 1;
    if (a.further != 0) {
        sign = -1;
        negate(a);
    }
    while (a.further != 0 || a.digits.size() != 1 || a.digits[0] != 0) {
        s.push_back((a % 10).digits[0] + '0');
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
    return a.further == b.further && unsigned_cmp(a, b) == 0;
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
    for (size_t i = 0; i < a.digits.size(); ++i) {
        a.digits[i] = ~a.digits[i];
    }
    a.further = ~a.further;
    return a;
}

big_integer &operator&=(big_integer &a, const big_integer &b) {
    return bitwise(a, b, and_);
}

big_integer operator&(big_integer a, const big_integer &b) {
    return a &= b;
}

big_integer &operator|=(big_integer &a, const big_integer &b) {
    return bitwise(a, b, or_);
}

big_integer operator|(big_integer a, const big_integer &b) {
    return a |= b;
}

big_integer &operator^=(big_integer &a, const big_integer &b) {
    return bitwise(a, b, xor_);
}

big_integer operator^(big_integer a, const big_integer &b) {
    return a ^= b;
}

big_integer &operator<<=(big_integer &a, int s) {
    if (s == 0 || (a.further == 0 && a.digits.size() == 1 && a.digits[0] == 0)) {
        return a;
    } else if (s < 0) {
        return a >>= -s;
    } else {
        uint32_t s1 = s / 32;
        trim(a, a.digits.size() + s1);
        for (size_t i = a.digits.size() - 1;; --i) {
            a.digits[i] = (i >= s1 ? a.digits[i - s1] : 0);
            if (i == 0) {
                break;
            }
        }
        s %= 32;
        if (s != 0) {
            a *= (1u << s);
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
        if (s1 >= a.digits.size()) {
            a.digits.clear();
            a.digits.push_back(a.further);
            return a;
        }
        for (size_t i = 0; i < a.digits.size() - s1; ++i) {
            a.digits[i] = a.digits[i + s1];
        }
        for (size_t i = 0; i < s1; ++i) {
            a.digits.pop_back();
        }
        s %= 32;
        if (s != 0) {
            if (a.further != 0) {
               a -= (1u << s) - 1;
            }
            a /= (1u << s);
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
    size_t i = 0;
    for (; i < a.digits.size() && a.digits[i] == (uint32_t)-1; ++i) {
        a.digits[i] = 0;
    }
    if (i < a.digits.size()) {
        ++a.digits[i];
    } else {
        if (a.further == 0) {
            a.digits.push_back(1);
        } else {
            a.further = 0;
        }
    }
    return a;
}

big_integer operator++(big_integer &a, int) {
    big_integer old = a;
    ++a;
    return old;
}

void negate(big_integer& a) {
    for (size_t i = 0; i < a.digits.size(); ++i) {
        a.digits[i] = ~a.digits[i];
    }
    a.further = ~a.further;
    ++a;
}

big_integer operator-(big_integer a) {
    negate(a);
    return a;
}

big_integer &operator-=(big_integer &a, const big_integer &b) {
    return evaluate(a, b, 1, sbb);
}

big_integer operator-(big_integer a, const big_integer &b) {
    return a -= b;
}

big_integer& operator--(big_integer &a) {
    size_t i = 0;
    for (; i < a.digits.size() && a.digits[i] == 0; ++i) {
        a.digits[i] = ~0;
    }
    if (i < a.digits.size()) {
        --a.digits[i];
    } else {
        if (a.further == 0) {
            a.further = ~0;
        } else {
            a.digits.push_back(~0 - 1);
        }
    }
    return a;
}

big_integer operator--(big_integer &a, int) {
    big_integer old = a;
    --a;
    return old;
}

big_integer &operator*=(big_integer &a, const big_integer &b) {
    return a = a * b;
}

big_integer operator*(big_integer a, big_integer b) {
    if (a.further == 0 && a.digits.size() == 1 && a.digits[0] == 0) {
        return a;
    } else if (b.further == 0 && b.digits.size() == 1 && b.digits[0] == 0) {
        return a = 0;
    }
    int sign = 1;
    if (a.further != 0) {
        sign = -sign;
        negate(a);
    }
    if (b.further != 0) {
        sign = -sign;
        negate(b);
    }
    big_integer c;
    uint32_t n = a.digits.size(), m = b.digits.size();
    trim(c, n + m);
    for (size_t i = 0; i < n; ++i) {
        uint32_t d = 0;
        for (size_t j = 0; j < m || d; ++j) {
            uint64_t cur = static_cast<uint64_t>(a.digits[i]) * b[j]
                    + d + c.digits[i + j];
            c.digits[i + j] = static_cast<uint32_t>(cur & UINT32_MAX);
            d = static_cast<uint32_t>(cur >> 32);
        }
    }
    //shrink(c);
    while (c.digits.size() > 1 && c.digits.back() == c.further) {
        c.digits.pop_back();
    }
    if (sign == -1) {
        negate(c);
    }
    return c;
}

big_integer div_long_short(big_integer a, uint32_t b) {
    if (a.further == 0 && a.digits.size() == 1 && a.digits[0] == 0) {
        return a;
    } else if (b == 0) {
        throw std::runtime_error("Division by zero");
    }
    int sign = (a.further == 0 ? 1 : -1);
    if (a.further > 0) {
        negate(a);
    }
    size_t n = a.digits.size();
    uint32_t d = 0;
    for (size_t i = n - 1;; --i) {
        uint64_t cur = a.digits[i] +
                (static_cast<uint64_t>(d) << 32);
        a.digits[i] = static_cast<uint32_t>(cur / b & UINT32_MAX);
        d = static_cast<uint32_t>(cur % b & UINT32_MAX);
        if (i == 0) {
            break;
        }
    }
    //shrink(a);
    while (a.digits.size() > 1 && a.digits.back() == a.further) {
        a.digits.pop_back();
    }
    if (sign == -1) {
        negate(a);
    }
    return a;
}

big_integer suffix(const big_integer &a, size_t len) {
    big_integer b;
    b.digits.pop_back();
    for (size_t i = 0, n = a.digits.size(); i < len; ++i) {
        b.digits.push_back(a[n - len + i]);
    }
    return b;
}

void unsigned_div(big_integer &a, big_integer &b, big_integer &q, big_integer &r) {
    if (a < b) {
        q = 0;
        r = a;
        return;
    }
    uint32_t x = (1u << 31), t = 0;
        while ((b.digits.back() & x) == 0) {
            x >>= 1;
            t++;
        }
    a <<= t;
    b <<= t;
    uint32_t q1;
    size_t n = a.digits.size(), m = b.digits.size();
    if (n == m) {
        unsigned_div_small_ans(a, b, q1, r);
        q = q1;
        return;
    }
    r = suffix(a, m);
    unsigned_div_small_ans(r, b, q1, r);
    q = q1;
    for (size_t i = 0; i < n - m; ++i) {
        q <<= 32;
        r <<= 32;
        r.digits[0] = a[n - m - i - 1];
        if (r >= b) {
            unsigned_div_small_ans(r, b, q1, r);
            q.digits[0] = q1;
        }
    }
    a >>= t;
    b >>= t;
}

static uint32_t div_3_2(uint32_t a2, uint32_t a1, uint32_t a0, uint32_t b1, uint32_t b0) {
    uint64_t q = ((static_cast<uint64_t>(a2) << 32) + a1) / b1;
    while (true) {
        uint32_t c2, c1, c0;
        uint64_t d = q * b0;
        c0 = static_cast<uint32_t>(d & UINT32_MAX);
        c1 = static_cast<uint32_t>(d >> 32);
        d = c1 + q * b1;
        c1 = static_cast<uint32_t>(d & UINT32_MAX);
        c2 = static_cast<uint32_t>(d >> 32);
        if (c2 > a2 || (c2 == a2 && c1 > a1) || (c2 == a2 && c1 == a1 && c0 > a0)) {
            --q;
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
    size_t n = a.digits.size(), m = b.digits.size();
    if (n == m) {
        q = 1;
    } else {
        q = div_3_2(a[n - 1], a[n - 2], a[n - 3], b[m - 1], b[m - 2]);
    }
    r = a - q * b;
    if (r.further > 0) {
        --q;
        r += b;
    }
}

big_integer operator/(big_integer a, big_integer b) {
    if (b.digits.size() == 1) {
        if (b.further > 0) {
            negate(a);
            negate(b);
        }
        return div_long_short(a, b[0]);
    }
    int sign = 1;
    if (a.further > 0) {
        sign = -sign;
        negate(a);
    }
    if (b.further > 0) {
        sign = -sign;
        negate(b);
    }
    if (a < b) {
        return 0;
    }
    big_integer q, r;
    unsigned_div(a, b, q, r);
    if (sign == -1) {
        negate(q);
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
