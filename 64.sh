echo -n $1 | ./ft_ssl base64 -q | tee base64_ft_ssl
echo -n $1 | openssl base64 | tee base64_openssl

# ./a.out | ./ft_ssl base64 -q > base64_ft_ssl
# ./a.out | openssl base64 > base64_openssl


diff --color=always -s base64_ft_ssl --label "ft_ssl" base64_openssl --label "openssl"

rm -rf base64_ft_ssl base64_openssl