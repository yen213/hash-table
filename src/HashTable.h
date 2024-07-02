// Defines each entry of the Hash Table
typedef struct
{
    char *key;
    char *value;
} Entry;

// Defines the Hash Table
typedef struct
{
    int count;    // Current number of Entries
    int capacity; // Amount of Entries the Hash Table can hold
    Entry **entries;
} HashTable;