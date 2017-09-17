#ifndef _QPOP_FRAME_H
#define _QPOP_FRAME_H

#include "wx/wx.h"
namespace Qpop{
	namespace gui{
	enum IDS{
		wxIP_TEXT = wxID_HIGHEST + 1,
		wxAUTH_TEXT = wxID_HIGHEST + 2,
		wxSTATUS_TEXT = wxID_HIGHEST + 3,
		wxDIFF_TEXT = wxID_HIGHEST + 4
	};

	class QpopFrame : public wxFrame{
		public:
			QpopFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	};
}}
#endif