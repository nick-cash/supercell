/*
 * Ludum Dare Jam 23 - 4/20/2012
 */

/*
 * Includes
 */
#include "main.h"

uint32_t crc_tab[256];
hashtab *default_hashtable = NULL;

/**
 * Sets global default hashtable;
 * @param hashtab *table
 * @return void
 */
void set_default_hashtab(hashtab *table)
{
    default_hashtable = table;
}

/**
 * Convenience function, calls hashtabset with the provided
 * key and value against the currently active global hashtab.
 */
void default_hashtab_set(char *key, void *value)
{
    if(default_hashtable != NULL)
    {
        hashtab_set(default_hashtable, key, value);
    }
}

/**
 * Convenience function, calls hashtabget with the provided
 * key against the currently active global hashtab.
 */
void *default_hashtab_get(char *key)
{
    if(default_hashtable == NULL) return NULL;
    return hashtab_get(default_hashtable, key);
}

/**
 * Convenience function, calls hashtabunset with the provided
 * key against the currently active global hashtab.
 */
void default_hashtab_unset(char *key, Uint8 freevalue)
{
    if(default_hashtable != NULL)
    {
        hashtab_unset(default_hashtable, key, freevalue);
    }
}

/*
 * Attempt to load a resource from the default hashtable. If it fails, try to
 * load it from file and put it in the table.
 */
void* default_hashtab_loadresource(char* key)
{
    void* obj = NULL;

    if (default_hashtable == NULL)
    {
        return NULL;
    }

    obj = hashtab_get(default_hashtable, key);

    if (!obj)
    {
        SDL_RWops *rwops;
        rwops = loadresource(key);

        if (rwops)
        {
            /* Insert automagic code to use the correct function to load the file */
            obj = IMG_LoadJPG_RW(rwops);
            default_hashtab_set(key, obj);
        }
    }

    return obj;
}

/**
 * Initializes a hash table, creating a dynamic array of
 * pointers to hashlists of the specified table_size.
 */
hashtab *init_hashtab(int table_size)
{
    hashtab *newtable;
    int i;

    newtable = malloc(sizeof(hashtab));

    if(table_size <= 1)
    {
        table_size = 256;
    }

    newtable->tabsize = table_size;
    newtable->table = malloc(sizeof(hashlist *) * table_size);

    for(i = 0; i < table_size; i++)
    {
        newtable->table[i] = NULL;
    }

    return newtable;
}

/**
 * Creates a new hashlist structure and returns it.
 */
hashlist *init_hashlist()
{
    hashlist *list = (hashlist*)malloc(sizeof(hashlist));
    list->next = NULL;

    return list;
}

/**
 * Sets a value in the hashtab hash table.
 * @param char *key - Null-terminated string to use for lookup.
 * @param void *value - Pointer to the data to store in the hashtable.
 */
void hashtab_set(hashtab *table, char *key, void *value)
{
    uint32_t hashvalue;
    int hashindex;
    int set = 0;
    char *keydupe;

    //Use default hashtable if it's set and a null pointer has been provided for the table.
    if (table == NULL)
    {
        if(default_hashtable == NULL)
        {
            return;
        }
        else
        {
            table = default_hashtable;
        }
    }

    hashlist *list;
    hashvalue = chksum_crc32((unsigned char *)key, strlen(key));
    hashindex = hashvalue%table->tabsize;
    if(table->table[hashindex] == NULL)
    {
        keydupe = strdup(key);
        table->table[hashindex] = init_hashlist();
        table->table[hashindex]->name = keydupe;
        table->table[hashindex]->value = value;
    }
    else
    {
        list = table->table[hashindex];

        if(strcmp(key, list->name) == 0)
        {
            list->value = value;
        }
        else
        {
            while(list->next != NULL)
            {
                list = list->next;
                if(strcmp(key, list->name) == 0)
                {
                    list->value = value;
                    set = 1;
                    break;
                }
            }
            if(set == 0)
            {
                list->next = init_hashlist();
                list = list->next;
                keydupe = strdup(key);
                list->name = keydupe;
                list->value = value;
            }
        }
    }
}

