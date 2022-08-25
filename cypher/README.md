#experimental cypher for short message protection:
These use the tcode tools ... best to install tcode tools in /usr/local/bin
cd tcodetools
make clean
make
make install

the files in cycpher folder are simply bash scripts and have been tested on the pi.
There are two further changes, improvements yet to be implemented to enhance the complexity of the 
encoding process..

DECSCRIPTION of algorithm.
The process of encyphering and decyphering requires a pass pharse.

The pass phrase is converted to an octal string that beccomes the seed for a logistic-map based 'random' number generator
lets start with the initial passphrase say "mypass"

looking at the script
PHRASE=mypass

Converting to Octal and saving in the seed file

echo $PHRASE | t_unpack  | awk '{printf "obase=8; ibase=2; %s\n",$1}' | bc -l > encodeseed

Displaying the seed file:
cat encodeseed
1553627014134671412

Then using this seed an initial round of ten octal lists is generated give ostensibly
each list as a random ordering of the 8 integers [1-8] saved temporarily in the file encodeblock

./blkgen encodeseed encodeblock

Looking at the first set:
cat encodeblock 
8	8	6	6	1	1	6	2	5	6
5	4	1	4	2	3	3	1	3	3
4	5	5	5	3	6	5	8	2	2
7	6	8	7	6	4	8	4	6	1
1	2	2	8	4	5	7	7	4	7
3	1	4	1	8	2	2	5	8	5
2	3	3	2	5	7	4	3	7	8
6	7	7	3	7	8	1	6	1	4

The seed is updated with each call to blkgen:
cat encodeseed 
98453999716416558743524675952595867529712924981835362417957253725375168858932455264419639

the next time a block is generated it uses the newly generated seed updated from last time
./blkgen encodeseed encodeblock
cat encodeblock 
3	6	3	2	4	6	4	1	7	3
5	4	5	5	2	1	7	5	3	7
6	5	1	3	8	4	8	2	4	5
8	3	4	6	7	3	3	3	2	6
2	1	7	1	1	2	5	7	8	1
1	7	8	7	5	8	2	8	6	8
7	2	2	4	3	7	1	4	5	2
4	8	6	8	6	5	6	6	1	4

the seed is again updated
cat encodeseed 
377146553191166547758548855828978832475582838723193191945658271281674655727653454967363463


Now the way the encypher proceeds is it uses a pair of lists to encode a single input octal message digit to which we add 1.
The idea is to bury the information being conveyed by marking the position in the second of the pair of lists inserting 0
immeidaetley after the corresponding appearance of the incremented message integer in first. Its a process
of obfuscation. Without knowledge of the initial state (derived from the pass phrase) of the logistic map
nor of the resolution used in calculating the state to state progression of the dynmics the successive encoding lists 
cannot be reliably recostituted.
 
Each octal integer in the resultant list is communcated by encoding to a variable length t-code as follows
0 00
1 11
2 011
3 100
4 0100
5 0101
6 1011
7 10100
8 10101

say we take the first list 
3    5    6    8    2   1   7   4 

and the 5th list to be our pair:
4    2    8    7    1   5   3   6   

lets assume the octal message digit to be encoded is 5.. add 1 it becomes 6
6 is the third element in the list 
so a 0 is added after the third element in the second 'unrelated' list 

4    2    8     0   7    1  5   3   6   
and this is encoded with the t-codes
0100 011 01010 00 10100 11 0101 100 1011

i.e. 
01000110101000101001101011001011

The binary encoding is always 33 bits or equivalently 11 octal digits 

echo 01000110101000101001101011001011 | awk '{printf "obase=8; ibase=2; %s\n",$1}' | bc

10650515313

so we have mapped a single 'octal digit' to  eleven ... thus expanding the symbol 
space. This makes the mapping effectively a one way mapping. The logistic calculation is also a one way computation, 
it cannot be reversed. The only way to reconstruct the sequence of lists is to have the correct starting point.

This in principle describes the algorithm. There are two additional measures shortly to be implemented. One is to us an aspect of the seed to modulate
the resolution of the bc calculator with each calling, the second is to use a third list in each round to xor with the binary coded 
list already described.

No cypher can claim absolute security untill its put to the test. The philosphy here then is to expose the algorithm in full, use tools and software technologies that are
generic and ubiquitous, so that the algorithm is not reliant on proprietary hardware or hiddent cmponents, to make the process 
deliberately complex and slow so its essentially immune to brute force attacks. Its hard to conceive of any effective hardware speed up for the bc calculator that 
might bring brute force options into play. The combinatorics for the above would appear to be against that.
 





