#pragma once

#include <vector>
#include <iostream>

using namespace std;

template< typename Key >
struct Node
{
    Node( Key key, uint64_t parent, uint64_t id = 0, bool is_root = false ):    m_key( key )
                                                                            ,   m_parent_id( parent )
                                                                            ,   m_id( id )
                                                                            ,   m_counter( 1 )
                                                                            ,   m_is_root( is_root )
    {} 

    bool operator<( const Node& node ) const
    {
        return ( m_key < node.key );
    }

    bool operator==( const Node& node ) const
    {
        return (   ( m_key          == node.m_key )
                && ( m_parent_id    == node.m_parent_id )
                && ( m_counter      == node.m_counter )
                && ( m_childs       == node.m_childs )
                && ( m_is_root      == node.m_is_root ) );
    }

    bool operator==( const Key& key_value ) const
    {
        return ( m_key == key_value );
    }

    Key m_key;
    uint64_t m_parent_id;
    uint64_t m_id;
    uint64_t m_counter;
    set< uint64_t > m_childs;
    bool    m_is_root;
};  
