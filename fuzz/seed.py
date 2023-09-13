import os
import shutil

f = open("seed.txt", "r")

shutil.rmtree('seeds', ignore_errors=True)
os.mkdir('seeds')

# loop over every lines in the file
for i, line in enumerate(f.readlines()):
	new_f = open(f"seeds/{i}.txt", "w")
	new_f.write(line[:-1].replace(" ", "\0"))
	print(line[:-1])