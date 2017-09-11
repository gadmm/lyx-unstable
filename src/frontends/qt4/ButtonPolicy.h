// -*- C++ -*-
/**
 * \file ButtonPolicy.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Allan Rae
 *
 * Full author contact details are available in file CREDITS.
 *
 * Provides a state machine implementation of the various button policies
 * used by the dialogs.
 */

#ifndef BUTTONPOLICY_H
#define BUTTONPOLICY_H

namespace lyx {
namespace frontend {

/** A class for button policies.
    A state machine implementation of the various button policies used by the
    dialogs. Only the policy is implemented here.  Separate ButtonController
    classes are needed for each GUI implementation.

    Policy                    | ReadOnly | Apply Button | Repeated Apply
    ========================================================================
    OkCancel                  |     N    |      N        |      -
    OkCancelReadOnly          |     Y    |      N        |      -
    OkApplyCancel             |     N    |      Y        |      Y
    OkApplyCancelReadOnly     |     Y    |      Y        |      Y
    NoRepeatedApply           |     N    |      Y        |      N
    NoRepeatedApplyReadOnly   |     Y    |      Y        |      N
    OkApplyCancelAutoReadOnly |     Y    |      Y        |      Y
    Preferences               |     N    |      Y        | No (Ok-Close)
    Ignorant                  |    N/A   |     N/A       |     N/A
    ========================================================================

    Policy
	The name of the policy
    ReadOnly
	Does the policy treat read-only docs differently to read-write docs?
	This usually means that when an SMI_READ_ONLY input arrives then
	all the buttons are disabled except Cancel/Close.  The state
	machine tracks the inputs (valid/invalid) and has states for all
	combinations. When an SMI_READ_WRITE input arrives the appropriate
	machine state is entered (just as if the document had always been
	read-write).
	NOTE: If a dialog doesn't care about the read-only status of a document
	(and uses an appropriate policy) it can never get into a read-only state
	so isReadOnly() can only ever return false even though the document may
	be read-only.
    Repeated Apply
	Simply means that it is alright to use the Apply button multiple times
	without requiring a change of the dialog contents.  If no repeating is
	allowed the Ok+Apply buttons are deactivated.  The Preferences dialog
	has its own special version of repeated apply handling because its Ok
	button is actually a Save button -- it is always reasonable to Save the
	preferences if the dialog has changed since the last save.

    The IgnorantPolicy is a special case that allows anything.
 */

class ButtonPolicy
{
public:

	// The various poicies
	enum Policy {
		/** Ok and Cancel buttons for dialogs with read-only operation.
			Note: This scheme supports the relabelling of Cancel to Close and
			vice versa.
			This is based on the value of the bool state of the Button::CANCEL.
			true == Cancel, false == Close
		 */
		OkCancelPolicy,


		/** Ok and Cancel buttons for dialogs where read-only operation is blocked.
			The state machine design for this policy allows changes to occur within
			the dialog while a file is read-only -- the okay button is disabled until
			a read-write input is given.  When the file is made read-write the dialog
			will then be in the correct state (as if the file had always been
			read-write).
			Note: This scheme supports the relabelling of Cancel to Close
			and vice versa.
			This is based on the value of the bool state of the Button::CANCEL.
			true == Cancel, false == Close
		 */
		OkCancelReadOnlyPolicy,

		/** Ok, Apply and Cancel buttons for dialogs where read-only operation
			is blocked.
			Repeated Apply are not allowed.  Likewise,  Ok cannot follow Apply without
			some valid input. That is, the dialog contents must change between
			each Apply or Apply and Ok.
			The state machine design for this policy allows changes to occur within
			the dialog while a file is read-only -- the Ok+Apply buttons are disabled
			until a read-write input is given.  When the file is made read-write the
			dialog will then be in the correct state (as if the file had always been
			read-write).
			Note: This scheme supports the relabelling of Cancel to Close
			and vice versa.
			This is based on the value of the bool state of the Button::CANCEL.
			true == Cancel, false == Close
		 */
		NoRepeatedApplyReadOnlyPolicy,

		/** Ok, Apply and Cancel buttons for dialogs where read-only
			operation is blocked.
			Repeated Apply is allowed.  Likewise,  Ok can follow Apply.
			The state machine design for this policy allows changes to occur within
			the dialog while a file is read-only -- the Ok+Apply buttons are disabled
			until a read-write input is given.  When the file is made read-write the
			dialog will then be in the correct state (as if the file had always been
			read-write).
			Note: This scheme supports the relabelling of Cancel to Close
			and vice versa.
			This is based on the value of the bool state of the Button::CANCEL.
			true == Cancel, false == Close
		 */
		OkApplyCancelReadOnlyPolicy,

