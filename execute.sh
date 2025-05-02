#!/bin/bash

# Run the makefiles
echo "Running make on Existence.c..."
make -f makenexist

echo "Running make on Nexistence.c ..."
make -f makenexist

echo "Running make on History.c ..."
make -f makehistory

echo "Running make on Depth.c ..."
make -f makedepth

cd pmc && make
cd ..

echo -e "\n\nBinaries created ..."

# List of expected binary names
binaries=("existence" "nexistence" "history" "depth")

# Check each binary and print if it exists
for bin in "${binaries[@]}"; do
    if [[ -f "$bin" && -x "$bin" ]]; then
        echo "$bin"
    fi
done
