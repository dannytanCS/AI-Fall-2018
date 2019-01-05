Input should be put into input.txt in the format of

*S +Noun people *VP +Modal can +Verb fish 
*S +Noun fish *VP +Modal can +Verb people 
*S +Noun fish *VP +Verb people +Noun can 
*S +Noun people *VP +Modal can +Verb people 
*S +Noun fish *VP +Verb can +Noun people 
*S +Noun fish *VP +Verb people +Noun can 
*S +Noun people *VP +Verb people +Noun can 
*S +Noun people *VP +Verb can +Noun fish 
*S *NP +Noun people +Noun people +Verb fish 
*S +Noun fish *VP +Modal can +Verb swim 

To compile:
g++ -std=c++11 -Wall lab4.cpp -o main

To run:
Without flag:

./main 5

With flag for detail output:
./main 5 -f


*The input.txt is changed throughout the program, need to put the original input into input.txt for multiple test cases