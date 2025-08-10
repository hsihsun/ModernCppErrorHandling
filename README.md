
Please download the repository and run the following command under the directory contains the c++ code files.

.rm -rf build
mkdir build && cd build
cmake ..
cmake --build .
ctest --output-on-failure

The code is wriiten under MacOS environment. 
