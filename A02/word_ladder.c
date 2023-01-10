//
// AED, November 2022 (Tomás Oliveira e Silva)
//
// Second practical assignement (speed run)
//
// Place your student numbers and names here
//   N.Mec. 108713  Name: Liliana Ribeiro
//
// Do as much as you can
//   1) MANDATORY: complete the hash table code
//      *) hash_table_create
//      *) hash_table_grow
//      *) hash_table_free
//      *) find_word
//      +) add code to get some statistical data about the hash table
//   2) HIGHLY RECOMMENDED: build the graph (including union-find data) -- use the similar_words function...
//      *) find_representative
//      *) add_edge
//   3) RECOMMENDED: implement breadth-first search in the graph
//      *) breadh_first_search
//   4) RECOMMENDED: list all words belonginh to a connected component
//      *) breadh_first_search
//      *) list_connected_component
//   5) RECOMMENDED: find the shortest path between to words
//      *) breadh_first_search
//      *) path_finder
//      *) test the smallest path from bem to mal
//         [ 0] bem
//         [ 1] tem
//         [ 2] teu
//         [ 3] meu
//         [ 4] mau
//         [ 5] mal
//      *) find other interesting word ladders
//   6) OPTIONAL: compute the diameter of a connected component and list the longest word chain
//      *) breadh_first_search
//      *) connected_component_diameter
//   7) OPTIONAL: print some statistics about the graph
//      *) graph_info
//   8) OPTIONAL: test for memory leaks
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// static configuration
//

#define _max_word_size_ 32

//
// data structures (SUGGESTION --- you may do it in a different way)
//

typedef struct adjacency_node_s adjacency_node_t;
typedef struct hash_table_node_s hash_table_node_t;
typedef struct hash_table_s hash_table_t;

struct adjacency_node_s
{
  adjacency_node_t *next;    // link to th enext adjacency list node
  hash_table_node_t *vertex; // the other vertex
};

struct hash_table_node_s
{
  // the hash table data
  char word[_max_word_size_]; // the word
  hash_table_node_t *next;    // next hash table linked list node
  // the vertex data
  adjacency_node_t *head;      // head of the linked list of adjancency edges
  int visited;                 // visited status (while not in use, keep it at 0)
  hash_table_node_t *previous; // breadth-first search parent
  // the union find data
  hash_table_node_t *representative; // the representative of the connected component this vertex belongs to
  int number_of_vertices;            // number of vertices of the conected component (only correct for the representative of each connected component)
  int number_of_edges;               // number of edges of the conected component (only correct for the representative of each connected component)
};

struct hash_table_s
{
  unsigned int hash_table_size;   // the size of the hash table array
  unsigned int number_of_entries; // the number of entries in the hash table
  unsigned int number_of_edges;   // number of edges (for information purposes only)
  hash_table_node_t **heads;      // the heads of the linked lists
};

//
// allocation and deallocation of linked list nodes (done)
//

static adjacency_node_t *allocate_adjacency_node(void)
{
  adjacency_node_t *node;

  node = (adjacency_node_t *)malloc(sizeof(adjacency_node_t));
  if (node == NULL)
  {
    fprintf(stderr, "allocate_adjacency_node: out of memory\n");
    exit(1);
  }
  return node;
}

static void free_adjacency_node(adjacency_node_t *node)
{
  free(node);
}

static hash_table_node_t *allocate_hash_table_node(void)
{
  hash_table_node_t *node;

  node = (hash_table_node_t *)malloc(sizeof(hash_table_node_t));
  if (node == NULL)
  {
    fprintf(stderr, "allocate_hash_table_node: out of memory\n");
    exit(1);
  }
  return node;
}

static void free_hash_table_node(hash_table_node_t *node)
{
  free(node);
}

//
// hash table stuff (mostly to be done)
//

unsigned int crc32(const char *str)
{
  static unsigned int table[256];
  unsigned int crc;

  if (table[1] == 0u) // do we need to initialize the table[] array?
  {
    unsigned int i, j;

    for (i = 0u; i < 256u; i++)
      for (table[i] = i, j = 0u; j < 8u; j++)
        if (table[i] & 1u)
          table[i] = (table[i] >> 1) ^ 0xAED00022u; // "magic" constant
        else
          table[i] >>= 1;
  }
  crc = 0xAED02022u; // initial value (chosen arbitrarily)
  while (*str != '\0')
    crc = (crc >> 8) ^ table[crc & 0xFFu] ^ ((unsigned int)*str++ << 24);
  return crc;
}

