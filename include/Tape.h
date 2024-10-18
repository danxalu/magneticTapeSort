#pragma once
#include <cstdint>
#include <string>

class Tape {
public:
    virtual ~Tape() {}

    virtual void rewind() = 0;
    virtual void moveForward() = 0;
    virtual void moveBackward() = 0;
    virtual void write(const int32_t&) = 0;
    virtual int32_t read() = 0;
    virtual std::string getfileName() const = 0;
};