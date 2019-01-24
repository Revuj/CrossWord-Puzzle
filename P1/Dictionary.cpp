#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include "Dictionary.h"

//==============================================================================================//
//==============================================================================================//

/*
Tests if a line only has valid characters
@param s - the line (string)
@function return value: True if the string does not have any invalid characters
*/
bool validString(const string &s)
{
	bool flag;

	for (size_t i = 0; i < s.length(); i++)
		if ((s.at(i) >= 'A' && s.at(i) <= 'Z') || (s.at(i) >= 'a' && s.at(i) <= 'z') || s.at(i) == ' ' || s.at(i) == ',' || s.at(i) == ':') // valid characters 
			flag = true;
		else
		{
			flag = false;
			break;
		}
	return flag;
}

/*
Tests if one string matches a wildcard string
@param str - string to test
@param strWild - string that determines the match between the word
@function return value: a string with the synonym
*/
bool wildcardMatch(const char *str, const char *strWild)
{
	while (*strWild)
	{
		if (*strWild == '?')
		{
			if (!*str)
				return false;
			++str;
			++strWild;
		}
		else if (*strWild == '*')
		{
			if (wildcardMatch(str, strWild + 1))
				return true;
			if (*str && wildcardMatch(str + 1, strWild))
				return true;
			return false;
		}
		else
		{
			if (toupper(*str++) != toupper(*strWild++))
				return false;
		}
	}
	return !*str && !*strWild;
}

//==============================================================================================//
//==============================================================================================//

/*
Seperates and loads the words that we consider valid on the dictionary
@param file_name - the name of the file that contains the dictionary
@constructor
*/
Dictionary::Dictionary(string &file_name)
{
	dictionaryFile.open(file_name);

	string s;

	while (getline(dictionaryFile, s))
	{
		if (s.length() != 0 && validString(s))
		{
			vector<string> synonyms;
			string::iterator pos;
			string word;

			// reads the word and synonyms and put then in a vector
			for (string::iterator it = s.begin(); it != s.end(); ++it)
			{
				if (*it == ':')
				{
					string word(s.begin(), it);
					transform(word.begin(), word.end(), word.begin(), toupper);
					if (word.find(" ") == string::npos)
						synonyms.push_back(word);
					pos = it;
				}
				else if (*it == ',')
				{
					string synon(pos + 2, it);
					transform(synon.begin(), synon.end(), synon.begin(), toupper);
					if (synon.find(" ") == string::npos)
						synonyms.push_back(synon);
					pos = it;
				}
			}

			if (*(s.end() - 1) != ',')
			{
				string synon(pos + 2, s.end());
				transform(synon.begin(), synon.end(), synon.begin(), toupper);
				if (synon.find(" ") == string::npos)
					synonyms.push_back(synon);
			}

			// introduce in wordsmap.first all the words in the vector and wordsmap.second the other words in the vector
			for (size_t i = 0; i < synonyms.size(); i++)
			{
				string word = synonyms[0];
				synonyms.erase(synonyms.begin());
				// if the word doesn´t exist in wordsmap it's introduced
				if (wordsMap.find(word) == wordsMap.end())
					wordsMap[word] = synonyms;
				// if the word exists append the words to the existing synonyms
				else
				{
					wordsMap.at(word).insert(wordsMap.at(word).end(), synonyms.begin(), synonyms.end());
					sort(wordsMap.at(word).begin(), wordsMap.at(word).end());
					wordsMap.at(word).erase(unique(wordsMap.at(word).begin(), wordsMap.at(word).end()), wordsMap.at(word).end());
				}
				synonyms.push_back(word);
			}
		}
	}

	dictionaryFile.close();
}

//==============================================================================================//
//==============================================================================================//

/*
Tests if a word belongs to the dictionary
@param word - word (string) to be tested
@function return value: True if the string belongs to the dictionary
*/
bool Dictionary::valid_word(string &word) const
{
	if (wordsMap.find(word) != wordsMap.end())
		return true;
	return false;
}

//==============================================================================================//
//==============================================================================================//

/*
Displays the words that can be put on a specified position by the user, when the input is "?"
@param match_words - vector where the words that the user can input, will be put
@param wildcard - respective line or column
@param max_size - size of the respective line or column
*/
void Dictionary::help(vector<string>& match_words, string& wildcard, size_t max_size)
{
	// replaces all the '#' and '.' for '?' and then replaces the end with a '*'
	for (size_t l = 0; l < max_size; l++)
		if (wildcard[l] == '#' || wildcard[l] == '.')
			wildcard[l] = '?';

	for (int l = max_size - 1; l >= 0; l--)
	{
		if (wildcard[l] == '?' && wildcard.size() != 1)
			wildcard.pop_back();
		else if (wildcard.size() == 1 && wildcard[0] != '.')
			wildcard.push_back('*');
		else
		{
			wildcard.push_back('*');
			break;
		}
	}

	// tests if the words matches the wildcard
	for (auto& x : wordsMap)
	{
		if (wildcardMatch(x.first.c_str(), wildcard.c_str()) && x.first.size() <= max_size)
		{
			match_words.push_back(x.first);
		}
	}
}

//==============================================================================================//
//==============================================================================================//

/*
Gets a new synonym to appear in the list of synonyms, when the user is solving the board
@param word - string whose another synonym the user wants
@function return value: a string with the synonym
*/
string Dictionary::help_solve(string& word)
{
	// choose another clue 
	if (wordsMap.at(word).size() != 0)
	{
		size_t randomIndex = rand() % wordsMap.at(word).size();
		string clue = wordsMap.at(word)[randomIndex];
		wordsMap.at(word).erase(remove(wordsMap.at(word).begin(), wordsMap.at(word).end(), clue), wordsMap.at(word).end());
		return clue;
	}

	else
	{
		cout << "No more clues\n";
		return "";
	}
}

//==============================================================================================//
//==============================================================================================//

/*
Gets a synonym to appear in the list of synonyms, before the user starts to solve the board
@param words - map with the position and words that was placed on the board
*/
void Dictionary::chooseclues(map<string, vector<string>>& words)
{
	// choose clue from a map with positions and respective words
	for (auto& x : words)
	{
		if (wordsMap.at(x.second[0]).size() != 0)
		{
			size_t randomIndex = rand() % wordsMap.at(x.second[0]).size();
			string s = wordsMap.at(x.second[0])[randomIndex];
			x.second[0] = s;
			wordsMap.at(x.second[0]).erase(remove(wordsMap.at(x.second[0]).begin(), wordsMap.at(x.second[0]).end(), s), wordsMap.at(x.second[0]).end());
		}
	}
}
