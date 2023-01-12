#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int SIZE = 5;
static int guess_number = 1;

typedef struct
{
  char letter;
  int count;
} count_l;

static int blacklist[26];
count_l count_list[26];
static char yellowlist[5];
static char greenlist[5];


int suggest_valid (char word[], char color_array[], char word_array[]);
void csv_to_table ();
int score_calculator (char word_array[], char color_array[]);
void suggest (char word_array[], char color_array[], char fname[]);
int guess ();

void cleanup()
{
  for (int i = 0; i < 7 ; i++){
      char buffer[100];
      snprintf(buffer, sizeof buffer, "possible_solution%d.csv", i);
      remove(buffer);
  }
}
// Main Function
// Calls csv_to_table
// Iterates through the 6 guesses that we have
int
main (void)
{
  // Storing each character in count_list
  for (int i = 0; i < 26; i++)
    {
      count_list[i].letter = (65 + i);
      count_list[i].count = 0;
    }
  cleanup();
  csv_to_table ();
  int chk = 0;
  printf("Guess Number = 1\n");
  while (chk == 0)
    {
      if (guess_number > 6)
	printf ("Better Luck Next Time\n"); // Program failed
      if (guess () == 0)
	{
	  printf ("YAY You beat Wordle in %d guesses.\n", guess_number);
	  chk = 1;
	}
    }
}

// Input all character in "valid_solutions.csv" to an array of struct count_l
// Containing letter and number of occurences of each of them in an array
void
csv_to_table (void)
{
  FILE *file;
  char word[6];
  char buffer[100];
  file = fopen ("valid_solutions.csv", "r");
  while ((fscanf (file, "%s", &word[0])) != EOF)
    {
      for (int i = 0; i < 5; i++)
	{
	  count_list[word[i] - 97].count++;
	}
    }
  fclose (file);
  return;
}

// Inputs user's guess and the corresponding color array
// Calculates score of that word using score_calculator()
// Uses this score to determine file to be used for traversal
// Transfers control to suggest()
int
guess ()
{
  char word_array[6] = "";
  char color_array[6] = "";
  char fname[] = "valid_solutions.csv";
  while (strlen (word_array) != SIZE)
    {
      printf ("Input Word:");
      scanf ("%s", word_array);
    }
  while (strlen (color_array) != SIZE)
    {
      printf ("Input Color Array (G-Green, Y-Yellow, B-Black):");
      scanf ("%s", color_array);
    }

  int score = score_calculator (word_array, color_array);
  if (score == 25) // All greens
    {
      return 0;
    }
  else if (score < 10)
    {
      strcpy (fname, "valid_guesses.csv");
    };
  guess_number++;
  printf ("-------------\nGuess Number = %d \n", guess_number);
  suggest (word_array, color_array, fname);
  return 1;
}

// Calculates Score produced based on values given to green,
// yellow and black characters
int
score_calculator (char word_array[], char color_array[])
{
  int green_score = 5;
  int yellow_score = 2;
  int grey_score = 1;
  int score = 0;
  for (int i = 0; i < SIZE; i++)
    {
      if ((color_array[i]) == 'G')
	score += green_score;
      else if ((color_array[i]) == 'Y')
	score += yellow_score;
      else if ((color_array[i]) == 'B')
	score += grey_score;
    }
  return score;
}

// Iterating function for each probable suggestion.
// Opens designated file and sends it to suggest_valid()
// After finding the suggestion with max score it will print it
void
suggest (char word_array[], char color_array[], char fname[])
{

  FILE *file;
  char next_guess[6];
  char buffer[100];
  int score = 0;
  char guess_word[10];
  snprintf(buffer, sizeof buffer, "possible_solution%d.csv", guess_number - 1);
  if (guess_number > 2) fname = buffer;
  file = fopen (fname, "r");
  // Iterating through designated file
  while ((fscanf (file, "%s", next_guess)) != EOF)
    {
      if (score < suggest_valid (next_guess, color_array, word_array))
	{
	  score = suggest_valid (next_guess, color_array, word_array);
	  strcpy (guess_word, next_guess);
	}
    }
  fclose (file);
  // Printing Next probable guess (capitalized)
  printf ("Next Suggested Guess is:");
  printf ("%c%c%c%c%c\n",
	  guess_word[0] - 32,
	  guess_word[1] - 32,
	  guess_word[2] - 32,
	  guess_word[3] - 32,
	  guess_word[4] - 32);
}

// Algorithm holding part of the program
// Checks validity of each word and returns either 0 if invalid
// Or returns words score if valid
int
suggest_valid (char next_guess[], char color_array[], char word_array[])
{
  char letter;
  int word_score = 0;
  int pre_count = 0;  // Checking yellow count is accurate

  for (int i = 0; i < 5; i++)
    {
      // greenlist updates, with yellowlist blank fills
      if (color_array[i] == 'G')
	{
	  yellowlist[i] = ' ';
	  greenlist[i] = word_array[i];
	}
      // blacklist updates with yellowlist and greenlist blank fills
      else if (color_array[i] == 'B')
	{
	  greenlist[i] = ' ';
	  yellowlist[i] = ' ';
	  blacklist[word_array[i] - 65] = 1;
	}
      // yellowlist updates with greenlist blank fills
      else if (color_array[i] == 'Y')
	{
	  greenlist[i] = ' ';
	  pre_count++;
	  yellowlist[i] = word_array[i];
	}
      letter = next_guess[i] - 32;

      // Check if greenlist[i] is not empty
      if (greenlist[i] != ' ')
	{
	  // If corresponding to green letter allow passage
	  if (greenlist[i] != letter)
	    return 0;
	  else
	    continue;
	}
	    // If corresponding to yellow letter disallow passage
      if (letter == yellowlist[i])
	return 0;
	    // If corresponding to black letter which is not an
	    // unused yellow letter allow passage
      if (blacklist[(letter - 65)] == 1)
	{
	  if (pre_count > 0)
	    {
	      for (int j = 0; j < 5; j++)
		{
		  if (color_array[i] == 'Y' && letter == word_array[i])
		    pre_count--;
		}
	      pre_count--;
	    }
	  else
	    return 0;
	}
      word_score += count_list[letter - 65].count;
    }


  // Final Check for yellow characters
  char ylist[5];
  strcpy (ylist, yellowlist);
  for (int i = 0; i < 5; i++)
    {
      if (ylist[i] != ' ')
	{
	  for (int j = 0; j < 5; j++)
	    {
	      if (ylist[i] == (next_guess[j] - 32))
		{
		  ylist[i] = ' ';
		}
	    }
	}
    }
  for (int i = 0; i < 5; i++)
    {
      if (ylist[i] != ' ')
	{
	  return 0;
	}
    }
  char buffer[100];
  snprintf(buffer, sizeof buffer, "possible_solution%d.csv", guess_number);
  FILE *file = fopen (buffer, "a");
  //printf("%s\n", next_guess);
  fprintf(file, "%s\n", next_guess);
  fclose(file);

  return word_score;
}
