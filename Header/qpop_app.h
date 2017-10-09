#ifndef _QPOP_WINDOW_H
#define _QPOP_WINDOW_H

#include <wx/wx.h>
#include <thread>

class QpopApp : public wxApp{
	public: 
		virtual bool OnInit();
	private:
		std::thread* qpop_scanner;
};

wxIMPLEMENT_APP(QpopApp);

#endif