// hash_table_create is a function that returns a empty table with size 107
static hash_table_t *hash_table_create(void)
{

  hash_table_t *hash_table;
  unsigned int i;

  hash_table = (hash_table_t *)malloc(sizeof(hash_table_t));
  if(hash_table == NULL)
  {
    fprintf(stderr,"create_hash_table: out of memory\n");
    exit(1);
  }

  // initialize the hash table
  hash_table->hash_table_size = 103u;
  hash_table->number_of_entries = 0u;
  hash_table->number_of_edges = 0u;
  hash_table->heads = (hash_table_node_t **)malloc(hash_table->hash_table_size * sizeof(hash_table_node_t *));
  
  // check for out of memory
  if(hash_table->heads == NULL)
  {
    fprintf(stderr,"create_hash_table: out of memory\n");
    exit(1);
  }

  // fill the hash table with NULL pointers

  for(i = 0u;i < hash_table->hash_table_size;i++){
    hash_table->heads[i] = NULL;
  }

  return hash_table;

//   hash_table_t *hash_table;
//   unsigned int i;

//   hash_table = (hash_table_t *)malloc(sizeof(hash_table_t));
//   if (hash_table == NULL)
//   {
//     fprintf(stderr, "create_hash_table: out of memory\n");
//     exit(1);
//   }

//   /* students code */
//   // choose size for the hash table (prime number)
//   unsigned int size = 107;

//   // save hash table atributes
//   hash_table->hash_table_size = size;
//   hash_table->number_of_entries = 0u;
//   hash_table->number_of_edges = 0u;

//   hash_table->heads = (hash_table_node_t **)malloc(size * sizeof(hash_table->heads));
//   memset(hash_table->heads, 0, hash_table->hash_table_size * sizeof(hash_table->heads));

//   /* end code */
//   return hash_table;
}

// hash_table_grow is a function that double the size of an hash_table preserving the old heads
static void hash_table_grow(hash_table_t *hash_table)
{
  /* students code */
  hash_table_node_t **old_heads, **new_heads;
  hash_table_node_t *node, *next_node;
  unsigned int i, old_hash_table_size;

  // save the old hash table
  old_heads = hash_table->heads;
  old_hash_table_size = hash_table->hash_table_size;

  // double the size of the hash table
  hash_table->hash_table_size *= 2u;
  new_heads = (hash_table_node_t **)malloc(hash_table->hash_table_size * sizeof(hash_table_node_t *));
  // check for out of memory
  for (i = 0u; i < hash_table->hash_table_size; i++)
    new_heads[i] = NULL;

  if (new_heads == NULL)
  {
    fprintf(stderr, "hash_table_grow: out of memory");
    exit(1);
  }

  // run the hash function for old values with new size

  for (i = 0u; i < old_hash_table_size; i++)
  {
    node = old_heads[i];
    while (node != NULL)
    {

      next_node = node->next;

      size_t index = crc32(node->word) % hash_table->hash_table_size;
      node->next = new_heads[index];
      new_heads[index] = node;

      node = next_node;
    }
  }
  free(old_heads);
  hash_table->heads = new_heads;
  /*aiiiiiiiiii*/
  // // variables to save old and new values
  // hash_table_node_t **heads, **new_heads;
  // hash_table_node_t *node, *next_node;
  // unsigned int size, new_size;
  // unsigned int i;

  // // save old values
  // heads = hash_table->heads;
  // size = hash_table->hash_table_size;

  // // new values (new size -> double the size)
  // new_size = size * 2u;
  // new_heads = (hash_table_node_t **)malloc(new_size * sizeof(hash_table_node_t *));

  // for (i = 0u; i < new_size; i++)
  //   new_heads[i] = NULL;

  // if (new_heads == NULL)
  // {
  //   fprintf(stderr, "hash_table_grow: out of memory\n");
  //   exit(1);
  // }

  // // do the hash function for the new size (and save on new_heads)
  // // go throught all the heads and replace them for the new hash function calculate
  // for (i = 0u; i < size; i++)
  //   for (node = heads[i]; node != NULL; node = next_node)
  //   {
  //     next_node = node->next;
  //     node->next = new_heads[crc32(node->word) % new_size];
  //     new_heads[crc32(node->word) % new_size] = node;
  //   }
  // // replace hash table old values (size e head) for new values
  // free(heads);
  // hash_table->hash_table_size = new_size;
  // hash_table->heads = new_heads;
  /* end code */
}

