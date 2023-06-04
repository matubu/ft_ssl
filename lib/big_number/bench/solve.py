with open('data.txt', 'r') as f:
	lines = f.readlines()

	for line in lines:
		a = int(line[:64], base=16)
		b = int(line[65:], base=16)

		print(f"{a*b:0128x}")