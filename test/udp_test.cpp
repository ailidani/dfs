

#include "dfs/mds/server.h"
#include "dfs/mds/udp_connection.hpp"
#include "dfs/mds/message.hpp"
#include "dfs/oriopt.h"

mount_ori_config config;
RemoteRepo remoteRepo;
OriPriv *priv;

int main()
{
	server::instance()->run();
	MessagePtr msg = boost::shared_ptr<CMessage>(new CMessage());
	msg->cmd_ = "unlink";
	msg->path_from_ = "test/test/test/test";
	msg->m_type = Update;

	server::instance()->send(msg);

	return 0;
}
