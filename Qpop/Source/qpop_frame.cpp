#include "qpop_frame.h"
namespace Qpop{
	namespace gui{
// Add containers to preven toverlapping
QpopFrame::QpopFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, wxID_ANY, title, pos, size){
	wxStaticText* status_button = new wxStaticText(this, wxSTATUS_TEXT, "connection");
	wxStaticText* ip = new wxStaticText(this, wxIP_TEXT, "IPADDRESS: ");
	wxStaticText* auth = new wxStaticText(this, wxAUTH_TEXT, "AUTHCODE: ");
	wxStaticText* difference = new wxStaticText(this, wxDIFF_TEXT, "DIFFERENCE");
	wxBoxSizer* layout = new wxBoxSizer(wxVERTICAL);
	layout->Add(ip);
	layout->AddSpacer(20);
	layout->Add(auth);
	layout->AddSpacer(20);
	layout->Add(difference);
	layout->AddSpacer(100);
	layout->Add(status_button);
	SetSizerAndFit(layout);
}
}}