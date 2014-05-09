
#ifndef mds_server_h
#define mds_server_h

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

#include "peer.hpp"
#include "message.hpp"


using boost::asio::ip::udp;


class server : public peer
{
public:
	static boost::asio::io_service io_service;
	~server() {};
	static server* instance()
	{
		if(server_ == 0)
		{
			server_ = new server();
		}
		return server_;
	}
	void run();

	//void handle_write(const boost::system::error_code & err);
	/// Handle completion of a read operation.
	void handle_message( const boost::system::error_code& err );
	void process_message(MessagePtr message);
	void send_msg(MessagePtr msg);
	void add_peer(const std::string & hostID);

protected:
	server();
	PeerSet peers;
	std::vector<MessagePtr> messages;
	boost::asio::ip::udp::endpoint RemoteEndpoint;

private:
	static server* server_;
};

//typedef boost::shared_ptr<server> server_ptr;

#endif /* mds_server_hpp */
