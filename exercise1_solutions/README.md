# Tasks

There are three different tasks in this exercise.

## sort_simple

Write a program that reads string from the standard input. All strings will contain no 0-byte characters and will be terminated by a \n newline. The last line of input might not be terminated by a newline character. After reading all strings, these strings should be sorted according of the order of the strcmp function as provided by <string.h>. After all strings have been sorted, these string should be printed to STDOUT in the new order. Every string should be terminated by a \n newline character when writing it to STDOUT.

In this task, there will be at most 20 strings in the input, each one of them at most 20 characters long including the terminating \n newline.

Your solution must be saved in sort.c. You are not allowed to add additional files or change the Makefile.

There is a test in test.sh, that checks your compiled code against a few test cases. Feel free to add more tests. When the tests are passed with success, then test.sh will print "OK".

## sort_advanced

This is the same task as in sort_simple, but your implementation must be able to handle an arbitrary number of input lines of arbitrary length. Should your implementation fail to allocate sufficient memory, then your program should exit with exit code 1 and should not generate any output.

## linked_list

Your task is to implement a linked list for integers (int). In linkedlist.h, there is already a header file that defines the interface of that list. You will define the type of your list there with _typedef struct list_ and you may define additional data type in this file. You must implement your list in linkedlist.c. In test_list.c is a simple test for your code that tests your code.

Again, there is a script in test.sh, that runs test_list and compares the output with a reference output.
