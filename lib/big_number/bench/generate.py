import random

for i in range(10_000):
	a = random.randint(0, 16**60)
	b = random.randint(0, 16**60)

	print(f"{a:064x} {b:064x}")