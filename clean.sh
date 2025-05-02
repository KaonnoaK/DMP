#!/bin/bash

echo "Cleaning up build files..."

find . -type f -name '*.o' -exec rm -v {} \;

find . -maxdepth 1 -type f -executable ! -name '*.sh' ! -name '*.*' -exec rm -v {} \;


echo "Clean-up complete. "
