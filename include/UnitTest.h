#include <string>

class UnitTest {
public:
    virtual ~UnitTest() {};
    virtual void run() = 0;
    virtual std::string getTestName() const = 0;
};