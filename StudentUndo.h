#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>
#include <string>
using namespace std;
class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
	struct undoItems {

		undoItems(const Action action, int row, int col, int ch)
			:m_action(action), m_row(row), m_col(col), batch("") {}

		string batch;
		int m_row, m_col;
		Action m_action;
	};
	stack<undoItems> m_stack;
	void addChartoBatch(const Action action, int row, int col, char ch);
};

inline
void StudentUndo::addChartoBatch(const Action action, int row, int col, char ch) {
	if (m_stack.empty()) {
		m_stack.push(undoItems(action, row, col, ch));
		m_stack.top().batch += ch;
		return;
	}
	if (m_stack.top().m_action == action && m_stack.top().m_row == row) {
		if (action == INSERT && m_stack.top().m_col + 1 == col) {	//for INSERT on the left not allowed, maybe todo
			m_stack.top().batch += ch;
			m_stack.top().m_col++;
		}
		else if (action == DELETE && (m_stack.top().m_col - 1 == col)) {	//for BACKSPACE
			m_stack.top().batch += ch;
			m_stack.top().m_col--;
		}
		else if (action == DELETE && (m_stack.top().m_col + 1 == col || m_stack.top().m_col == col)) {	//for DELETE
			m_stack.top().batch += ch;
			m_stack.top().m_col++;
		}
		else {
			m_stack.push(undoItems(action, row, col, ch));
			m_stack.top().batch += ch;
		}
	}
	else {
		m_stack.push(undoItems(action, row, col, ch));
		m_stack.top().batch += ch;
	}
}

#endif // STUDENTUNDO_H_
