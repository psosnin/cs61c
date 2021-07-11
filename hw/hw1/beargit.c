#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "beargit.h"
#include "util.h"

/* Implementation Notes:
 *
 * - Functions return 0 if successful, 1 if there is an error.
 * - All error conditions in the function description need to be implemented
 *   and written to stderr. We catch some additional errors for you in main.c.
 * - Output to stdout needs to be exactly as specified in the function description.
 * - Only edit this file (beargit.c)
 * - You are given the following helper functions:
 *   * fs_mkdir(dirname): create directory <dirname>
 *   * fs_rm(filename): delete file <filename>
 *   * fs_mv(src,dst): move file <src> to <dst>, overwriting <dst> if it exists
 *   * fs_cp(src,dst): copy file <src> to <dst>, overwriting <dst> if it exists
 *   * write_string_to_file(filename,str): write <str> to filename (overwriting contents)
 *   * read_string_from_file(filename,str,size): read a string of at most <size> (incl.
 *     NULL character) from file <filename> and store it into <str>. Note that <str>
 *     needs to be large enough to hold that string.
 *  - You NEED to test your code. The autograder we provide does not contain the
 *    full set of tests that we will run on your code. See "Step 5" in the homework spec.
 */

/* beargit init
 *
 * - Create .beargit directory
 * - Create empty .beargit/.index file
 * - Create .beargit/.prev file containing 0..0 commit id
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_init(void) {
  fs_mkdir(".beargit");

  FILE* findex = fopen(".beargit/.index", "w");
  fclose(findex);
  
  write_string_to_file(".beargit/.prev", "0000000000000000000000000000000000000000");

  return 0;
}


/* beargit add <filename>
 * 
 * - Append filename to list in .beargit/.index if it isn't in there yet
 *
 * Possible errors (to stderr):
 * >> ERROR: File <filename> already added
 *
 * Output (to stdout):
 * - None if successful
 */

int beargit_add(const char* filename) {
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");

  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      fprintf(stderr, "ERROR: File %s already added\n", filename);
      fclose(findex);
      fclose(fnewindex);
      fs_rm(".beargit/.newindex");
      return 3;
    }

    fprintf(fnewindex, "%s\n", line);
  }

  fprintf(fnewindex, "%s\n", filename);
  fclose(findex);
  fclose(fnewindex);

  fs_mv(".beargit/.newindex", ".beargit/.index");

  return 0;
}


/* beargit rm <filename>
 * 
 * See "Step 2" in the homework 1 spec.
 *
 */

int beargit_rm(const char* filename) {
  /* COMPLETE THE REST */
  FILE* findex = fopen(".beargit/.index", "r");
  FILE *fnewindex = fopen(".beargit/.newindex", "w");
  int found = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    strtok(line, "\n");
    if (strcmp(line, filename) == 0) {
      found = 1;
    } else {
      fprintf(fnewindex, "%s\n", line);
    }

  }
  if (found !=1) {
    fprintf(stderr, "ERROR: File %s not tracked\n", filename);
  } else {
    fs_mv(".beargit/.newindex", ".beargit/.index");
  }
  fclose(findex);
  fclose(fnewindex);
  return 0;
}

/* beargit commit -m <msg>
 *
 * See "Step 3" in the homework 1 spec.
 *
 */

const char* go_bears = "GO BEARS!";

int is_commit_msg_ok(const char* msg) {
  /* COMPLETE THE REST */
  int i = 0;
  int j = 0;
  char current = msg[0];

  while (current != '\0') {
    if (current == go_bears[j]) {
      j++;
      if (j == 9) return 1;
    } else {
      j = 0;
    }
    i++;
    current = msg[i];
  }
  return 0;
}

void next_commit_id(char* commit_id) {
  /* COMPLETE THE REST */
  int length, i;
  for (length = 0; commit_id[length] != '\0'; length++);
  if (commit_id[length-1] == '0') {
    strcpy(commit_id, "1111111");
  } else {
    for (i = length-1; i >= 0; i--) {
      if (commit_id[i] == '1') {
        commit_id[i] = '6';
        break;
      } else if (commit_id[i] == '6') {
        commit_id[i] = 'c';
        break;
      } else {
        commit_id[i] = '1';
      }
    }
  }
}

char * concat (const char* str1, const char* str2) {
  char *result = malloc(strlen(str1) + strlen(str2) + 1);
  strcpy(result, str1);
  strcat(result, str2);
  return result;
}

int beargit_commit(const char* msg) {
  char newdir[COMMIT_ID_SIZE];

  if (!is_commit_msg_ok(msg)) {
    fprintf(stderr, "ERROR: Message must contain \"%s\"\n", go_bears);
    return 1;
  }

  char commit_id[COMMIT_ID_SIZE];
  read_string_from_file(".beargit/.prev", commit_id, COMMIT_ID_SIZE);
  next_commit_id(commit_id);
  /* COMPLETE THE REST */ 
  strcat(newdir, ".beargit/");
  strcat(newdir, commit_id);
  fs_mkdir(newdir);
  fs_cp(".beargit/.prev", concat(newdir, "/.prev"));
  fs_cp(".beargit/.index", concat(newdir, "/.index"));
  
  //copy all tracked files into .beargit/commit_id
  FILE* findex = fopen(".beargit/.index", "r");
  int i = 0;
  char line[FILENAME_SIZE];
  while(fgets(line, sizeof(line), findex)) {
    i++;
    strtok(line, "\n");
    fs_cp(line, concat(newdir, concat("/",line)));
    }
  fclose(findex);
  write_string_to_file(concat(newdir, "/.msg"), msg);
  write_string_to_file(".beargit/.prev", commit_id);
  return 0;
}

/* beargit status
 *
 * See "Step 1" in the homework 1 spec.
 *
 */

int beargit_status() {
  /* COMPLETE THE REST */
  FILE* findex = fopen(".beargit/.index", "r");
  int i = 0;
  char line[FILENAME_SIZE];
  printf("Tracked files:\n\n");
  while(fgets(line, sizeof(line), findex)) {
    i++;
    strtok(line, "\n");
    printf("  %s\n", line);
    }
  printf("\n%i files total\n", i);
  fclose(findex);

  return 0;
}

/* beargit log
 *
 * See "Step 4" in the homework 1 spec.
 *
 */

int beargit_log() {
  /* COMPLETE THE REST */
  char prev[COMMIT_ID_SIZE];
  char msg[MSG_SIZE];

  read_string_from_file(".beargit/.prev", prev, COMMIT_ID_SIZE);
  if (prev[1] == '0') {
    fprintf(stderr, "ERROR: There are no commits!\n");
    return 1;
  }
  printf("\n");

  while (prev[1] != '0') {
    printf("commit %s\n", prev);
    read_string_from_file(concat(concat(".beargit/", prev), "/.msg"), msg, MSG_SIZE);
    printf("    %s\n", msg);
    strcpy(msg, "");
    read_string_from_file(concat(concat(".beargit/", prev), "/.prev"), prev, COMMIT_ID_SIZE);
    printf("\n");
  }
  return 0;
}
