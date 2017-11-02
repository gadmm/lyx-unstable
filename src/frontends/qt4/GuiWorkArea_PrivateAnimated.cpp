/**
 * \file GuiWorkArea_PrivateAnimated.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Guillaume Munch
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "GuiView.h"
#include "GuiWorkArea_PrivateAnimated.h"

#include "BufferView.h"
#include "qt_helpers.h"

#include "support/debug.h"

#include <QScrollBar>

#include <algorithm>


namespace lyx {
namespace frontend {


GuiWorkArea::PrivateAnimated::PrivateAnimated(GuiWorkArea * parent)
	: GuiWorkArea::Private(parent), scrolled_(0),
	  scroll_animation_(new QPropertyAnimation(parent))
{
	scroll_animation_->setTargetObject(this);
	scroll_animation_->setPropertyName("docScrollValue");
	scroll_animation_->setEasingCurve(QEasingCurve(QEasingCurve::OutSine));
	connect(scroll_animation_, &QPropertyAnimation::stateChanged,
	        this, &GuiWorkArea::PrivateAnimated::scrollingChanged);
	connect(scroll_animation_, &QPropertyAnimation::finished,
	        this, &GuiWorkArea::PrivateAnimated::scrollingFinished);
}


void GuiWorkArea::PrivateAnimated::setGuiView(GuiView * gv)
{
	GuiWorkArea::Private::setGuiView(gv);
	connect(this, &GuiWorkArea::PrivateAnimated::scrollingStarted,
	        lyx_view_, &GuiView::scrollingStarted);
	connect(this, &GuiWorkArea::PrivateAnimated::scrollingFinished,
	        lyx_view_, &GuiView::scrollingFinished);
}


int GuiWorkArea::PrivateAnimated::docScrollValue() const
{
	return scrolled_;
}


void GuiWorkArea::PrivateAnimated::scrollTo(int const value)
{
	// duration is defined as a continuous piecewise affine function of the
	// distance.
	//  * 0px -> 0ms
	//  * 1 line height -> 60ms
	//  * 1 page height -> 150ms
	//  * stops increasing at 350ms.
	double const single_step_duration = 60;
	double const page_step_duration = 150;
	int const max_duration = 350;

	if (value == 0)
		return;
	int const offset = value + scroll_animation_->endValue().toInt()
		- scroll_animation_->currentValue().toInt();
	stopScrolling(false);
	if (p->verticalScrollBar()->isSliderDown()) {
		// do not attempt to animate when the slider is dragged
		GuiWorkArea::Private::scrollTo(offset);
		return;
	}
	scroll_animation_->setEndValue(offset);
	// compute duration
	{
		int const single_step = p->verticalScrollBar()->singleStep();
		int const page_step = std::max(p->verticalScrollBar()->pageStep(),
		                               single_step);
		int const abs_offset = abs(offset);
		int duration;
		if (abs_offset <= single_step)
			duration = int((single_step_duration * abs_offset) / single_step);
		else {
			double const slope = (page_step_duration - single_step_duration) /
				(page_step - single_step);
			duration = int(slope * (abs_offset - single_step)
			               + single_step_duration);
			duration = std::min(duration, max_duration);
		}
		scroll_animation_->setDuration(duration);
	}
	p->stopBlinkingCaret();
	scroll_animation_->start();
}


void GuiWorkArea::PrivateAnimated::setDocScrollValue(int value)
{
	int offset = value - scrolled_;
	if (!buffer_view_)
		return;
	buffer_view_->scrollDocView(offset);
	scrolled_ = value;
	hideCaret();
	updateScrollbar();
	updateCursorShape();
}


void GuiWorkArea::PrivateAnimated::stopScrolling(bool emit)
{
	scroll_animation_->stop();
	scroll_animation_->setStartValue(0);
	scroll_animation_->setEndValue(0);
	scrolled_ = 0;
	if (emit)
		Q_EMIT scrollingFinished();
}


void
GuiWorkArea::PrivateAnimated::scrollingChanged
(QAbstractAnimation::State new_state, QAbstractAnimation::State old_state)
{
	if (new_state == QAbstractAnimation::Running)
		Q_EMIT scrollingStarted();
	if (old_state == QAbstractAnimation::Running
	    && new_state == QAbstractAnimation::Stopped)
		scrollFinish();
}


} // namespace frontend
} // namespace lyx

#include "moc_GuiWorkArea_PrivateAnimated.cpp"
