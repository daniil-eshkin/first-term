#pragma once

#include <vector>
#include <string>

class big_integer {
    std::vector<unsigned int> digits;
    unsigned int further;

    unsigned int operator[](size_t) const;
    size_t size() const;

    friend void trim(big_integer&, size_t);

    friend big_integer &evaluate(big_integer&, const big_integer&, unsigned int,
        unsigned int (*)(unsigned int, unsigned int, unsigned int&));

    friend int unsigned_cmp(const big_integer&, const big_integer&);

    friend void unsigned_div(big_integer&, const big_integer&, big_integer&, big_integer&);

public:
    big_integer();
    big_integer(int);
    big_integer(const big_integer &);
    ~big_integer() = default;
    explicit big_integer(const std::string&);

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

    friend big_integer &operator*=(big_integer&, int);
    friend big_integer operator*(big_integer, int);
    friend big_integer &operator*=(big_integer&, const big_integer&);
    friend big_integer operator*(big_integer, big_integer);

    friend big_integer &operator/=(big_integer&, int);
    friend big_integer operator/(big_integer, int);
    friend big_integer &operator/=(big_integer&, const big_integer&);
    friend big_integer operator/(big_integer, big_integer);

    friend big_integer &operator%=(big_integer&, int);
    friend int operator%(big_integer, int);
    friend big_integer &operator%=(big_integer&, const big_integer&);
    friend big_integer operator%(big_integer, const big_integer&);


    friend std::string to_string(big_integer);
};

