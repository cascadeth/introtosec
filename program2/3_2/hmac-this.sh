#!/bin/bash

if [ ! -d ./keys ]
then
	mkdir ./keys
fi

if [ ! -d ./hmacs ]
then
	mkdir ./hmacs
fi


# Generate 128bit hash of argument 1 for key
openssl dgst -md5 $1 | cut -d " " -f 2 > ./keys/len-128.key

# Generate 160bit hash of argument 1 for key
openssl dgst -ripemd160 $1 | cut -d " " -f 2 > ./keys/len-160.key

# Generate 256bit hash of argument 1 for key
openssl dgst -sha256 $1 | cut -d " " -f 2 > ./keys/len-256.key


# Develop HMAC-SHA256 of argument 2 using each key
openssl dgst -sha256 -hmac ./keys/len-128.key $2 | cut -d " " -f 2 > ./hmacs/"$2"-sha256-128key.hmac
openssl dgst -sha256 -hmac ./keys/len-160.key $2 | cut -d " " -f 2 > ./hmacs/"$2"-sha256-160key.hmac
openssl dgst -sha256 -hmac ./keys/len-256.key $2 | cut -d " " -f 2 > ./hmacs/"$2"-sha256-256key.hmac


# Develop HMAC-SHA512 of argument 2 using each key
openssl dgst -sha512 -hmac ./keys/len-128.key $2 | cut -d " " -f 2 > ./hmacs/"$2"-sha512-128key.hmac
openssl dgst -sha512 -hmac ./keys/len-160.key $2 | cut -d " " -f 2 > ./hmacs/"$2"-sha512-160key.hmac
openssl dgst -sha512 -hmac ./keys/len-256.key $2 | cut -d " " -f 2 > ./hmacs/"$2"-sha512-256key.hmac


printf "\n"

printf "HMAC-SHA256 using 128bit key: "
cat ./hmacs/"$2"-sha256-128key.hmac

printf "HMAC-SHA256 using 160bit key: "
cat ./hmacs/"$2"-sha256-160key.hmac

printf "HMAC-SHA256 using 256bit key: "
cat ./hmacs/"$2"-sha256-256key.hmac

printf "\n##############################################################################################################################################################\n\n"

printf "HMAC-SHA512 using 128bit key: "
cat ./hmacs/"$2"-sha512-128key.hmac

printf "HMAC-SHA512 using 160bit key: "
cat ./hmacs/"$2"-sha512-160key.hmac

printf "HMAC-SHA512 using 256bit key: "
cat ./hmacs/"$2"-sha512-256key.hmac


