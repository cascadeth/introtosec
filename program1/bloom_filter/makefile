
bloom:
	gcc bloom.c -o bloom_filter

exec: 
	./bloom_filter -d dictionary.txt -i sample_input.txt -o output3.txt output5.txt
	sleep 2
	cat output3.txt
	sleep 2
	cat output5.txt

clean:
	rm -f output*.txt bloom_filter