// hash_table_free: This function should free all the memory used by the hash table.
static void hash_table_free(hash_table_t *hash_table)
{
  /* students code */
  hash_table_node_t *node, *next_node;
  unsigned int i;
  for (i = 0u; i < hash_table->hash_table_size; i++)
  {
    node = hash_table->heads[i];
    while (node != NULL)
    {
      adjacency_node_t *adjNode = node->head, *tempAdjNode;
      while (adjNode != NULL)
      {
        tempAdjNode = adjNode->next;
        free_adjacency_node(adjNode);
        adjNode = tempAdjNode;
      }
      next_node = node->next;
      free_hash_table_node(node);
      node = next_node;
    }
  }

  free(hash_table->heads);
  free(hash_table);
  /* aiiiiiiiiiiiiiiiiiiiiii */
  // unsigned int i;
  // hash_table_node_t *node, *next_node;
  // adjacency_node_t *adj_node, *next_adj_node;
  // // Iterate over all the elements in the hash table
  // for (i = 0u; i < hash_table->hash_table_size; i++)
  // {
  //   // Free the linked list of hash table nodes
  //   node = hash_table->heads[i];
  //   while (node)
  //   {
  //     next_node = node->next;
  //     // Free the linked list of adjacency nodes
  //     adj_node = node->head;
  //     while (adj_node)
  //     {
  //       next_adj_node = adj_node->next;
  //       free(adj_node);
  //       adj_node = next_adj_node;
  //     }
  //     free(node);
  //     node = next_node;
  //   }
  //   /* end code */
  // }

  // // Free the array of linked list heads
  // free(hash_table->heads);

  // // Free the hash table structure itself
  // free(hash_table);
}

static hash_table_node_t *find_word(hash_table_t *hash_table, const char *word, int insert_if_not_found)
{

  hash_table_node_t *node;
  unsigned int i;

  i = crc32(word) % hash_table->hash_table_size;
  node = hash_table->heads[i];
  while (node != NULL)
  {
    if (strcmp(node->word, word) == 0)
      return node;
    node = node->next;
  }

  if (insert_if_not_found && strlen(word) < _max_word_size_)
  {
    node = allocate_hash_table_node();
    strncpy(node->word, word, _max_word_size_);
    node->representative = node;
    node->next = hash_table->heads[i];
    node->previous = NULL;
    node->number_of_edges = 0;
    node->number_of_vertices = 1;
    node->visited = 0;
    node->head = NULL;
    hash_table->heads[i] = node;
    hash_table->number_of_entries++;
    if (hash_table->number_of_entries > hash_table->hash_table_size)
      hash_table_grow(hash_table);
    return node;
  }

  return NULL;
  /* aiiiiiiiiiiiiiiiiiiiiiii */
  // hash_table_node_t *node;
  // unsigned int i;

  // i = crc32(word) % hash_table->hash_table_size;

  // /* studants code */
  // // Search the linked list at the corresponding index in the hash table array for a node with the same word
  // node = hash_table->heads[i];
  // while (node != NULL)
  // {
  //   if (strcmp(node->word, word) == 0)
  //   {
  //     // Return a pointer to the node if the word is found
  //     return node;
  //   }
  //   node = node->next;
  // }

  // // If the insert_if_not_found flag is set, insert a new node with the word
  // if (insert_if_not_found && strlen(word) < _max_word_size_)
  // {
  //   node = allocate_hash_table_node();
  //   strncpy(node->word, word, _max_word_size_);
  //   node->next = hash_table->heads[i]; // 2
  //   // Initialize the vertex data
  //   node->head = NULL;     // 7
  //   node->visited = 0;     // 6
  //   node->previous = NULL; // 3
  //   // Initialize the union find data
  //   node->representative = node;  // 1
  //   node->number_of_vertices = 1; // 5
  //   node->number_of_edges = 0;    // 4
  //   // save values and incremente entries
  //   hash_table->heads[i] = node;     // 8
  //   hash_table->number_of_entries++; // 9
  //   // grow if needs
  //   if (hash_table->number_of_entries > hash_table->hash_table_size)
  //   {
  //     hash_table_grow(hash_table);
  //   }
  // }
  // else
  // {
  //   // Return NULL if the word is not found and the insert_if_not_found flag is not set
  //   return NULL;
  // }
  // // Return a pointer to the newly inserted node if the insert_if_not_found flag is set
  // return node;

  /* end studants code */
}

