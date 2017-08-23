#include <iostream>
#include <qpop_server.h>

int main(){
	Qpop_Server* main_server = new Qpop_Server();
	main_server->set_port(8000);
	//std::cout << "connected?: " << main_server->is_Connected();
	main_server->start_server();
	Sleep(5000);
	main_server->send("popped");
	//main_server->stop_server();
	std::cout << "starting server: " << std::endl;
	Sleep(10000);
}