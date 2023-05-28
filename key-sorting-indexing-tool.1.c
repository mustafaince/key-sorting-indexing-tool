#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct jsonFile
{
	char dataFileName[20];
	char indexFileName[20];
	int recordLength;
	char keyEncoding[20];
	int keyStart;
	int keyEnd;
	char order[20];
};
struct _Index
{
	char name[25];
	int index;
};
typedef struct _Index Index;

//functions
void readJsonFile(char jsonFile[20]);
void freeString(char *str);
void printMenu(int *answer);
void createIndexFile();
int compareIndexes(const void *a, const void *b);
int findRecordByName(char name1[22]);
int binaryFileSearch(FILE *fp, const char name1[22], int firstIdx, int lastIdx);
void printFile();

//variables
int command;
struct jsonFile json;
FILE *indexfile;
int recordNumber;
char searchKey[25];

int main()
{
	char jsonFileName[20] = "";

	while (command != 6)
	{
		printMenu(&command);
		if (command == 1) //open
		{
			printf("Please enter json file name> ");
			scanf("%s", jsonFileName);
			strcat(jsonFileName, ".json");
			readJsonFile(jsonFileName);

			if ((indexfile = fopen(json.indexFileName, "wb")) == NULL)
			{
				printf("\nindexfile Opening Error!");
			}
			else
			{
				printf("\nJsonfile have been opened and parsed\n");
			}
		}
		else if (command == 2) //create_index
		{
			createIndexFile();
			printf("\nIndexFile have been created\n");
		}
		else if (command == 3) //search
		{
			// Open the file
			FILE *dataFile;
			dataFile = fopen(json.dataFileName, "rb");
			int index;
			char record[json.recordLength];

			printf("Enter a key you want to search> ");
			scanf("%s", searchKey);
			index = findRecordByName(searchKey);
			printf("index is> %d\n", index);

			if (index > -1)
			{
				fseek(dataFile, index * json.recordLength, SEEK_SET);
				fread(record, json.recordLength, 1, dataFile);
				printf("%d\n", json.recordLength);
				printf("The searched record is> %s\n", record);
			}
			else
			{
				printf("Not found...\n");
			}
		}
		else if (command == 4) // close
		{
			printf("\nHave been closed the used files and freed the memory\n");
		}
		else if (command == 5) //print dataFile
		{
			printf("\n");
			printFile();
		}
		else if (command == 6) //Quit
		{
			printf("\nprogram is terminating\n");
		}
		else
		{
			printf("\nCommand can not be found\n");
		}
	}

	return (0);
}

int binaryFileSearch(FILE *fp, const char name1[22], int firstIdx, int lastIdx)
{
	
	Index first, last, middle;
	int returnData;

	// Calculate the middle Index
	int middleIdx = (firstIdx + lastIdx) / 2;

	// Read first record and return if it is the searched one.
	fseek(fp, firstIdx * (sizeof(Index)), SEEK_SET);
	fread(&first, sizeof(Index), 1, fp);
	if (strcmp(first.name, name1) == 0)
	{
		returnData = first.index;
		return returnData;
	}
	// Read last record and return if it is the searched one.
	fseek(fp, lastIdx * sizeof(Index), SEEK_SET);
	fread(&last, sizeof(Index), 1, fp);
	if (strcmp(last.name, name1) == 0)
	{
		returnData = last.index;

		return returnData;
	}
	// Recursion exit condition, If middle index is equal to first or last index
	// required comparisons are already made, so record is not found.
	// Create and return an empty person.
	if (middleIdx == firstIdx || middleIdx == lastIdx)
	{
		int d = -1;
		return d;
	}

	// Read the middle record and return if it is the searched one.
	fseek(fp, middleIdx * sizeof(Index), SEEK_SET);
	fread(&middle, sizeof(Index), 1, fp);
	if (strcmp(middle.name, name1) == 0)
	{
		returnData = middle.index;
		return returnData;
	}
	// Determine the record position and recursively call with appropriate attributes.
	if (strcmp(middle.name, name1) > 0)
	{
		return binaryFileSearch(fp, name1, firstIdx + 1, middleIdx - 1);
	}
	else
	{
		return binaryFileSearch(fp, name1, middleIdx + 1, lastIdx - 1);
	}
}

int findRecordByName(char name1[22])
{
	printf("\nRunning Binary Search...\n");
	indexfile = fopen(json.indexFileName, "rb");

	// Calculate initial first and last indexes.
	int firstIdx = 0;
	fseek(indexfile, recordNumber * sizeof(Index), SEEK_SET);
	int lastIdx = (ftell(indexfile) / sizeof(Index)) - 1;

	// Initiate the search.
	int result = binaryFileSearch(indexfile, name1, firstIdx, lastIdx);
	fclose(indexfile);
	return result;
}

