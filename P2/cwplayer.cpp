#include "Player.h"
#include <ctime>
#include <algorithm>
#include <iomanip>
#include "Board.h"
#include "Dictionary.h"

//==============================================================================================//
//==============================================================================================//
/*
Choose the clues to the respective position
@param words - dictionary associated to the board
@param moves - map with the positions and the respective words of the board
@param horizontal_words - map with the horizontal positions and the respective clues
@param vertical_words - map with the vertical positions and the respective clues
*/

void clues(Dictionary &words, map<string, vector<string>> &horizontal_words, map<string, vector<string>> &vertical_words, const map<string, string>&	moves)
{
	for (auto x : moves)
	{
		// separates the positions
		if (x.first[2] == 'H')
		{
			vector<string> horizontal;
			horizontal.push_back(x.second);
			horizontal_words.insert(pair<string, vector<string>>(x.first, horizontal));
		}
		if (x.first[2] == 'V')
		{
			vector<string> vertical;
			vertical.push_back(x.second);
			vertical_words.insert(pair<string, vector<string>>(x.first, vertical));
		}
	}

	srand(static_cast<unsigned int>(time(NULL)));
	words.chooseclues(horizontal_words);
	words.chooseclues(vertical_words);
}

//==============================================================================================//
//==============================================================================================//

/*
Determines the position where the word will be inserted
@param moves - map with the positions and the respective words of the board
@param line_column_direction - string that contains the position of the word to be placed
*/
void position(const map<string, string> &moves, string &line_column_direction)
{
	while (moves.find(line_column_direction) == moves.end())
	{
		// checks if there is any word in the position introduced
		if (line_column_direction != "")
			cout << "None of the words are in this position\n";
		while (cout << "\nPosition ( LDC / CTRL-Z = stop ) ? " && !(cin >> line_column_direction) && !cin.eof())
		{
			cin.clear();
			cin.ignore(100000, '\n');
			cout << "Invalid input, please re-enter\n";
		}

		while (cin.eof())
		{
			cin.clear();
			char option;
			cout << "You didn't finish solve the puzzle, do you want to leave (Y or N) ? ";
			// if the user introduces CTRL-Z and the grid isn't finished
			while (!(cin >> option) || (option != 'N' && option != 'Y'))
			{
				cin.clear();
				cin.ignore(100000, '\n');
				cout << "Invalid input, please re-enter\n";
				cout << "You didn't finish solve the puzzle, do you want to leave (Y or N) ? ";
			}

			if (option == 'Y')
			{
				cout << "Thanks for playing,bye!\n";
				exit(0);
			}
			else
				while (cout << "\nPosition ( LDC / CTRL-Z = stop ) ? " && !(cin >> line_column_direction) && !cin.eof())
				{
					cin.clear();
					cin.ignore(100000, '\n');
					cout << "Invalid input, please re-enter\n";
				}
		}

		// tests if the user introduces a valid position 
		while (line_column_direction.length() != 3 || (line_column_direction[2] != 'H' && line_column_direction[2] != 'V') || islower(line_column_direction[0]) || isupper(line_column_direction[1]))
		{
			cout << "LCD ( L-uppercase , C-lowercase , D-H or V ) ? ";
			cin >> line_column_direction;
		}
	}
}

//==============================================================================================//
//==============================================================================================//

