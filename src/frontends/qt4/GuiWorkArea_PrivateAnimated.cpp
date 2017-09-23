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
	scroll_animation_->setEasingCurve(QEasingCurve(QEasingCurve::OutQuad));
	connect(scroll_animation_, SIGNAL(stateChanged(QAbstractAnimation::State,
	                                               QAbstractAnimation::State)),
	        this, SLOT(scrollAnimationFinish(QAbstractAnimation::State,
	                                         QAbstractAnimation::State)));
}


int GuiWorkArea::PrivateAnimated::docScrollValue() const
{
	return scrolled_;
}


void GuiWorkArea::PrivateAnimated::scrollTo(int const value)
{
	if (value == 0)
		return;
	int const offset = value + scroll_animation_->endValue().toInt()
		- scroll_animation_->currentValue().toInt();
	stopScrolling();
	if (p->verticalScrollBar()->isSliderDown()) {
		// do not attempt to animate when the slider is dragged
		GuiWorkArea::Private::scrollTo(offset);
		return;
	}
	scroll_animation_->setEndValue(offset);
	// compute duration
	{
		// duration is defined as a continuous piecewise affine function of the
		// distance.
		//  * 0px -> 0ms
		//  * 1 line height -> 100ms
		//  * 1 page height -> 250ms
		//  * stops increasing at 500ms.
		int const single_step = p->verticalScrollBar()->singleStep();
		int const page_step = std::max(p->verticalScrollBar()->pageStep(),
		                               single_step);
		double const single_step_duration = 100;
		double const page_step_duration = 250;
		int const max_duration = 500;
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


void GuiWorkArea::PrivateAnimated::stopScrolling()
{
	scroll_animation_->stop();
	scroll_animation_->setStartValue(0);
	scroll_animation_->setEndValue(0);
	scrolled_ = 0;
}


void
GuiWorkArea::PrivateAnimated::scrollAnimationFinish
(QAbstractAnimation::State new_state, QAbstractAnimation::State old_state)
{
	if (old_state != QAbstractAnimation::Running
	    || new_state != QAbstractAnimation::Stopped)
		return;
	scrollFinish();
}


} // namespace frontend
} // namespace lyx

#include "moc_GuiWorkArea_PrivateAnimated.cpp"
