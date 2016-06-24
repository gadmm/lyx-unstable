/**
 * \file qt4/GuiToolbar.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Lars Gullik Bjønnes
 * \author John Levon
 * \author Jean-Marc Lasgouttes
 * \author Angus Leeming
 * \author Stefan Schimanski
 * \author Abdelrazak Younes
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "GuiToolbar.h"

#include "Action.h"
#include "GuiApplication.h"
#include "GuiCommandBuffer.h"
#include "GuiView.h"
#include "IconPalette.h"
#include "InsertTableWidget.h"
#include "LayoutBox.h"
#include "qt_helpers.h"
#include "TextClass.h"
#include "Toolbars.h"

#include "Buffer.h"
#include "BufferParams.h"
#include "BufferView.h"
#include "Cursor.h"
#include "FuncRequest.h"
#include "FuncStatus.h"
#include "KeyMap.h"
#include "LyX.h"
#include "LyXRC.h"
#include "Session.h"
#include "Text.h"

#include "support/debug.h"
#include "support/gettext.h"
#include "support/lstrings.h"

#include <list>

#include <QSettings>
#include <QShowEvent>
#include <QString>
#include <QToolBar>
#include <QToolButton>

#include "support/lassert.h"

using namespace std;
using namespace lyx::support;

namespace lyx {
namespace frontend {

GuiToolbar::GuiToolbar(ToolbarInfo const & tbinfo, GuiView & owner)
	: QToolBar(toqstr(tbinfo.gui_name), &owner), visibility_(0),
	  owner_(owner), command_buffer_(0), tbinfo_(tbinfo), filled_(false),
	  restored_(false)
{
	setIconSize(owner.iconSize());
	connect(&owner, SIGNAL(iconSizeChanged(QSize)), this,
		SLOT(setIconSize(QSize)));

	// This is used by QMainWindow::restoreState for proper main window state
	// restauration.
	setObjectName(toqstr(tbinfo.name));
	restoreSession();
}


void GuiToolbar::setVisible(bool visible)
{
	// This is a hack to find out which toolbars have been restored by
	// MainWindow::restoreState and which toolbars should be initialized
	// by us (i.e., new toolbars)
	restored_ = true;
	QToolBar::setVisible(visible);
}


bool GuiToolbar::isRestored() const
{
	return restored_;
}


void GuiToolbar::fill()
{
	if (filled_)
		return;
	ToolbarInfo::item_iterator it = tbinfo_.items.begin();
	ToolbarInfo::item_iterator end = tbinfo_.items.end();
	for (; it != end; ++it)
		add(*it);
	filled_ = true;
}


void GuiToolbar::showEvent(QShowEvent * ev)
{
	fill();
	ev->accept();
}


void GuiToolbar::setVisibility(int visibility)
{
	visibility_ = visibility;
}


Action * GuiToolbar::addItem(ToolbarItem const & item)
{
	QString text = toqstr(item.label_);
	// Get the keys bound to this action, but keep only the
	// first one later
	KeyMap::Bindings bindings = theTopLevelKeymap().findBindings(*item.func_);
	if (!bindings.empty())
		text += " [" + toqstr(bindings.begin()->print(KeySequence::ForGui)) + "]";

	Action * act = new Action(item.func_, getIcon(*item.func_, false), text,
	                          text, this);
	actions_.append(act);
	return act;
}

namespace {

class PaletteButton : public QToolButton
{
private:
	GuiToolbar * bar_;
	ToolbarItem const & tbitem_;
	bool initialized_;
public:
	PaletteButton(GuiToolbar * bar, ToolbarItem const & item)
		: QToolButton(bar), bar_(bar), tbitem_(item), initialized_(false)
	{
		QString const label = qt_(to_ascii(tbitem_.label_));
		setToolTip(label);
		setStatusTip(label);
		setText(label);
		connect(bar_, SIGNAL(iconSizeChanged(QSize)),
			this, SLOT(setIconSize(QSize)));
		setCheckable(true);
		ToolbarInfo const * tbinfo = guiApp->toolbars().info(tbitem_.name_);
		if (tbinfo)
			// use the icon of first action for the toolbar button
			setIcon(getIcon(*tbinfo->items.begin()->func_, true));
	}

	void mousePressEvent(QMouseEvent * e)
	{
		if (initialized_) {
			QToolButton::mousePressEvent(e);
			return;
		}

		initialized_ = true;

		ToolbarInfo const * tbinfo = guiApp->toolbars().info(tbitem_.name_);
		if (!tbinfo) {
			LYXERR0("Unknown toolbar " << tbitem_.name_);
			return;
		}
		IconPalette * panel = new IconPalette(this);
		QString const label = qt_(to_ascii(tbitem_.label_));
		panel->setWindowTitle(label);
		connect(this, SIGNAL(clicked(bool)), panel, SLOT(setVisible(bool)));
		connect(panel, SIGNAL(visible(bool)), this, SLOT(setChecked(bool)));
		ToolbarInfo::item_iterator it = tbinfo->items.begin();
		ToolbarInfo::item_iterator const end = tbinfo->items.end();
		for (; it != end; ++it)
			if (!getStatus(*it->func_).unknown())
				panel->addButton(bar_->addItem(*it));

		QToolButton::mousePressEvent(e);
	}
};

} // namespace


MenuButton::MenuButton(GuiToolbar * bar, ToolbarItem const & item, bool const sticky)
	: QToolButton(bar), bar_(bar), tbitem_(item)
{
	setPopupMode(QToolButton::InstantPopup);
	QString const label = qt_(to_ascii(tbitem_.label_));
	setToolTip(label);
	setStatusTip(label);
	setText(label);
	QString const name = toqstr(tbitem_.name_);
	QStringList imagedirs;
	imagedirs << "images/math/" << "images/";
	for (int i = 0; i < imagedirs.size(); ++i) {
		QString imagedir = imagedirs.at(i);
		FileName const fname = imageLibFileSearch(imagedir, name, "svgz,png",
			theGuiApp()->imageSearchMode());
		if (fname.exists()) {
			setIcon(QIcon(getPixmap(imagedir, name, "svgz,png")));
			break;
		}
	}
	if (sticky)
		connect(this, SIGNAL(triggered(QAction *)),
			this, SLOT(actionTriggered(QAction *)));
	connect(bar, SIGNAL(iconSizeChanged(QSize)),
		this, SLOT(setIconSize(QSize)));
	initialize();
}


void MenuButton::initialize()
{
	QString const label = qt_(to_ascii(tbitem_.label_));
	ButtonMenu * m = new ButtonMenu(label, this);
	m->setWindowTitle(label);
	m->setTearOffEnabled(true);
	connect(bar_, SIGNAL(updated()), m, SLOT(updateParent()));
	connect(bar_, SIGNAL(updated()), this, SLOT(updateTriggered()));
	ToolbarInfo const * tbinfo = guiApp->toolbars().info(tbitem_.name_);
	if (!tbinfo) {
		LYXERR0("Unknown toolbar " << tbitem_.name_);
		return;
	}
	ToolbarInfo::item_iterator it = tbinfo->items.begin();
	ToolbarInfo::item_iterator const end = tbinfo->items.end();
	for (; it != end; ++it)
		if (!getStatus(*it->func_).unknown())
			m->add(bar_->addItem(*it));
	setMenu(m);
}


void MenuButton::actionTriggered(QAction * action)
{
	QToolButton::setDefaultAction(action);
	setPopupMode(QToolButton::DelayedPopup);
}


void MenuButton::updateTriggered()
{
	if (!menu())
		return;

	bool enabled = false;
	QList<QAction *> acts = menu()->actions();
	for (int i = 0; i < acts.size(); ++i)
		if (acts[i]->isEnabled()) {
			enabled = true;
			break;
		}
	// Enable the MenuButton if at least one menu item is enabled
	setEnabled(enabled);
	// If a disabled item is default, switch to InstantPopup
	// (this can happen if a user selects e.g. DVI and then
	// turns non-TeX fonts on)
	if (defaultAction() && !defaultAction()->isEnabled())
		setPopupMode(QToolButton::InstantPopup);
}


class InsetMenuButton::Private
{
	/// noncopyable
	Private(Private const &);
	void operator=(Private const &);
public:
	Private() : inset_(0)
	{}

	/// since Action retains a reference to the
	/// FuncRequest, we need to keep these around
	typedef std::list<FuncRequest> FuncCache;
	///
	DocumentClassConstPtr text_class_;
	///
	InsetText const * inset_;
	///
	FuncCache fcache_;
};


InsetMenuButton::InsetMenuButton(GuiToolbar * bar)
	: QToolButton(bar), bar_(bar), d(new Private())
{
	setPopupMode(QToolButton::InstantPopup);
	QString const label = qt_("Add Custom Inset");
	setToolTip(label);
	setStatusTip(label);
	setText(label);
	connect(bar, SIGNAL(iconSizeChanged(QSize)),
		this, SLOT(setIconSize(QSize)));
	initialize();
}


void InsetMenuButton::initialize()
{
	QString const label = qt_("Add Custom Inset");

	QMenu * m = new QMenu(label, this);
	m->setWindowTitle(label);
	m->setTearOffEnabled(true);
	setMenu(m);

	connect(bar_, SIGNAL(updated()), this, SLOT(updateTriggered()));
	updateTriggered();
}


void InsetMenuButton::updateTriggered()
{
	GuiView const & owner = bar_->owner();
	BufferView const * bv = owner.currentBufferView();
	QMenu * m = menu();
	if (!bv) {
		if (m)
			m->clear();
		setEnabled(false);
		setMinimumWidth(sizeHint().width());
		d->text_class_.reset();
		d->inset_ = 0;
			d->fcache_.clear();
		return;
    }
    
	// we'll only update the inset list if the text class has changed
	// or we've moved from one inset to another
	DocumentClassConstPtr text_class = bv->buffer().params().documentClassPtr();
	InsetText const * inset = &(bv->cursor().innerText()->inset());
	if (d->text_class_ == text_class && d->inset_ == inset)
		return;
    
	if (m)
		m->clear();
	d->inset_ = inset;
	d->text_class_ = text_class;
	// FIXME If this is uncommented, then we crash.
	// Presumably, the Action objects are still active
	// for some reason, even though they have been
	// removed from the menu.
	//d->fcache_.clear();
	
	TextClass::InsetLayouts const & insetLayouts = d->text_class_->insetLayouts();
	TextClass::InsetLayouts::const_iterator iit = insetLayouts.begin();
	TextClass::InsetLayouts::const_iterator ien = insetLayouts.end();
	
	for (; iit != ien; ++iit) {
		InsetLayout const & il = iit->second;
		if (il.lyxtype() != InsetLayout::CUSTOM)
			continue;
        
		docstring const name = iit->first;
		QString const loc_item = toqstr(translateIfPossible(
				prefixIs(name, from_ascii("Flex:")) ? 
				name.substr(5) : name));

		FuncRequest const func(LFUN_FLEX_INSERT, 
			from_ascii("\"") + name + from_ascii("\""), FuncRequest::TOOLBAR);
		d->fcache_.push_back(func);
		FuncRequest const & cfunc = d->fcache_.back();
		Action * act = 
			new Action(getIcon(cfunc, false), loc_item, cfunc, loc_item, this);
		m->addAction(act);
	}
	setEnabled(!d->fcache_.empty());
}


void GuiToolbar::add(ToolbarItem const & item)
{
	switch (item.type_) {
	case ToolbarItem::SEPARATOR:
		addSeparator();
		break;
	case ToolbarItem::LAYOUTS: {
		LayoutBox * layout = owner_.getLayoutDialog();
		QObject::connect(this, SIGNAL(iconSizeChanged(QSize)),
			layout, SLOT(setIconSize(QSize)));
		QAction * action = addWidget(layout);
		action->setVisible(true);
		break;
	}
	case ToolbarItem::INSETS: {
		addWidget(new InsetMenuButton(this));
		break;
	}
	case ToolbarItem::MINIBUFFER:
		command_buffer_ = new GuiCommandBuffer(&owner_);
		addWidget(command_buffer_);
		/// \todo find a Qt4 equivalent to setHorizontalStretchable(true);
		//setHorizontalStretchable(true);
		break;
	case ToolbarItem::TABLEINSERT: {
		QToolButton * tb = new QToolButton;
		tb->setCheckable(true);
		tb->setIcon(getIcon(FuncRequest(LFUN_TABULAR_INSERT), true));
		QString const label = qt_(to_ascii(item.label_));
		tb->setToolTip(label);
		tb->setStatusTip(label);
		tb->setText(label);
		InsertTableWidget * iv = new InsertTableWidget(tb);
		connect(tb, SIGNAL(clicked(bool)), iv, SLOT(show(bool)));
		connect(iv, SIGNAL(visible(bool)), tb, SLOT(setChecked(bool)));
		connect(this, SIGNAL(updated()), iv, SLOT(updateParent()));
		addWidget(tb);
		break;
		}
	case ToolbarItem::ICONPALETTE:
		addWidget(new PaletteButton(this, item));
		break;

	case ToolbarItem::POPUPMENU: {
		addWidget(new MenuButton(this, item, false));
		break;
		}
	case ToolbarItem::STICKYPOPUPMENU: {
		addWidget(new MenuButton(this, item, true));
		break;
		}
	case ToolbarItem::COMMAND: {
		if (!getStatus(*item.func_).unknown())
			addAction(addItem(item));
		break;
		}
	default:
		break;
	}
}


void GuiToolbar::update(int context)
{
	if (visibility_ & Toolbars::AUTO) {
		setVisible(visibility_ & context & Toolbars::ALLOWAUTO);
		if (isVisible() && commandBuffer() && (context & Toolbars::MINIBUFFER_FOCUS))
			commandBuffer()->setFocus();
	}

	// update visible toolbars only
	if (!isVisible())
		return;

	// This is a speed bottleneck because this is called on every keypress
	// and update calls getStatus, which copies the cursor at least two times
	for (int i = 0; i < actions_.size(); ++i)
		actions_[i]->update();

	LayoutBox * layout = owner_.getLayoutDialog();
	if (layout)
		layout->setEnabled(lyx::getStatus(FuncRequest(LFUN_LAYOUT)).enabled());

	// emit signal
	updated();
}


QString GuiToolbar::sessionKey() const
{
	return "views/" + QString::number(owner_.id()) + "/" + objectName();
}


void GuiToolbar::saveSession(QSettings & settings) const
{
	settings.setValue(sessionKey() + "/visibility", visibility_);
	settings.setValue(sessionKey() + "/movability", isMovable());
}


void GuiToolbar::restoreSession()
{
	QSettings settings;
	int const error_val = -1;
	int visibility =
		settings.value(sessionKey() + "/visibility", error_val).toInt();
	if (visibility == error_val || visibility == 0) {
		// This should not happen, but in case we use the defaults
		LYXERR(Debug::GUI, "Session settings could not be found! Defaults are used instead.");
		visibility =
			guiApp->toolbars().defaultVisibility(fromqstr(objectName()));
	}
	setVisibility(visibility);

	int movability = settings.value(sessionKey() + "/movability", true).toBool();
	setMovable(movability);
}


void GuiToolbar::toggle()
{
	docstring state;
	if (visibility_ & Toolbars::ALLOWAUTO) {
		if (!(visibility_ & Toolbars::AUTO)) {
			visibility_ |= Toolbars::AUTO;
			hide();
			state = _("auto");
		} else {
			visibility_ &= ~Toolbars::AUTO;
			if (isVisible()) {
				hide();
				state = _("off");
			} else {
				show();
				state = _("on");
			}
		}
	} else {
		if (isVisible()) {
			hide();
			state = _("off");
		} else {
			show();
			state = _("on");
		}
	}

	owner_.message(bformat(_("Toolbar \"%1$s\" state set to %2$s"),
		qstring_to_ucs4(windowTitle()), state));
}

void GuiToolbar::movable(bool silent)
{
	// toggle movability
	setMovable(!isMovable());

	// manual update avoids bug in qt that the drag handle is not removed
	// properly, e.g. in Windows
	Q_EMIT update();

	// silence for toggling of many toolbars for performance
	if (!silent) {
		docstring state;
		if (isMovable())
			state = _("movable");
		else
			state = _("immovable");
		owner_.message(bformat(_("Toolbar \"%1$s\" state set to %2$s"),
			qstring_to_ucs4(windowTitle()), state));
	}
}

} // namespace frontend
} // namespace lyx

#include "moc_GuiToolbar.cpp"
