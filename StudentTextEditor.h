#ifndef STUDENTTEXTEDITOR_H_
#define STUDENTTEXTEDITOR_H_

#include "TextEditor.h"
#include <list>
#include <string>
#include <map>
using namespace std;

//class Undo;

class StudentTextEditor : public TextEditor {
public:

	StudentTextEditor(Undo* undo);
	~StudentTextEditor();
	bool load(std::string file);
	bool save(std::string file);
	void reset();
	void move(Dir dir);
	void del();
	void backspace();
	void insert(char ch);
	void enter();
	void getPos(int& row, int& col) const;
	int getLines(int startRow, int numRows, std::vector<std::string>& lines) const;
	void undo();

private:
	
	//map<int, string> m_map;

	list<string> m_list;
	list<string>::iterator m_it;
	//string undoForDelete;//for DELETE, JOIN, SPLIT
	int atCol;
	int atRow;
	int lineNums;
	bool inorder;
	bool isundo;
	void partitionString(string& subfront, string& subback) {
		subfront = (*m_it).substr(0, atCol);	//partition front
		subback = (*m_it).substr(atCol);		//partition back
	}

	struct undochars {
		undochars(char ch, bool isJoin=false, bool isSplit=false)
		:content(ch), m_isJoin(isJoin), m_isSplit(isSplit){}
		char content;
		bool m_isJoin, m_isSplit;
		
	};
	list<undochars> m_undoItems;
};



#endif // STUDENTTEXTEDITOR_H_
