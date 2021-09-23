#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

using namespace std;

struct Position {
	int start;
	int end; // inclusive
};

class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck() {
		root = createNewNode();
	}
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	
	
	struct node {
		~node() {
			for (auto p : children)
			{
				delete p;
			}
			children.clear();
		}
		vector<node*> children;
		bool isEndOfAWord;
		char val;
	};
	node* root;
	node* createNewNode() {
		node* nNode = new node;
		nNode->isEndOfAWord = false;
		
		for (int i = 0; i != 27; i++) {
			nNode->children.push_back(nullptr);
		}
		return nNode;
	}
	
	void insert(node* parent, string key);		//for load file
	bool search(node* root, const string& key); //quick search whether there is a desired word inside the dictionary
	void partitionWords(const std::string line, std::vector<Position>& listOfAddresses);	//partition lines into words
	void giveSuggestions(std::vector<std::string>& suggestions, node* curr, string key, string& temp, int lengthOfKey, int maxDiff, int max_suggestions); //recusively push suggestions to the vector

	
	int convertCharToNum(char ch) {
		if (ch == '\'') {
			return 26;
		}
		else
			return tolower(ch) - 'a';
	}

	//push word's address to the vector
	void pushWords(int start, int end, std::vector<SpellCheck::Position>& AddressBook) {
		Position* temp = new Position;
		temp->start = start;
		temp->end = end;
		AddressBook.push_back(*temp);
		delete temp;
	}


};

#endif  // STUDENTSPELLCHECK_H_
