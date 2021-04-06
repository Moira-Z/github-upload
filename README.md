# Assignment 3 - LZW Compression

## Contents
- Code Skeleton: lzw_skeleton.c
- Program Example: lzw_example_win.exe (just for windows platform)
  lzw_example_linux.out (for Linux)
  lzw_example_macos.out (for MacOS)
- Test File: Ephesians.txt, Matthew.txt

## Task

Complete the LZW algorithm based on lzw_skeleton.c and **name your source file "lzw.c"**. Please refer to specification of assignment 3 for other requirements.

## Correctness Checking by Yourself
An example programe is provided for you to check wheather your own implement is correct. You can do this as following:
1. Compress some files using the example programe to get a compressed file. 
```
lzw_example_win.exe -c example.lzw file1.txt file2.txt file3.txt
```
2. For compression part:
Compress the same files using your own programe to get a compressed file. 
i.Check wheather the md5sum of the two compressed files are the same.
ii.Decompress your generated lzw with example program.
3. For decompression part:
i.Decompress your compressed file using example program.
ii.Decompress your compressed file using your own programe. 
Then check wheather the md5sum is the same with the original files.




