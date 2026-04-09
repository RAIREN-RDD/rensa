# Rensa / 連鎖
A parallelized C++23 build system with integrated incremental compilation and VCS primitives.

## Quick Start
```bash
# Download binary
curl -fL -o rensa https://rairen.net/redirect/rensa-linux-86_64-gnu

# Install to /usr/local/bin or in a location of your choice
sudo mv rensa /usr/local/bin/rensa

# Check version
rensa version
# rensa 1.0.0 commit 83bc3d9f60a3def3590f769bf60c764be6781ebd

# Create a C++23 parallelized project
rensa create-project name:MyProject template:cpp23-parallelized
# Project 'MyProject' with template 'cpp23-parallelized' created successfully.

cd MyProject
ls -a
# .rensa/ include/ README.md src/ rensa.build

cat src/main.cpp
# #include <iostream>
# 
# int main() {
# 
#   return 0;
# }

# Build the current project in parallel where rensa.build is located
rensa build .
# rensa: [INFO] mkdir build
# rensa: [INFO] g++ -Iinclude -std=c++23 -c src/main.cpp -o build/main.o
# rensa: [INFO] g++ -Iinclude -std=c++23 build/main.o -o MyProject
# rensa: [INFO] Elapsed: 858462 us | Energy used: 75.544656 J

# Check output files
ls build
# MyProject main.o

# Check Rensa's project config and cache
ls .rensa
# config cache

# Cache for incremental compilation
cat cache
# src/main.cpp | g++ -Iinclude -std=c++23 -c src/main.cpp -o build/main.o |  2290e510b3b3133e4384c66e241b70d6b04767c25edde996a94ba1f536fcb9938dacf0a95c1bd899e2b5eabe8695c50be241ed3841e21d034102e3815289bdc8

# Add source to confirm incremental compilation
echo "void(){}" > src/test.cpp

# Build
rensa build .
# rensa: [INFO] g++ -Iinclude -std=c++23 -c src/test.cpp -o build/test.o
# rensa: [INFO] g++ -Iinclude -std=c++23 build/main.o build/test.o -o MyProject
# rensa: [INFO] Elapsed: 458462 us | Energy used: 45.544656 J
`
# Check output files
ls build
# MyProject main.o test.o
```

## Installation
```bash
# Download binary
curl -o rensa https://rairen.net/redirect/rensa-linux-86_64-gnu

# Install to /usr/local/bin or in a location of your choice
sudo mv rensa /usr/local/bin/rensa # requires sudo
```

## Core Usage (will update)
```bash
Usage: rensa <command> [options]

Commands:
  version: Print project version and commit hash
  help: Prints this message. Add <command> for detailed related usage instructions.
  init: Initialize the current directory as a Rensa project.

Project: https://codeberg.org/rairen-rdd/rensa
For irregularities, contact Kai: https://reiren.net/contact
```

## Contributing
Soon

## License
This project is licensed under the [MIT License](/LICENSE)

## Contact
For inquiries or irregularities, contact Kai at: [https://reiren.net/contact](https://reiren.net/contact)

## Donations
Coming soon.
