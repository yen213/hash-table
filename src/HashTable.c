#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "HashTable.h"

#define A 0.6180339887 // Set to the 'golden ratio,' used in multiplicative hash functions
#define MAX_LOAD_FACTOR 0.75
#define MIN_LOAD_FACTOR 0.25

// Keep hashing related variables to be powers of 2 when using multiplicative hash, performance reasons
#define INITIAL_TABLE_CAPACITY 1024
#define GROWTH_FACTOR 2
#define SHRINK_FACTOR 0.5

// Keeps track of the Hash Table's current capacity
int CURR_TABLE_CAPACITY = INITIAL_TABLE_CAPACITY;

// Implicit declaration warning
void insert(HashTable *hashTable, const char *key, const char *value);

// Allocates memory for a new Entry
Entry *createEntry(const char *key, const char *value)
{
    Entry *entry = malloc(sizeof(Entry));

    entry->key = strdup(key);
    entry->value = strdup(value);

    return entry;
}

// Initialize new Hash Table
HashTable *newHashTable()
{
    HashTable *hashTable = malloc(sizeof(HashTable));

    hashTable->count = 0;
    hashTable->capacity = CURR_TABLE_CAPACITY;
    hashTable->entries = calloc((size_t)hashTable->capacity, sizeof(Entry *)); // Allocate memory for entries initialized to NULL

    return hashTable;
}

// Deallocates memory of a deleted Entry
static void deleteEntry(Entry *entry)
{
    if (entry != NULL)
    {
        free(entry->key);
        free(entry->value);
        free(entry);
    }
}

// Deallocates memory of deleted a Hash Table and all its relevant members
void deleteHashTable(HashTable *hashTable)
{
    for (int i = 0; i < hashTable->capacity; i++)
    {
        deleteEntry(hashTable->entries[i]);
    }

    free(hashTable->entries);
    free(hashTable);
}

/**
 * The loop generates a hash value that spreads out the
 * keys more uniformly across the HashTable, reducing the
 * likelihood of collisions. It's just performing mathematical
 * operations to create variability among hashes.
 */
unsigned int multiplicativeHash(const char *key, int capacity)
{
    unsigned long hash = 0;
    int currChar;

    while ((currChar = *key++))
    {
        hash = currChar + (hash << 6) + (hash << 16) - hash;
    }

    // Apply golden ratio to final hash
    double fractionalPart = (hash * A) - (int)(hash * A);

    // Important to mod the final value with the table capacity as
    // the resulting index can be greater than the table's capacity
    return (unsigned int)(capacity * fractionalPart) % capacity;
}

/**
 * This is similar to the first multiplicative hash function except
 * there are fewer calculations here. Since the first hash function
 * is pretty decent for the current use case and collisions aren't
 * too likely, keeping this simple helps with the performance of
 * the HashTable.
 */
unsigned int secondMultiplicativeHash(const char *key, int capacity)
{
    unsigned long hash = 0;
    int currChar;

    while ((currChar = *key++))
    {
        hash = currChar + (hash << 5) + hash;
    }

    return (hash % (capacity - 1)) + 1;
}

/**
 * Whenever the Hash Table's capacity goes over or under the
 * MIN/MAX load factor, call this function to properly adjust
 * it's size.
 */
void resizeTable(HashTable *hashTable, int newCapacity)
{
    CURR_TABLE_CAPACITY = newCapacity;

    Entry **oldEntries = hashTable->entries;
    int oldCapacity = hashTable->capacity;

    hashTable->entries = calloc(newCapacity, sizeof(Entry *));
    hashTable->capacity = newCapacity;
    hashTable->count = 0;

    // Rehash all old entries into the resized table
    for (int i = 0; i < oldCapacity; i++)
    {
        Entry *entry = oldEntries[i];

        if (entry != NULL)
        {
            insert(hashTable, entry->key, entry->value);
            deleteEntry(entry);
        }
    }

    free(oldEntries);
}

