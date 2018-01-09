# word2vec

This repository includes a modified version of the word2vec library to find
the cosine similarity among words.

This API has to be compiled as a shared library and be copied to
a library directory used by your compiler (by default `/usr/local/lib`):

```
make
cp libdistance.dylib /usr/local/lib
cp libdistance.so /usr/local/lib
```
