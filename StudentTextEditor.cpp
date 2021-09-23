#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <list>

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) {
	atCol = 0;
	atRow = 0;
	lineNums = 1;
	//undoForDelete = "";
	m_list.push_back("");	//a dummy line
	m_it = m_list.begin();
	inorder = true;
	isundo = false;
}

StudentTextEditor::~StudentTextEditor(){

	m_list.clear();
		

}

bool StudentTextEditor::load(std::string file) {

	string line;
	ifstream myfile(file);
	
	if (myfile.is_open())
	{

		reset();
		while (getline(myfile, line))
		{
			if (!line.empty()&&line.back() == '\r')	//get rid of \r
				line.pop_back();
			m_list.push_back(line);
			if (m_it == m_list.begin())
				m_it++;
			lineNums++;
		}
		myfile.close();
	}

	else {
		cerr << "Error: Cannot open" << file << "!" << endl;
		return false;
	}

	atCol = 0;
	atRow = 0;

	return true;

	
}

bool StudentTextEditor::save(std::string file) {

	ofstream outfile(file);   // outfile is a name of our choosing.
	if (!outfile)		   // Did the creation fail?
	{
		cerr << "Error: Cannot create"<<file << endl;
		return false;
	}
	auto it = m_list.begin();
	it++;	//ignore the first dummy line
	for (; it != m_list.end(); it++) {
		outfile << *it+'\n';
	}
	return true;
}

void StudentTextEditor::reset() {
	while (!m_list.empty())
		m_list.pop_back();
	m_list.push_back("");//dummy line
	m_it = m_list.begin();//reset iterator
	lineNums = 1;	//reset the line numbers
	atCol = 0;
	atRow = 0;
	inorder = true;
	isundo = false;
}

void StudentTextEditor::move(Dir dir) {
	list<string>::iterator tempit = m_it;
	switch (dir) {
	case UP:
		if (m_it != m_list.begin())		//make sure the temp iterator is not out of range
			tempit--;
		if (tempit ==m_list.begin()) {	//at the first line 
			return;						//does nothing
		}
		else if (atCol > (*tempit).length()) {
			atCol=(*tempit).length();	//move cursor to the end of the previous line
			m_it--;
			atRow--;
		}
		else {
			m_it--;	//atCol doesn't change position
			atRow--;
		}
		break;
	case DOWN:
		if (m_it != m_list.end())		//make sure the temp iterator is not out of range
			tempit++;
		else
			return;

		if (tempit == m_list.end())		//cursor on the last line
			return;
		
		if (atCol > (*tempit).length()) {
			atCol = (*tempit).length();	//move cursor to the end of the previous line
			m_it++;
			atRow++;
		}
		else {
			m_it++;
			atRow++;
		}
		break;
	case LEFT:
		if (m_it != m_list.begin())		//make sure the temp iterator is not out of range
			tempit--;
		if (tempit == m_list.begin()) {	//at the first line 
			if (atCol == 0)				//cursor at (0,0) does nothing
				return;
		}

		if (atCol == 0) {
			m_it--;
			atRow--;
			atCol = (*m_it).length();	//move to the last col of the consecutive previous line
		}
		else
			atCol--;
		break;
	case RIGHT:
		if (m_it != m_list.end())		//make sure the temp iterator is not out of range
			tempit++;
		else
			return;
		if (tempit == m_list.end()) {		//cursor on the last line
			if (atCol == (*m_it).length())	//cursor at the position after the last char
				return;
		}
		
		if (atCol == (*m_it).length()) {	//cursor at the end
			atCol = 0;
			m_it++;
			atRow++;						//move to next row
		}
		else
			atCol++;
		break;
	case HOME:
		atCol = 0;
		break;
	case END:
		atCol = (*m_it).length();
		break;
	}
}

