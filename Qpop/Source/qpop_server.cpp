#include <qpop_server.h>
using namespace std;

Qpop_Server::Qpop_Server(){
	this->_connected.lock();
	this->is_connected = false;
	this->_connected.unlock();
	this->condition = false;
	this->createAuthNum();
}
// Crashes system, because calls are on different threads
void Qpop_Server::stop_server(){
	this->q_server.stop_listening();
}

void Qpop_Server::start_server(){
	using namespace websocketpp::lib;
	this->q_server.set_message_handler(bind(&Qpop_Server::on_message, this, placeholders::_1, placeholders::_2));
	this->q_server.set_open_handler(bind(&Qpop_Server::on_open, this, placeholders::_1));
	this->q_server.set_close_handler(bind(&Qpop_Server::on_close, this, placeholders::_1));
	this->q_server.set_validate_handler(bind(&Qpop_Server::validate_connection, this, placeholders::_1));
	this->network_thread = boost::thread(&Qpop_Server::intialize, this);
}

void Qpop_Server::send(string message){
	if(this->is_connected == true) {
		this->q_server.send(this->current_connection, message, websocketpp::frame::opcode::text);
	}
}

void Qpop_Server::intialize(Qpop_Server* s){
	cout << "initializing server" << endl;
	s->q_server.init_asio();
	s->q_server.listen(s->port);
	try{
		s->q_server.start_accept();
	}catch(exception e){
		throw e;
	}
	s->q_server.run();
}

bool Qpop_Server::conditionSatisfied(){
	return this->condition;
}

void Qpop_Server::setCondition(bool newState){
	this->condition = newState;
}



void Qpop_Server::on_message(Qpop_Server* s, websocketpp::connection_hdl hdl, message_ptr msg){
    cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << endl;

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    if (msg->get_payload() == "stop-listening") {
        s->q_server.stop_listening();
        return;
    }

    if(msg->get_payload() == "accepted"){
    	//do stuff
    	cout << "queue accepted" << endl;
    	s->setCondition(true);
    }

    try {
        s->q_server.send(hdl, msg->get_payload(), msg->get_opcode());
        cout << "\n\n\nSending the following" << msg->get_opcode() << "\n\n\n";
    } catch (const websocketpp::lib::error_code& e) {
        cout << "Echo failed because: " << e
                  << "(" << e.message() << ")" << endl;
    }
}

void Qpop_Server::on_open(Qpop_Server* s, websocketpp::connection_hdl hdl){
	cout << "\nPROPER CONNECTION!\n";
    s->current_connection = hdl;
    s->_connected.lock();
    s->is_connected = true;
    s->_connected.unlock();
}

void Qpop_Server::on_close(Qpop_Server* s, websocketpp::connection_hdl hdl){
	s->_connected.lock();
	s->is_connected = false;
	s->_connected.unlock();
}

void Qpop_Server::set_port(int num){
	this->port = num;
}

bool Qpop_Server::is_Connected(){
	return this->is_connected;
}

void Qpop_Server::createAuthNum(){
	this->auth_num = (rand() % 9000) + 1000;
}

int Qpop_Server::getAuthNum(){
	return this->auth_num;
}

bool Qpop_Server::validate_connection(Qpop_Server* s, websocketpp::connection_hdl hdl){
	server::connection_ptr p = 	s->q_server.get_con_from_hdl(hdl);
	try{
		int auth_input = stoi(p->get_resource().substr(1));
		if(auth_input == s->auth_num) return true;
	}catch(std::exception& e){
		cerr << e.what();
	}
	return false;
}