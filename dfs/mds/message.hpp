
#ifndef mds_message_hpp
#define mds_message_hpp

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/shared_ptr.hpp>

enum EMessageType
{
	INVALID_TYPE = -1,
	Update = 0,
	Request,
	Response,
	Ready,
	Accept,
	MAX_TYPE
};

enum EAnswerType { No=0, Yes=1 };

struct CMessage
{
public:
	CMessage() :
		m_type(INVALID_TYPE),
		m_seq(0),
		m_group(""),
		m_source(""),
		m_ResponseType(INVALID_TYPE),
		m_Response(No),
		mode_(0)
		{
		};
	CMessage( std::string p_src ) :
		m_type(INVALID_TYPE),
		m_seq(0),
		m_group(""),
		m_source( p_src ),
		m_ResponseType(INVALID_TYPE),
		m_Response(No),
		mode_(0)
		{ };

	virtual ~CMessage() { };

	EMessageType m_type;
	int			 m_seq;
	std::string		m_group; //cluster name
	std::string		m_source; //ip

	EMessageType m_ResponseType;
	EAnswerType  m_Response;

	std::string cmd_;
	std::string path_from_;
	std::string path_to_;
	mode_t mode_;

	// TODO Fix this into the API instead of manual setting
	std::string	 m_hostname;

	//virtual bool operator == ( const CMessage &p_rhs ) const;

protected:
	CMessage( EMessageType p_type, std::string p_group, std::string p_src ) :
		m_type( p_type ), m_group( p_group), m_source( p_src ), m_Response(No)
		{ };

	CMessage( EMessageType p_type ) :
		m_type( p_type ),
		m_seq(0),
		m_source("")
		{
		};

private:
	friend class boost::serialization::access;
	template< class Archive >
	void serialize( Archive & ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_NVP(m_type);
		ar & BOOST_SERIALIZATION_NVP(m_seq);
		ar & BOOST_SERIALIZATION_NVP(m_group);
		ar & BOOST_SERIALIZATION_NVP(m_source);
		ar & BOOST_SERIALIZATION_NVP(m_hostname);
		ar & BOOST_SERIALIZATION_NVP(m_ResponseType);
		ar & BOOST_SERIALIZATION_NVP(m_Response);
		ar & BOOST_SERIALIZATION_NVP(cmd_);
		ar & BOOST_SERIALIZATION_NVP(path_from_);
		ar & BOOST_SERIALIZATION_NVP(path_to_);
		ar & BOOST_SERIALIZATION_NVP(mode_);

	};
};

struct CUpdate : public CMessage
{
	CUpdate() :
		CMessage( Update )
		{};
	CUpdate( std::string p_group, std::string p_src) :
		CMessage( Update, p_group, p_src )
		{ };

private:
	friend class boost::serialization::access;
	template< class Archive >
	void serialize( Archive & ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(CMessage);
	};
};

struct CRequest : public CMessage
{
	CRequest() :
		CMessage(Request)
		{};
	CRequest( std::string p_group, std::string p_src) :
		CMessage( Request, p_group, p_src )
		{};

private:
	friend class boost::serialization::access;
	template< class Archive >
	void serialize( Archive & ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(CMessage);
	};
};


struct CReady : public CMessage
{
public:
	CReady() :
		CMessage( Ready ) {};
	CReady( std::string p_group, std::string p_src) :
		CMessage( Ready, p_group, p_src )
		{};

	//TODO Need some sort of "work vector"
	//	or at least the UpNodes list
private:
	friend class boost::serialization::access;
	template< class Archive >
	void serialize( Archive & ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(CMessage);
	};
};

struct CAccept : public CMessage
{
public:
	CAccept() :
		CMessage( Accept ) {};
	CAccept( std::string p_group, std::string p_src) :
		CMessage( Accept, p_group, p_src )
		{};
private:
	friend class boost::serialization::access;
	template< class Archive >
	void serialize( Archive & ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(CMessage);
	};
};

struct CResponse : public CMessage
{
public:

	CResponse() :
		CMessage( Response ) {};
	CResponse( std::string p_group, std::string p_source, EMessageType p_type, EAnswerType p_ans ) :
		CMessage( Response, p_group, p_source )
		{
			m_ResponseType = p_type;
			m_Response = p_ans;
		};

private:
	friend class boost::serialization::access;
	template< class Archive >
	void serialize( Archive & ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(CMessage);
	};
};

typedef boost::shared_ptr<CMessage> MessagePtr;


#endif /* mds_message_hpp */
