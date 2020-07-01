#pragma once

#include <vector>
#include <string>

class big_integer {
    std::vector<uint32_t> digits;
    uint32_t further;

    uint32_t operator[](size_t) const;

    friend void trim(big_integer&, size_t);

    friend big_integer &evaluate(big_integer&, const big_integer&, uint32_t,
        uint32_t (*)(uint32_t, uint32_t, uint32_t&));

    friend int unsigned_cmp(const big_integer&, const big_integer&);

    friend big_integer div_long_short(big_integer, uint32_t);

    friend void unsigned_div(big_integer&, big_integer&, big_integer&, big_integer&);
    friend void unsigned_div_small_ans(big_integer&, big_integer&, uint32_t&, big_integer&);
    friend big_integer suffix(const big_integer&, size_t);

public:
    big_integer();
    big_integer(int);
    big_integer(uint32_t);
    big_integer(uint64_t);
    big_integer(const big_integer &);
    ~big_integer() = default;
    explicit big_integer(const std::string&);

    friend std::string to_string(big_integer);

    big_integer& operator=(const big_integer&);

    friend bool operator==(const big_integer&, const big_integer&);
    friend bool operator!=(const big_integer&, const big_integer&);
    friend bool operator>(const big_integer&, const big_integer&);
    friend bool operator<(const big_integer&, const big_integer&);
    friend bool operator>=(const big_integer&, const big_integer&);
    friend bool operator<=(const big_integer&, const big_integer&);

    friend big_integer operator~(big_integer);

    friend big_integer &operator&=(big_integer&, const big_integer&);
    friend big_integer operator&(big_integer, const big_integer&);

    friend big_integer &operator|=(big_integer&, const big_integer&);
    friend big_integer operator|(big_integer, const big_integer&);

    friend big_integer &operator^=(big_integer&, const big_integer&);
    friend big_integer operator^(big_integer, const big_integer&);

    friend big_integer &operator<<=(big_integer&, int);
    friend big_integer operator<<(big_integer, int);

    friend big_integer &operator>>=(big_integer&, int);
    friend big_integer operator>>(big_integer, int);

    friend big_integer operator+(const big_integer&);
    friend big_integer &operator+=(big_integer&, const big_integer&);
    friend big_integer operator+(big_integer, const big_integer&);
    friend big_integer &operator++(big_integer&);
    friend big_integer operator++(big_integer&, int);

    friend big_integer operator-(big_integer);
    friend big_integer &operator-=(big_integer&, const big_integer&);
    friend big_integer operator-(big_integer, const big_integer&);
    friend big_integer &operator--(big_integer&);
    friend big_integer operator--(big_integer&, int);

    friend big_integer &operator*=(big_integer&, const big_integer&);
    friend big_integer operator*(big_integer, big_integer);

    friend big_integer &operator/=(big_integer&, const big_integer&);
    friend big_integer operator/(big_integer, big_integer);

    friend big_integer &operator%=(big_integer&, const big_integer&);
    friend big_integer operator%(big_integer, const big_integer&);
};

