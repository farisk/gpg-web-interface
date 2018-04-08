print_string("Starting program...");
print_string(Friendsonly.version);


let test_data = {|-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

hello world! this is the original message
-----BEGIN PGP SIGNATURE-----
Comment: GPGTools - https://gpgtools.org

iQIcBAEBCgAGBQJayQnpAAoJED8TPppOFLmSxbwP/R+IGFDbFKjYchw2semNIlfC
PLDbGnMt5zxiSYKZB3fZM0MkUxf2dXmmnvhiT0/kHzCJcJi4amkcG4Lh2JiZmfEu
L98UZhmGVNvG2yzh9o7gi0Q1yVqRSJg0GdcHo3bc07lzFFurTcTKQFfT/Mq8MtOV
jHeJqGjIdqVTuHgDnrjUADVVHw7g6FUreFKNngcFavx6X3qQOKBbkPgkI2L0RTCd
3L0PAvXxbpUcYMy2iKW/ZhNVig2sdAIAUeuZAqufPsA6V0huel29WbYAYuCb85z8
rRPILyx08Clr7jBRA6xJweOXM9GkNjOHbrWNjIfhhOYdBQmnSWmXvVDBD0vTxOXc
4jW7OQYVg7IkQiU7gw7oOGUq23SopN4HJzlXIizKV125Vz9Ox2MNhvYy07/7Rg0V
jQzpmq6jdne3yv6RPp1q3A7ndoGnyGnv82yLfOpxz5KOTIatjMYDxJi/sp6pFmXl
//ffaCqepIVLoZekBjeO7NDNf1bDJGSZwBmEhi6RnW5+wkE0xvpIwzAsNK6oOGnf
CWuYtowo65AN+V84evejnPcHjB1+UUHsmGolPy50xyub7xFa4QMA3espXxOQ5oSq
1oTRYsXvcM/+wUE9RWVzI3vLAtrbCTwG6MBrpjNzo+yDaEprwO2H4nrh/E4sNhXo
VHxaSf3cjQwZvUZ7miwV
=lX8e
-----END PGP SIGNATURE-----|};

let msg: Friendsonly.message = {
  body: test_data,
};

Lwt_main.run(Unixcrypto.BasicFriendsonly.store_message(msg));
