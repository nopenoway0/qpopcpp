#ifndef _QPOP_WINDOW_H
#define _QPOP_WINDOW_H

#include <wx/wx.h>
#include <boost/thread.hpp>

class QpopApp : public wxApp{
	public: 
		virtual bool OnInit();
	private:
		boost::thread* qpop_scanner;
};

wxIMPLEMENT_APP(QpopApp);

#endif