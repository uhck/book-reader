#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <regex>
#include <iomanip>
#include <algorithm>
#include <queue>
#include <ctime>
#include <map>
#include "bst.h"
#include "node.h"

using namespace std;

//Obtains file document for reading
bool getInput(string& filename, ifstream& fin);
//Read words into orchard trees and calculate stats from doc
bool process(ifstream& fin);
//Check if the line is the beginning of a new paragraph
void checkNewParOrLine(const string& line, unsigned int& parNum, unsigned int& lineNum);
//Set up the dictionary map for existing English contractions
void setMap(map<string,string>& EnglishContractions);
//Count the syllables of each word
unsigned int countSyllables(string word);
//Find and organize most frequently occuring words in book
void getFrequentWords(bst<string> orchard, priority_queue<node<string>*>& freqWords);
//Return the appropriate reading level of the book
string getReadingLevel(double readingScore);
//Output the stat summary of book
void display(ostream &out, unsigned int wordNum, unsigned int parNum, string readingLevel,
        const vector<string>& wordsPerLetter, double duration);
//Save summary information to output file
bool toSave(istream& in, ofstream& fout);

int main(int argc, char *argv[])
{
    ifstream fin;
    string filename;
    while(getInput(filename,fin))
        process(fin);
    cout<<"Exiting program.\n";
    return 0;
}

bool getInput(string& filename, ifstream& fin)
{
    cout << "Enter file name or \'/e\' to exit: ";
    getline(cin, filename);
    cin.clear();
    if (filename == "/e")
        return false;
    fin.open(filename.c_str());
    while (fin.fail())
    {
        cout << "Failed to open file!\nEnter file name or (\'/e\' to exit: ";
        getline(cin, filename);
        if (filename == "/e")
            return false;
        fin.open(filename.c_str());
    }
    return true;
}

bool process(ifstream& fin)
{
    stringstream ss;
    ofstream fout;
    bst<string> orchard[26];
    string line, word, ans;
    unsigned int lineNum=0, parNum=1, wordNum=0, sentenceNum=0, syllNum=0, pos;
    double duration;
    map<string,string> EnglishContractions;
    setMap(EnglishContractions);
    regex words("([A-Za-z]+'?[A-Za-z]*)|([.!?])+");
    regex_token_iterator<string::iterator> rend;
    map<string,string>::iterator findContractions;
    map<string,string>::iterator cend = EnglishContractions.end();

    clock_t start;

    start = clock();
    while (getline(fin, line))
    {
        checkNewParOrLine(line, parNum, lineNum);
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        regex_token_iterator<string::iterator> next(line.begin(), line.end(), words);
        while (next != rend)
        {
            word = *next;
            if ((pos = word.find("...")) != string::npos)
                word.erase(pos, 3);
            if ((pos = word.find_first_of(".?!")) != string::npos)
            {
                word.erase(pos, word.length()-pos);
                sentenceNum++;
            }
            else if ((word != "") && isalpha(word[0]))
            {
                wordNum++;
                if (word.find("\'") != string::npos)
                {
                    findContractions = EnglishContractions.find(word);
                    if(findContractions != cend)
                        word = EnglishContractions.find(word)->second;
                }
                ss << word;
                while (ss >> word)
                {
                    syllNum += countSyllables(word);
                    word[0] = toupper(word[0]);
                    orchard[word[0]-65].insert(word, 1, lineNum, parNum);
                }
                ss.clear();
            }
            *next++;
        }
    }
    duration = (clock()-start) / (double)CLOCKS_PER_SEC;

    double readingScore = 206.835 - (1.015 * wordNum / sentenceNum) - (84.6 * syllNum / wordNum);
    string readingLevel = getReadingLevel(readingScore);
    vector<string> wordsPerLetter;
    for (int i=0; i<26; i++)
    {
        ss << (char)(i+65)<<"["<<orchard[i].count(1)<<"]";
        getline(ss,line);
        wordsPerLetter.push_back(line);
        ss.clear();
    }

    display(cout, wordNum, parNum, readingLevel, wordsPerLetter, duration);
    if (toSave(cin, fout))
        display(fout, wordNum, parNum, readingLevel, wordsPerLetter, duration);
    fin.close();
}