/**
 * Get a value-pointer from the hash table.
 * @param hashtab *table The hashtab table itself
 * @param char *key The name by which the value goes
 */
void *hashtab_get(hashtab *table, char *key)
{
    uint32_t hashvalue;
    int hashindex;
    hashlist *list;

    //Use default hashtable if it's set and a null pointer has been provided for the table.
    if(table == NULL)
    {
        if(default_hashtable == NULL)
        {
            return NULL;
        }
        else
        {
            table = default_hashtable;
        }
    }

    hashvalue = chksum_crc32((unsigned char *)key, strlen(key));
    hashindex = hashvalue%table->tabsize;
    if(table->table[hashindex] == NULL)
    {
        return NULL;
    }
    else
    {
        list = table->table[hashindex];
        if(strcmp(key, list->name) == 0)
        {
            return list->value;
        }
        while(list->next != NULL)
        {
            list = list->next;
            if(strcmp(key,list->name) == 0)
            {
                return list->value;
            }
        }
        return NULL;
    }
}

/**
 * Unsets a variable from the hash table.  Optionally can also free the
 * memory attached to the value as well.
 * @param hashtab *table the hashtab table itself.
 * @param char *key string name of the key
 * @param Uint8 freevalue set this to 1 to free the memory the value points to
 */
void hashtab_unset(hashtab *table, char *key, Uint8 freevalue)
{
    uint32_t hashvalue;
    int hashindex;
    hashlist *list;
    hashlist *prev;

    //Use default hashtable if it's set and a null pointer has been provided for the table.
    if(table == NULL)
    {
        if(default_hashtable == NULL)
        {
            return;
        }
        else
        {
            table = default_hashtable;
        }
    }

    hashvalue = chksum_crc32((unsigned char *)key, strlen(key));
    hashindex = hashvalue%table->tabsize;
    if(table->table[hashindex] == NULL)
    {
        //Don't do anything.
    }
    else
    {
        list = table->table[hashindex];
        if(strcmp(key, list->name) == 0)
        {
            if(freevalue == 1)
            {
                free(list->value);
            }
            //In this case, we set table[hashindex] to
            //point to the next hashlist object.  Pretty simple eh?
            table->table[hashindex] = list->next;
            free(list->name); //Free the duplicate string we made initially in hashtabset.
            free(list);
        }
        else
        {
            while(list->next != NULL)
            {
                prev = list;
                list = list->next;
                if(strcmp(key,list->name) == 0)
                {
                    prev->next = list->next;
                    if(freevalue == 1)
                    {
                        free(list->value);
                    }
                    free(list);
                    break;
                }
            }
        }
    }
}




/* chksum_crc() -- to a given block, this one calculates the
 *				crc32-checksum until the length is
 *				reached. the crc32-checksum will be
 *				the result.
 */
uint32_t chksum_crc32 (unsigned char *block, unsigned int length)
{
   register unsigned long crc;
   unsigned long i;

   crc = 0xFFFFFFFF;
   for (i = 0; i < length; i++)
   {
      crc = ((crc >> 8) & 0x00FFFFFF) ^ crc_tab[(crc ^ *block++) & 0xFF];
   }
   return (crc ^ 0xFFFFFFFF);
}

/* hashsystem_init() --      to a global crc_tab[256], this one will
 *				calculate the crcTable for crc32-checksums.
 *				it is generated to the polynom [..]
 */

void hashtabsystem_init ()
{
    unsigned long crc, poly;
    int i, j;

    poly = 0xEDB88320L;
    for (i = 0; i < 256; i++)
    {
      crc = i;
        for (j = 8; j > 0; j--)
        {
            if (crc & 1)
            {
                crc = (crc >> 1) ^ poly;
            }
            else
            {
                crc >>= 1;
            }
        }
        crc_tab[i] = crc;
    }
}
