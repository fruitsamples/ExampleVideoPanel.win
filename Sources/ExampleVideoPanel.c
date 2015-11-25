/*
	File:		ExampleVideoPanel.c
	
	Description: 	Example video panel component routines.

	Author:		Apple Developer Support, original code by Gary Woodcock

	Copyright: 	� Copyright 1992-2002 Apple Computer, Inc. All rights reserved.
	
	Disclaimer:	IMPORTANT:  This Apple software is supplied to you by Apple Computer, Inc.
				("Apple") in consideration of your agreement to the following terms, and your
				use, installation, modification or redistribution of this Apple software
				constitutes acceptance of these terms.  If you do not agree with these terms,
				please do not use, install, modify or redistribute this Apple software.

				In consideration of your agreement to abide by the following terms, and subject
				to these terms, Apple grants you a personal, non-exclusive license, under Apple�s
				copyrights in this original Apple software (the "Apple Software"), to use,
				reproduce, modify and redistribute the Apple Software, with or without
				modifications, in source and/or binary forms; provided that if you redistribute
				the Apple Software in its entirety and without modifications, you must retain
				this notice and the following text and disclaimers in all such redistributions of
				the Apple Software.  Neither the name, trademarks, service marks or logos of
				Apple Computer, Inc. may be used to endorse or promote products derived from the
				Apple Software without specific prior written permission from Apple.  Except as
				expressly stated in this notice, no other rights or licenses, express or implied,
				are granted by Apple herein, including but not limited to any patent rights that
				may be infringed by your derivative works or by other works in which the Apple
				Software may be incorporated.

				The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
				WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
				WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
				PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN
				COMBINATION WITH YOUR PRODUCTS.

				IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
				CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
				GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
				ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION AND/OR DISTRIBUTION
				OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT
				(INCLUDING NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN
				ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
				
	Change History (most recent first):
	
	   <2>		2/09/02		SRK				Carbonized/Win32
	   <1>	 	1992		Gary Woodcock		first file
*/


//////////
//
// header files
//
//////////


#include "ExampleVideoPanelPrivate.h"

#if __APPLE_CC__
    #include <Carbon/Carbon.h>
    #include <QuickTime/QuickTime.h>
#else
    #include <QuickTimeComponents.h>
    #include <MacErrors.h>
    #include <Resources.h>
    #include <Gestalt.h>
#endif


//////////
//
// dispatch stuff
//
//////////

// Used by Component Dispatch Helper to call our routines
#define CALLCOMPONENT_BASENAME()		_ExampleVideoPanel
#define	CALLCOMPONENT_GLOBALS()			PanelGlobalsHdl storage

#define	SG_GLOBALS()					CALLCOMPONENT_GLOBALS()
#define SG_BASENAME()					CALLCOMPONENT_BASENAME()

// Other defines for Component Dispatch Helper
#define COMPONENT_DISPATCH_FILE			"ExampleVideoPanelDispatch.h"		// describes what to dispatch

#define COMPONENT_UPP_PREFIX()			uppSG
#define COMPONENT_SELECT_PREFIX()  		kSGPanel

#include <Components.k.h>				// StdComponent's .k.h
#if __APPLE_CC__
    #include <QuickTime/QuickTimeComponents.k.h>
    #include <QuickTime/ComponentDispatchHelper.c>
#else
    #include <QuickTimeComponents.k.h>		// vdig stuff
    #include <ComponentDispatchHelper.c>	// make our dispatcher and cando
#endif

//////////
//
// _ExampleVideoPanelOpen
// Handle the Standard Component Manager Open Request
//
// Set up the components global data storage
//
//////////

pascal ComponentResult
_ExampleVideoPanelOpen (PanelGlobalsHdl storage, ComponentInstance self)
{
	ComponentResult	result = noErr;

	// Can we open another instance?
	if (CountComponentInstances ((Component) self) <= kMaxExampleVideoPanelInstances)
	{
		// Did we get our storage?
		storage = (PanelGlobalsHdl) NewHandleClear (sizeof (PanelGlobals));
		if (storage != nil)
		{
			// Keep a reference to self
			(**storage).self = (Component) self;
						
			// Set storage ref
			SetComponentInstanceStorage (self, (Handle) storage);
		}
		else	// NewHandleClear failed
		{
			result = MemError();
		}
	}
	else	// No more instances can be opened
	{
		result = kGenericError;
	}
	return (result);
}

