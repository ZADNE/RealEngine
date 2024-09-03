#!/usr/bin/env python

import os, sys, subprocess, json

# Get arguments
print("Arguments: " + str(sys.argv))
clangTidyFilepath = sys.argv[1]
jsonFilepath = sys.argv[2]

# Parse JSON to dict
json = json.load(open(jsonFilepath))

# Runs clang-tidy on one entry
# Return 1 if failed, 0 otherwise
def runClangTidy(entry):
    filepath = entry['file']
    print("Checking: " + filepath)
    command = entry['command']
    proc = subprocess.Popen(f'{clangTidyFilepath} {filepath} -- {command}', shell=True)
    return proc.wait() != 0

# Check files
failedCount = 0
for entry in json:
    failedCount += runClangTidy(entry)

# Report output
if failedCount > 0:
    print(f'{failedCount} files did not pass linting!')
    sys.exit(1)
sys.exit(0)