//
// add edges to the word ladder graph (mostly do be done)
//

static hash_table_node_t *find_representative(hash_table_node_t *node)
{
  hash_table_node_t *Representative, *next_node, *temp_node;

  for (Representative = node; Representative != Representative->representative; Representative = Representative->representative)
    ;

  for (next_node = node; next_node != Representative; next_node = temp_node)
  {
    temp_node = next_node->representative;
    next_node->representative = Representative;
  }

  return Representative;
  // hash_table_node_t *representative, *next_node;

  // /* students code */
  // // Follow the chain of representatives until the representative of the connected component is found
  // representative = node;
  // next_node = node->representative;
  // while (next_node != representative)
  // {
  //   representative = next_node;
  //   next_node = representative->representative;
  // }
  // /* end code */
  // return representative;
}

static void add_edge(hash_table_t *hash_table, hash_table_node_t *from, const char *word)
{
   hash_table_node_t *to,*from_representative,*to_representative;
  adjacency_node_t *linkfrom, *linkto;

  from_representative = find_representative(from);
  to = find_word(hash_table,word,0);
  
  if(to == NULL || to == from) 
    return;
  
  
  to_representative = find_representative(to);
  if (from_representative == to_representative){
    from_representative->number_of_edges++;
  } else {
    if (from_representative->number_of_vertices < to_representative->number_of_vertices)
    {
      from_representative->representative = to_representative;
      to_representative->number_of_vertices += from_representative->number_of_vertices;
      to_representative->number_of_edges += (from_representative->number_of_edges)+1;
      from_representative->number_of_edges = 0;
      from_representative->number_of_vertices = 0;
    }
    else
    {
      to_representative->representative = from_representative;
      from_representative->number_of_vertices += to_representative->number_of_vertices;
      from_representative->number_of_edges += (to_representative->number_of_edges)+1;
      to_representative->number_of_edges = 0;
      to_representative->number_of_vertices = 0;
    }
  }
  

  linkfrom = allocate_adjacency_node();
  linkto = allocate_adjacency_node();

  if(linkfrom == NULL || linkto == NULL)
  {
    fprintf(stderr,"add_edge: out of memory\n");
    exit(1);
  }
  
  linkfrom->vertex = to;
  linkfrom->next = from->head;
  from->head = linkfrom;

  linkto->vertex = from;
  linkto->next = to->head;
  to->head = linkto;
  
  hash_table->number_of_edges++;
  return;
  //njsabadbk kasdbkbjkbjkbjlasbjksbjksbksdbasdbkasdbjkabjkas
  // hash_table_node_t *to, *from_representative, *to_representative;
  // adjacency_node_t *link;

  // to = find_word(hash_table, word, 0);
  // /* students code */
  // if (to == NULL)
  //   return;
  // link = allocate_adjacency_node();
  // if (link == NULL)
  // {
  //   fprintf(stderr, "add_edge: out of memory\n");
  //   exit(1);
  // }
  // link->vertex = to;
  // link->next = from->head;
  // from->head = link;
  // hash_table->number_of_edges++;

  // // atualizar os representantes das componentes conectadas
  // to_representative = find_representative(to);
  // from_representative = find_representative(from);
  // if (from_representative == to_representative)
  // {
  //   from_representative->number_of_edges++;
  // }
  // else
  // {
  //   if (from_representative->number_of_vertices < to_representative->number_of_vertices)
  //   {
  //     from_representative->representative = to_representative;
  //     to_representative->number_of_vertices += from_representative->number_of_vertices;
  //     to_representative->number_of_edges += (from_representative->number_of_edges) + 1;
  //     from_representative->number_of_edges = 0;
  //     from_representative->number_of_vertices = 0;
  //   }
  //   else
  //   {
  //     to_representative->representative = from_representative;
  //     from_representative->number_of_vertices += to_representative->number_of_vertices;
  //     from_representative->number_of_edges += (to_representative->number_of_edges) + 1;
  //     to_representative->number_of_edges = 0;
  //     to_representative->number_of_vertices = 0;
  //   }
  // }
  /* end code */
}

//
// generates a list of similar words and calls the function add_edge for each one (done)
//
// man utf8 for details on the uft8 encoding
//

