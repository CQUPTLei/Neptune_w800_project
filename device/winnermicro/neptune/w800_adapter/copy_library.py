#! usr/bin/env python
import sys
import shutil
import os
import argparse

def main():
    parser = argparse.ArgumentParser(description='Copy prebuilt library to out dir.')
    parser.add_argument('--source', help='Source file')
    parser.add_argument('--target', help='Target file')
    args = parser.parse_args()

    source = args.source
    target = args.target

    if (source or target) is None:
        print("Source file or taget file is None.")
        return -1
    print("source is {}".format(source))
    print("target is {}".format(target))
    try:
        shutil.copyfile(source, target)
    except Exception as e:
        print("Copy {} to {} failed: {}".format(source, target, e))
        return -1
    else:
        return
        
if __name__ == "__main__":
    sys.exit(main())
    
