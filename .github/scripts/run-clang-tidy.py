#!/usr/bin/env python

import os, sys, subprocess, json

# Get arguments
print("Arguments: " + str(sys.argv))
clangTidyFilepath = sys.argv[1]
jsonFilepath = sys.argv[2]
outputFilepath = sys.argv[3]

open(outputFilepath, 'a').close()

# Parse JSON to dict
json = json.load(open(jsonFilepath))

# Runs clang-tidy on one entry
def runClangTidy(entry):
    filepath = entry['file']
    print("Checking: " + filepath, flush=True)
    command = entry['command']
    proc = subprocess.Popen(f'{clangTidyFilepath} --quiet {filepath} -- {command} 2>/dev/null | tee -a {outputFilepath}', shell=True)
    proc.wait()

# Check files
for entry in json:
    runClangTidy(entry)

# Report output
if os.stat(outputFilepath).st_size > 0:
    print(f'Some files did not pass linting! Problems follow:')
    log = open(outputFilepath, "r")
    for line in log:
        print(line)
    sys.exit(1)
sys.exit(0)
