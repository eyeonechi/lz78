#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL  1        /* Initial size for malloc */
#define MULTIPLY 2        /* Size multiplier         */
#define TRIEVARS 4        /* Variables in a trie     */

/************************************************************************/

/* Document structure (Input file) */
typedef struct
{	int   kmax;           /* Maximum ascii value      */
	int   kmin;           /* Minimum ascii value      */
	char *text;           /* Array for storing text   */
} docu_s;

/* Node structure (Dictionary phrase) */
typedef struct trie_t
{	int    fact;          /* Dictionary index         */
	int    find;          /* Matching character index */
	char   text;          /* Character stored in node */
	struct trie_t **chld; /* Array of children nodes  */
} trie_s;

/************************************************************************/

/* Function prototypes */
docu_s *initText();
trie_s *initNode(int tlen);
void    growText(docu_s *docu, char *file);
void    growTrie(docu_s *docu, trie_s *trie, int tlen);
void    growNode(trie_s *trie, char text, int fact, int tkey, int tlen);
void    freeText(docu_s *docu);
void    freeTrie(trie_s *trie, int tlen);
void    printPair(char text, int find);
void    printEncoding(int indx, int fact);

/************************************************************************/

/* Encodes an input file following the LZ78 compression algorithm */
int main(int argc, char **argv)
{	docu_s *docu = initText();
	growText(docu, argv[argc - 1]);
	int tlen = docu->kmax - docu->kmin + 1;
	trie_s *trie = initNode(tlen);
	growTrie(docu, trie, tlen);
	freeText(docu);
	freeTrie(trie, tlen);
	return EXIT_SUCCESS;
}

/************************************************************************/

/* Initialises and returns a pointer to an array for storing characters */
docu_s *initText()
{	docu_s *docu = (docu_s *)malloc(sizeof(docu_s));
	assert(docu != NULL);
	docu->text = (char *)malloc(sizeof(docu->text));
	assert(docu->text != NULL);
	docu->kmax = 0;
	docu->kmin = 0;
	return docu;
}

/************************************************************************/

/* Stores characters from the input file into an array */
void growText(docu_s *docu, char *file)
{	int indx;
	size_t size = INITIAL;
	for (indx = 0; (docu->text[indx] = (char)getchar()) != EOF; indx ++)
	{	/* Determines the minimum and maximum ascii value encountered */
		if ((int)docu->text[indx] > docu->kmax)
		{	docu->kmax = (int)docu->text[indx];
		}
		else if ((int)docu->text[indx] < docu->kmin)
		{	docu->kmin = (int)docu->text[indx];
		}
		/* Reallocs the text accordingly */
		if (indx >= size)
		{	size *= MULTIPLY;
			docu->text = realloc(docu->text, size * sizeof(docu_s));
			assert(docu != NULL);
		}
	}
	docu->text[indx] = '\0';
}

/************************************************************************/

/* Initialises and returns a pointer to a trie node */
trie_s *initNode(int tlen)
{	trie_s *trie = (trie_s *)calloc(TRIEVARS, sizeof(trie_s));
	assert(trie != NULL);
	trie->chld = (trie_s **)calloc(tlen, sizeof(trie_s *));
	assert(trie->chld != NULL);
	return trie;
}

/************************************************************************/

/* Processes the text while adding factors to the trie dictionary */
void growTrie(docu_s *docu, trie_s *root, int tlen)
{	int indx, tkey, fact = 1;
	trie_s *trie = root;
	for (indx = 0; docu->text[indx]; indx ++)
	{	/* Hashes each character according to its positive ascii value */
		tkey = (int)docu->text[indx] - (int)docu->kmin;
		/* Case : Matching character found in the current level */
		if (trie->chld[tkey])
		{	trie = trie->chld[tkey];
			/* Case : Last character being a newline */
			if (!docu->text[indx + 1])
			{	printPair(docu->text[indx], trie->find);
				fact ++;
			}
		}
		/* Case : Matching character not found */
		else
		{	growNode(trie, docu->text[indx], fact, tkey, tlen);
			trie = root;
			fact ++;
		}
	}
	printEncoding(indx, fact);
}

/************************************************************************/

/* Adds a child node and its corresponding characteristics to the trie */
void growNode(trie_s *trie, char text, int fact, int tkey, int tlen)
{	trie->chld[tkey] = initNode(tlen);
	trie->chld[tkey]->text = text;
	trie->chld[tkey]->find = trie->fact;
	trie->chld[tkey]->fact = fact;
	printPair(trie->chld[tkey]->text, trie->chld[tkey]->find);
}

/************************************************************************/

/* Frees memory allocated to the input text array */
void freeText(docu_s *docu)
{	free(docu->text);
	docu->text = NULL;
	free(docu);
	docu = NULL;
}

/************************************************************************/

/* Recursively frees memory allocated to the trie dictionary */
void freeTrie(trie_s *trie, int tlen)
{	int indx;
	for (indx = 0; indx < tlen; indx ++)
	{	if (trie->chld[indx])
		{	return freeTrie(trie->chld[indx], tlen);
			free(trie->chld[indx]);
			trie->chld[indx] = NULL;
		}
	}
	free(trie);
	trie = NULL;
}

/************************************************************************/

/* Prints the character-factor pair as encoding output */
void printPair(char text, int find)
{	printf("%c%d\n", text, find);
}

/************************************************************************/

/* Prints encoding messages to stderr */
void printEncoding(int indx, int fact)
{	fprintf(stderr, "encode: %6d bytes input\n", indx);
	fprintf(stderr, "encode: %6d factors generated\n", fact - 1);
}
