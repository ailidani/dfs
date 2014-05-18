

#include "server.h"
#include "../logging.h"

boost::asio::io_service server::io_service;

server::server()
	: peer(MDS::instance().myInfo.getPreferredIp()),
	  Thread()
{
	conn = boost::shared_ptr<udp_connection>(new udp_connection(io_service, MDS_SERVER_PORT));
	endpoint = conn->socket().local_endpoint();
}

/*
void server::handle_write(const boost::system::error_code & err)
{
	if(!err)
	{
		// Write completed without error
		std::cout<< "PEER size: " <<sender_endpoints_.size() <<std::endl;

			conn_->async_receive_from(messages_, endpoint_,
								boost::bind(&server::handle_message, this,
								boost::asio::placeholders::error));
	}
	else
	{
		throw boost::system::system_error(err);
	}
	// Since we are not starting a new operation the io_service will run out of
	// work to do and the client will exit.
}
*/

void server::run()
{
	PeerPtr self(this);
	peers.insert(self);
	try {
		conn->async_receive_from(messages, RemoteEndpoint,
				boost::bind(&server::handle_message, this,
				boost::asio::placeholders::error));
		io_service.run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}

void server::add_peer(const std::string ip)
{
	if(ip == MDS::instance().myInfo.getPreferredIp())
		return;
	PeerPtr tmp;
	try {
		tmp.reset(new peer(io_service, ip));
		peers.insert(tmp);
	} catch (boost::system::system_error &e) {
		if(e.code() == boost::asio::error::host_not_found)
			std::cout << ip << "Host not found." << std::endl;
		else throw e;
	}
}

void server::send_msg(MessagePtr msg)
{
	BOOST_FOREACH(PeerPtr p, peers)
	{
		if(p->ip == MDS::instance().myInfo.getPreferredIp())
			continue;
		p->send(msg);
	}
}

void server::handle_message( const boost::system::error_code& err )
{
	if(!err)
	{
		BOOST_FOREACH(MessagePtr msg, messages)
		{
			std::cout<<"DEBUG: handle_msg : "<< msg->cmd_ <<std::endl;
			process_message(msg);
		}
	}
	else throw boost::system::system_error(err);

	// restart listen
	conn->async_receive_from(messages, RemoteEndpoint ,
						boost::bind(&server::handle_message, this,
						boost::asio::placeholders::error));
}

void server::process_message(MessagePtr message)
{
	if(message->m_type == Response)
	{
		if(message->m_Response == Yes) {}
		else {} // XXX fix the failed operation
	}
	switch(message->m_type)
	{
	case Ready:
		if(message->m_group == MDS::instance().rc.getCluster())
			add_peer(message->m_source);
		break;
	case Update:
	case Request:
		if(message->cmd_.compare("unlink") == 0)
			MDS::instance().mds_unlink(message->path_from_, false);
		else if(message->cmd_.compare("symlink") == 0)
			MDS::instance().mds_symlink(message->path_from_, message->path_to_, false);
		else if(message->cmd_.compare("rename") == 0)
			MDS::instance().mds_rename(message->path_from_, message->path_to_, false);
		else if(message->cmd_.compare("mkdir") == 0) {
			std::cout<<"Made it before MDS::get()->mds_mkdir()"<<std::endl;
			MDS::instance().mds_mkdir(message->path_from_, message->mode_, false);
		}
		else if(message->cmd_.compare("rmdir") == 0)
			MDS::instance().mds_rmdir(message->path_from_, false);
		else FUSE_LOG("process_message Error: Unknown command\n");
		break;
	case INVALID_TYPE:
	default:
		FUSE_LOG("process_message Error: message invalid_type\n");
		break;
	}
}

