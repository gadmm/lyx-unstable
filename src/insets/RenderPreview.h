// -*- C++ -*-
/**
 * \file RenderPreview.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Angus Leeming
 *
 * Full author contact details are available in file CREDITS.
 *
 * graphics::RenderPreview is an abstract base class that can help
 * insets to generate previews. The daughter class must instantiate two small
 * methods. The Inset would own an instance of this daughter class.
 */

#ifndef RENDERPREVIEW_H
#define RENDERPREVIEW_H

#include "RenderBase.h"

#include "support/docstring.h"
#include "support/FileMonitor.h"
#include "support/FileName.h"

#include <boost/signals2.hpp>
#include <boost/signals2/trackable.hpp>
#include <boost/signals2/connection.hpp>

namespace lyx {

class Buffer;
class MetricsInfo;
class PainterInfo;

namespace graphics {

class PreviewImage;
class PreviewLoader;

} // namespace graphics


class RenderPreview : public RenderBase, public boost::signals2::trackable {
public:
	/// Return true if preview is enabled in text (from LyXRC::preview)
	static bool previewText();
	/// Return true if preview is enabled in mathed (from LyXRC::preview)
	static bool previewMath();

	RenderPreview(Inset const *);
	RenderPreview(RenderPreview const &, Inset const *);
	~RenderPreview();
	RenderBase * clone(Inset const *) const;

	/// Compute the size of the object, returned in dim
	void metrics(MetricsInfo &, Dimension & dim) const;
	///
	void draw(PainterInfo & pi, int x, int y) const;

	/** Find the PreviewLoader and add a LaTeX snippet to it.
	 *  Do not start the loading process.
	 *  \param ignore_lyxrc: generate the preview no matter what LyXRC says
	 */
	void addPreview(docstring const & latex_snippet, Buffer const &,
			bool ignore_lyxrc = false);

	/** Add a LaTeX snippet to the PreviewLoader.
	 *  Do not start the loading process.
	 *  \param ignore_lyxrc: generate the preview no matter what LyXRC says
	 */
	void addPreview(docstring const & latex_snippet,
			graphics::PreviewLoader & ploader,
			bool ignore_lyxrc = false);

	/// Begin the loading process.
	/// \param forexport : whether this is intended for export. if so,
	/// then we ignore LyXRC and wait for the image to be generated.
	void startLoading(Buffer const & buffer, bool forexport = false) const;

	/** Remove a snippet from the cache of previews.
	 *  Useful if previewing the contents of a file that has changed.
	 */
	void removePreview(Buffer const &);

	/** \returns a pointer to the PreviewImage associated with this snippet
	 *  of latex.
	 */
	graphics::PreviewImage const *
	getPreviewImage(Buffer const & buffer) const;

	/// equivalent to dynamic_cast
	virtual RenderPreview * asPreview() { return this; }

private:
	/// Not implemented.
	RenderPreview & operator=(RenderPreview const &);

	/// This method is connected to the PreviewLoader::imageReady signal.
	void imageReady(graphics::PreviewImage const &);

	/// The thing that we're trying to generate a preview of.
	std::string snippet_;

	/** Store the connection to the preview loader so that we connect
	 *  only once.
	 */
	boost::signals2::connection ploader_connection_;

	/// Inform the core that the inset has changed.
	Inset const * parent_;
};


class RenderMonitoredPreview : public RenderPreview {
public:
	RenderMonitoredPreview(Inset const *);
	///
	void draw(PainterInfo & pi, int x, int y) const;
	///
	void setAbsFile(support::FileName const & file);
	///
	bool monitoring() const;
	void startMonitoring() const;
	void stopMonitoring() const;

	/// Connect and you'll be informed when the file changes.
	typedef boost::signals2::signal<void()> ChangedSig;
	boost::signals2::connection connect(ChangedSig::slot_type const &);

	/// equivalent to dynamic_cast
	virtual RenderMonitoredPreview * asMonitoredPreview() { return this; }

private:
	/// This signal is emitted if the file is modified
	ChangedSig changed_;
	///
	mutable support::ActiveFileMonitorPtr monitor_;
	///
	support::FileName filename_;
};



} // namespace lyx

#endif // RENDERPREVIEW_H