void StudentTextEditor::del() {
	list<string>::iterator tempit = m_it;
	tempit++;
	//at the end do nothing
	if (tempit == m_list.end()) {	//it is on the last line
		if (atCol == (*m_it).length()) {//on the last line and the pos after the last char
			return;
		}
	}
	//merge line
	if (atCol == (*m_it).length()) {//on the pos after the last char
		string subback = *tempit;

		if (!isundo) {
			getUndo()->submit(Undo::Action::JOIN, atRow, atCol, '\n');		//char maybe todo
			m_undoItems.push_back(undochars('\n', true));
			isundo = false;
		}

		//undoForDelete += '\n';


		(*m_it) += subback;
		m_list.erase(tempit);		//maybe need to reset the m_it or maybe not
	}
	//delete in the middle
	else {
		string subfront, subback;
		partitionString(subfront, subback);
		if (!isundo) {
			getUndo()->submit(Undo::Action::DELETE, atRow, atCol, subback.front());
			m_undoItems.push_back(undochars(subback.front()));
			isundo = false;
		}
		inorder = false;
		(*m_it) = subfront + subback.substr(1);	//erase the char at the editing position
	}
}

void StudentTextEditor::backspace() {
	list<string>::iterator tempit = m_it;
	tempit--;
	//at the end do nothing
	if (tempit == m_list.begin()) {	//it is on the first line
		if (atCol == 0) {			//at (0,0)
			return;
		}
	}
	//merge line
	if (atCol == 0) {			//at the first pos of this line
		string subback = *m_it;
		atCol = (*tempit).length();	//set cursor to the end of previous line
		if (!isundo) {
			getUndo()->submit(Undo::Action::JOIN, atRow, atCol, '\n');		//char maybe todo
			m_undoItems.push_back(undochars('\n', true));
			isundo = false;
		}

		(*tempit) += subback;
		m_it=m_list.erase(m_it);	//iterator points to the next line		
		m_it--;						//set iterator points to the correct line
		atRow--;
		
	}
	//delete in the middle
	else if (atCol<(*m_it).length()){
		string subfront, subback;
		partitionString(subfront, subback);
		if (!isundo) {
			getUndo()->submit(Undo::Action::DELETE, atRow, atCol, subfront.back());		//submit back
			m_undoItems.push_back(undochars(subfront.back()));
			isundo = false;
		}
		
		inorder = true;
		subfront.pop_back();
		(*m_it) = subfront + subback;
		atCol--;
		

		
		//undoForDelete += subfront.back();
		
	}
	//delete the end
	else {
		atCol--;
		if (!isundo) {
			getUndo()->submit(Undo::Action::DELETE, atRow, atCol, (*m_it).back());
			m_undoItems.push_back(undochars((*m_it).back()));
			isundo = false;
		}
		//undoForDelete += (*m_it).back();
		inorder = true;
		(*m_it).pop_back();
	}
}

void StudentTextEditor::insert(char ch) {
	if (m_it == m_list.begin()) {							//the list is empty with only dummy line
		string temp = "";
		temp += ch;
		m_list.push_back(temp);
		m_it++;												//only increments when the list has only dummy line
		atCol++;
		if (!isundo) {
			getUndo()->submit(Undo::Action::INSERT, atRow, atCol, ch);
			isundo = false;
		}
		return;
	}

	if (atCol == (*m_it).length()){	//cursor is at the end of the line or a line is empty 
	
		(*m_it) += ch;
		atCol++;
		//todo link with undo
	}
	else{
		string subfront, subback;
		partitionString(subfront, subback);
		(*m_it) = subfront + ch + subback;					
		atCol++;
		//todo link with undo
	}
	if (!isundo) {
		getUndo()->submit(Undo::Action::INSERT, atRow, atCol, ch);
		isundo = false;
	}
}

void StudentTextEditor::enter() {
	list<string>::iterator tempit = m_it;
	
	/*if (m_it != m_list.end()) 
		tempit++;
	if (tempit == m_list.end()) {
		m_list.push_back("");
		m_it++;
		atRow++;
		atCol = 0;
		lineNums++;
	}*/



	if (tempit == m_list.end()){	//there are no more lines underneath
		if (!isundo) {
			getUndo()->submit(Undo::Action::SPLIT, atRow, atCol, '\n');		//char maybe todo
			m_undoItems.push_back(undochars('\n', false, true));
			isundo = false;
		}
		//if (m_list.size() == 1) {
		//	m_list.push_back("\n");
		//	m_list.push_back("");	//start edit with an empty line
		//	advance(m_it, 2);		//move to that empty line

		//}
		//else {
		//	m_list.push_back("\n");
		//	m_it++;
		//}

		m_list.push_back("");
		m_it++;
		lineNums++;	//increment line number
		atRow++;	
		atCol = 0;	//move to starting position
	}
	else if (m_it != m_list.begin()) {						//seperates paragraph from the middle
		string subfront, subback;
		partitionString(subfront, subback);
		if (!isundo) {
			getUndo()->submit(Undo::Action::SPLIT, atRow, atCol, '\n');		//char maybe todo
			m_undoItems.push_back(undochars('\n', false, true));
			isundo = false;
		}

		(*m_it) = subback;
		m_list.insert(m_it, subfront);	//m_it still points to the subback line	| this insertion inserts a line of subfront before subback
		atRow++;
		atCol = 0;	//move to starting position
	}
}