/*
Depending of the user input, edit_board calls the respective functions that are needed to edit the bord
@param grid - board to solve
@param dic - dictionary associated to the board
@param moves - map with the positions and the respective words of the board
@param word - input word that can be "-", "?" or a word to be placed on the board
@param line_column_direction - string that contains the position of the word to be placed
@param horizontal_words - map with the horizontal positions and the respective clues
@param vertical_words - map with the vertical positions and the respective clues
*/
void edit_board(Board &grid, Dictionary &dic, Player& player, map<string, string> &moves, string &word, string &line_column_direction, map<string, vector<string>> &horizontal_words, map<string, vector<string>> &vertical_words)
{
	char line, column, direction;
	istringstream iss(line_column_direction);
	iss >> line >> column >> direction;

	if (word == "-")
	{
		grid.removeword(line, column, direction);
	}

	else if (word == "?")
	{
		// updates the clues
		player.num_helps++;
		if (line_column_direction[2] == 'H')
		{
			vector<string> horizontal = horizontal_words.at(line_column_direction);
			horizontal.push_back(dic.help_solve(moves.at(line_column_direction)));
			horizontal_words[line_column_direction] = horizontal;
		}

		if (line_column_direction[2] == 'V')
		{
			vector<string> vertical = vertical_words.at(line_column_direction);
			vertical.push_back(dic.help_solve(moves.at(line_column_direction)));
			vertical_words[line_column_direction] = vertical;
		}
	}

	else
	{
		transform(word.begin(), word.end(), word.begin(), toupper);
		if (dic.valid_word(word) && grid.can_be_placed(line, column, direction, word) == 4)
			grid.insertword(line, column, direction, word);
		else if (grid.can_be_placed(line, column, direction, word) == 5)
			cout << "Word already introduced\n";
		else if (grid.can_be_placed(line, column, direction, word) == 1)
			cout << "This word doesn't fit on the board\n";
		else if (grid.can_be_placed(line, column, direction, word) == 2)
			cout << "Invalid interseption of words\n";
		else if (!dic.valid_word(word))
			cout << "Invalid word\n";
	}
}

//==============================================================================================//
//==============================================================================================//

/*
Sends the player time and number of needed clues to be saved on a file
@param board-name - string to find the file name
@param dic - dictionary associated to the board
@param moves - map with the positions and the respective words of the board
@param player - player stats to send to the file
*/
void save_BoardSolved(string &board_name, string &dictionary_name, Board &grid, Player &player, map<string, string> &moves)
{
	string solved_name = board_name.substr(0, 4) + "_p.txt";
	ifstream infile;
	infile.open(solved_name);
	// tests if the file exists
	if (!infile.fail())
	{
		ofstream outfile;
		outfile.open(solved_name, std::ios_base::app);
		player.sendToFile(outfile);
		outfile.close();
	}
	else
	{
		ofstream outfile;
		outfile.open(solved_name);
		player.sendToFile(outfile);
		outfile << endl;
		outfile.close();
	}
}

//==============================================================================================//
//==============================================================================================//

/*
Allows the user to solve a crossword puzzle
@param grid - board to solve
@param words - dictionary associated to the board
@param moves - map with the positions and the respective words of the board
@param player - player stats to send to the file
@param horizontal_words - map with the horizontal positions and the respective clues
@param vertical_words - map with the vertical positions and the respective clues
*/
void solve_board(Board& grid, Dictionary& words, Player &player, map<string, vector<string>> &horizontal_words, map<string, vector<string>> &vertical_words, map<string, string>& moves)
{
	while (!grid.finished())
	{
		grid.showBoard_toSolve();
		cout << "\n----------------------------------------\nClues\n----------------------------------------\n";
		// display the clues divided by ';'
		cout << "Horizontal words:\n";
		for (auto& x : horizontal_words)
		{
			cout << x.first << " - ";
			for (size_t i = 0; i < x.second.size(); i++)
			{
				if (i == x.second.size() - 1)
					cout << x.second[i] << endl;
				else
					cout << x.second[i] << ", ";
			}
		}

		cout << "Vertical words:\n";
		for (auto& x : vertical_words)
		{
			cout << x.first << " - ";
			for (size_t i = 0; i < x.second.size(); i++)
			{
				if (i == x.second.size() - 1)
					cout << x.second[i] << endl;
				else
					cout << x.second[i] << ", ";
			}
		}

		string line_column_direction = "";
		char line, column, direction;

		position(moves, line_column_direction);

		string word = "";
		while (word.size() != moves.at(line_column_direction).size() && word != "?" && word != "-")
		{
			// tests if the word introduced has the same size as the correct word
			if (word != "")
				cout << "Incorrect word size\n";
			while (cout << "Word ( - = remove / ? = help ) ? " && !(cin >> word))
			{
				cin.clear();
				cin.ignore(100000, '\n');
				cout << "Invalid input, please re-enter\n";
			}
		}

		istringstream iss(line_column_direction);
		iss >> line >> column >> direction;
		edit_board(grid, words, player, moves, word, line_column_direction, horizontal_words, vertical_words);
	}
}

