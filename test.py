import os

for i in range(1300):
	os.system(f"./ft_ssl sha512 -s {'a' * i}")