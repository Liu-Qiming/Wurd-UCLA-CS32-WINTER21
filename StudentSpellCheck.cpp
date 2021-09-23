#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::~StudentSpellCheck() {
	delete root;
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
	string line;
	ifstream myfile(dictionaryFile);
	auto currentNode = root;
	if (myfile.is_open())
	{
		//int i = 0;
		//node* temp = root;
		while (getline(myfile, line))
		{	
			insert(currentNode, line);
			currentNode = root;	
		}
		myfile.close();
	}

	else {
		cerr << "Error: Cannot open" << dictionaryFile << "!" << endl;
		return false;
	}
	return true;
}

bool StudentSpellCheck::search(node* root, const string& key) {

	node* temp = root;

	for (int i = 0; i < key.length(); i++)
	{
		int num = convertCharToNum(key[i]);
		if (!temp->children[num])
			return false;

		temp = temp->children[num];
	}
	//now we reached to the end
	if (temp) {
		if (temp->isEndOfAWord)	//this is the end of a word
			return true;
	}
	return false;
}

void StudentSpellCheck::insert(node* parent, string key) {
	node* temp = parent;
	for (int i = 0; i != key.size(); i++) {

		int num = convertCharToNum(key[i]);

		if (!temp->children[num]) {
			temp->children[num] = createNewNode();
			temp->children[num]->val = key[i];
		}
		temp = temp->children[num];
	}
	temp->isEndOfAWord = true;	//remember to mark the end of word

}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	suggestions.clear();	//clear vector first
	
	auto temp = root;
	if (search(temp, word))	//search to see if the word is good
		return true;
	else {
		auto curr = root;
		for (int i = 0; i < 27; i++) {
			
			string line;
			int maxDiff = 0;
			giveSuggestions(suggestions, curr->children[i], word, line, word.length(), maxDiff, max_suggestions);	//recursively give suggestions
		}
	}
	return false;
}

void StudentSpellCheck::giveSuggestions(std::vector<std::string>& suggestions, node* curr, string key, string& temp, int lengthOfKey, int maxDiff, int max_suggestions) {
	if (!curr)	//reach the end
		return;
	if (lengthOfKey == 0)	//reach the end of the word
		return;

	if (curr->val != key[0]) {
		maxDiff++;		//diffrent char, only allow once
		temp += curr->val;	//add char into temp
		lengthOfKey--;		//decrement the length of key to keep track
		for (int i = 0; i != 27; i++) {
			if (maxDiff > 1) {	//more than one char diffrent, invalid
				break;
			}
			if (lengthOfKey == 0)	//run out of length
			{

				break;
			}
			if (curr->children[i]) {
				giveSuggestions(suggestions, curr->children[i], key.substr(1), temp, lengthOfKey, maxDiff, max_suggestions);//move to its children with a shrinked key
			}
		}
	}
	else {
		temp += curr->val;	//add char into temp
		lengthOfKey--;		//decrement length
		for (int i = 0; i != 27; i++) {
			if (maxDiff > 1) {	//more than one diffrent char
				break;
			}
			if (lengthOfKey == 0)
			{
				break;
			}
			if (curr->children[i]) {
				giveSuggestions(suggestions, curr->children[i], key.substr(1), temp, lengthOfKey, maxDiff, max_suggestions);
			}
		}
	}
	if (curr->isEndOfAWord && maxDiff <= 1 && lengthOfKey == 0) {//it is end of word with at most one diffrent, and we reached the end
		if (suggestions.size() < max_suggestions)
			suggestions.push_back(temp);
	}

	temp.pop_back();	//remove bad
}

void StudentSpellCheck::partitionWords(const std::string line, std::vector<Position>& listOfAddresses) {
	int length = 0;
	int i = 0;
	for (; i != line.length(); i++) {
		if (isalpha(line[i])  || line[i] == '\'') {
			length++;
			
		}
		else {
			pushWords(i - length, i - 1, listOfAddresses);
			length = 0;
		}
		
	}
	if ((isalpha(line[i-1]) || line[i-1] == '\'')) {	//for the cases such as "hi ihi" etc.
		pushWords(i-length, i-1, listOfAddresses);
		length = 0;
	}
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<Position>& problems) {
	problems.clear();	//clean up
	vector<Position> listOfAdd;
	partitionWords(line, listOfAdd);
	for (int i = 0; i != listOfAdd.size(); i++) {
		int startpos = listOfAdd[i].start;
		int length = listOfAdd[i].end - listOfAdd[i].start + 1;
		string word = line.substr(startpos, length);
		if (!search(root, word))	//it is not a correct word
			problems.push_back(listOfAdd[i]);
	}
}