static void break_utf8_string(const char *word, int *individual_characters)
{
  int byte0, byte1;

  while (*word != '\0')
  {
    byte0 = (int)(*(word++)) & 0xFF;
    if (byte0 < 0x80)
      *(individual_characters++) = byte0; // plain ASCII character
    else
    {
      byte1 = (int)(*(word++)) & 0xFF;
      if ((byte0 & 0b11100000) != 0b11000000 || (byte1 & 0b11000000) != 0b10000000)
      {
        fprintf(stderr, "break_utf8_string: unexpected UFT-8 character\n");
        exit(1);
      }
      *(individual_characters++) = ((byte0 & 0b00011111) << 6) | (byte1 & 0b00111111); // utf8 -> unicode
    }
  }
  *individual_characters = 0; // mark the end!
}

static void make_utf8_string(const int *individual_characters, char word[_max_word_size_])
{
  int code;

  while (*individual_characters != 0)
  {
    code = *(individual_characters++);
    if (code < 0x80)
      *(word++) = (char)code;
    else if (code < (1 << 11))
    { // unicode -> utf8
      *(word++) = 0b11000000 | (code >> 6);
      *(word++) = 0b10000000 | (code & 0b00111111);
    }
    else
    {
      fprintf(stderr, "make_utf8_string: unexpected UFT-8 character\n");
      exit(1);
    }
  }
  *word = '\0'; // mark the end
}

static void similar_words(hash_table_t *hash_table, hash_table_node_t *from)
{
  static const int valid_characters[] =
      {                                                                                          // unicode!
       0x2D,                                                                                     // -
       0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,             // A B C D E F G H I J K L M
       0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,             // N O P Q R S T U V W X Y Z
       0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,             // a b c d e f g h i j k l m
       0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A,             // n o p q r s t u v w x y z
       0xC1, 0xC2, 0xC9, 0xCD, 0xD3, 0xDA,                                                       // Á Â É Í Ó Ú
       0xE0, 0xE1, 0xE2, 0xE3, 0xE7, 0xE8, 0xE9, 0xEA, 0xED, 0xEE, 0xF3, 0xF4, 0xF5, 0xFA, 0xFC, // à á â ã ç è é ê í î ó ô õ ú ü
       0};
  int i, j, k, individual_characters[_max_word_size_];
  char new_word[2 * _max_word_size_];

  break_utf8_string(from->word, individual_characters);
  for (i = 0; individual_characters[i] != 0; i++)
  {
    k = individual_characters[i];
    for (j = 0; valid_characters[j] != 0; j++)
    {
      individual_characters[i] = valid_characters[j];
      make_utf8_string(individual_characters, new_word);
      // avoid duplicate cases
      if (strcmp(new_word, from->word) > 0)
        add_edge(hash_table, from, new_word);
    }
    individual_characters[i] = k;
  }
}

//
// breadth-first search (to be done)
//
// returns the number of vertices visited; if the last one is goal, following the previous links gives the shortest path between goal and origin
//

static int breadh_first_search(int maximum_number_of_vertices, hash_table_node_t **list_of_vertices, hash_table_node_t *origin, hash_table_node_t *goal)
{
  /* students code */
  int n = 0;
  int i = 0;
  int found = 0;
  // Iterate through every adjacency node
  origin->visited = 1;
  list_of_vertices[0] = origin;

  while (found == 0 && n < maximum_number_of_vertices)
  {
    for (adjacency_node_t *link = list_of_vertices[n]->head; link != NULL; link = link->next)
    {
      // Dont check the path that is already travaled by
      if (link->vertex->visited == 1)
      {
        continue;
      }

      i++;
      link->vertex->visited = 1;
      link->vertex->previous = list_of_vertices[n];
      list_of_vertices[i] = link->vertex;

      if (link->vertex == goal)
      {
        found = 1;
        return n;
      }
    }
    n++;
  }
  /* end code */
  return -1;
}

//
// list all vertices belonging to a connected component (complete this)
//

