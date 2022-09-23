#!/bin/python3

import os

commands = ["md5", "sha224", "sha256", "sha384", "sha512", "sm3"]
testcases_folder = "./testcases"

for command in commands:
	print(f"--- {command} ---")
	for filename in os.listdir(testcases_folder):
		f = os.path.join(testcases_folder, filename)
		os.system(f"./comp.sh {command} {f} --minimal")