//////////
//
// _ExampleVideoPanelClose
// Handle the Standard Component Manager Close Request
//
// Clean up the components global data storage
//
//////////

pascal ComponentResult
_ExampleVideoPanelClose (PanelGlobalsHdl storage, ComponentInstance self)
{
	PanelGlobalsHdl	globals = (PanelGlobalsHdl) storage;
	ComponentResult	result = noErr;
	
	// Do we have any clean up to do?
	if (globals != nil)
	{
		// Dispose globals
		DisposeHandle ((Handle) globals);
	}
	return (result);
}

//////////
//
// _ExampleVideoPanelVersion
// Handle the Standard Component Manager Version Request
//
// Return the components version information
//
//////////

pascal ComponentResult
_ExampleVideoPanelVersion (PanelGlobalsHdl storage)
{
	#pragma	unused (storage)

	// Return the version info
	return (exampleVideoPanelInterfaceRevision);
}

//////////
//
// _ExampleVideoPanelPanelGetDitl
// Handle the SGPanelGetDITL Request
//
// Lets a sequence grabber component determine the dialog items managed by the panel component
//
//////////

pascal ComponentResult
_ExampleVideoPanelPanelGetDitl (PanelGlobalsHdl storage, Handle *ditl)
{
	// This routine gets our ditl and hands it back to the sequence grabber
	#pragma unused (storage)

	ComponentResult	result = noErr;
	Handle			panelDITL = GetResource ('DITL', kExampleVideoPanelDITLID);

	// Did we get the DITL resource okay?
	result = ResError();	
	if (panelDITL != nil)
	{
		// Detach it
		DetachResource (panelDITL);
	}

	*ditl = panelDITL;
	
	return (result);
}

//////////
//
// _ExampleVideoPanelPanelGetTitle
// Handle the SGPanelGetTitle Request
//
// Return the displayed title of the sequence grabber panel
//
//////////

pascal ComponentResult
_ExampleVideoPanelPanelGetTitle (PanelGlobalsHdl storage, Str255 title)
{
	#pragma unused (storage)
	#pragma unused (title)
	
	// Nothing to do here right now
	return (paramErr);
}

//////////
//
// _ExampleVideoPanelPanelCanRun
// Handle the SGPanelCanRun Request
//
// Lets a sequence grabber component determine whether a panel 
// component can work with the current sequence grabber channel 
// component.
//
//////////

pascal ComponentResult
_ExampleVideoPanelPanelCanRun (PanelGlobalsHdl storage, SGChannel c)
{
	#pragma unused (storage)
	#pragma unused (c)

	// Our 'thng' resource has the channelFlagHasDependency flag set, 
	// which means that this routine gets called to find out if
	// it can run in the current environment.  This usually means
	// finding out if the panel is compatible with the currently
	// selected digitizer.  This is handy for adding manufacturer
	// specific panels to the settings dialog.  We don't do anything
	// here except SysBeep just to show we actually get called.
	int i;
	for(i=0;i<3;i++) SysBeep(5);
	
	return (noErr);
}

//////////
//
// _ExampleVideoPanelPanelInstall
// Handle the SGPanelInstall Request
//
// Installs added items in a sequence grabber settings dialog box 
// before the dialog box is displayed to the user
//
//////////

pascal ComponentResult
_ExampleVideoPanelPanelInstall (PanelGlobalsHdl storage, SGChannel c, DialogPtr d,
	short itemOffset)
{
	// Do our setup in this routine
	
	#pragma unused (d)
	#pragma unused (itemOffset)
	
	PanelGlobalsHdl	globals = (PanelGlobalsHdl) storage;
	ComponentResult	result = noErr;
	unsigned short 	blackLevel;
	
	
	// Get current black level
	result = VDGetBlackLevelValue (SGGetVideoDigitizerComponent (c), &blackLevel);
	((**globals).savedBlackLevel) = blackLevel;
	if (result != noErr)
	{
		DoSetControlState(d, itemOffset + kZeroBlackButton, false);
	}

	return (noErr);
}

//////////
//
// _ExampleVideoPanelPanelEvent
// Handle the SGPanelEvent Request
//
// Lets a component receive and process dialog events
//
//////////

