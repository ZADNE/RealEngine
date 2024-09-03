#!/usr/bin/env python

import os, sys, subprocess, multiprocessing, json

# Get arguments
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
pool = multiprocessing.Pool(4)
pool.map(runClangTidy, json)
pool.close()
pool.join()

# Report output
if os.stat(outputFilepath).st_size > 0:
    print(f'-------------------------------------------------')
    print(f'Some files did not pass linting! Problems follow:')
    with open(outputFilepath, 'r') as f:
        print(f.read())
    sys.exit(1)
sys.exit(0)
