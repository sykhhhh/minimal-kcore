# minimal_kcore
## usage
```
g++ main.cpp -O3 -o main
./main <file> <k> <s>
```
e.g.
```
./main ./jazz.bin 20 35
```
## input
The input graph should be a binary format. We use ToBin to make a input document become a binary document
usage:
```
g++ ToBin.cpp -o ToBin
./ToBin <graph_file>
```
and it will output a binary document

e.g.
```
./ToBin ./jazz.txt
```
output: jazz.bin
