// $Id: Dialog_ShowMacro.h 15950 2014-10-22 04:57:00Z davidl $
// $HeadURL: https://halldsvn.jlab.org/repos/trunk/online/packages/RootSpy/src/RootSpy/Dialog_ShowMacro.h $
//
//    File: Dialog_ShowMacro.h
// Creator: davidl


#ifndef _Dialog_ShowMacro_
#define _Dialog_ShowMacro_

#include <TGClient.h>
#include <TGButton.h>
#include <TGListBox.h>

using namespace std;

class Dialog_ShowMacro:public TGMainFrame{
	public:
		Dialog_ShowMacro(string title, string macro_str);
		virtual ~Dialog_ShowMacro();
		
		void CloseWindow(void);

	private:
		ClassDef(Dialog_ShowMacro,1)


};

#endif // _Dialog_ShowMacro_

