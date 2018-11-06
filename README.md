# lz78
Lempel-Zip 1978 compression mechanism

### Compile
gcc -o decoder decoder.c
gcc -o encoder_array encoder_array.c
gcc -o encoder_tree encoder_tree.c
gcc -o encoder_trie encoder_trie.c

### Check
./encoder_array < ../data/[file] | ./decoder | diff - ../data/[file]
./encoder_tree < ../data/[file] | ./decoder | diff - ../data/[file]
./encoder_trie < ../data/[file] | ./decoder | diff - ../data/[file]
