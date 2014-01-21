#pragma once

#include <vector>
#include <set>
#include <algorithm>

#include "KeySplitter.hpp"
#include "Node.hpp"

using namespace std;

template< typename Key >
class Trie
{
protected:
	typedef Node< Key > 					NodeClass;
	typedef pair< Key, uint64_t > 			Leaf;
	typedef KeySplitter< Key >				Splitter;

public:
	typedef pair< bool, uint64_t > 							Result;
	typedef typename vector< NodeClass >::iterator 			iterator;
	typedef typename vector< NodeClass >::const_iterator 	const_iterator;

	Trie() : m_capacity( 1024 )
	{ 
		m_nodes.reserve( m_capacity );
		m_nodes.push_back( NodeClass( "", 0, 0, true ) );
	}

	~Trie() 
	{
	}

	/* returns number of occurencies of key in a tree, else 1 if it is the first occurency */
	int insert( const Key& value )
	{	
		m_splitter.assign( value );
		size_t id = 0;

		while ( !m_splitter.isEnd( ) )
		{	
			Key to_insert = m_splitter( );
			id = node_insert( id, to_insert ); 
		}

		return 0;
	}

	bool erase_leaf( const Key& value )
	{
		vector< uint64_t > path = find_path( value );

		if ( path.empty( ) )
		{
			return false;
		}
		else
		{
			for ( uint64_t id : path )
			{
				m_nodes[ id ].m_counter--;

				if ( !m_nodes[ id ].m_counter )
				{
					uint64_t parent_id = m_nodes[ id ].m_parent_id;
					m_nodes[ parent_id ].m_childs.erase( id );
					m_free.push_back( id );
					m_leafes.erase( id );
				}
			}

			return true;
		}
	}

	/* clears the tree */
	void clear( ) 
	{
		m_nodes.clear( );
		m_leafes.clear( );
	}

	/* returns number of occurencies of key in a tree, else 0 (root) if not found */
	uint64_t find( const Key& value )
	{
		m_splitter.assign( value );
		size_t id = 0;

		while ( !m_splitter.isEnd( ) )
		{
			Key to_find = m_splitter( );
			id = find_child( to_find, id );
		}

		if ( id )
		{
			return m_nodes[ id ].m_counter;
		}
		else
		{
			return 0;
		}
	}

	uint64_t get_value( uint64_t id ) const
	{
		return m_nodes[ id ].m_counter;
	}

	uint64_t get_capacity( ) const
	{
		return m_capacity;
	}

	uint64_t get_free( ) const
	{
		return m_free.size( );
	}

	uint64_t get_real_size( ) const
	{
		return m_nodes.size( );
	}

	bool operator==( const Trie& tr )
	{
		return ( m_nodes == tr.m_nodes );
	}

	bool operator==( const vector<NodeClass>& tr )
	{
		return ( m_nodes == tr);
	}

	const vector< NodeClass >& get_nodes( ) const
	{
		return m_nodes;
	}

	const set< uint64_t >& get_leafes( ) const
	{
		return m_leafes;
	}

	NodeClass& operator[]( int i ) const
	{
		return m_nodes[ i ];
	}

	size_t size( ) const
	{
		return m_nodes.size( );
	}

	iterator end( )
	{
		return m_nodes.end( );
	}

	const_iterator end() const
	{
		return m_nodes.end( );
	}

	iterator begin( )
	{
		return m_nodes.begin( );
	}

	const_iterator begin( ) const
	{
		return m_nodes.begin( );
	}

protected:

	uint64_t node_insert( uint64_t node_id, Key& key_value )
	{
		for ( uint64_t id : m_nodes[ node_id ].m_childs )
		{
			if ( m_nodes[ id ].m_key == key_value )
			{
				m_nodes[ id ].m_counter++;
				return id;
			}	
		}

		uint64_t id = add_node( key_value, node_id );
		return id;
	}

	uint64_t add_node( Key& key_value, uint64_t parent_id )	
	{
		NodeClass node( key_value, parent_id );
		
		if ( !m_free.empty( ) )
		{
			node.m_id = m_free.front( );
			m_free.erase( m_free.begin( ) );
			m_nodes[ node.m_id ] = node;
		}
		else
		{
			node.m_id = m_nodes.size( );
			m_nodes.push_back( node );
			if ( m_nodes.size( ) == m_capacity )
			{
				m_capacity *= 2;
				m_nodes.reserve( m_capacity );
			}
		}

		m_nodes[ parent_id ].m_childs.insert( m_nodes[ parent_id ].m_childs.end( ), node.m_id );

		/* checking if it was leaf */
		m_leafes.erase( parent_id );
		m_leafes.insert( node.m_id );

		return node.m_id;
	}

	uint64_t find_child( Key& key_value, uint64_t id ) const
	{
		for ( uint64_t i : m_nodes[ id ].m_childs )
		{
			if ( m_nodes[i] == key_value ) 
				return i;
		}

		return 0;
	}

	vector< uint64_t > find_path( const Key& key_value )
	{
		vector< uint64_t > path;
	
		m_splitter.assign( key_value );
		size_t id = 0;

		while ( !m_splitter.isEnd( ) )
		{
			Key to_find = m_splitter( );
			id = find_child( to_find, id );

			if ( id )
				path.push_back( id );
			else
				return vector< uint64_t >( );
		}

		return path;

	}

	/* non needed */
	Trie( Trie const & );
	Trie& operator=( Trie const & );

	Splitter 			m_splitter;
	vector< NodeClass > m_nodes;
	set< uint64_t > 	m_leafes;

	vector< uint64_t > 	m_free;
	size_t				m_capacity;
};


template<>
inline uint64_t Trie< string >::node_insert( uint64_t node_id, string& key_value )
{
	NodeClass node = m_nodes[node_id];

	for (uint64_t id : node.m_childs)
	{
		// need to check if it is a faster way for std::string (hope so)
		if ( m_nodes[ id ].m_key[ 0 ] == key_value[ 0 ] )
		{
			if ( m_nodes[ id ].m_key == key_value )
			{
				m_nodes[ id ].m_counter++;
				return id;
			}
		}	
	}

	uint64_t id = add_node( key_value, node_id );
	return id;
}


class TopTrie : public Trie< string >
{
public:
	const vector< Leaf >& get_leafes_top( )
	{
		build_leafes_top( );
		return m_top;
	}

	const vector< Leaf >& get_top( )
	{
		for ( auto& node : m_nodes )
		{
			m_top.push_back( make_pair( get_key( node.m_id ), node.m_counter ) );
		}
		return m_top;
	}

private:
	string get_key( uint64_t id ) const
	{
		string to_return;

		while ( id )
		{
			to_return.insert( 0, m_nodes[ id ].m_key );
			id = m_nodes[id].m_parent_id;
		}

		return to_return;
	}

	void build_leafes_top( )
	{
		for ( uint64_t i : m_leafes )
		{
			m_top.push_back( make_pair( get_key( i ), m_nodes[ i ].m_counter) );
		}
	}

	vector< Leaf > m_top;
};
