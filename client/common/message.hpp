#ifndef TCP_MESSAGE_H__
#define TCP_MESSAGE_H__

#include <stdlib.h>
#include <cstring>
#include <memory>
#include <iostream>
namespace rpc
{
class Message
{
public:
#define MAXDATALEN 4096
enum MSGTYPE {REQUEST = 1, RESPONSE, NOTIFY};

struct Header
{
	uint32_t 	    type;
	uint32_t		length;
	uint32_t		id;
	size_t		    hash;
	uint32_t        error;
	uint32_t        reverse[3];
    Header()
	{
		type = 0;
		length = 0;
		id = 0;
		hash = 0;
		error = 0;
	};

    Header(uint32_t settype, uint32_t setlength, uint32_t setid, size_t hash)
	{
		type = settype;
		length = setlength;
		id = setid;
		hash = hash;
		error = 0;
	};

    Header(const Header& other)
	{
		type = other.type;
		length = other.length;
		id = other.id;
		hash = other.hash;
		error = other.error;
	};

};

public:
    Message()
    {
        m_body_length = (0);
        m_header_length = (sizeof(m_header));
        m_max_body_length = (MAXDATALEN - m_header_length);
    }
	
	Message(Header& header, char *data, size_t len)
    {
	    m_max_body_length = (MAXDATALEN - m_header_length);
	    m_header_length = (sizeof(m_header));
        m_body_length = len;
        m_header = header;
        (void)std::memcpy(m_data, reinterpret_cast<char*>(&m_header), sizeof(m_header));
        (void)std::memcpy(m_data + sizeof(m_header), data, len);
    }

    char* data()
    {
        return m_data;
    }
	
    size_t length() const
    {
        return m_header_length + m_body_length;
    }

    char* body()
    {
        return m_data + m_header_length;
    }

    size_t bodylen() const
    {
        return m_body_length;
    }

    Header& header()
    {
        return m_header;
    }

    size_t headerlen() const
    {
        return m_header_length;
    }

    bool encode_msg(Header& header, char *data, size_t len)
    {
        if (header.length > m_max_body_length)
        {
            return false;
        }

        m_header_length = (sizeof(m_header));
        m_body_length = len;
        m_header = header;
        (void)std::memcpy(m_data, reinterpret_cast<char*>(&m_header), sizeof(m_header));
        (void)std::memcpy(m_data + sizeof(m_header), data, len);
		return true;
    }

    bool decode_msg()
    {
        std::memcpy(&m_header, reinterpret_cast<Header*>(m_data), m_header_length);
        if (m_header.length > m_max_body_length)
        {
            std::cout << "header.length" << m_header.length << std::endl;
			std::cout << "m_max_body_length" << m_max_body_length << std::endl;
            return false;
        }

        m_header_length = (sizeof(m_header));
        m_body_length = m_header.length;

		return true;
    }

    size_t hash()
    {
        return m_header.hash;
    }

	size_t type()
	{
		return m_header.type;
	}

	void settype(MSGTYPE type)
	{
		m_header.type = type;
	}

	size_t id()
	{
		return m_header.id;
	}

	size_t error()
	{
		return m_header.error;
	}

private:
	size_t         m_max_body_length;
	size_t         m_header_length;
    size_t         m_body_length;
	Header         m_header;
    char           m_data[MAXDATALEN];
};
}

#endif
