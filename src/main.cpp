#include <stdexcept>

#include "engine.h"
#include "utils.h"

int main()
{
    try {
        Engine engine{};
        engine.run();
    } catch (const std::runtime_error& err) {
        log("ENGINE", err.what(), Logtype::ERROR);
    }
    return 0;
}
