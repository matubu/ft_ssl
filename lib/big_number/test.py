import pwn
import random
import time

for i in range(100):
	a = random.randint(0, 2**10000)
	b = random.randint(0, 2**10000)

	argv = ["./test", f"{a:x}", f"{b:x}"]

	before=time.perf_counter()
	conn = pwn.process(argv)

	lines = [conn.recvline() for _ in range(4)]
	after=time.perf_counter()
	print("me", after-before)

	ra  =int(lines[0], base=16)
	rb  =int(lines[1], base=16)
	radd=int(lines[2], base=16)
	rmul=int(lines[3], base=16)

	conn.close()

	before=time.perf_counter()
	add = a+b
	mul = a*b
	after=time.perf_counter()
	print("py", after-before)

	print(f"a   = {a:x}")
	print(f"ra  = {ra:x}")
	print(f"b   = {b:x}")
	print(f"rb  = {rb:x}")
	print(f"add = {add:x}")
	print(f"radd= {radd:x}")
	print(f"mul = {mul:x}")
	print(f"rmul= {rmul:x}")

	assert ra  == a
	assert rb  == b
	assert radd == add
	assert rmul == mul