void StudentTextEditor::getPos(int& row, int& col) const {
	row = atRow;
	col = atCol;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	if (startRow < 0 || numRows<0 || startRow>lineNums)
		return -1;

	//list<string>::const_iterator tempit = m_list.begin();
	//tempit++;		//ignore the dummy line
	//int addedLines = 0;
	//int i = 0;

	//if (startRow < lineNums)
	//	advance(tempit, startRow);
	//else {
	//	return 0;
	//}
	////list<string>::const_iterator checkit= tempit;
	////checkit++;
	//lines.resize(numRows);
	//

	//for (; tempit != m_list.end() && i != numRows; i++, tempit++) {
	//	lines[i] = *tempit;
	//	addedLines++;
	//}
	//
	//
	//if (i < numRows) {	//not filled up|   maybe i+1<numRows
	//	for (; i != numRows; i++)	//the index maybe incorrect
	//		lines[i].erase();
	//}
	//
	//return addedLines;


	//version at least work
	int addedlines = 0;
	lines.clear();
	list<string>::const_iterator tempit = m_list.begin();

	tempit++;		//ignore the dummy line
	if (tempit != m_list.end()) {
		if (lineNums > startRow)
			advance(tempit, startRow);
		else
			return 0;
	}
	else
		return -1;	//list is empty
	

	for (int i=0; tempit != m_list.end() && i != numRows; i++, tempit++) {
		lines.push_back(*tempit);
		addedlines++;
	}
	return addedlines;

}

void StudentTextEditor::undo() {
	int row, col, count;
	string text;
	isundo = true;
	getUndo()->get(row, col, count, text);

	list<undochars>::iterator temp;
	if (!m_undoItems.empty()) {
		temp = m_undoItems.end();	//work like a stack
		temp--;
	}
	else if (m_undoItems.empty()) {
		if (text.empty()) {
			isundo = false;
			return;
		}
			
	}
	
	if (text==""&& !(*temp).m_isJoin&& !(*temp).m_isSplit) {	//BACKSPACE OR DELETE
		int num;
		if (inorder) {	//BACKSPACE
			num = 0;
			int temp = atCol;	//reset cursor to the previous position
			//if (atCol!=0)
			//	atCol = col-1;
			for (auto it = m_undoItems.crbegin(); it != m_undoItems.crend() && num < count;  num++) {
				insert((*it).content);
				//it++;
				m_undoItems.pop_back();
				it = m_undoItems.crbegin();
			}
			atCol = temp;		//reset cursor to the previous position
			
		}
		else if (!inorder) { 	//DELETE
			num = count;
			//atCol = col;
			int placeholder = atCol;
			for (; temp != m_undoItems.begin() && num > 0; temp--, num--);
			
			for (; temp != m_undoItems.end() && num < count+1; num++) {
				insert((*temp).content);
				temp = m_undoItems.erase(temp);
			}
			atCol = placeholder;	//remember to move cursor back
		}
	}
	else if (!text.empty()) {	//INSERT
		int length = text.length();
		atCol = col;
		
		for (int i = 0; i != length; i++) {
			backspace();
		}
	}
	else if ((*temp).m_isJoin) {
		int temp = atCol;
		enter();
		atRow--;		//maybe cause an error
		atCol = temp;
		m_undoItems.pop_back();
	}
	else if ((*temp).m_isSplit) {
		backspace();
		m_undoItems.pop_back();	
	}
	isundo = false;
}
