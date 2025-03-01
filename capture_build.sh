#!/bin/bash

# Run the build command and capture both stdout and stderr into a temporary file
./build.sh -p 1 2>&1 | tee /tmp/build_output.log

# Filter the output to include only warnings and errors, then copy to clipboard
grep -E "warning:|error:" /tmp/build_output.log | pbcopy

# Clean up the temporary file
rm /tmp/build_output.log

# Notify the user
echo "Build output (warnings and errors) copied to clipboard!"