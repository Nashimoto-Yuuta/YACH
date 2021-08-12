#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const char *kHangman_string =
  "=========\n"
  "   +---+ \n"
  "   |   | \n"
  "       | \n"
  "       | \n"
  "       | \n"
  "       | \n"
  "=========";

char *kCurrent_hangman;
char kWord[64];
char kLetters[25];
int kLives;

bool init();
void reset_game();
bool choose_word();
void update_hangman();
bool win();
void draw();
void cleanup();
void flushinput();
size_t lines_in_file(FILE* file);

int main()
{
  if (!init())
  {
    printf("Error: You're probably out of memory!\nExiting...");
    getchar();
    exit(EXIT_FAILURE);
  }

  reset_game();

  char input;
  do
  {
    draw();

    input = getchar();
    flushinput();

    if (kLives == 0 || win())
    {
      reset_game();
      continue;
    }

    if ((input >= 'a' && input <= 'z') && !strchr(kLetters, input))
    {
      kLetters[strlen(kLetters)] = input;

      if (!strchr(kWord, input))
      {
        kLives--;
        update_hangman();
      }
    }
  }while(input != '0');

  cleanup();

  return EXIT_SUCCESS;
}

bool init()
{
  srand(time(NULL));

  size_t len = strlen(kHangman_string);;

  if (len == 0)
    return false;

  kCurrent_hangman = (char*)malloc(len+1);

  if (!kCurrent_hangman)
    return false;

  printf("Yet Another Hangman (type 0 to exit or anything else to start)");
  getchar();
  flushinput();
  return true;
}

void reset_game()
{
  strcpy(kCurrent_hangman, kHangman_string);
  kLives = 6;
  memset(kLetters,0,strlen(kLetters));
  memset(kWord,0,strlen(kWord));

  if (!choose_word())
  {
    printf("Error: Couldn't load \'wordlist.txt\'!\nExiting...");
    getchar();
    cleanup();
    exit(EXIT_FAILURE);
  }
}

bool choose_word()
{
  FILE * file;
  file = fopen ("wordlist.txt","r");

  if (file!=NULL)
  {
    char buffer[64];
    size_t lineno = 0;
    size_t linerand = rand() % lines_in_file(file);

    if (fgets(buffer, sizeof(buffer), file))
      strcpy(kWord, buffer);
    else
      strcpy(kWord, "erroremptyfile");

    lineno++;

    while (fgets(buffer, sizeof(buffer), file))
    {
      if (lineno++ == linerand)
      {
        strcpy(kWord, buffer);
        break;
      }
    }

    // fuck trailing characters, also fuck fgets
    char *pos;
    if ((pos=strchr(kWord, '\n')))
      *pos = '\0';

    fclose(file);
    return true;
  }

  return false;
}

void update_hangman()
{
  switch (kLives)
  {
    case 5: kCurrent_hangman[0x21] = '0'; break;
    case 4: kCurrent_hangman[0x2B] = '|'; break;
    case 3: kCurrent_hangman[0x2A] = '/'; break;
    case 2: kCurrent_hangman[0x2C] = '\\'; break;
    case 1: kCurrent_hangman[0x34] = '/'; break;
    case 0: kCurrent_hangman[0x36] = '\\'; break;
  }
}

bool win()
{
  for(int i = 0;i < strlen(kWord);++i)
  {
    if (!strchr(kLetters, kWord[i]))
      return false;
  }
  return true;
}

void draw()
{
  system("CLS");
  printf("%s\n\n", kCurrent_hangman);

  for(int i = 0;i < strlen(kWord);++i)
  {
    if (kLives == 0)
      printf("%c ", kWord[i]);
    else
    if (strchr(kLetters, kWord[i]))
      printf("%c ", kWord[i]);
    else
      printf("_ ", kWord[i]);
  }

  if (kLives > 0)
    printf("\n\nSelect character: ");
}

size_t lines_in_file(FILE* file)
{
  size_t lines = 0;
  if (file)
  {
    char buffer[1024];
    size_t read;
    do
    {
      read = fread(buffer, 1, 1024, file);

      for(int i = 0;i < read;++i)
      {
        if (buffer[i] == '\n')
          lines++;
      }
    }while(read != 0);
    fseek(file, 0, SEEK_SET);
  }
  return lines;
}

void cleanup()
{
  free(kCurrent_hangman);
}

void flushinput()
{
  char c;
  while((c = getchar()) != '\n' && c != EOF);
}
