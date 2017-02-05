/**
 * \file GuiSelectionManager.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Richard Heck
 * \author Et Alia
 *
 * Full author contact details are available in file CREDITS.
 */

#ifndef GUISELECTIONMANAGER_H
#define GUISELECTIONMANAGER_H

#include <QObject>

class QAbstractItemModel;
class QAbstractListModel;
class QModelIndex;
class QListView;
class QPushButton;
class QVariant;
class QAbstractItemView;
class QItemSelection;
template <class T, class U> class QMap;

namespace lyx {
namespace frontend {

/** Class to manage a collection of widgets that allows selection
 *  of items from a list of available items. Adapted from code originally
 *  written for GuiCitationDialog. 
 *  Note that this is a not a QWidget, though it could be converted to
 *  one. Rather, the managed widgets---see constructor for descripton 
 *  of them---should be created independently, and then passed to the
 *  constructor.
 */
class GuiSelectionManager : public QObject
{
	Q_OBJECT

public:
	///
	GuiSelectionManager(
		QAbstractItemView * availableLV,
		QAbstractItemView * selectedLV,
		QPushButton * addPB, 
		QPushButton * delPB, 
		QPushButton * upPB, 
		QPushButton * downPB,
		QAbstractListModel * availableModel,
		QAbstractItemModel * selectedModel,
		int const main_sel_col = 0);
	/// Sets the state of the various push buttons, depending upon the
	/// state of the widgets. (E.g., "delete" is enabled only if the
	/// selection is non-empty.)
	/// Note: this is separated out into updateAddPB(), etc, below, 
	/// for easy over-riding of these functions.
	void update();
	
	/// Not strictly a matter of focus, which may be elsewhere, but
	/// whether selectedLV is `more focused' than availableLV. Intended
	/// to be used, for example, in displaying information about a
	/// highlighted item: should it be the highlighted available item
	/// or the highlighted selected item that is displayed?
	bool selectedFocused() const { return selectedHasFocus_; }
	/// Returns the selected index. Note that this will depend upon
	/// selectedFocused().
	QModelIndex getSelectedIndex(int const c = 0) const;

Q_SIGNALS:
	/// Emitted when the list of selected items has changed. 
	void selectionChanged();
	/// Emitted when something has changed that might lead the containing 
	/// dialog to want to update---the focused subwidget or selected item.
	/// (Specifically, it is emitted by *_PB_clicked() and *_LV_clicked.)
	/// NOTE: No automatic update of the button state is done here. If you
	/// just want to do that, connect updateHook() to updateView(). Much of the
	/// time, though, you will want to do a bit more processing first, so
	/// you can connect to some other function that itself calls updateView().
	void updateHook();
	/// Emitted on Ctrl-Enter in the availableLV. Intended to be connected 
	/// to an "OK" event in the parent dialog.
	void okHook();


protected:
	/// Given a QModelIndex from availableLV, determines whether it has
	/// been selected (i.e., is also in selectedLV).
	bool isSelected(const QModelIndex & idx);
	///
	bool insertRowToSelected(int i, QMap<int, QVariant> const & itemData);
	///
	bool insertRowToSelected(int i, QMap<int, QMap<int, QVariant>> &);
	///
	QAbstractItemView * availableLV;
	///
	QAbstractItemView * selectedLV;
	///
	QPushButton * addPB;
	///
	QPushButton * deletePB; 
	///
	QPushButton * upPB;
	///
	QPushButton * downPB;
	///
	QAbstractListModel * availableModel;
	///
	QAbstractItemModel * selectedModel;

protected Q_SLOTS:
	///
	void availableChanged(QModelIndex const & idx, QModelIndex const &);
	///
	void selectedChanged(QModelIndex const & idx, QModelIndex const &);
	///
	void availableChanged(QItemSelection const & qis, QItemSelection const &);
	///
	void selectedChanged(QItemSelection const & qis, QItemSelection const &);
	///
	void selectedEdited();
	///
	virtual void addPB_clicked();
	///
	virtual void deletePB_clicked();
	///
	virtual void upPB_clicked();
	///
	virtual void downPB_clicked();
	///
	void availableLV_doubleClicked(const QModelIndex &);
	///
	bool eventFilter(QObject *, QEvent *);

private:
	///
	virtual void updateAddPB();
	///
	virtual void updateDelPB();
	///
	virtual void updateDownPB();
	///
	virtual void updateUpPB();
	///
	bool selectedHasFocus_;
	///
	int main_sel_col_;
};

} // namespace frontend
} // namespace lyx

#endif  // GUISELECTIONMANAGER