void printFile()
{
	printf("recordNumber: %d\n", recordNumber);
	FILE *inputFile;
	inputFile = fopen(json.dataFileName, "rb");
	fseek(inputFile, 0, SEEK_SET);
	int i;
	for (i = 0; i < recordNumber; i++)
	{
		char buffer[json.recordLength];
		fread(buffer, json.recordLength, 1, inputFile);
		printf("%d. %s\n",i, buffer);
	}
	fclose(inputFile);
}

void createIndexFile() //creating indexfile
{
	FILE *fp;
	fp = fopen(json.dataFileName, "rb");

	if (!fp)
		return;
	int i = 0;
	fseek(fp, 0, SEEK_END);
	recordNumber = ftell(fp) / json.recordLength; //counting record number in File
	Index indexes[recordNumber];
	char name[25];
	fseek(fp, (i * json.recordLength) + json.keyStart, SEEK_SET);
	fread(name, json.keyEnd - json.keyStart + 1, 1, fp);

	while (!feof(fp)) //Fulling the indexes maze with information in datafile.
	{
		strcpy(indexes[i].name, name);
		indexes[i].index = i;
		i++;
		fseek(fp, (i * json.recordLength) + json.keyStart, SEEK_SET);
		fread(name, json.keyEnd - json.keyStart + 1, 1, fp);
	}

	qsort(indexes, recordNumber, sizeof(Index), compareIndexes); // sorting the index file
	if (strcmp(json.order, "DESC") == 0)
	{
		int j;
		Index temp[recordNumber];
		for (j = recordNumber - 1; j >= 0; j--)
		{
			temp[recordNumber - j - 1] = indexes[j];
		}
		for (j = 0; j < recordNumber; j++)
		{
			indexes[j] = temp[j];
		}
	}
	FILE *fp1;
	fp1 = fopen(json.indexFileName, "wb");
	if (!fp1)
		return;
	fseek(fp1, 0, SEEK_SET);
	fwrite(indexes, sizeof(Index), recordNumber, fp1); //writing indexed data to indexfile.
	fclose(fp1);
	fclose(fp);
}

int compareIndexes(const void *a, const void *b) //compare fonction using for quick sort algorithm.
{
	if(strcmp(json.keyEncoding, "BIN")==0) // if keyEncoding is BIN do this
	{
		return atoi(((Index *)a)->name) - atoi(((Index *)b)->name);
	}
	else // if keyEncoding is not BIN
	{
		return (strcmp(((Index *)a)->name, ((Index *)b)->name));
	}
	
}

void printMenu(int *answer) //print the user menu
{

	printf("\nYou can perform the following tasks: \n");
	printf("(1) Open \n");
	printf("(2) Create_index \n");
	printf("(3) Search \n");
	printf("(4) Close \n");
	printf("(5) Print dataFile \n");
	printf("(6) Quit \n");
	printf("Please Select one> ");
	scanf("%d", answer);
}

void readJsonFile(char jsonFile[20]) // parsing json file
{
	FILE *fJson = malloc(sizeof(FILE) * 1);
	char c;
	char *p[7];
	int i = 0;
	int j = 0;

	for (i = 0; i < 7; i++)
	{
		p[i] = malloc(sizeof(char) * 50);
	}

	if ((fJson = fopen(jsonFile, "r")) == NULL)
	{
		printf("\njson File Opening Error!");
	}

	i = 0;
	j = 0;
	while ((c = fgetc(fJson)) != EOF) //reading json file and creating a maze
	{
		if (c == ',')
		{
			i++;
			j = 0;
		}
		else if (c != '{' && c != '}' && c != '\n' && c != ' ')
		{
			p[i][j] = c;
			j++;
		}
	}

	bool cont = false;

	char str[20];
	freeString(str);
	int m = 0;

	for (i = 0; i < 7; i++)
	{
		m = -1;
		for (j = 0; p[i][j] != '\0'; j++)
		{
			if (p[i][j] == ':')
			{
				cont = true;
			}

			if (cont == true && p[i][j] != '\"' && p[i][j] != ':')
			{
				m++;
				str[m] = p[i][j];
			}
		}
		// assinging informations to json Struct
		if (i == 0)
		{
			strcpy(json.dataFileName, str);
		}
		else if (i == 1)
		{
			strcpy(json.indexFileName, str);
		}
		else if (i == 2)
		{
			json.recordLength = atoi(str);
		}
		else if (i == 3)
		{
			strcpy(json.keyEncoding, str);
		}
		else if (i == 4)
		{
			json.keyStart = atoi(str);
		}
		else if (i == 5)
		{
			json.keyEnd = atoi(str);
		}
		else if (i == 6)
		{
			strcpy(json.order, str);
		}

		freeString(str);
		cont = false;
	}

	fclose(fJson);
	for (i = 0; i < 7; i++)
	{
		free(p[i]);
	}
}

void freeString(char *str) // deleting values in a String
{
	int i;
	for (i = 0; i < 20; i++)
	{
		str[i] = '\0';
	}
}
