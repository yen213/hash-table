#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "HashTable.c"

// Used to create random keys/values
char *generateRandomString()
{
    int length = rand() % (21 - 5) + 5; // Generate string length between 5 and 20
    char *str = malloc(length + 1);

    if (str == NULL)
    {
        return NULL;
    }

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    for (int i = 0; i < length; i++)
    {
        int key = rand() % (int)(sizeof(charset) - 1);

        str[i] = charset[key];
    }

    str[length] = '\0';

    return str;
}

int main()
{
    srand(time(NULL));
    HashTable *hashTable = newHashTable();

    if (!hashTable)
    {
        printf("Failed to create hash table\n");

        return 1;
    }

    // Array to store keys for later retrieval
    char *keys[100];
    int keyLength = (int)(sizeof(keys) / sizeof(keys[0]));

    // Insert random key-value pairs
    for (int i = 0; i < keyLength; i++)
    {
        keys[i] = generateRandomString();
        char *value = generateRandomString();

        insert(hashTable, keys[i], value);
        free(value);
    }

    // Update some keys' values
    for (int i = 0; i < keyLength; i += 3)
    {
        char *value = generateRandomString();

        insert(hashTable, keys[i], value);
        free(value);
    }

    // Delete some random entries
    for (int i = 1; i < keyLength; i *= 2)
    {
        delete (hashTable, keys[rand() % keyLength]);
    }

    // Retrieve and print all keys
    for (int i = 0; i < keyLength; i++)
    {
        printf("%s: %s\n", keys[i], get(hashTable, keys[i]));
        free(keys[i]); // Free the keys after retrieval
    }

    // Cleanup
    deleteHashTable(hashTable);

    return 0;
}