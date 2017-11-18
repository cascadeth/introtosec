#!/bin/bash


# Grab the original file name to use for the output files

original=$(echo $1 | cut -d "." -f 1)



# The cipher types to use

cipher1='-aes-256-cbc'
cipher2='-aes-256-ecb'



# The Cipher Key and Initialization Vector

key=123456789010293847560987654321
iv=09876543211029384756



# Encipher the input file argument with both ciphers

openssl enc $cipher1 -e -in $1 -out "$original""$cipher1".bin -K $key -iv $iv
openssl enc $cipher2 -e -in $1 -out "$original""$cipher2".bin -K $key