pascal ComponentResult
_ExampleVideoPanelPanelEvent (PanelGlobalsHdl storage, SGChannel c, DialogRef d,
	short itemOffset, const EventRecord *theEvent, short *itemHit, Boolean *handled)
{
	// This routine is quite similar to a normal event filter proc.
	
	PanelGlobalsHdl		globals = (PanelGlobalsHdl) storage;
	ComponentResult		result = noErr;
	GrafPtr				savedPort;

	// Set up the port stuff
	GetPort (&savedPort);

#if TARGET_API_MAC_CARBON
	SetPortDialogPort(d);
#else
	MacSetPort ((GrafPtr)d);
#endif
						
	// Assume we don't handle it
	*handled = false;
	
	// Look for a key hit
	if ((theEvent->what == keyDown) || (theEvent->what == autoKey))
	{
		char	theChar = theEvent->message & charCodeMask;
		char	theKeyCode = ((theEvent->message & keyCodeMask) >> 8);
		Boolean	cmdKeyDown = ((theEvent->modifiers & cmdKey) != 0) ? true : false;

		if (cmdKeyDown && (theKeyCode == kDKey))
		{
			unsigned short	maxBlackLevel = 0;
			
			// Fake a "Do it" button hit
			FakeDialogButtonHit (d, itemOffset + kZeroBlackButton);
			result = VDSetBlackLevelValue (SGGetVideoDigitizerComponent (c), &maxBlackLevel);
			*itemHit = itemOffset + kZeroBlackButton;
			*handled = true;
		}
		else if (cmdKeyDown && (theKeyCode == kRKey))
		{
			unsigned short	maxBlackLevel = 0;

			// Fake a "Reset" button hit
			FakeDialogButtonHit (d, itemOffset + kResetButton);
			if (globals != NULL)
			{
				maxBlackLevel = ((**globals).savedBlackLevel);
				result = VDSetBlackLevelValue (SGGetVideoDigitizerComponent (c), &maxBlackLevel);
				((**globals).savedBlackLevel) = maxBlackLevel;
			}
			*itemHit = itemOffset + kResetButton;
			*handled = true;
		}
	}
	else if (theEvent->what == updateEvt)
	{
		Rect		r;
		PenState	savedPen;
		Pattern		pattern;
	#if TARGET_OS_WIN32
		short		i;
	#endif
		
		GetPenState (&savedPen);

		// Draw the separator line whenever we get an update cuz I'm lazy
		GetItemBox (d, itemOffset + kSeparator, &r);
		#if TARGET_OS_WIN32
				/* create black pattern */
			for (i=0; i < sizeof(Pattern); ++i)
			{
				pattern.pat[i] = 0xff;
			}
			PenPat(&pattern);
		#else
			PenPat(GetQDGlobalsBlack(&pattern));
		#endif
		MacFrameRect (&r);

		SetPenState (&savedPen);
	}

	// Restore stuff
	MacSetPort (savedPort);
	
	return (result);
}

//////////
//
// _ExampleVideoPanelPanelItem
// Handle the SGPanelItem Request
//
// Receives and processes mouse clicks in the sequence 
// grabber settings dialog box.
//
//////////

pascal ComponentResult
_ExampleVideoPanelPanelItem (PanelGlobalsHdl storage, SGChannel c, DialogPtr d,
	short itemOffset, short itemNum)
{
	#pragma unused (d)
	
	PanelGlobalsHdl	globals = (PanelGlobalsHdl) storage;
	ComponentResult	result = noErr;
	short			theItem = itemNum - itemOffset;	// Remember to account for the item offset
	
	// What item got hit?
	switch (theItem)
	{
		case kZeroBlackButton:
		{
			unsigned short	maxBlackLevel = 0;
			
			result = VDSetBlackLevelValue (SGGetVideoDigitizerComponent (c), &maxBlackLevel);
			break;
		}
		case kResetButton:
		{
			unsigned short	maxBlackLevel = 0;

			if (globals != NULL)
			{
				maxBlackLevel = ((**globals).savedBlackLevel);
				result = VDSetBlackLevelValue (SGGetVideoDigitizerComponent (c), &maxBlackLevel);
				((**globals).savedBlackLevel) = maxBlackLevel;
			}

			break;
		}
		default:
		{
			break;
		}
	}
	return (result);
}

//////////
//
// _ExampleVideoPanelPanelRemove
// Handle the SGPanelRemove Request
//
// Removes a panel from the sequence grabber settings dialog box.
//
//////////

