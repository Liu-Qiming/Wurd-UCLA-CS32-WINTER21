#include "StudentUndo.h"


Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {

	switch (action) {
	case INSERT:
		addChartoBatch(action, row, col, ch);
		break;
	case DELETE:
		addChartoBatch(action, row, col, ch);
		break;
	case JOIN:
		m_stack.push(undoItems(action, row, col, ch));
		break;
	case SPLIT:
		m_stack.push(undoItems(action, row, col, ch));
		break;
	default:
		return;
	}
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) {
	if (m_stack.empty())
		return Action::ERROR;
	switch (m_stack.top().m_action) {
	case DELETE:
		row = m_stack.top().m_row;
		col = m_stack.top().m_col;
		text = "";
		count = m_stack.top().batch.length();
		m_stack.pop();
		return Undo::Action::DELETE;
	case INSERT:
		row = m_stack.top().m_row;
		col = m_stack.top().m_col;
		text = m_stack.top().batch;
		count = 1;
		m_stack.pop();
		return Undo::Action::INSERT;
	case JOIN:
		row = m_stack.top().m_row;
		col = m_stack.top().m_col;
		text = "";
		count = 1;
		m_stack.pop();
		return Undo::Action::JOIN;
	case SPLIT:
		row = m_stack.top().m_row;
		col = m_stack.top().m_col;
		text = "";
		count = 1;
		m_stack.pop();
		return Undo::Action::SPLIT;
	}
}

void StudentUndo::clear() {
	while (!m_stack.empty())
		m_stack.pop();
}
