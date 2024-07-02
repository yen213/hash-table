# Implementation

The Hash Table is implemented using a multiplicative hash function. I didn't want anything too simple or too complex so I found this to be a nice middle ground. The code in the [main hash function](https://github.com/yen213/hash-table/blob/6bacb37afe6d50835b4b77cc472a477cf85c7f40/src/HashTable.c#L74) makes use of prime numbers and powers of 2 in order to hash keys more uniformly across the Hash Table, reducing the likelihood of collisions. In the case that collisions do occur, the concept of double hashing was used with a ['simpler' hash function](https://github.com/yen213/hash-table/blob/6bacb37afe6d50835b4b77cc472a477cf85c7f40/src/HashTable.c#L99) and open addressing with linear probing. To put simply, the main hash function is used to find the index of where in the Hash Table to put the key/value pair, [Entry](https://github.com/yen213/hash-table/blob/6bacb37afe6d50835b4b77cc472a477cf85c7f40/src/HashTable.h#L6), if collision does occur, then both the hash values are used to increment the index and find the next available index. Using the two hash values helps to address issues with collisions and clustering.

The three main functions for the Hash Table are:

-   `insert(hashTable, key, value)`: Inserts a new `Entry` into the Hash Table or updates the value if key already exists.
-   `get(hashTable, key)`: Returns the value associate with the key from the Hash Table or NULL if key doesn't exist.
-   `delete(hashTable, key)`: Deletes an `Entry` from the Hash Table matching the key, if it exists.

## Why C?

I decided to code up the implementation in C for a few reasons. There is no class for a `HashTable` or `HashMap` in C or even the concept of a class as seen in higher level languages like Java. The closest thing to the concept of a class is `struct`, which is what is used here to implement the Hash Table. I wanted to challenge myself a little so I chose C. If understanding the implementations of Hash Tables is the only goal, it would be better to implement using a higher level programming language as there would be no need to worry about memory management, concepts like classes and generics would make implementation a lot easier, and it would be easier to test. But I thought it would be fun to worry about these kind of things while also learning and implementing a Hash Table. And this is a personal opinion but I think working with the memory management aspect has helped me to understand the implementation better.

## Few Things To Note

-   I've left out things handling exceptions and errors that may arise in the code as it wasn't really part of my goals. When I did encounter any error, using the debugger was sufficient enough for me to address any errors I encountered and test my code.
-   The Hash Table only accepts keys and values of type `Strings` or `char*` in C. While adding code to accept keys/values of different types is definitely doable it would over complicate my goal of a 'simple' implementation.
-   The [main file](https://github.com/yen213/hash-table/blob/6bacb37afe6d50835b4b77cc472a477cf85c7f40/src/main.c) contains some code to help test the Hash Table There is a function to generate random keys and values and then code to insert, update, delete, and print the Hash Table.

Overall, I believe this a a good middle ground implementation of a Hash Table in C as it's not too complex or too simple. And more importantly, it was enough to accomplish the goals I had for this implementation.
