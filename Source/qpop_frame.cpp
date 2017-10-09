#include "qpop_frame.h"
namespace Qpop{
	namespace gui{
// TODO: CHANGE FONT, FIX SPACE, center text, add background
		QpopFrame::QpopFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, wxID_ANY, title, pos, size) {
			this->SetBackgroundColour(wxColour(0xF0, 0xFF, 0XFF));
			wxStaticText* status_button = new wxStaticText(this, wxSTATUS_TEXT, "connection");
			wxStaticText* auth = new wxStaticText(this, wxAUTH_TEXT, "AUTHCODE:");
			wxStaticText* difference = new wxStaticText(this, wxDIFF_TEXT, "");
			wxBoxSizer* layout = new wxBoxSizer(wxVERTICAL);

			wxSizerFlags flags(2);
			flags.Center();
			layout->Add(auth, flags);
			layout->Add(difference, flags);
			layout->Add(status_button, flags);
			SetSizer(layout);
		}
}}