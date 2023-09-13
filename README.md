# ft_ssl
🔐 A custom C implementation of base64, des, md5, sha256, sha512, sm3 and rsa

Run old openssl version on nixos:

```sh
export NIXPKGS_ALLOW_INSECURE=1; nix-shell -p openssl_1_1
```

openssl rsault usage example:

```sh
echo -n 12345678 | openssl rsautl -inkey test_ft_pub -pubin -encrypt -raw | openssl rsautl -inkey test_ft -decrypt -raw

echo aaaaaaa | openssl rsautl -inkey test_ft -encrypt -raw | ./ft_ssl rsautl -inkey test_ft -decrypt
echo aaaaaaa | ./ft_ssl rsautl -inkey test_ft -encrypt | openssl rsautl -inkey test_ft -decrypt -raw

echo aaaaaaa | ./ft_ssl rsautl -inkey test_ft -encrypt | ./ft_ssl rsautl -inkey test_ft -decrypt
```

data too large for key size:

```sh
echo -ne "\xff\xff\xff\xff\xff\xff\xff\xff" | openssl rsautl -inkey test_ft -encrypt -raw
echo -ne "\xff\xff\xff\xff\xff\xff\xff\xff" | ./ft_ssl rsautl -inkey test_ft -encrypt
```