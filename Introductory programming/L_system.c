/**
 * "L_system"
 *
 * Project for the Introduction to Programming course.
 *
 * Compilation command:
 * gcc @options L_system.c -o L_system
 *
 * The program allows the interpretation of an input word belonging to the language
 * defined by the L_system.
 *
 * Reads a non-negative integer specifying the length of the derivation.
 *
 * Reads lines describing the L-system:
 * a line containing the axiom,
 * lines with replacement rules.
 *
 * Reads lines describing the interpretation of the word:
 * prologue lines preceded by an empty line,
 * interpretation rule lines preceded by an empty line,
 * epilogue lines preceded by an empty line.
 *
 * Writes the interpretation of the word consisting of:
 * prologue lines,
 * transformed and interpreted axiom,
 * epilogue lines.
 * Author: Julia Lucyn
 * Date: 2023-12-11
 */


#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>


/**
 * Maximum allowable line length.
 */
#define COL 102


/**
 * Fills an array of size CHAR_MAX by COL with zeros ('0').
 */
void clear(char array[][COL]) {
    for (int i = 0; i < CHAR_MAX; ++i) {
        for (int j = 0; j < COL; ++j) {
            array[i][j] = '0';
        }
    }
}


/**
 * Reads and returns the length of the output.
 */
int outputLength(void) {
    int n;
    scanf("%d", &n);
    int c = getchar();
    while ((char) c != '\n') {
        c = getchar();
    }
    return n;
}


/**
 * Reads the axiom.
 */
void readAxiom(char* axiom) {
    int w = 0;
    int c = getchar();
    while ((c != '\n') && (c != EOF) && (w < COL)) {
        axiom[w] = (char) c;
        ++w;
        c = getchar();
    }
    axiom[w] = (char) c;
}


/**
 * Reads substitution or interpretation rules and saves them in the 'text' array.
 */
void readRules(char text[][COL]) {
    int w = 0;
    int d = 0;
    int c = getchar();

    while ((c != EOF) && (w < CHAR_MAX) && (c != '\n')) {
        while ((c != '\n') && (c != EOF) && (d < COL)) {
            text[w][d] = (char) c;
            ++d;
            c = getchar();
        }

        text[w][d] = '\n';
        c = getchar();
        ++w;
        d = 0;
    }

    text[w][d] = '\n';
}


/**
 * Prints lines of a prologue or epilogue.
 */
void printText(char** text) {
    int i = 0;
    int j = 0;

    while (text[i][j] != '\n') {
        while (text[i][j] != '\n') {
            printf("%c", text[i][j]);
            ++j;
        }

        printf("\n");
        ++i;
        j = 0;
    }
}


/**
 * Reads and prints lines of a prologue or epilogue.
 */
void readText(void) {
    char line[COL];

    while ((fgets(line, sizeof(line), stdin)) && (strlen(line) > 1)) {
        printf("%s", line);
    }
}


/**
 * Interprets an 'axiom' character.
 * Searches for an appropriate interpretation rule for the given character.
 * If it finds one, it prints it.
 * If there is no interpretation rule for the character, it prints nothing.
 * 'reg_inter' is an array containing interpretation rules.
 */
void interpret(char axiom, char reg_inter[][COL]) {
    int j = 0;
    int k = 0;

    while ((reg_inter[j][k] != '\n') && (reg_inter[j][k] != axiom)) {
        ++j;
    }

    while (reg_inter[j][k] != '\n') {
        ++k;
        printf("%c", reg_inter[j][k]);
    }
}


/**
 * Replaces the axiom - 'axiom' according to substitution rules - 'sub_rules'.
 * Interprets the modified axiom according to interpretation rules 'int_rules'.
 * 'n' specifies how many times each character will be replaced.
 * If there is no substitution rule for a character, it does not change it.
 */
void substitute(char axiom[], int n, char sub_rules[][COL], char int_rules[][COL]) {
    char *temp = (char*) calloc(COL, sizeof(char));
    int i = 0;

    if (n == 0) {
        while (axiom[i] != '\n') {
            interpret(axiom[i], int_rules);
            ++i;
        }
    }

    else {
        while (axiom[i] != '\n') {
            int j = 0;
            int k = 0;
            int index = 0;

            while ((sub_rules[j][k] != '\n') && (sub_rules[j][k] != axiom[i])) {
                ++j;
            }

            if ((sub_rules[j][k] == '\n') || (sub_rules[j][k] != axiom[i])) {
                temp[index] = axiom[i];
                ++index;
                temp[index] = '\n';
                substitute(temp, 0, sub_rules, int_rules);
            }

            else {
                ++k;

                while (sub_rules[j][k] != '\n') {
                    temp[index] = sub_rules[j][k];
                    ++k;
                    ++index;
                }

                temp[index] = '\n';
                substitute(temp, n - 1, sub_rules, int_rules);
            }

            ++i;
        }
    }

    free(temp);
}


/**
 * Reads and interprets the L-system.
 */
void process(char sub_rules[][COL], char int_rules[][COL], char axiom[]) {
    int n = outputLength();
    clear(sub_rules);
    clear(int_rules);

    for (int i = 0; i < COL; ++i) {
        axiom[i] = '0';
    }

    readAxiom(axiom);
    readRules(sub_rules);
    readText();
    readRules(int_rules);
    substitute(axiom, n, sub_rules, int_rules);
    readText();
}


/**
 * Runs the program.
 */
int main(void) {
    char axiom[COL];
    char sub_rules[CHAR_MAX][COL];
    char int_rules[CHAR_MAX][COL];
    process(sub_rules, int_rules, axiom);
    return 0;
}

