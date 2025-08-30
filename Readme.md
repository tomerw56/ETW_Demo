# General
A project to demo wpa and wpr , the code demonstrates verious issues and has innstructions on how to debug them

# Prerquisits
- cmake
- doxygen
- wpa for your windows version - https://learn.microsoft.com/en-us/windows-hardware/get-started/adk-install

# How to build using Cmake
```
mkdir build
cd build
cmake ..
cmake --build .          # build your code
cmake --build . --target doc_doxygen   # generate docs

```

this will generate a build folder with debug and release folder
