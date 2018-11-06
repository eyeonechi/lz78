#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL  1  /* Initial malloc size */
#define MULTIPLY 2  /* Size multiplier     */
#define UNFOUND  0  /* Match not found     */

/* File structure */
typedef struct
{	int  indx;      /* Text lookup index    */
	int  find;      /* Text search index    */
	int  leng;      /* Text length          */
	int  size;      /* Malloc size          */
	char *text;     /* Text                 */
} docu_s;

 /* Phrase structure */
typedef struct
{	int  find;      /* Phrase search index  */
	int  leng;      /* Phrase length        */
	int  numb;      /* Phrase number        */
	char *text;     /* Phrase               */
} phrs_s;

/* Dictionary structure */
typedef struct
{	int    leng;    /* Dictionary length    */
	int    find;    /* Found / Not found    */
	int    size;    /* Malloc size          */
	char   cats[2]; /* Concatenation buffer */
	phrs_s *phrs;   /* Array of phrases     */
} dict_s;

/* Function prototypes */
void buildText(docu_s *docu, char *file);
void initText(docu_s *docu);
void resizeText(docu_s *docu);
void buildDict(docu_s *docu, dict_s *dict);
void initDict(dict_s *dict);
void resizeDict(dict_s *dict);
void initPhrase(dict_s *dict, phrs_s *phrs);
void linearSearch(docu_s *docu, dict_s *dict);
void matchNotFound(docu_s *docu, dict_s *dict);
void matchFound(docu_s *docu, dict_s *dict);
void printPair(dict_s *dict);
void printSummary(docu_s *docu, dict_s *dict);
void freeMem(docu_s *docu, dict_s *dict);

/* Encodes a text file using the LZ78 compression algorithm. */
int main(int argc, char **argv)
{	docu_s docu;
	dict_s dict;
	buildText(&docu, argv[argc - 1]);
	buildDict(&docu, &dict);
	printSummary(&docu, &dict);
	freeMem(&docu, &dict);
	return EXIT_SUCCESS;
}

/* Stores text characters into a docu_s structure. */
void buildText(docu_s *docu, char *file)
{	initText(docu);
	for (docu->leng = 0; (docu->text[docu->leng] = (char)getchar()) != EOF; docu->leng ++)
	{	if (docu->leng >= docu->size)
		{	resizeText(docu);
		}
	}
	docu->text[docu->leng] = '\0';
}

/* Initialises text array. */
void initText(docu_s *docu)
{	docu->size = INITIAL;
	docu->text = (char *)malloc(INITIAL * sizeof(docu->text));
	assert(docu->text != NULL);
}

/* Resizes text array. */
void resizeText(docu_s *docu)
{	docu->size *= MULTIPLY;
	docu->text = realloc(docu->text, docu->size * sizeof(docu->text));
	assert(docu->text != NULL);
}

/* Looks through docu_s structure and builds a dictionary of phrases *
 * to be matched with unprocessed characters.                        */
void buildDict(docu_s *docu, dict_s *dict)
{	initDict(dict);
	for (docu->indx = 0; docu->indx < docu->leng; dict->leng ++)
	{	if (dict->leng >= dict->size)
		{	resizeDict(dict);
		}
		linearSearch(docu, dict);
		if (!dict->find)
		{	matchNotFound(docu, dict);
		}
		else
		{	matchFound(docu, dict);
		}
		printPair(dict);
	}
}

/* Initialises dictionary. */
void initDict(dict_s *dict)
{	dict->size = INITIAL;
	dict->phrs = (phrs_s *)malloc(INITIAL * sizeof(dict->phrs));
	assert(dict->phrs != NULL);
	dict->leng = 1;
	dict->find = UNFOUND;
	dict->phrs[0].numb = 0;
	dict->phrs[0].leng = 0;
	dict->phrs[0].text = (char *)malloc(sizeof(char));
	assert(dict->phrs[0].text != NULL);
	dict->phrs[0].text = "";
	dict->cats[1] = '\0';
}

/* Resizes dictionary. */
void resizeDict(dict_s *dict)
{	dict->size *= MULTIPLY;
	dict->phrs = realloc(dict->phrs, dict->size * sizeof(phrs_s) + sizeof(phrs_s));
	assert(dict->phrs != NULL);
}

/* Initialises dictionary phrase. */
void initPhrase(dict_s *dict, phrs_s *phrs)
{	if (!dict->find)
	{	dict->phrs[dict->leng].text = (char *)malloc(sizeof(char));
		assert(dict->phrs[dict->leng].text != NULL);
	}
	else
	{	dict->phrs[dict->leng].text = (char *)malloc(dict->phrs[dict->find].leng * sizeof(char));
		assert(dict->phrs[dict->leng].text != NULL);
	}
}

/* Linear search through dictionary for longest matching phrase. */
void linearSearch(docu_s *docu, dict_s *dict)
{	for (dict->find = dict->leng - 1; dict->find > 0; dict->find --)
	{	if (dict->phrs[dict->find].text[0] == docu->text[docu->indx])
		{	dict->phrs[dict->leng].find = docu->indx;
			docu->find = 0;
			while (docu->text[dict->phrs[dict->leng].find] && dict->phrs[dict->find].text[docu->find] == docu->text[dict->phrs[dict->leng].find])
			{	dict->phrs[dict->leng].find ++;
				docu->find ++;
			}
			if (docu->find != dict->phrs[dict->find].leng || !docu->text[docu->indx + docu->find])
			{	continue;
			}
			break;
		}
	}
}

/* Match not found in dictionary. */
void matchNotFound(docu_s *docu, dict_s *dict)
{	initPhrase(dict, &dict->phrs[dict->leng]);
	dict->cats[0] = docu->text[docu->indx];
	strcpy(dict->phrs[dict->leng].text, dict->cats);
	dict->phrs[dict->leng].numb = 0;
	dict->phrs[dict->leng].leng = strlen(dict->phrs[dict->leng].text);
	docu->indx ++;
}

/* Match found in dictionary. */
void matchFound(docu_s *docu, dict_s *dict)
{	initPhrase(dict, &dict->phrs[dict->leng]);
	strcpy(dict->phrs[dict->leng].text, dict->phrs[dict->find].text);
	dict->cats[0] = docu->text[docu->indx + docu->find];
	strcat(dict->phrs[dict->leng].text, dict->cats);
	dict->phrs[dict->leng].leng = strlen(dict->phrs[dict->leng].text);
	if (!docu->text[docu->indx + docu->find])
	{	dict->phrs[dict->leng].numb = 0;
	}
	else
	{	dict->phrs[dict->leng].numb = dict->find;
	}
	docu->indx += dict->phrs[dict->leng].leng;
}

/* Prints the (c,k) pair according to specification for decompression. */
void printPair(dict_s *dict)
{	printf("%c%d\n", dict->phrs[dict->leng].text[dict->phrs[dict->leng].leng - 1], dict->phrs[dict->leng].numb);
}

/* Prints encoder summary output messages. */
void printSummary(docu_s *docu, dict_s *dict)
{	fprintf(stderr, "encode:%7d bytes input\n", docu->leng);
	fprintf(stderr, "encode:%7d factors generated\n", dict->leng - 1);
}

/* Frees memory malloc'ed for docu_s and dict_s structures */
void freeMem(docu_s *docu, dict_s *dict)
{	while (-- dict->leng)
	{	free(dict->phrs[dict->leng].text);
		dict->phrs[dict->leng].text = NULL;
	}
	free(dict->phrs);
	dict->phrs = NULL;
	free(docu->text);
	docu->text = NULL;
}
