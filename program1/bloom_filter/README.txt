______ _                           ______ _ _ _            
| ___ \ |                          |  ___(_) | |           
| |_/ / | ___   ___  _ __ ___      | |_   _| | |_ ___ _ __ 
| ___ \ |/ _ \ / _ \| '_ ` _ \     |  _| | | | __/ _ \ '__|
| |_/ / | (_) | (_) | | | | | |    | |   | | | ||  __/ |   
\____/|_|\___/ \___/|_| |_| |_|    \_|   |_|_|\__\___|_|   
                                                                   
______ _____  ___ _________  ___ _____                       _ 
| ___ \  ___|/ _ \|  _  \  \/  ||  ___|                     | |
| |_/ / |__ / /_\ \ | | | .  . || |__         __ _ _ __   __| |
|    /|  __||  _  | | | | |\/| ||  __|       / _` | '_ \ / _` |
| |\ \| |___| | | | |/ /| |  | || |___      | (_| | | | | (_| |
\_| \_\____/\_| |_/___/ \_|  |_/\____/       \__,_|_| |_|\__,_|                          
______                                          
| ___ \                                         
| |_/ /___  ___ _ __   ___  _ __  ___  ___  ___ 
|    // _ \/ __| '_ \ / _ \| '_ \/ __|/ _ \/ __|
| |\ \  __/\__ \ |_) | (_) | | | \__ \  __/\__ \
\_| \_\___||___/ .__/ \___/|_| |_|___/\___||___/
               | |                              
               |_|                             

                                                                                 Maximillian Schmidt
                                                                                             11/1/17
                                                                          CS 370 - Intro to Security

    Usage:

            $  make

                (compile the source and header)


            $  make exec

                (execute the code with the following args)

                [ ./bloom_filter -d dictionary.txt -i sample_input.txt -o output3.txt output5.txt ]

                (pause, display output files)
            

            $  make clean

                (remove: executable, output files)



    Responses to questions:

        a) What hash functions did you choose and why (Hint: Cryptographic or non-cryptographic)? 
           What is the output range of the hash functions? What is the size of the Bloom filter in each 
           case?  If you used a library for your hash function, make sure to cite it.

        Answer:

            All hash functions are non-cryptographic.  These are meant to be speedy, and complete in the 
            shortest amount of time possible, while still providing decent hashes.  

            Funtions used:

                djb2 - Dan Bernstein's hash was used because it is quick and has fewer collisions and
                       better avalanching that most simple hashes, due to its use of two prime numbers.

                jenkins - Bob Jenkins's one-at-a-time hash function was used for similar reasons to the
                          above hash.  It's a tad more complex, but easy for a processor to handle.  

                FNV1a - Fowler-Noll-VO (iteration 1a) was used because of its primary use in checksums,
                        and speed to compute.  

                sdbm - sdbm was an algorithm created for SDBM, a clone of NDBM, was selected because it 
                       has good distribution, while still being speedy.  

                polynomial - A custom written hash that acts similarly to CRC.  Using a large prime, 
                             each iteration uses a power of a selected prime number.  While still being 
                             speedy, it has not been tested on its collision properties or its 
                             distribution.  



        b) How long does it take for your Bloom Filter to check 1 password in each case? Why does one 
           perform better than other?

        Answer: 

            The C implementation I developed is much much faster than I was expecting.  I should have
            used some cryptographic hashes to slow it down.  Execution completes in less than a second
            and there is no noticable difference between the 3-hash and 5-hash method.  I cannot tell
            how fast each hash function takes currently.  Considering the following: 

                8 hashes * 623517 entries / 1 second(run time) = 14,964,408 hashes per second
                1 second / 14,964,408 hashes = ~67ns per hash(average)



        c) What isthe probability of False Positive in your Bloom Filter in each case?  What is the 
           probability of False Negative in your Bloom Filter?

        Answer:

            P ~= (1 - e^(-kn/m))^k
            
            Variable                                 This Case
            P - false positve error rate             .0192   .0109
            k - number of hash functions             3       5
            n - number of elements being inserted    623,517(both)
            m - size of the filter                   6,000,000(both)

            Probability of a false negative is or rather always *should be*) zero.


        d) How can you reduce the rate of False Positives?

        Answer:

            Assuming our inputs to be hashed is a fixed set, one can reduce false positives by either:

                A - increasing the bit array size
                B - increasing the number of hash functions used
                C - both A and B for better effect

                