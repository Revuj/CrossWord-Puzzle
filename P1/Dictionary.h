#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

class Dictionary
{
public:
	Dictionary(string &file_name);
	bool valid_word(string &word) const;
	void help(vector<string>& match_words, string& wildcard, size_t max_size);
	void chooseclues(map<string, vector<string>>& words);
	string help_solve(string& word);
private:
	map <string, vector<string>> wordsMap;
	ifstream dictionaryFile;
};

#endif
