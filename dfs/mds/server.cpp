

#include "server.h"

boost::asio::io_service server::io_service_;

server * server::server_ = 0;

server::server()
{
	//io_service_ = boost::asio::io_service;
	conn_ = udp_connection_ptr( new udp_connection(io_service_, MDS_SERVER_PORT));
	BOOST_FOREACH(udp::endpoint peer, sender_endpoints_)
	{
		conn_->async_receive_from(messages_, peer,
				boost::bind(&server::handle_message, this,
				boost::asio::placeholders::error));
	}
}

void server::send(MessagePtr p_msg)
{
	std::vector<MessagePtr> msgs;
	msgs.push_back(p_msg);
	std::map<std::string, HostInfo *>::iterator it;
	for(it = MDS::get()->hosts.begin(); it!=MDS::get()->hosts.end(); it++)
	{
		try {
			udp::resolver resolver(io_service_);
			udp::resolver::query query(udp::v4(), it->second->getPreferredIp(), std::to_string(MDS_SERVER_PORT));
			udp::resolver::iterator iterator = resolver.resolve(query);
			sender_endpoints_.insert(*iterator);
		} catch ( const std::exception &e ) {
			FUSE_LOG("server Error: %s", e.what());
		}
	}
	std::set<udp::endpoint>::iterator iterator;
	try {
		for(iterator=sender_endpoints_.begin(); iterator!=sender_endpoints_.end(); iterator++)
		{
			conn_->async_send_to(msgs, *iterator ,
					boost::bind(&server::handle_write, this,
							boost::asio::placeholders::error));
		}
	} catch (std::exception &e) {
			FUSE_LOG("server send Error: %s", e.what());
	}
}

void server::handle_write(const boost::system::error_code & err)
{
	if(!err)
	{
		// Write completed without error
	}
	else
	{
		throw boost::system::system_error(err);
	}
	// Since we are not starting a new operation the io_service will run out of
	// work to do and the client will exit.
}

void server::handle_message( const boost::system::error_code& err )
{
	if(!err)
	{
		BOOST_FOREACH(MessagePtr msg, messages_)
		{
			process_message(msg);
		}
	}
	else throw boost::system::system_error(err);
	// restart listen
	BOOST_FOREACH(udp::endpoint peer, sender_endpoints_)
	{
		conn_->async_receive_from(messages_, peer,
				boost::bind(&server::handle_message, this,
				boost::asio::placeholders::error));
	}
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
	case Update:
	case Request:
		if(message->cmd_ == "unlink")
			MDS::get()->mds_unlink(message->path_from_, false);
		else if(message->cmd_ == "symlink")
			MDS::get()->mds_symlink(message->path_from_, message->path_to_, false);
		else if(message->cmd_ == "rename")
			MDS::get()->mds_rename(message->path_from_, message->path_to_, false);
		else if(message->cmd_ == "mkdir")
			MDS::get()->mds_mkdir(message->path_from_, message->mode_, false);
		else if(message->cmd_ == "rmdir")
			MDS::get()->mds_rmdir(message->path_from_, false);
		else FUSE_LOG("process_message Error: Unknown command\n");
		break;
	case INVALID_TYPE:
	default:
		FUSE_LOG("process_message Error: message invalid_type\n");
		break;
	}
}

