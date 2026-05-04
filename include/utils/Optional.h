#pragma once

template <typename T>
class Optional {
public:
    Optional() : has_(false), val_() {}
    Optional(const T& v) : has_(true), val_(v) {}
    bool has_value() const { return has_; }
    const T& value() const { return val_; }
    T value_or(const T& def) const { return has_ ? val_ : def; }
private:
    bool has_;
    T val_;
};