string getReadingLevel(double readingScore)
{
    if (readingScore > -1 && readingScore < 31)
        return "College graduate";
    else if (readingScore < 51)
        return "College";
    else if (readingScore < 61)
        return "10th to 12th grade";
    else if (readingScore < 71)
        return "8th to 9th grade";
    else if (readingScore < 81)
        return "7th grade";
    else if (readingScore < 91)
        return "6th grade";
    else if (readingScore < 101 && readingScore > 90)
        return "5th grade";
    else
        return "Reading level information unavailable";
}

void display(ostream &out, unsigned int wordNum, unsigned int parNum, string readingLevel,
        const vector<string>& wordsPerLetter, double duration)
{
    out << "SUMMARY\n"
        << "Word count: " << wordNum << endl
        << "Paragraph count: " << parNum << endl
        << "Reading level: " << readingLevel << endl
        << "\nWords per letter:" << endl;
    for (int i=0; i<26; i++)
    {
        out << setw(10) << right << wordsPerLetter[i];
        if (!(i%6))
            out << endl;
    }
    out << "\nTotal time process: " << duration << " ms" << endl << endl;
}

bool toSave(istream& in, ofstream& fout)
{
    string ans;
    cout << "Would you like to save? ";
    getline(in, ans);
    if (toupper(ans[0]) != 'Y')
        return false;
    cout << "Enter filename to save to or overwrite: ";
    getline(in, ans);
    fout.open(ans.c_str());
    while (fout.fail())
    {
        cout << "Unable to open file!\nEnter filename to save to or overwrite: ";
        fout.open(ans.c_str());
    }
    return true;
}

void checkNewParOrLine(const string& line, unsigned int& parNum, unsigned int& lineNum)
{
    if(line == "")
        ++parNum;
    else
        ++lineNum;
}

unsigned int countSyllables(string word)
{
    int pos = word.find_first_of("aeiouy"), prevPos = -1, count = 0;
    while (pos != string::npos)
    {
        if (prevPos != pos)
        {
            if (pos == word.length()-1 && word[pos] == 'e' && word[pos-1] != 'l')
                break;
            ++count;
        }
        prevPos = pos;
        word.erase(pos, 1);
        pos = word.find_first_of("aeiouy");
    }
    return count;
}

