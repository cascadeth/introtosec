#!/bin/bash

original=$(echo $1 | cut -d '.' -f 1)
altered=$(echo $2 | cut -d '.' -f 1)

#Crypto hashes to use
h1="sha256"
h2="sha512"
h3="ripemd160"



# generate SHA-256 hash and output to file
openssl dgst -"$h1" -out temp "$1"
openssl dgst -$h1 -out temp_alt "$2"

# cut the useless jargon in front of the hash value
cut -d " " -f 2 < temp > "$original".$h1
cut -d " " -f 2 < temp_alt > "$altered".$h1



# generate SHA-512 hash and output to file
openssl dgst -$h2 -out temp "$1"
openssl dgst -$h2 -out temp_alt "$2"

# cut again
cut -d " " -f 2 < temp > "$original".$h2
cut -d " " -f 2 < temp_alt > "$altered".$h2



# generate RIPEMD-160 hash and output to file
openssl dgst -"$h3" -out temp "$1"
openssl dgst -"$h3" -out temp_alt "$2"

# cut again
cut -d " " -f 2 < temp > "$original"."$h3"
cut -d " " -f 2 < temp_alt > "$altered"."$h3"

# remove our temp files
rm temp temp_alt

stoopid_printf_divider="---"

# print a short template
printf "####################################################\n"
printf "<  Hashed Original\n"
printf "%s\n" $stoopid_printf_divider
printf ">  Hashed Altered-by-1-bit\n"
printf "=> Matching Hex Chars (4 bit binary values)\n"
printf "   Match Count\n"
printf "####################################################\n"



printf "\n\n\n###################################  SHA-256  ###################################  \n"
diff "$original.$h1" "$altered.$h1"	# Print if different(will print entire line)

char1=0					# A single char from original hash
char2=0					# A single char from the altered hash

count=0					# Counter

hash_1=$(cat $original.$h1)			# Get the string from original hash
hash_2=$(cat $altered.$h1)			# Get the string from the altered hash

printf "=>"				# Print leading spaces

for (( i=0; i<${#hash_1}; i++ ))	# For i less than length of the hash, increment through
do
        char1=${hash_1:$i:1}		# Snag char from original hash string
        char2=${hash_2:$i:1}		# Snag char from altered hash string
        if [[ $char1 == $char2 ]]	# If both are the same char
        then
		printf "$char1"		# Print the char "in column"
                ((count = count + 1))	# Increment count of same char
        else
		printf "_" 	# Else print a space, signifying the chars are different
	fi

done

printf "\n  matching hex values: $count  <><><>  non-matching: $(($i - $count))\n"	# Print number of similar chars




printf "\n\n\n###################################  SHA-512  ###################################  \n"

diff "$original.$h2" "$altered.$h2"


count=0

hash_1=$(cat "$original".$h2)
hash_2=$(cat "$altered".$h2)

printf "=>"

for (( i=0; i<${#hash_1}; i++ ))
do
        char1=${hash_1:$i:1}
        char2=${hash_2:$i:1}
        if [[ $char1 == $char2 ]]
        then
                printf "$char1"
                ((count = count + 1))
        else
                printf "_"
        fi

done

printf "\n  matching hex values: $count  <><><>  non-matching: $(($i - $count))\n"




printf "\n\n\n###################################  RIPEMD160  ###################################  \n"

diff "$original"."$h3" "$altered"."$h3"


count=0

hash_1=$(cat $original."$h3")
hash_2=$(cat $altered."$h3")

printf "=>"

for (( i=0; i<${#hash_1}; i++ ))
do
        char1=${hash_1:$i:1}
        char2=${hash_2:$i:1}
        if [[ $char1 == $char2 ]]
        then
                printf "$char1"
                ((count = count + 1))
        else
                printf "_"
        fi

done

printf "\n  matching hex values: $count  <><><>  non-matching: $(($i - $count))\n" 




if [ ! -d ./hashes ]
then
  mkdir ./hashes
fi
mv *."$h1" ./hashes
mv *."$h2" ./hashes
mv *."$h3" ./hashes

# DONE
exit
