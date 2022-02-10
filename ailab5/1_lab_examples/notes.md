# Make and Install

```
wget https://github.com/Gecode/gecode/archive/release-6.2.0.tar.gz
tar -xvf release-6.2.0.tar.gz
cd gecode-release-6.2.0/
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$PWD/../install
make && make install
```

# Using Gecode

Modelling in Gecode
Section 2.3.3
Linux and relatives

For GCC the *library link order* is important. The order is shown in this
section of the book as:

-lgecodeflatzinc -lgecodedriver -lgecodegist -lgecodesearch -lgecodeminimodel -lgecodeset -lgecodefloat -lgecodeint -lgecodekernel -lgecodesupport

So, to compile `solution-lab3.cpp`:

$ g++ -I gecode-release-6.2.0/install/include solution-lab3.cpp -L gecode-release-6.2.0/install/lib -lgecodeflatzinc -lgecodedriver -lgecodesearch -lgecodeminimodel -lgecodeset -lgecodefloat -lgecodeint -lgecodekernel -lgecodesupport -lpthread