void setMap(map<string,string>& EnglishContractions)
{
    EnglishContractions.insert(std::pair<string, string> ("ain't", "am not"));
    EnglishContractions.insert(std::pair<string, string> ("aren't", "are not"));
    EnglishContractions.insert(std::pair<string, string> ("can't", "can not"));
    EnglishContractions.insert(std::pair<string, string> ("could've", "could have"));
    EnglishContractions.insert(std::pair<string, string> ("couldn't", "could not"));
    EnglishContractions.insert(std::pair<string, string> ("couldn't've", "could not have"));
    EnglishContractions.insert(std::pair<string, string> ("didn't", "did not"));
    EnglishContractions.insert(std::pair<string, string> ("doesn't", "does not"));
    EnglishContractions.insert(std::pair<string, string> ("don't", "do not"));
    EnglishContractions.insert(std::pair<string, string> ("gonna", "going to"));
    EnglishContractions.insert(std::pair<string, string> ("hadn't", "had not"));
    EnglishContractions.insert(std::pair<string, string> ("hadn't've", "had not have"));
    EnglishContractions.insert(std::pair<string, string> ("hasn't", "has not"));
    EnglishContractions.insert(std::pair<string, string> ("haven't", "have not"));
    EnglishContractions.insert(std::pair<string, string> ("he'd", "he would"));
    EnglishContractions.insert(std::pair<string, string> ("he'd've", "he would have"));
    EnglishContractions.insert(std::pair<string, string> ("he's", "he is"));
    EnglishContractions.insert(std::pair<string, string> ("he'sn't", "he is not"));
    EnglishContractions.insert(std::pair<string, string> ("how'd", "how did"));
    EnglishContractions.insert(std::pair<string, string> ("how'll", "how will"));
    EnglishContractions.insert(std::pair<string, string> ("how's", "how is"));
    EnglishContractions.insert(std::pair<string, string> ("i'd", "i would"));
    EnglishContractions.insert(std::pair<string, string> ("i'd've", "i would have"));
    EnglishContractions.insert(std::pair<string, string> ("i'll", "i will"));
    EnglishContractions.insert(std::pair<string, string> ("i'm", "i am"));
    EnglishContractions.insert(std::pair<string, string> ("i've", "i have"));
    EnglishContractions.insert(std::pair<string, string> ("i'ven't", "i have not"));
    EnglishContractions.insert(std::pair<string, string> ("isn't", "is not"));
    EnglishContractions.insert(std::pair<string, string> ("it'd", "it would"));
    EnglishContractions.insert(std::pair<string, string> ("it'd've", "it would have"));
    EnglishContractions.insert(std::pair<string, string> ("it'll", "it will"));
    EnglishContractions.insert(std::pair<string, string> ("it's", "it is"));
    EnglishContractions.insert(std::pair<string, string> ("it'sn't", "it is not"));
    EnglishContractions.insert(std::pair<string, string> ("let's", "let us"));
    EnglishContractions.insert(std::pair<string, string> ("ma'am", "madam"));
    EnglishContractions.insert(std::pair<string, string> ("mightn't", "might not"));
    EnglishContractions.insert(std::pair<string, string> ("mightn't've", "might not have"));
    EnglishContractions.insert(std::pair<string, string> ("might've", "might have"));
    EnglishContractions.insert(std::pair<string, string> ("mustn't", "must not"));
    EnglishContractions.insert(std::pair<string, string> ("must've", "must have"));
    EnglishContractions.insert(std::pair<string, string> ("needn't", "need not"));
    EnglishContractions.insert(std::pair<string, string> ("not've", "not have"));
    EnglishContractions.insert(std::pair<string, string> ("o'clock", "of the clock"));
    EnglishContractions.insert(std::pair<string, string> ("oughtn't", "ought not"));
    EnglishContractions.insert(std::pair<string, string> ("shan't", "shall not"));
    EnglishContractions.insert(std::pair<string, string> ("she'd", "she would"));
    EnglishContractions.insert(std::pair<string, string> ("she'd've", "she would have"));
    EnglishContractions.insert(std::pair<string, string> ("she'll", "she is"));
    EnglishContractions.insert(std::pair<string, string> ("she's", "am not"));
    EnglishContractions.insert(std::pair<string, string> ("she'sn't", "she is not"));
    EnglishContractions.insert(std::pair<string, string> ("should've", "should have"));
    EnglishContractions.insert(std::pair<string, string> ("shouldn't", "should not"));
    EnglishContractions.insert(std::pair<string, string> ("shouldn't've", "should not have"));
    EnglishContractions.insert(std::pair<string, string> ("somebody'd", "somebody would"));
    EnglishContractions.insert(std::pair<string, string> ("somebody'd've", "somebody would have"));
    EnglishContractions.insert(std::pair<string, string> ("somebody'dn't've", "somebody would not have"));
    EnglishContractions.insert(std::pair<string, string> ("somebody'll", "somebody will"));
    EnglishContractions.insert(std::pair<string, string> ("somebody's", "somebody is"));
    EnglishContractions.insert(std::pair<string, string> ("someone'd", "someone would"));
    EnglishContractions.insert(std::pair<string, string> ("someone'd've", "someone would have"));
    EnglishContractions.insert(std::pair<string, string> ("someone'll", "someone will"));
    EnglishContractions.insert(std::pair<string, string> ("someone's", "someone is"));
    EnglishContractions.insert(std::pair<string, string> ("something'd", "something would"));
    EnglishContractions.insert(std::pair<string, string> ("something'd've", "something would have"));
    EnglishContractions.insert(std::pair<string, string> ("something'll", "something will"));
    EnglishContractions.insert(std::pair<string, string> ("something's", "something is"));
    EnglishContractions.insert(std::pair<string, string> ("that'll", "that will"));
    EnglishContractions.insert(std::pair<string, string> ("that's", "that is"));
    EnglishContractions.insert(std::pair<string, string> ("there'd", "there would"));
    EnglishContractions.insert(std::pair<string, string> ("there'd've", "there would have"));
    EnglishContractions.insert(std::pair<string, string> ("there're", "there are"));
    EnglishContractions.insert(std::pair<string, string> ("there's", "there is"));
    EnglishContractions.insert(std::pair<string, string> ("they'd", "they would"));
    EnglishContractions.insert(std::pair<string, string> ("they'dn't", "they would not"));
    EnglishContractions.insert(std::pair<string, string> ("they'dn't've", "they would not have"));
    EnglishContractions.insert(std::pair<string, string> ("they'd've", "they would have"));
    EnglishContractions.insert(std::pair<string, string> ("they'd'ven't", "they would have not"));
    EnglishContractions.insert(std::pair<string, string> ("they'll", "they will"));
    EnglishContractions.insert(std::pair<string, string> ("they'lln't've", "they will not have"));
    EnglishContractions.insert(std::pair<string, string> ("they'll'ven't", "they will have not"));
    EnglishContractions.insert(std::pair<string, string> ("they're", "they are"));
    EnglishContractions.insert(std::pair<string, string> ("they've", "they have"));
    EnglishContractions.insert(std::pair<string, string> ("they'ven't", "they have not"));
    EnglishContractions.insert(std::pair<string, string> ("'tis", "it is"));
    EnglishContractions.insert(std::pair<string, string> ("'twas", "it was"));
    EnglishContractions.insert(std::pair<string, string> ("wanna", "want to"));
    EnglishContractions.insert(std::pair<string, string> ("wasn't", "was not"));
    EnglishContractions.insert(std::pair<string, string> ("we'd", "we would"));
    EnglishContractions.insert(std::pair<string, string> ("we'd've", "we would have"));
    EnglishContractions.insert(std::pair<string, string> ("we'dn't've", "we would not have"));
    EnglishContractions.insert(std::pair<string, string> ("we'll", "we will"));
    EnglishContractions.insert(std::pair<string, string> ("we'lln't've", "we will not have"));
    EnglishContractions.insert(std::pair<string, string> ("we're", "we are"));
    EnglishContractions.insert(std::pair<string, string> ("we've", "we have"));
    EnglishContractions.insert(std::pair<string, string> ("weren't", "were not"));
    EnglishContractions.insert(std::pair<string, string> ("what'll", "what will"));
    EnglishContractions.insert(std::pair<string, string> ("what're", "what are"));
    EnglishContractions.insert(std::pair<string, string> ("what's", "what is"));
    EnglishContractions.insert(std::pair<string, string> ("what've", "what have"));
    EnglishContractions.insert(std::pair<string, string> ("when's", "when is"));
    EnglishContractions.insert(std::pair<string, string> ("where'd", "where did"));
    EnglishContractions.insert(std::pair<string, string> ("where's", "where is"));
    EnglishContractions.insert(std::pair<string, string> ("where've", "where have"));
    EnglishContractions.insert(std::pair<string, string> ("who'd", "who would"));
    EnglishContractions.insert(std::pair<string, string> ("who'd've", "who would have"));
    EnglishContractions.insert(std::pair<string, string> ("who'll", "who will"));
    EnglishContractions.insert(std::pair<string, string> ("who're", "who are"));
    EnglishContractions.insert(std::pair<string, string> ("who's", "who is"));
    EnglishContractions.insert(std::pair<string, string> ("who've", "who have"));
    EnglishContractions.insert(std::pair<string, string> ("why'll", "why will"));
    EnglishContractions.insert(std::pair<string, string> ("why're", "why are"));
    EnglishContractions.insert(std::pair<string, string> ("why's", "why is"));
    EnglishContractions.insert(std::pair<string, string> ("won't", "will not"));
    EnglishContractions.insert(std::pair<string, string> ("won't've", "will not have"));
    EnglishContractions.insert(std::pair<string, string> ("would've", "would have"));
    EnglishContractions.insert(std::pair<string, string> ("wouldn't", "would not"));
    EnglishContractions.insert(std::pair<string, string> ("wouldn't've", "would not have"));
    EnglishContractions.insert(std::pair<string, string> ("y'all", "you all"));
    EnglishContractions.insert(std::pair<string, string> ("you'd", "you would"));
    EnglishContractions.insert(std::pair<string, string> ("you'd've", "you would have"));
    EnglishContractions.insert(std::pair<string, string> ("you'll", "you will"));
    EnglishContractions.insert(std::pair<string, string> ("you're", "you are"));
    EnglishContractions.insert(std::pair<string, string> ("you'ren't", "you are not"));
    EnglishContractions.insert(std::pair<string, string> ("you've", "you have"));
    EnglishContractions.insert(std::pair<string, string> ("you'ven't", "you have not"));
}
