#!/bin/python3

import os
import sys

for i in range(1300):
	os.system(f"../ft_ssl sha512 -s {'a' * i} > /dev/null")
	sys.stdout.write(".")
	sys.stdout.flush()