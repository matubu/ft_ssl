ref=$(openssl des-ecb -S 4242 -pbkdf2 -P -in a -pass "pass::test")

for n in {1..100000};
do
	echo $n
	a=$(openssl des-ecb -S 4242 -iter $n -P -in a -pass "pass::test")
	if [ "$a" = "$ref" ]; then
		echo "Found it!"
		echo $n
		break
	fi
done