//==============================================================================================//
//==============================================================================================//

int main()
{
	cout << "CROSSWORDS PUZZLE PLAYER\n";
	cout << "========================================\n";
	cout << "INSTRUCTIONS:\n";
	cout << "LCD ( L - uppercase , C - lowercase , D- H or V )\n\n";
	int board_number;
	string player_name;
	cout << "----------------------------------------\n";
	cout << "SOLVE PUZZLE\n";
	cout << "----------------------------------------\n";

	cout << "What is your name ? ";
	getline(cin, player_name);
	// reads the player name 
	while (player_name == "")
	{
		cout << "Invalid input, please re-enter\n";
		cout << "What is your name ? ";
		getline(cin, player_name);
	}

	// reads the number board and then opens the respective file
	while (cout << "\nBoard number ? " && (!(cin >> board_number) || board_number < 1 || board_number > 999))
	{
		cin.clear();
		cin.ignore(100000, '\n');
		cout << "Invalid input, please re-enter\n";
	}

	ostringstream ss;
	ss << setw(3) << setfill('0') << board_number;
	string board_name(ss.str());
	board_name = "b" + board_name + ".txt";
	ifstream boardFile;
	boardFile.open(board_name);

	if (boardFile.fail())
	{
		cerr << "Board not found!\n";
		exit(1);
	}

	// read and starts tge dictionary and the board
	string dictionary_name;
	getline(boardFile, dictionary_name);
	cout << "Loading dictionary from " << dictionary_name << " ...\n";
	Dictionary words(dictionary_name);
	map<string, string> moves;
	Board grid(boardFile, moves);
	cout << "Loading board from " << board_name << " ...\n";

	// tests if the board is finished
	if (!grid.finished())
	{
		cout << "The board you selected is not finished!\n";
		return 0;
	}

	map<string, vector<string>> horizontal_words;
	map<string, vector<string>> vertical_words;
	clues(words, horizontal_words, vertical_words, moves);
	grid.boardToSolve();
	Player player(player_name);
	solve_board(grid, words, player, horizontal_words, vertical_words, moves);
	grid.showBoard_toSolve();

	// tests if the user solved the crosswords puzzle
	bool solved;
	for (auto& x : moves)
	{
		char line, column, direction;
		istringstream iss(x.first);
		iss >> line >> column >> direction;
		if (grid.get_word(line, column, direction) != x.second)
		{
			solved = false;
			break;
		}
		else solved = true;
	}

	// if the user doesn't solve the board 
	while (!solved)
	{
		cout << "\nYou didn't solve the board ...\n";
		char s;
		while (cout << "Do you want to try again ( T ) or to exit ( E ) ? " && (!(cin >> s) || (s != 'T' && s != 'E')))
		{
			cin.clear();
			cin.ignore(100000, '\n');
			cout << "Invalid input, please re-enter\n";
		}

		if (s == 'E')
			return 0;
		else
		{
			ifstream board_file;
			board_file.open(board_name);
			Board grid(board_file, moves);
			grid.boardToSolve();
			solve_board(grid, words, player, horizontal_words, vertical_words, moves);
			grid.showBoard_toSolve();

			for (auto& x : moves)
			{
				char line, column, direction;
				istringstream iss(x.first);
				iss >> line >> column >> direction;
				if (grid.get_word(line, column, direction) != x.second)
				{
					solved = false;
					break;
				}
				else solved = true;
			}
		}
	}

	// save the player stats if the crossword is solved
	if (solved)
	{
		cout << "\nCongratulations, you have solved the crossword puzzle\nYour time is registed in b" << setw(3) << setfill('0') << board_number << "_p.txt" << endl;
		save_BoardSolved(board_name, dictionary_name, grid, player, moves);
	}

	return 0;
}

