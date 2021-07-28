/*Functions that find if a word is in a file and also a funcion that sees if a word similary matched the requested word */

#include<math.h>
#include<string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

typedef struct
{
  char thisWord[30];
}word;

//main functions
int isExactMatch(char fileName[], char searchFor[]);
int isRoughMatch(char fileName[], char searchFor[],int characters);

//helper funcitons
word getWord(FILE *searchThrough);
void makeLowercase(char word[]);
void cleanString(char string[]);

int closeWord(char keyWord[],char currentWord[],int characters);
int continueCheck(char keyword[],char currentWord[]);



//returns 1 if word(searchFor) found in file
int isExactMatch(char fileName[], char searchFor[])
{
  FILE *searchThrough = fopen(fileName,"r");

  if (searchThrough == NULL)
	{
		printf("Couldn't open file\n");
    return -1;
	}

  /*make the word that you are looking for all lowercase so it will match the lowercase words as they come through*/
  char keyWord[30];
  strcpy(keyWord,searchFor);
  makeLowercase(keyWord);
  cleanString(keyWord);

  //for the word that will be compared
  word currentWord;

  do
  {
    //scan the word
    currentWord=getWord(searchThrough);

    //if the file is empty stops
    if(currentWord.thisWord[0]=='\0')
    {
      break;
    }

    //makes the word compareable
    cleanString(currentWord.thisWord);

    //printf("Current word: %s\n",currentWord.thisWord);

    //compare the word, change if match
    if(strcmp(keyWord,currentWord.thisWord)==0)
    {
      printf("Your requested word was found exactly.\n");
      return 1;
      break;
    }


  }while(1);

  fclose(searchThrough);

  return 0;
}

int isRoughMatch(char fileName[], char searchFor[], int characters)
{
  FILE *searchThrough = fopen(fileName,"r");

  if (searchThrough == NULL)
	{
		printf("Couldn't open file\n");
    return -1;
	}

  /*make the word that you are looking for all lowercase so it will match the lowercase words as they come through*/
  char keyWord[30];
  strcpy(keyWord,searchFor);
  makeLowercase(keyWord);
  cleanString(keyWord);

  //for the word that will be compared
  word currentWord;

  do
  {
    //scan the word
    currentWord=getWord(searchThrough);

    //if the file is empty stops
    if(currentWord.thisWord[0]=='\0')
    {
      break;
    }

    //makes the word compareable
    makeLowercase(currentWord.thisWord);
    cleanString(currentWord.thisWord);

    //printf("Current word: %s\n",currentWord.thisWord);

    //compare the word, change if match
    if(strcmp(keyWord,currentWord.thisWord)==0)
    {
      printf("Your requested word was found exactly.\n");
      return 1;
      break;
    }
    if(closeWord(keyWord,currentWord.thisWord,characters)==1)
    {
      printf("There was a close match to your word.\n");
      return 1;
      break;
    }


  }while(1);

  fclose(searchThrough);

  return 0;
}

//makes words all lowercase
void makeLowercase(char word[])
{
  int strLength= strlen(word);

  for(int i=0; i<strLength; i++)
  {
    word[i] = tolower(word[i]);
  }
}

//makes the string into a all lowercase no punctuation or space word.
void cleanString(char string[])
{
  char holdString[30];
  int charsAdded=0;
  char compare;
  for(int i=0;i<strlen(string);i++)
  {
    compare=string[i];
    if(('A'<=compare && compare<='Z') || ('a'<=compare && compare<='z'))
    {
      string[i] = tolower(compare);
      string[charsAdded]=compare;
      charsAdded++;
    }
  }
  string[charsAdded]='\0';
}

//reads a word from the file
word getWord(FILE *searchThrough)
{

  word placeHolder;
  int spaceIncountered=0;
  int charsAdded=0;

  char currentCharacter;
  currentCharacter=fgetc(searchThrough);

  if(currentCharacter==EOF)
  {
    //printf("adding blank\n");
    placeHolder.thisWord[0]='\0';
    return placeHolder;
  }
  
  while( currentCharacter != EOF && spaceIncountered==0)
  { 

    if(currentCharacter==32)
    {
      //printf("SPACE INCOUNTERED\n");
      spaceIncountered=1;
    }
    // if space and first disregarded
    if(spaceIncountered==1 && charsAdded==0)
    {
      //printf("Reseting spaces added\n");
      spaceIncountered=0;
    }
    else if(spaceIncountered==0)
    {
      //printf("Adding: %c\n",currentCharacter);
      placeHolder.thisWord[charsAdded]=currentCharacter;
      charsAdded++;
    }
    if(spaceIncountered==0)
    {
      currentCharacter= fgetc(searchThrough);
    }
  }

  placeHolder.thisWord[charsAdded]='\0';
  return placeHolder;
}

//Sees if a word has (character) many characters matching
int closeWord(char keyWord[],char currentWord[],int characters)
{
  //printf("New word: %s\n",currentWord);
  int mostConsecutive=0;
  int currentConsecutive=0;

  for(int i=0;i< strlen(keyWord); i++)
  {
    for(int j=0;j<strlen(currentWord);j++)
    {
      if(keyWord[i]==currentWord[j])
      {
        currentConsecutive=continueCheck(&keyWord[i],&currentWord[j]);
        if(currentConsecutive>mostConsecutive)
        {
          mostConsecutive=currentConsecutive;
        }
      }
    }
  }

  if(mostConsecutive>=characters)
  {
    return 1;
  }

  return 0;
}

//takes a matching character and sees how many match after that
int continueCheck(char keyword[],char currentWord[])
{
  int matchingChars=1;
  int max=0;
  if(strlen(keyword)>strlen(currentWord))
  {
    max=strlen(currentWord);
  }
  else
  {
    max=strlen(keyword);
  }
  for(int i=1; i<max; i++)
  {
    if(keyword[i]==currentWord[i])
    {
      matchingChars++;
    }
    else
    {
      return matchingChars;
    }
  }
  return matchingChars;
}