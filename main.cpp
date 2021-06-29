#include <iostream>
#include "file_io.hpp"

int main(void)
{
    std::cout << "Hello world!\n";

    sys::File f{};

    try {
        f = sys::open("/etc/host", O_RDONLY);
    } catch (const sys::Error &e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
