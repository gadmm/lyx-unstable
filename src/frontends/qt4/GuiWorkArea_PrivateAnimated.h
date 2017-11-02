// -*- C++ -*-
/**
 * \file GuiWorkArea_PrivateAnimated.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Guillaume Munch
 *
 * Full author contact details are available in file CREDITS.
 */

#ifndef WORKAREA_PRIVATEANIMATED_H
#define WORKAREA_PRIVATEANIMATED_H

#include "GuiWorkArea.h"
#include "GuiWorkArea_Private.h"

#include <QPropertyAnimation>


namespace lyx {
namespace frontend {


/// Reimplements scrollTo() using a QPropertyAnimator
class GuiWorkArea::PrivateAnimated : public QObject, public GuiWorkArea::Private
{
	Q_OBJECT
	Q_PROPERTY(int docScrollValue READ docScrollValue WRITE setDocScrollValue)

public:
	PrivateAnimated(GuiWorkArea *);
	//
	void setGuiView(GuiView * gv) override;
	//
	void scrollTo(int value) override;
	// How much was scrolled so far in the current animation.
	int scrolled_;
	//
	QPropertyAnimation * const scroll_animation_;
	//
	int docScrollValue() const;
	//
	void stopScrolling(bool emit) override;

Q_SIGNALS:
	//
	void scrollingStarted();
	//
	void scrollingFinished();

public Q_SLOTS:
	void setDocScrollValue(int value);
	//
	void scrollingChanged(QAbstractAnimation::State new_state,
	                      QAbstractAnimation::State old_state);
};


} // namespace frontend
} // namespace lyx


#endif // WORKAREA_PRIVATEANIMATED_H
