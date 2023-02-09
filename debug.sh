for n in {1..10000};
do
	openssl des-ecb -S 4242 -iter $n -P -in a -pass "pass::$(cat password_file)"
done