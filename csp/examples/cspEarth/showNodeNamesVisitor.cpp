/*************** <auto-copyright.pl BEGIN do not edit this line> **************
 *
 * osgWorks is (C) Copyright 2009 by Kenneth Mark Bryden
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *************** <auto-copyright.pl END do not edit this line> ***************/

#include <osgDB/ReadFile>
#include <osg/NodeVisitor>

#include <string>

#include "ShowNodeNamesVisitor.h"

void ShowNodeNamesVisitor::apply( osg::Node& n )
{
    int idx;
    for( idx=0; idx<_level; idx++ )
        osg::notify( osg::ALWAYS ) << "  ";

    if( !n.getName().empty() )
        osg::notify( osg::ALWAYS ) << n.getName();
    else
        osg::notify( osg::ALWAYS ) << "NULL";
    osg::notify( osg::ALWAYS ) << " (" << n.className() << ")" << std::endl;

    _level++;
    traverse( n );
    _level--;
}

void ShowNodeNamesVisitor::apply( osg::Geode& n )
{
    if( _showGeodes )
        apply( *( osg::Node* )&n );
}