pascal ComponentResult
_ExampleVideoPanelPanelRemove (PanelGlobalsHdl storage, SGChannel c, DialogPtr d,
	short itemOffset)
{
	#pragma unused (storage)
	#pragma unused (c)
	#pragma unused (d)
	#pragma unused (itemOffset)
	
	// This is where you do your panel related cleanup.  Note that
	// this is different from the cleanup you do in your close component
	// routine.  Basically, this gets called when your ditl is getting
	// removed from the dialog and a new one is being added (like when
	// someone chooses a different panel from the panel popup menu).
	// Examples of stuff you might do here include getting rid of any
	// custom controls or popup menus, or saving panel settings.
	
	return (noErr);
}

//////////
//
// _ExampleVideoPanelPanelSetGrabber
// Handle the SGPanelSetGrabber Request
//
// Identifies a sequence grabber component to a panel component
//
//////////

pascal ComponentResult
_ExampleVideoPanelPanelSetGrabber (PanelGlobalsHdl storage, SeqGrabComponent sg)
{
	PanelGlobalsHdl	globals = (PanelGlobalsHdl) storage;
	
	// Save our grabber
	(**globals).seqGrabber = sg;
	
	return (noErr);
}

//////////
//
// _ExampleVideoPanelPanelSetResFile
// Handle the SGPanelSetResFile Request
//
// Lets the sequence grabber pass a resource file's reference number.
//
//////////

pascal ComponentResult
_ExampleVideoPanelPanelSetResFile (PanelGlobalsHdl storage, short resRef)
{
	
	// Since we don't have the channelFlagDontOpenResFile flag
	// set in our 'thng' resource, the sequence grabber will 
	// open our resource file for us.
	
	// Save our resfile ref
	(**storage).resRefNum = resRef;
	
	return (noErr);
}
														 
//////////
//
// _ExampleVideoPanelPanelGetSettings
// Handle the SGPanelGetSettings Request
//
// Retrieves a panel's current settings for a sequence grabber component.
//
//////////


pascal ComponentResult
_ExampleVideoPanelPanelGetSettings (PanelGlobalsHdl storage, SGChannel c, UserData *ud, long flags)
{
	#pragma unused (storage)
	#pragma unused (c)
	#pragma unused (flags)
	
	ComponentResult	result = noErr;
	UserData		userStuff = 0L;
	
	// You HAVE to give something valid back as user data when you
	// return from this routine or your panel will fail to install.  
	// What is being asked for basically amounts to a handle to some 
	// state info specific to your panel.  We don't really have any 
	// state info we care about, so we just hand back an empty user 
	// data just to keep everyone happy.
	
	if ((result = NewUserData (&userStuff)) == noErr)
	{
		*ud = userStuff;
	}
	return (result);
}
														
//////////
//
// _ExampleVideoPanelPanelSetSettings
// Handle the SGPanelSetSettings Request
//
// Restores a panel's current settings for a sequence grabber component
//
//////////

pascal ComponentResult
_ExampleVideoPanelPanelSetSettings (PanelGlobalsHdl storage, SGChannel c, UserData ud, long flags)
{
	#pragma unused (storage)
	#pragma unused (c)
	#pragma unused (ud)
	#pragma unused (flags)
	
	// Here is where you would decode your user data structure and use
	// the info to set the state of your panel items.  Again, we don't
	// really have anything that has any meaningful state in our panel,
	// so we ignore this.
	
	return (noErr);
}
														 
//////////
//
// _ExampleVideoPanelPanelValidateInput
// Handle the SGPanelValidateInput Request
//
// Validates the contents of the user dialog box for a sequence grabber component.
//
//////////

pascal ComponentResult
_ExampleVideoPanelPanelValidateInput (PanelGlobalsHdl storage, Boolean *ok)
{
	#pragma unused (storage)
	
	// This is where you do a sanity check on the user-definable
	// items in your panel.  If there are any bad values, you
	// should alert the user somehow, and return false for *ok.
	// If everything's fine, return true.  We don't really have
	// anything to check on in our example, so we return true
	// all the time.
	
	*ok = true;
	return (noErr);
}

//////////
//
// IsAppearanceMgrPresent
// Check for the presence of the Appearance Manager
//
//////////

#if TARGET_OS_MAC && !TARGET_API_MAC_CARBON

static pascal OSStatus IsAppearanceMgrPresent (Boolean *haveAppearance)
{
	OSStatus	err = noErr;
	long 		response;

	// Attempt to call Gestalt; if we succeed, test for presence of Appearance Mgr
	if (!(err = Gestalt (gestaltAppearanceAttr,&response)))
		*haveAppearance = response & (1 << gestaltAppearanceExists) ? true : false;
	// If the Appearance Mgr selector is undefined, the Appearance Mgr is not present
	else if (err == gestaltUndefSelectorErr)
	{
		*haveAppearance = false;
		err = noErr;
	}

	return err;
} 

