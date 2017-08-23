#ifndef QPOP_SERVER
#define QPOP_SERVER

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

namespace boost
{
	#ifdef BOOST_NO_EXCEPTIONS
	void throw_exception( std::exception const & e ){
	    throw e; // or whatever
	};
#endif
}

typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::server<websocketpp::config::asio> endpoint;
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
		void setCondition(bool newState);
	private:
		websocketpp::connection_hdl current_connection;
		static void on_message(Qpop_Server* s, websocketpp::connection_hdl hdl, message_ptr msg);
		static void on_open(Qpop_Server *s, websocketpp::connection_hdl hdl);
		static void on_close(Qpop_Server* s, websocketpp::connection_hdl hdl);
		int port;
		bool is_connected;
		boost::mutex _status;
		boost::mutex _connected;
		boost::mutex _chng_mutex;
		bool condition;
		static void intialize(Qpop_Server* s);
		boost::thread network_thread;
};

#endif