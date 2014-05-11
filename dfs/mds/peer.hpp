
#ifndef mds_peer_hpp
#define mds_peer_hpp

#include <set>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include "message.hpp"
#include "udp_connection.hpp"
#include "mds.h"

/*
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(CUpdate);
BOOST_CLASS_EXPORT(CRequest);
BOOST_CLASS_EXPORT(CResponse);
BOOST_CLASS_EXPORT(CReady);
BOOST_CLASS_EXPORT(CAccept);
*/

// Serialization and smart ptrs
#include <boost/bind.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

using boost::asio::ip::udp;

extern MDS* mds;

class CMessage;
typedef boost::shared_ptr<CMessage> MessagePtr;

class udp_connection;
typedef boost::shared_ptr<udp_connection> udp_connection_ptr;

class peer
{
public:
	peer(std::string hostID)
	{
		host_id = hostID;
	}
	peer(std::string hostID, boost::asio::ip::udp::endpoint end)
	{
		host_id = hostID;
		endpoint = end;
	}
	peer(boost::asio::io_service & ios, const std::string hostID)
	{
		conn = udp_connection_ptr(new udp_connection(ios, 0));
		udp::resolver resolver(ios);
		std::string ip = mds->hosts.find(hostID)->second->getPreferredIp();
		std::string port = std::to_string(MDS_SERVER_PORT);
		udp::resolver::query query(udp::v4(), ip, port);
		try {
			udp::resolver::iterator iterator = resolver.resolve(query);
			endpoint = *iterator;
		} catch (boost::system::system_error &e) {
			throw e;
		}
	}
	~peer() {}

	void send(MessagePtr msg)
	{
		std::vector<MessagePtr> msgs;
		msgs.push_back(msg);

		std::cout << "about to send message: " << msg->cmd_ <<std::endl;
		try {
			conn->async_send_to(msgs, endpoint,
					boost::bind(&peer::handle_write, this,
					boost::asio::placeholders::error));
		} catch ( const std::exception &e ) {
			throw e;
		}
	}

protected:
	friend class server;
	void handle_write(const boost::system::error_code& err)
	{
		if(!err)
		{
			std::cout <<"Write completed without error" << std::endl;
		}
		else
		{
			std::cout << err << std::endl;
			throw boost::system::system_error(err);
		}
		// Since we are not starting a new operation the io_service will run out of
		// work to do and the client will exit.
	}

	std::string host_id;
	udp_connection_ptr conn;
	boost::asio::ip::udp::endpoint endpoint;

private:
	peer();
};

typedef boost::shared_ptr<peer> PeerPtr;
typedef std::set<PeerPtr> PeerSet;

#endif
