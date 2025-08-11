#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#define MAX_QUES_LEN 300
#define MAX_OPTION_LEN 100

volatile int timeout_happened = 0;

// Windows color handling
void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void resetColor() {
    setColor(7); // Default white on black
}

// Define color constants using Windows color codes
#define COLOR_END 7
#define RED 12
#define GREEN 10
#define YELLOW 14
#define BLUE 9
#define PINK 13
#define AQUA 11

typedef struct {
    char text[MAX_QUES_LEN];
    char options[4][MAX_OPTION_LEN];
    char correct_option;
    int timeout;
    int prize_money;
} Question;

int read_questions(char* file_name, Question** questions);
void print_formatted_question(Question question);
void play_game(Question* questions, int no_of_questions);
int use_lifeline(Question* question, int* lifeline);

HANDLE hStdin;
DWORD fdwSaveOldMode;

void reset_terminal_attributes() {
    SetConsoleMode(hStdin, fdwSaveOldMode);
}

void set_terminal_attributes() {
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hStdin, &fdwSaveOldMode);
    DWORD fdwMode = fdwSaveOldMode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    SetConsoleMode(hStdin, fdwMode);
    atexit(reset_terminal_attributes);
}

DWORD WINAPI TimerThread(LPVOID lpParam) {
    int timeout = *(int*)lpParam;
    Sleep(timeout * 1000);
    if (!timeout_happened) {
        timeout_happened = 1;
        setColor(RED);
        printf("\n\nTime out!!!!!  Press Any Key...\n");
        resetColor();
        fflush(stdout);
    }
    return 0;
}

int main() {
    srand((unsigned int)time(NULL));
    set_terminal_attributes();
    setColor(PINK);
    printf("\t\tChalo Kehlte hain KAUN BANEGA CROREPATI !!!");
    resetColor();
    Question* questions;
    int no_of_questions = read_questions("questions.txt", &questions);
    play_game(questions, no_of_questions);
    free(questions);
    return 0;
}

void play_game(Question* questions, int no_of_questions) {
    int money_won = 0;
    int lifeline[] = {1, 1};

    for (int i = 0; i < no_of_questions; i++) {
        timeout_happened = 0;
        print_formatted_question(questions[i]);
        
        // Create timer thread
        HANDLE hTimer = CreateThread(NULL, 0, TimerThread, &questions[i].timeout, 0, NULL);
        
        char ch = _getch();
        
        if (!timeout_happened) {
            TerminateThread(hTimer, 0);
            CloseHandle(hTimer);
            
            printf("%c", ch);
            ch = toupper(ch);

            if (timeout_happened == 1) {
                break;
            }

            if (ch == 'L') {
                int value = use_lifeline(&questions[i], lifeline);
                if (value != 2) {
                    i--;
                }
                continue;
            }

            if (ch == questions[i].correct_option) {
                setColor(GREEN);
                printf("\nCorrect!");
                resetColor();
                money_won = questions[i].prize_money;
                setColor(BLUE);
                printf("\nYou have won: Rs %d", money_won);
                resetColor();
            } else {
                setColor(RED);
                printf("\nWrong! Correct answer is %c.", questions[i].correct_option);
                resetColor();
                break;
            }
        } else {
            break;
        }
    }
    setColor(BLUE);
    printf("\n\nGame Over! Your total winnings are: Rs %d\n", money_won);
    resetColor();
}

int use_lifeline(Question* question, int* lifeline) {
    setColor(PINK);
    printf("\n\nAvailable Lifelines:");
    resetColor();
    if (lifeline[0]) {
        setColor(PINK);
        printf("\n1. Fifty-Fifty (50/50)");
        resetColor();
    }
    if (lifeline[1]) {
        setColor(PINK);
        printf("\n2. Skip the Question");
        resetColor();
    }
    setColor(PINK);
    printf("\nChoose a lifeline or 0 to return: ");
    resetColor();

    char ch = _getch();
    printf("%c", ch);

    switch (ch) {
    case '1':
        if (lifeline[0]) {
            lifeline[0] = 0;
            int removed = 0;
            while (removed < 2) {
                int num = rand() % 4;
                if ((num + 'A') != question->correct_option &&
                    question->options[num][0] != '\0') {
                    question->options[num][0] = '\0';
                    removed++;
                }
            }
            return 1;
        }
        break;
    case '2':
        if (lifeline[1]) {
            lifeline[1] = 0;
            return 2;
        }
        break;
    default:
        setColor(PINK);
        printf("\nReturning to the Question.");
        resetColor();
        break;
    }
    return 0;
}

void print_formatted_question(Question question) {
    setColor(YELLOW);
    printf("\n\n%s", question.text);
    resetColor();
    for (int i = 0; i < 4; i++) {
        if (question.options[i][0] != '\0') {
            setColor(AQUA);
            printf("\n%c. %s", ('A' + i), question.options[i]);
            resetColor();
        }
    }
    setColor(YELLOW);
    printf("\nHurry!! You have only %d Seconds to answer..", question.timeout);
    resetColor();
    setColor(GREEN);
    printf("\nEnter your answer (A, B, C, or D) or L for lifeline: ");
    resetColor();
}

int read_questions(char* file_name, Question** questions) {
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        printf("\nUnable to open the questions bank.");
        exit(0);
    }
    char str[MAX_QUES_LEN];
    int no_of_lines = 0;
    while (fgets(str, MAX_QUES_LEN, file)) {
        no_of_lines++;
    }
    int no_of_questions = no_of_lines / 7; 
    *questions = (Question*)malloc(no_of_questions * sizeof(Question));
    rewind(file);
    for (int i = 0; i < no_of_questions; i++) {
        fgets((*questions)[i].text, MAX_QUES_LEN, file);
        
        (*questions)[i].text[strcspn((*questions)[i].text, "\n")] = 0;
        
        for (int j = 0; j < 4; j++) {
            fgets((*questions)[i].options[j], MAX_OPTION_LEN, file);
            
            (*questions)[i].options[j][strcspn((*questions)[i].options[j], "\n")] = 0;
        }
        
        char option[10];
        fgets(option, 10, file);
        (*questions)[i].correct_option = toupper(option[0]);

        char timeout[10];
        fgets(timeout, 10, file);
        (*questions)[i].timeout = atoi(timeout);

        char prize_money[10];
        fgets(prize_money, 10, file);
        (*questions)[i].prize_money = atoi(prize_money);
    }
    fclose(file);
    return no_of_questions;
}