#ifndef QPOP_SERVER
#define QPOP_SERVER

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>
#include <thread>
#include <mutex>

typedef websocketpp::server<websocketpp::config::asio> server;
//typedef boost::asio::io_service io_service;
typedef server::message_ptr message_ptr;

class Qpop_Server{
	public:
		server q_server;
		Qpop_Server();
		void start_server();
		void set_port(int num);
		void stop_server();
		void send(std::string message);
		bool is_Connected();
		bool conditionSatisfied();
		bool isAuthenticated();
		void setCondition(bool newState);
		void createAuthNum();
		int getAuthNum();
		//make private
	private:
		websocketpp::connection_hdl current_connection;
		static void on_message(Qpop_Server* s, websocketpp::connection_hdl hdl, message_ptr msg);
		static void on_open(Qpop_Server *s, websocketpp::connection_hdl hdl);
		static void on_close(Qpop_Server* s, websocketpp::connection_hdl hdl);
		static bool validate_connection(Qpop_Server* s, websocketpp::connection_hdl hdl);
		int ip;
		int port;
		bool is_connected;
		std::mutex _status;
		std::mutex _connected;
		std::mutex _chng_mutex;
		bool condition;
		int auth_num;
		static void intialize(Qpop_Server* s);
		std::thread network_thread;
};

#endif