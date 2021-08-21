/* Brian's hashtable stuff */

#ifndef HASHTAB_H_INCLUDED
#define HASHTAB_H_INCLUDED

struct hashlist_el
{
    char *name;
    void *value;
    struct hashlist_el *next;
};

typedef struct hashlist_el hashlist;

typedef struct
{
    int tabsize;
    hashlist **table;
} hashtab;


hashtab *init_hashtab(int table_size);
uint32_t chksum_crc32(unsigned char *block, unsigned int length);
void  hashtabsystem_init();
void  hashtab_set(hashtab *table, char *key, void *value);
void* hashtab_get(hashtab *table, char *key);
void  hashtab_unset(hashtab *table, char *key, Uint8 freevalue);
void  set_default_hashtab(hashtab *table);

/*
 * Convenience functions for use with game_state structures.
 * After setting a default hashtable, these can be used directly from a
 * properly initialized structure, almost like a class call.
 */
void  default_hashtab_set(char *key, void *value);
void  default_hashtab_unset(char *key, Uint8 freevalue);
void* default_hashtab_get(char *key);
void* default_hashtab_loadresource(char* key);

#endif // HASHTAB_H_INCLUDED
