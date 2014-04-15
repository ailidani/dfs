
#ifndef MDS_MASTER_H
#define MDS_MASTER_H

#include <libdfsutil/mutex.h>
#include <libdfs/metadatalog.h>
#include <unordered_map>


class Master
{
public:
	enum
	{
		HANDLE_NOT_DEL = 1,
    	HANDLE_MARKED_FOR_DEL = 2
    };

    Master(ConnectionManagerPtr &, PropertiesPtr &, ServerKeepaliveHandlerPtr &,
         ApplicationQueuePtr &app_queue_ptr);
    ~Master();
    bool is_master(Object obj) {}
    String get_current_master(Object obj) {}

    // MDS command implementations
    void mkdir(ResponseCallback *cb, uint64_t session_id, const char *name, const std::vector<Attribute>& init_attrs);
    void mkdirs(ResponseCallback *cb, uint64_t session_id, const char *name, const std::vector<Attribute>& init_attrs);
	//void unlink(ResponseCallback *cb, uint64_t session_id, const char *name);
	void open(ResponseCallbackOpen *cb, uint64_t session_id, const char *name,
	        uint32_t flags, uint32_t event_mask,
	        std::vector<Attribute> &init_attrs);
	void close(ResponseCallback *cb, uint64_t session_id, uint64_t handle);
	void attr_set(ResponseCallback *cb, uint64_t session_id, uint64_t handle,
	            const char *name, uint32_t oflags, const std::vector<Attribute> &attrs);
	void attr_get(ResponseCallbackAttrGet *cb, uint64_t session_id,
	            uint64_t handle, const char *name, const std::vector<String> &attrs);
	void attr_incr(ResponseCallbackAttrIncr *cb, uint64_t session_id,
	             uint64_t handle, const char *name, const char *attr);
	void attr_del(ResponseCallback *cb, uint64_t session_id, uint64_t handle,
	            const char *name);
	void attr_exists(ResponseCallbackAttrExists *cb, uint64_t session_id, uint64_t handle,
	               const char *name, const char *attr);
	void attr_list(ResponseCallbackAttrList *cb,
	             uint64_t session_id, uint64_t handle);
	void exists(ResponseCallbackExists *cb, uint64_t session_id,
	          const char *name);
	void readdir(ResponseCallbackReaddir *cb, uint64_t session_id,
	           uint64_t handle);
	void readdir_attr(ResponseCallbackReaddirAttr *cb, uint64_t session_id,
	                uint64_t handle, const char *name, const char *attr,
	                bool include_sub_entries);
	void readpath_attr(ResponseCallbackReadpathAttr *cb, uint64_t session_id,
	                 uint64_t handle, const char *name, const char *attr);
	void shutdown(ResponseCallback *cb, uint64_t session_id);
	void lock(ResponseCallbackLock *cb, uint64_t session_id, uint64_t handle,
	        uint32_t mode, bool try_lock);
	void release(ResponseCallback *cb, uint64_t session_id, uint64_t handle);


	/*
	* Creates a new session by allocating a new SessionData object, obtaining a
	* new session ID and inserting the object into the Session map.
	*
	* @param addr Address from which the UDP client is sending
	* @return the new session ID
	*/
	uint64_t create_session(struct sockaddr_in &addr);

	/*
	* Obtains the SessionData object for the given id from the session map.
	*
	* @param session_id Session ID to lookup
	* @param session_data Reference to SessionData smart pointer
	* @return true if found, false otherwise
	*/
	bool get_session(uint64_t session_id, SessionDataPtr &session_data);

	void destroy_session(uint64_t session_id);
	void initialize_session(uint64_t session_id, const String &name);

	void get_datagram_send_address(struct sockaddr_in *addr)
	{
		memcpy(addr, &m_local_addr, sizeof(m_local_addr));
	}

private:
	struct EventContext
	{
		HyperspaceEventPtr event;
		NotificationMap notifications;
		bool persisted_notifications;

		EventContext()
		  : persisted_notifications(false) {}

		EventContext(HyperspaceEventPtr _event)
		  : event(_event), persisted_notifications(false) {}

		void reset() {
			event = 0;
			notifications.clear();
			persisted_notifications = false;
		}
	};

	struct CommandContext {
		const char* friendly_name;
		uint64_t session_id;
		SessionDataPtr session_data;
		BDbTxn *txn;
		std::vector<EventContext> evts;
		bool aborted;
		int error;
		String error_msg;

		CommandContext(const char* _friendly_name, uint64_t _session_id)
		  : friendly_name(_friendly_name), session_id(_session_id), txn(0),
		    aborted(false), error(Error::OK) {}

		void set_error(int _error, const char *_error_msg, bool abort=true) {
			error = _error;
			error_msg = _error_msg;
			aborted = abort;
		}

		void set_error(int _error, const String &_error_msg, bool abort=true) {
			error = _error;
			error_msg = _error_msg;
			aborted = abort;
		}

		void reset_error() {
			aborted = false;
			error = Error::OK;
			error_msg.clear();
		}

		void reset(BDbTxn *_txn) {
			session_data = 0;
			txn = _txn;
			evts.clear();
			reset_error();
		}
	};

	void mkdir(CommandContext &ctx, const char *name);
	void unlink(CommandContext &ctx, const char *name);
	void open(CommandContext &ctx, const char *name,
	        uint32_t flags, uint32_t event_mask,
	        std::vector<Attribute> &init_attrs, uint64_t& handle,
	        bool& created, uint64_t& lock_generation);
	void close(CommandContext &ctx, uint64_t handle);
	void attr_set(CommandContext &ctx, uint64_t handle,
	            const char *name, const std::vector<Attribute> &attrs);
	void attr_get(CommandContext &ctx, uint64_t handle,
	            const char *name, const char *attr, DynamicBuffer &dbuf);
	void attr_get(CommandContext &ctx, uint64_t handle,
	            const char *name, const std::vector<String> &attrs,
	            std::vector<DynamicBufferPtr> &dbufs);
	void attr_incr(CommandContext &ctx, uint64_t handle,
	             const char *name, const char *attr, uint64_t& attr_val);
	void attr_del(CommandContext &ctx, uint64_t handle, const char *name);
	void attr_exists(CommandContext& ctx, uint64_t handle,
	               const char *name, const char *attr, bool& exists);
	void attr_list(CommandContext& ctx, uint64_t handle, std::vector<String>& attributes);
	void exists(CommandContext& ctx, const char *name, bool& file_exists);
	void readdir(CommandContext& ctx, uint64_t handle, std::vector<DirEntry>& listing);
	void readdir_attr(CommandContext& ctx, uint64_t handle, const char *name, const char *attr,
	                bool include_sub_entries, std::vector<DirEntryAttr>& listing);
	void readpath_attr(CommandContext& ctx, uint64_t handle, const char *name, const char *attr,
	                 std::vector<DirEntryAttr>& listing);




	typedef std::vector<SessionDataPtr> SessionDataVec;
	typedef std::unordered_map<uint64_t, SessionDataPtr> SessionMap;

	std::string   m_base_dir;
	std::string   m_lock_file;
	int           m_lock_fd;
	uint32_t      m_generation;
	Mutex         m_session_map_mutex;
	Mutex         m_last_tick_mutex;
	Mutex         m_maintenance_mutex;
	bool          m_maintenance_outstanding;


	typedef std::unordered_map<ObjectHash, Object> ObjectMap;
	MetadataLog m_metadata_log;

};



#endif // MDS_MASTER_H
