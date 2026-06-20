#include "yoyo.hpp"
#include "fs.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    // Setup yoyo
    yoyo::init(argc, argv);
    // Run main.py
    auto contents = yoyo::fs::iread_file("src/main.py");
    auto res = yoyo::run(pxs_Runtime::pxs_Python, contents, "src/main.py");
    if (res.is(pxs_Exception)) {
        std::cout << res.get_string() << std::endl;
    }
    yoyo::stop();
}