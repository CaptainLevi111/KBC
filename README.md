# KBC
A Windows-compatible implementation of the popular Indian game show "Kaun Banega Crorepati" (Who Wants to Be a Millionaire).

Features
Multiple-choice quiz game with increasing difficulty

Timed questions (must answer within specified time)

Two lifelines:

50:50 (eliminates two wrong answers)

Skip Question (pass to next question without penalty)

Colorful console interface

Progressive prize money

Requirements
Windows OS

C compiler (MSVC, MinGW, or other Windows-compatible C compiler)

How to Run
Compile the program:

text
gcc kbc.c -o kbc.exe
Make sure you have a questions.txt file in the same directory with questions in this format:

text
Question text
Option A
Option B
Option C
Option D
Correct option letter (A-D)
Time limit in seconds
Prize money for this question
Run the executable:

text
kbc.exe
Controls
Press A/B/C/D to answer

Press L to use a lifeline

During lifeline selection:

Press 1 for 50:50

Press 2 to Skip Question

Press 0 to cancel

Notes
The game ends if you answer incorrectly or run out of time

Your total winnings are displayed at the end

Questions should be arranged in increasing order of difficulty

Enjoy the game and test your knowledge!