		/** Ok, Apply and Cancel buttons for dialogs where repeated
			Apply is allowed.
			Note: This scheme supports the relabelling of Cancel to Close
			and vice versa.
			This is based on the value of the bool state of the Button::CANCEL.
			true == Cancel, false == Close
		 */
		OkApplyCancelPolicy,

		/** Ok, Apply and Cancel buttons for dialogs with no repeated Apply.
			Note: This scheme supports the relabelling of Cancel to Close
			and vice versa.
			This is based on the value of the bool state of the Button::CANCEL.
			true == Cancel, false == Close
		 */
		NoRepeatedApplyPolicy,

		/** Ok, Apply and Cancel buttons and an AutoApply checkbox.
			Note: This scheme supports the relabelling of Cancel to Close
			and vice versa.
			This is based on the value of the bool state of the Button::CANCEL.
			true == Cancel, false == Close
		 */
		OkApplyCancelAutoReadOnlyPolicy,

		/** Defines the policy used by the Preferences dialog.
			Four buttons: Ok (Save), Apply, Cancel/Close, Restore.
			Note: This scheme supports the relabelling of Cancel to Close
			and vice versa.
			This is based on the value of the bool state of the Button::CANCEL.
			true == Cancel, false == Close
		 */
		PreferencesPolicy,

		/** Defines the policy used by dialogs that are forced to support a button
			controller when they either don't have a use for one or are not ready to
			use one.  This may be useful when testing a new button policy but wishing
			to minimise problems to users by supplying an anything-goes policy via a
			preprocessor directive.
		 */
		IgnorantPolicy
	};

	/// Constructor
	explicit ButtonPolicy(Policy policy);
	/// Destructor
	~ButtonPolicy();
	///
	void setPolicy(Policy policy);

	/** The various possible state names.
	    Not all state-machines have this many states.  However, we need
	    to define them all here so we can share the code.
	*/
	enum State {
		///
		INITIAL = 0,
		///
		VALID,
		///
		INVALID,
		///
		APPLIED,
		///
		AUTOAPPLY_INITIAL,
		///
		AUTOAPPLY_CHANGED,
		///
		RO_INITIAL,
		///
		RO_VALID,
		///
		RO_INVALID,
		///
		RO_APPLIED,
		///
		RO_AUTOAPPLY,
		///
		BOGUS = 55
	};

	/// The various button types.
	enum Button {
		///
		CLOSE     = 0,  // Not a real button, but effectively !CANCEL
		///
		OKAY      = 1,
		///
		APPLY     = 2,
		///
		CANCEL    = 4,
		///
		RESTORE   = 8,
		///
		AUTOAPPLY = 16  // This is usually a checkbox
	};
	///
	static const Button ALL_BUTTONS =
		Button(OKAY | APPLY | CANCEL | RESTORE | AUTOAPPLY);

	/** State machine inputs.
	    All the policies so far have both CANCEL and HIDE always going to
	    INITIAL. This won't necessarily be true for all [future] policies
	    though so I'll leave those two as distinct inputs rather than merge
	    them.  For example, a dialog that doesn't update it's input fields
	    when reshown after being hidden needs a policy where CANCEL and
	    HIDE are treated differently.
	 */
	enum SMInput {
		/// the dialog contents are now valid
		SMI_VALID = 0,
		/// the dialog contents are now invalid
		SMI_INVALID,
		/// an apply-and-hide action has happened
		SMI_OKAY,
		/// an apply action has happened
		SMI_APPLY,
		/// a cancel action has happened
		SMI_CANCEL,
		/// a restore action has happened
		SMI_RESTORE,
		/// apply auto-apply
		SMI_AUTOAPPLY,
		/// the dialog has been hidden
		SMI_HIDE,
		/// the dialog contents are read-only
		SMI_READ_ONLY,
		/// the dialog contents can be modified
		SMI_READ_WRITE,
		/// the state of the dialog contents has not changed
		SMI_NOOP,
		/// for internal use
		SMI_TOTAL
	};

	/// Trigger a transition with this input.
	void input(SMInput);
	/** Activation status of a button.
	    We assume that we haven't gotten into an undefined state.
	    This is reasonable since we can only reach states defined
	    in the state machine and they should all have been defined in
	    the outputs_ variable.  Perhaps we can do something at compile
	    time to check that all the states have corresponding outputs.
	 */
	bool buttonStatus(Button) const;
	/// Are we in a read-only state?
	bool isReadOnly() const;

private:
	/// noncopyable
	ButtonPolicy(ButtonPolicy const &);
	void operator=(ButtonPolicy const &);

	/// pimpl
	class Private;
	Private * d;
};


} // namespace frontend
} // namespace lyx

#endif