#endif

//////////
//
// Carbon_SetControlState
// Set the control state using Carbon APIs
//
//////////

static void Carbon_SetControlState(DialogPtr theDialog, short theItem, Boolean enable)
{
	OSErr		err;
	ControlRef	outControl;
	
	err = GetDialogItemAsControl (theDialog, theItem, &outControl);
	if (err == noErr)
	{
		if (enable)
		{
			ActivateControl(outControl);
		}
		else
		{
			DeactivateControl(outControl);
		}
	}

}

//////////
//
// OldStyle_SetControlState
// Set the control state using Pre-Carbon APIs
//
//////////

static void OldStyle_SetControlState(DialogPtr theDialog, short theItem, Boolean enable)
{
	Handle	item = NULL;
	Rect	box;
	short	itemType;

	GetDialogItem (theDialog, theItem, &itemType, &item, &box);
	if (item != NULL)
	{
		if (enable)
		{
			HiliteControl((ControlHandle)item, 0);
		}
		else
		{
			HiliteControl((ControlHandle)item, 255);
		}
		
	}
}

//////////
//
// DoSetControlState
// Set the control state
//
//////////

static void DoSetControlState(DialogPtr theDialog, short theItem, Boolean enable)
{

#if TARGET_API_MAC_CARBON

	Carbon_SetControlState(theDialog, theItem, enable);

#else

	#if TARGET_OS_MAC

		OSStatus status;
		Boolean haveAppearance;

			/* check for Appearance Manager - if present, use
				new-style calls to hilight our controls */
		status = IsAppearanceMgrPresent(&haveAppearance);
		if ((status == noErr) && (haveAppearance))
		{
			Carbon_SetControlState(theDialog, theItem, enable);
		}
		else
		{
				/* Appearance Manager is not available, so let's use
					old-style calls */
					
	 		OldStyle_SetControlState(theDialog, theItem, enable);
	 	}
		
	#else	/* Windows environment, use old-style calls */
	
	 		OldStyle_SetControlState(theDialog, theItem, enable);
	
	#endif

#endif

}

//////////
//
// DoEnableControl
// Enable the control
//
//////////

static void
DoEnableControl (DialogPtr theDialog, short theItem)
{
	DoSetControlState(theDialog, theItem, true);
}

//////////
//
// DoDisableControl
// Disable the control
//
//////////

static void
DoDisableControl (DialogPtr theDialog, short theItem)
{
	DoSetControlState(theDialog, theItem, false);
}

//////////
//
// FakeDialogButtonHit
// Fake a button hit
//
//////////

static OSErr
FakeDialogButtonHit (DialogPtr theDialog, short theButtonItem)
{
	OSErr	result = noErr;
	
	if (theDialog != nil)
	{
		unsigned long	dummyTicks = 0L;
		
		DoSetControlState(theDialog, theButtonItem, true);
		Delay (8, &dummyTicks);
		DoSetControlState(theDialog, theButtonItem, false);
	}
	else
	{
		result = nilHandleErr;
	}
	return (result);
}


//////////
//
// GetItemBox
//
//////////

static void
GetItemBox (DialogPtr theDialog, short theItem, Rect *theRect)
{
	Handle	item;
	short	itemType;
	
	GetDialogItem (theDialog, theItem, &itemType, &item, theRect);
}

//////////
//
// RegisterExampleVideoPanel
// Manually register the component if we are not running standalone
// but are instead linked
//
//////////

#ifdef DEBUG_ME

Component
RegisterExampleVideoPanel (void)
{
	ComponentDescription	foo;
	Handle 					h;
	Component				theCompID;

  	foo.componentType = kTheComponentType;
  	foo.componentSubType = kTheComponentSubType;
  	foo.componentManufacturer = kTheComponentManufacturer;
  	foo.componentFlags = channelFlagHasDependency;	// Indicate that we want a can run message
  	foo.componentFlagsMask = 0L;

	PtrToHand ("\pExample (linked)", &h, 17);
	theCompID = RegisterComponent (&foo, (void *)ExampleVideoPanelDispatcher, 0, h, 0, 0);
	if (h != nil)
	{
		DisposeHandle (h);
	}
	return (theCompID);
}

#endif DEBUG_ME