// Returns the next index of the linear probing sequence
unsigned int getNextIndex(unsigned int index, unsigned int increaseIndex, unsigned int capacity)
{
    return (index + increaseIndex) % capacity;
}

// Insert a new Entry into the Hash Table or update existing one
void insert(HashTable *hashTable, const char *key, const char *value)
{
    // Resize if load factor is too high
    float loadFactor = (float)hashTable->count / hashTable->capacity;

    if (loadFactor > MAX_LOAD_FACTOR)
    {
        resizeTable(hashTable, hashTable->capacity * GROWTH_FACTOR);
    }

    unsigned int index = multiplicativeHash(key, hashTable->capacity);
    unsigned int increaseIndex = secondMultiplicativeHash(key, hashTable->capacity);
    unsigned int start = index;

    while (hashTable->entries[index] != NULL)
    {
        // Update existing key
        if (strcmp(hashTable->entries[index]->key, key) == 0)
        {
            free(hashTable->entries[index]->value);
            hashTable->entries[index]->value = strdup(value);

            return;
        }

        index = getNextIndex(index, increaseIndex, hashTable->capacity);

        if (index == start)
        {
            // Table is full, should not happen due to resizing
            return;
        }
    }

    // Add new Entry to Hash Table
    hashTable->entries[index] = createEntry(key, value);
    hashTable->count++;
}

// Returns an Entry from the Hash Table matching the key, NULL otherwise
char *get(HashTable *hashTable, const char *key)
{
    unsigned int index = multiplicativeHash(key, hashTable->capacity);
    unsigned int increaseIndex = secondMultiplicativeHash(key, hashTable->capacity);
    unsigned int start = index;

    while (hashTable->entries[index] != NULL)
    {
        if (strcmp(hashTable->entries[index]->key, key) == 0)
        {
            return hashTable->entries[index]->value;
        }

        index = getNextIndex(index, increaseIndex, hashTable->capacity);

        if (index == start)
        {
            break; // Looped back to start, key not present
        }
    }

    return NULL;
}

/**
 * Deletes a Entry from the Hash Table, if key exists. It is very
 * important to re-hash and re-insert subsequent entries of the table
 * after an Entry that's part of a chain is deleted. Since this Hash Table
 * implementation is using open addressing with linear probing, doing this
 * ensures that the probing sequence is maintained and helps to ensure that
 * Entries aren't lost because sequence chain is broken.
 */
void delete(HashTable *hashTable, const char *key)
{
    unsigned int index = multiplicativeHash(key, hashTable->capacity);
    unsigned int increaseIndex = secondMultiplicativeHash(key, hashTable->capacity);
    unsigned int start = index;

    while (hashTable->entries[index] != NULL)
    {
        if (strcmp(hashTable->entries[index]->key, key) == 0)
        {
            deleteEntry(hashTable->entries[index]);
            hashTable->entries[index] = NULL;
            hashTable->count--;

            // Re-insert subsequent entries to maintain the Hash Table's integrity
            unsigned int nextIndex = getNextIndex(index, increaseIndex, hashTable->capacity);

            while (hashTable->entries[nextIndex] != NULL)
            {
                Entry *temp = hashTable->entries[nextIndex];

                hashTable->entries[nextIndex] = NULL;
                hashTable->count--;
                insert(hashTable, temp->key, temp->value);
                deleteEntry(temp);

                nextIndex = getNextIndex(nextIndex, increaseIndex, hashTable->capacity);
            }

            break;
        }

        index = getNextIndex(index, increaseIndex, hashTable->capacity);

        if (index == start)
        {
            return; // Looped back to start, key not present
        }
    }

    // Resize if load factor is too low
    float loadFactor = (float)hashTable->count / hashTable->capacity;

    if (loadFactor < MIN_LOAD_FACTOR && hashTable->capacity > INITIAL_TABLE_CAPACITY)
    {
        resizeTable(hashTable, hashTable->capacity * SHRINK_FACTOR);
    }
}