static void list_connected_component(hash_table_t *hash_table, const char *word)
{
  /* students code */
  hash_table_node_t *node, *rep;
  int i, n, maximum_number_of_vertices;
  hash_table_node_t **list_of_vertices;

  node = find_word(hash_table, word, 0);

  if (node == NULL)
  {
    printf("list_connected_component: word not found\n");
    return;
  }

  rep = find_representative(node);
  maximum_number_of_vertices = rep->number_of_vertices;
  list_of_vertices = (hash_table_node_t **)malloc(maximum_number_of_vertices * sizeof(hash_table_node_t *));

  if (list_of_vertices == NULL)
  {
    fprintf(stderr, "list_connected_component: out of memory\n");
    exit(1);
  }

  n = breadh_first_search(maximum_number_of_vertices, list_of_vertices, node, NULL);
  int count = 0;
  for (i = 0; i < n; i++)
  {
    printf("%d: %s\n", count, list_of_vertices[i]->word);
    count++;
  }
  free(list_of_vertices);

  /* end code */
}

//
// compute the diameter of a connected component (optional)
//

static int largest_diameter;
static hash_table_node_t **largest_diameter_example;

static int connected_component_diameter(hash_table_node_t *node)
{
  int diameter;

  //
  // complete this
  //
  return diameter;
}

//
// find the shortest path from a given word to another given word (to be done)
//

static void path_finder(hash_table_t *hash_table, const char *from_word, const char *to_word)
{
  /* students code */
  hash_table_node_t *fromNode, *fromRep, *toNode, *toRep;
  fromNode = find_word(hash_table, from_word, 0);
  toNode = find_word(hash_table, to_word, 0);

  if (fromNode == NULL || toNode == NULL)
  {
    printf("One of the words is not in the dictionary\n");
    return;
  }

  fromRep = find_representative(fromNode);
  toRep = find_representative(toNode);

  if (fromRep != toRep)
  {
    printf("The words are not in the same connected component, so there's no path between them.\n");
    return;
  }

  hash_table_node_t **list_of_vertices = malloc(sizeof(hash_table_node_t *) * fromRep->number_of_vertices);

  if (list_of_vertices == NULL)
  {
    fprintf(stderr, "path_finder: malloc failed\n");
    exit(1);
  }

  int goalIndex = breadh_first_search(fromRep->number_of_vertices, list_of_vertices, toNode, fromNode);

  hash_table_node_t *p = list_of_vertices[goalIndex - 1];
  int count = 0;
  while (p != NULL)
  {
    printf("%d: %s \n", count, p->word);
    count++;
    p = p->previous;
  }

  free(list_of_vertices);
  /* end code */
}

//
// some graph information (optional)
//

static void graph_info(hash_table_t *hash_table)
{
  //
  // complete this
  //
}

static void print_list(hash_table_t *hash_table)
{
  for (size_t i = 0; i < hash_table->hash_table_size; i++)
  {
    hash_table_node_t *p = hash_table->heads[i];
    if (p == NULL)
      continue;
    while (p != NULL)
    {
      printf("%s ", p->word);
      p = p->next;
    }
    printf("\n");
  }
  return;
}

//
// main program
//

int main(int argc, char **argv)
{
  char word[100], from[100], to[100];
  hash_table_t *hash_table;
  hash_table_node_t *node;
  unsigned int i;
  int command;
  FILE *fp;

  // initialize hash table
  hash_table = hash_table_create();
  // read words
  fp = fopen((argc < 2) ? "wordlist-four-letters.txt" : argv[1], "rb");
  if (fp == NULL)
  {
    fprintf(stderr, "main: unable to open the words file\n");
    exit(1);
  }
  while (fscanf(fp, "%99s", word) == 1)
    (void)find_word(hash_table, word, 1);
  fclose(fp);
  // find all similar words
  for (i = 0u; i < hash_table->hash_table_size; i++)
    for (node = hash_table->heads[i]; node != NULL; node = node->next)
      similar_words(hash_table, node);
  graph_info(hash_table);
  // ask what to do
  for (;;)
  {
    fprintf(stderr, "Your wish is my command:\n");
    fprintf(stderr, "  1 WORD       (list the connected component WORD belongs to)\n");
    fprintf(stderr, "  2 FROM TO    (list the shortest path from FROM to TO)\n");
    fprintf(stderr, "  3            (terminate)\n");
    fprintf(stderr, "> ");
    if (scanf("%99s", word) != 1)
      break;
    command = atoi(word);
    if (command == 1)
    {
      if (scanf("%99s", word) != 1)
        break;
      list_connected_component(hash_table, word);
    }
    else if (command == 2)
    {
      if (scanf("%99s", from) != 1)
        break;
      if (scanf("%99s", to) != 1)
        break;
      path_finder(hash_table, from, to);
    }
    else if (command == 3)
      break;
  }
  // clean up
  hash_table_free(hash_table);
  return 0;
}
