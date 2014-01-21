#pragma once

#include <string>
#include <array>
#include <iostream>
#include <vector>

using namespace std;

namespace
{
	static const string special_symbols = "./";
	static const char special_symbol_after_domain = '/';
}

template< typename Key > 
class KeySplitter
{
public:
	Key operator( )( const Key& for_split );
}; 

template<>
class KeySplitter< string >
{
public:
	KeySplitter( const string& key ): 	m_for_split( key )
									, 	m_found( 0 )
									, 	m_end( false )
									,	m_slash( false )
	{}

	KeySplitter( ):	m_for_split( "" )
				,	m_found( 0 )
				,	m_end( false )
				,	m_slash( false )
	{}

	void assign( const string& key )
	{
		m_for_split = key;
		m_found = 0;
		m_end = false;
		m_slash = false;
	}

	bool isEnd( ) const
	{
		return m_end;
	}

	string operator()( )
	{	
		for ( size_t i = 0; i < special_symbols.size( ); ++i )
		{	
			size_t pos = string::npos;
			
			if ( !m_slash )
			{
				pos = m_for_split.find_first_of( special_symbols, m_found);

				if ( m_for_split[ pos ] == special_symbol_after_domain )
					m_slash = true;
			}
			else
			{
				pos = m_for_split.find( special_symbol_after_domain, m_found );
			}

			if ( pos != string::npos )
			{
				string result;
				if ( m_found == 0 )
					result = m_for_split.substr( m_found, pos - m_found );
				else
					result = m_for_split.substr( m_found - 1, pos - m_found + 1 );
					
				m_found = pos + 1;
				return result;	
			}
		}

		m_end = true;

		if ( m_found == 0 )
			return m_for_split.substr( m_found );
		else
			return m_for_split.substr( m_found - 1 );
	}

	vector< string > get_splitted_vector( )
	{
		vector< string > result;
		while ( !this->isEnd( ) )
		{
			result.push_back( this->operator()( ) );
		}
		return result;
	}

private:
	string 	m_for_split;
	size_t 	m_found;
	bool 	m_end;
	bool 	m_slash;
};
