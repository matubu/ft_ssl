make

echo

echo -e "<<< \x1b[1;96m FTSSL \x1b[0m >>>"
./ft_ssl $1 $2 | awk '{print $NF}' | tee .ft_ssl.txt

echo

echo -e "<<< \x1b[1;96mOPENSSL\x1b[0m >>>"
openssl $1 $2 | awk '{print $NF}' | tee .openssl.txt

echo

diff --color=always -s .ft_ssl.txt --label ft_ssl .openssl.txt --label openssl

echo

rm -rf .ft_ssl.txt
rm -rf .openssl.txt