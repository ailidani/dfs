
#ifndef mds_server_hpp
#define mds_server_hpp

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <set>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/export.hpp>
#include <boost/foreach.hpp>
#include "mds.h"
#include "udp_connection.hpp"
#include "message.hpp"


using boost::asio::ip::udp;

class CMessage;
typedef boost::shared_ptr<CMessage> MessagePtr;

class udp_connection;
typedef boost::shared_ptr<udp_connection> udp_connection_ptr;


class server
{
public:
	static boost::asio::io_service io_service_;
	server();
	~server() {};
	static server* instance()
	{
		if(server_ == 0)
		{
			server_ = new server();
		}
		return server_;
	}
	void run()
	{
		io_service_.run();
	}

	void send(MessagePtr p_msg);
	void handle_write(const boost::system::error_code & err);
	/// Handle completion of a read operation.
	void handle_message( const boost::system::error_code& err );
	void process_message(MessagePtr message);

protected:
	//boost::asio::io_service & io_service_;
	udp_connection_ptr conn_;
	std::set<udp::endpoint> sender_endpoints_;
	udp::endpoint endpoint_;
	std::vector<MessagePtr> messages_;

private:
	static server* server_;
};

typedef boost::shared_ptr<server> server_ptr;

#endif /* mds_server_hpp */
