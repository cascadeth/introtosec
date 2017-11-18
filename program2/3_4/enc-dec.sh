#!/bin/bash

# The cipher types to use

cipher1='-aes-128-cbc'



# The Cipher Key and Initialization Vector

key=123456789010293847560987654321
iv=09876543211029384756



# Encipher the input file argument with both ciphers

openssl enc $cipher1 -e -in $1 -out encrypt1.bin -K $key -iv $iv
openssl enc $cipher1 -e -in encrypt1.bin -out encrypt2.bin -K $key -iv $iv
openssl enc $cipher1 -e -in encrypt2.bin -out encrypt3.bin -K $key -iv $iv


openssl enc $cipher1 -d -in encrypt2.bin -out decrypt2.txt -K $key -iv $iv
openssl enc $cipher1 -d -in encrypt3.bin -out decrypt3.txt -K $key -iv $iv


