#ifndef TCP_MESSAGE_H__
#define TCP_MESSAGE_H__

#include <stdlib.h>
#include <cstring>
#include <memory>
#include <iostream>

namespace sindia
{
class Message
{
public:
#define MAXDATALEN  4096
enum MSGTYPE 
{
    REQUEST = 1, 
	RESPONSE,
	NOTIFY
};

struct Header
{
	uint32_t 	    type;
	uint32_t        length;
	uint32_t		id;
	uint32_t        hash;
	uint32_t        error;
    Header()
	{
		type = 0;
		length = 0;
		id = 0;
		hash = 0;
		error = 0;
	};
};

public:
    Message()
		:m_header_length(sizeof(Header))
		,m_body_length(0)
		,m_header()
    {
        (void)std::memset(m_buffer, 0, sizeof(m_buffer));
    }
	
	Message(const char *data, size_t len)
		:m_header_length(sizeof(Header))
		,m_body_length(len)
		,m_header()
	{
	    (void)std::memset(m_buffer, 0, sizeof(m_buffer));
        (void)std::memcpy(m_buffer, reinterpret_cast<char*>(&m_header), sizeof(m_header));
        (void)std::memcpy(m_buffer + sizeof(m_header), data, len);
		m_header.length = len;
    }

	void set_type(MSGTYPE type)
	{
		m_header.type = type;
		(void)std::memcpy(m_buffer, reinterpret_cast<char*>(&m_header), sizeof(m_header));
	}

	void set_id(uint32_t id)
	{
		m_header.id = id;
		(void)std::memcpy(m_buffer, reinterpret_cast<char*>(&m_header), sizeof(m_header));
	}

	void set_hash(uint32_t hash)
	{
		m_header.hash = hash;
		(void)std::memcpy(m_buffer, reinterpret_cast<char*>(&m_header), sizeof(m_header));
	}

	void set_errcode(uint32_t error)
	{
		m_header.error = error;
		(void)std::memcpy(m_buffer, reinterpret_cast<char*>(&m_header), sizeof(m_header));
	}

    size_t get_hash()
    {
        return m_header.hash;
    }

	size_t get_type()
	{
		return m_header.type;
	}

	size_t get_id()
	{
		return m_header.id;
	}

	size_t get_errcode()
	{
		return m_header.error;
	}

    char* buffer()
    {
        return m_buffer;
    }
	
    size_t buffer_length() const
    {
        return m_header_length + m_body_length;
    }

    char* data()
    {
        return m_buffer + m_header_length;
    }

    size_t data_length() const
    {
        return m_body_length;
    }

    Header& header()
    {
        return m_header;
    }

    size_t header_length() const
    {
        return m_header_length;
    }

    bool parse_header()
    {
        std::memcpy(&m_header, reinterpret_cast<Header*>(m_buffer), m_header_length);
        if ((m_header.length + m_header_length) > MAXDATALEN)
        {
            return false;
        }
        m_body_length = m_header.length;
		return true;
    }

private:
	size_t         m_header_length;
    size_t         m_body_length;
	Header         m_header;
    char           m_buffer[MAXDATALEN];
};
}

#endif
