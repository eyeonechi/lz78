#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INITIAL  1  /* Initial malloc size */
#define MULTIPLY 2  /* Size multiplier     */

/* File structure */
typedef struct
{	int  leng;      /* Text length          */
	int  size;      /* Malloc size          */
	char *text;     /* Text                 */
} docu_s;

/* Leaf structure (Phrase) */
typedef struct
{	int fact;
	int indx;
	char *text;
} leaf_s;

/* Node structure (Dictionary) */
typedef struct node_t
{	leaf_s leaf;
	struct node_t *left;
	struct node_t *rght;
} node_s;

/* Function prototypes */
void buildText(docu_s *docu, char *file);
void initText(docu_s *docu);
void resizeText(docu_s *docu);
node_s *initTree();
int compPhrs(leaf_s leaf1, leaf_s leaf2);
void growTree(docu_s *docu, node_s *tree);
node_s *growLeaf(leaf_s *leaf, node_s *tree);
node_s *searchTree(leaf_s *leaf, node_s *node);
void freeText(docu_s *docu);
void freeTree(node_s *node);
void printTree(node_s *tree);
void printPair(leaf_s *leaf);

/* Encodes a text file using the LZ78 compression algorithm. */
int main(int argc, char **argv)
{	clock_t start, end;
	start = clock();
	docu_s docu;
	node_s *tree;
	tree = initTree();
	buildText(&docu, argv[argc - 1]);
	growTree(&docu, tree);
	freeText(&docu);
	freeTree(tree);
	end = clock();
	fprintf(stderr, "%fs\n", ((double)(end - start)) / CLOCKS_PER_SEC);
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

/* Initialises binary search tree */
node_s *initTree()
{	node_s *tree;
	tree = (node_s *)malloc(sizeof(node_s));
	assert(tree != NULL);
	tree->leaf.text = (char *)malloc(sizeof(char));
	assert(tree->leaf.text != NULL);
	memset(tree->leaf.text, '\0', sizeof(char));
	tree->leaf.indx = 0;
	tree->left = NULL;
	tree->rght = NULL;
	return tree;
}

/* Adds nodes to the tree when matching or unmatching phrases are *
 * found while iterating through the text                         */
void growTree(docu_s *docu, node_s *tree)
{	int i, lsize = INITIAL;
	node_s *locn;
	leaf_s phrs;
	phrs.text = (char *)malloc(lsize * sizeof(char));
	assert(phrs.text != NULL);
	phrs.indx = 1;
	phrs.fact = 0;
	char cats[2];
	for (i = 0; docu->text[i]; phrs.indx ++)
	{	cats[0] = docu->text[i];
		cats[1] = '\0';
		strcpy(phrs.text, cats);
		locn = searchTree(&phrs, tree);
		if (!locn)
		{	phrs.fact = 0;
			tree = growLeaf(&phrs, tree);
			i ++;
		}
		else
		{	i ++;
			while (locn && docu->text[i])
			{	cats[0] = docu->text[i];
				if (strlen(phrs.text) == lsize)
				{	lsize *= MULTIPLY;
					phrs.text = realloc(phrs.text, lsize * sizeof(char) + sizeof(char));
					assert(phrs.text != NULL);
				}
				if (docu->text[i + 1])
				{	strcat(phrs.text, cats);
					locn = searchTree(&phrs, tree);
				}
				else
				{	locn = searchTree(&phrs, tree);
					strcat(phrs.text, cats);
				}
				i ++;
			}
			locn = growLeaf(&phrs, tree);
			if (!docu->text[i])
			{	if (strlen(phrs.text) == 1)
				{	phrs.fact = 0;
				}
				if (docu->text[i - 1] == '\n')
				{	strcat(phrs.text, cats);
					printPair(&phrs);
				}
			}
		}
	}
	fprintf(stderr, "encode:%7d bytes input\n", i);
	fprintf(stderr, "encode:%7d factors generated\n", phrs.indx - 1);
	free(phrs.text);
}

/* Traverses the tree and adds a node at an appropriate location */
node_s *growLeaf(leaf_s *leaf, node_s *tree)
{	if (!tree)
	{	node_s *node;
		node = (node_s *)malloc(sizeof(node_s));
		assert(node != NULL);
		node->leaf.text = (char *)malloc(strlen(leaf->text) * sizeof(char) + sizeof(char));
		assert(node->leaf.text != NULL);
		strcpy(node->leaf.text, leaf->text);
		node->leaf.indx = leaf->indx;
		node->leaf.fact = leaf->fact;
		node->left = NULL;
		node->rght = NULL;
		printPair(leaf);
		return node;
	}
	else if (compPhrs(*leaf, tree->leaf) < 0)
	{	tree->left = growLeaf(leaf, tree->left);
	}
	else if (compPhrs(*leaf, tree->leaf) > 0)
	{	tree->rght = growLeaf(leaf, tree->rght);
	}
	return tree;
}

/* Recursively searches the tree and returns a matching node */
node_s *searchTree(leaf_s *leaf, node_s *node)
{	if (!node)
	{	return NULL;
	}
	if (compPhrs(*leaf, node->leaf) < 0)
	{	return searchTree(leaf, node->left);
	}
	else if (compPhrs(*leaf, node->leaf) > 0)
	{	return searchTree(leaf, node->rght);
	}
	leaf->fact = node->leaf.indx;
	return node;
}

/* Frees memory allocated to the text */
void freeText(docu_s *docu)
{	free(docu->text);
	docu->text = NULL;
}

/* Recursively traverses and frees the tree */
void freeTree(node_s *node)
{	if (node->left)
	{	freeTree(node->left);
		node->left = NULL;
	}
	if (node->rght)
	{	freeTree(node->rght);
		node->rght = NULL;
	}
	if (node)
	{	free(node->leaf.text);
		node->leaf.text = NULL;
		free(node);
		node = NULL;
	}
}

/* Prints nodes in the tree for debugging */
void printTree(node_s *tree)
{	if (!tree)
	{	return;
	}
	printf("N:'%s'%d	", tree->leaf.text, tree->leaf.fact);
	if (tree->left)
	{	printf("L:'%s'%d	", tree->left->leaf.text, tree->left->leaf.fact);
	}
	if (tree->rght)
	{	printf("R:'%s'%d	", tree->rght->leaf.text, tree->rght->leaf.fact);
	}
	printf("\n");
	printTree(tree->left);
	printTree(tree->rght);
}

/* Prints the (c, k) pair as encoder output */
void printPair(leaf_s *leaf)
{	printf("%c%d\n", leaf->text[strlen(leaf->text) - 1], leaf->fact);
}

/* Comparison function which returns 0 if two strings are equal, *
 * returns 1 if the first string is greater than the second,     *
 * and -1 if otherwise                                           */
int compPhrs(leaf_s leaf1, leaf_s leaf2)
{	return strcmp(leaf1.text, leaf2.text);
}
