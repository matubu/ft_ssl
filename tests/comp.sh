# usage: comp.sh [command] [file]

if [ $3 != '--minimal' ]; then
	(cd .. && make)
fi

../ft_ssl $1 $2 | awk '{print $NF}' > .ft_ssl.txt
openssl $1 $2 | awk '{print $NF}' > .openssl.txt

if [ $3 = '--minimal' ]; then
	echo -n "$2: "
else
	echo -e "<<< \x1b[1;96m FTSSL \x1b[0m >>>"
	cat .ft_ssl.txt
	echo -e "<<< \x1b[1;96mOPENSSL\x1b[0m >>>"
	cat .openssl.txt
fi

diff --color=always -s .ft_ssl.txt --label "ft_ssl" .openssl.txt --label "openssl"

rm -rf .ft_ssl.txt .openssl.txt