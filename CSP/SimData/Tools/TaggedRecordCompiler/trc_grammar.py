# SimData: Data Infrastructure for Simulations
# Copyright 2004 Mark Rose <mkrose@users.sourceforge.net>
#
# This file is part of SimData.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

"""
TRC grammar module: input grammar definition for ply.yacc
"""

import sys
from trc_lex import tokens, state
import trc_gen


# top-level (list of declarations):

def p_toplevel_1(p):
	'toplevel : header declaration_list'
	p[0] = [p[1]] + p[2]

def p_toplevel_2(p):
	'toplevel : declaration_list'
	print '#header must be the first declaration'
	sys.exit(1)

# declaration_list:

def p_declaration_list_1(p):
	'declaration_list : declaration'
	p[0] = [p[1]]

def p_declaration_list_2(p):
	'declaration_list : declaration_list declaration'
	p[0] = p[1] + [p[2]]

# declaration:

def p_declaration(p):
	'''declaration : message
	               | include
	               | type
	               | typedef
	               | using
	'''
	p[0] = p[1]

def p_declaration_1(p):
	'declaration : NAMESPACE ID LBRACE declaration_list RBRACE'
	p[0] = trc_gen.Namespace(p[2], p[4])

def p_declaration_err1(p):
	'declaration : header'
	print '#header multiply defined'
	sys.exit(1)

# set the namespace for all declarations

def p_using(p):
	'using : USING qualified_id SEMI'
	p[0] = trc_gen.Using(p[2])


# specify the include path for the generated header

def p_header(p):
	'header : SHARP HEADER pathspec'
	p[0] = trc_gen.Header(p[3])


# include additional headers in the generated header

def p_include(p):
	'include : SHARP INCLUDE pathspec'
	p[0] = trc_gen.Include(p[3])

# type shorthand (used internally)

def p_typedef(p):
	'typedef : TYPEDEF qualified_id ID SEMI'
	state.typedefs[p[3]] = p[2]
	p[0] = '__IGNORE__'

# declare external types

def p_type(p):
	'type : TYPE qualified_id SEMI'
	state.types[p[2]] = 'EXTERN'
	p[0] = '__IGNORE__'

# a file path specification for include files (e.g. "a.h" or <a.h>)

def p_pathspec(p):
	'''pathspec : FILEPATH_Q
	            | FILEPATH_A
	'''
	p[0] = p[1]

# message definition

def p_message_0(p):
	'message : MESSAGE_TYPE ID LBRACE RBRACE'
	name = p[2]
	p[0] = trc_gen.Message(name, [], state=state)

def p_message_1(p):
	'message : MESSAGE_TYPE ID LBRACE message_body RBRACE'
	name = p[2]
	body = p[4]
	p[0] = trc_gen.Message(name, body, state=state)

def p_message_2(p):
	'message : MESSAGE_TYPE ID COLON qualified_id LBRACE message_body RBRACE'
	name = p[2]
	base = p[4]
	body = p[6]
	p[0] = trc_gen.Message(name, body, base=base, state=state)

def p_message_body_1(p):
	'message_body : message_element'
	p[0] = [p[1]]

def p_message_body_2(p):
	'message_body : message_body message_element'
	p[0] = p[1] + [p[2]]

def p_mesage_element(p):
	'''message_element : annotated_field
	                   | message_setting
	                   | group
	                   | enum
	'''
	p[0] = p[1]

def p_annotated_field(p):
	'annotated_field : field'
	p[0] = p[1]

def p_annotated_field_required(p):
	'annotated_field : REQUIRED field'
	p[2].required = 1
	p[0] = p[2]

def p_annotated_field_deprecated(p):
	'annotated_field : DEPRECATED field'
	p[2].deprecated = 1
	p[0] = p[2]

def p_message_setting(p):
	'message_setting : SETTING EQ expression SEMI'
	p[0] = trc_gen.Option(p[1], p[3])

# compound fields

def p_field_compound(p):
	'field : bitset'
	p[0] = p[1]

def p_group(p):
	'group : GROUP_TYPE ID LBRACE group_body RBRACE'
	p[0] = trc_gen.Group(p[2], p[4], state)

def p_group_body_0(p):
	'group_body : annotated_field'
	p[0] = [p[1]]

def p_group_body_1(p):
	'group_body : group_body annotated_field'
	p[0] = p[1] + [p[2]]

def p_bitset(p):
	'bitset : BITSET_TYPE LBRACE bitset_body RBRACE'
	p[0] = trc_gen.Bitset(p[3])

def p_bitset_body_0(p):
	'bitset_body : field'
	if p[1].type.category != 'BOOL_TYPE':
		print 'Expected bool at line %d' % p.lineno(1)
		sys.exit(1)
	p[0] = [p[1]]

def p_bitset_body_1(p):
	'bitset_body : bitset_body field'
	if p[2].type.category != 'BOOL_TYPE':
		print 'Expected bool at line %d' % p.lineno(2)
		sys.exit(1)
	p[0] = p[1] + [p[2]]

# simple fields

def p_field_scalar(p):
	'field : simple_type ID SEMI'
	p[0] = trc_gen.Field(p[1], p[2])

def p_field_default(p):
	'field : simple_type ID EQ expression SEMI'
	p[0] = trc_gen.Field(p[1], p[2], default=p[4])

def p_field_array(p):
	'field : simple_type LBRACKET RBRACKET ID SEMI'
	p[0] = trc_gen.Field(p[1], p[4], array=1)

# simple types

def p_simple_type(p):
	'''simple_type : BOOL_TYPE
	               | INT_TYPE
	               | FLOAT_TYPE
	               | STRING_TYPE
	               | SIMDATA_TYPE
	               | qualified_type
	'''
	p[0] = trc_gen.Type(state.types[p[1]], p[1])

def p_qualified_type(p):
	'''qualified_type : qualified_id'''
	p[0] = state.typedefs.get(p[1], p[1])
	if not state.types.has_key(p[0]):
		print 'Unknown type %s at line %d' % (p[1], p.lineno(1))
		sys.exit(1)

# enums

def p_enum(p):
	'enum : ENUM LBRACE enum_body RBRACE'
	p[0] = trc_gen.Enum(p[3])

def p_enum_body_0(p):
	'enum_body : enum_body COMMA enum_value'
	p[0] = p[1] + [p[3]]

def p_enum_body_1(p):
	'enum_body : enum_value'
	p[0] = [p[1]]

def p_enum_value_0(p):
	'enum_value : ID EQ ICONST'
	p[0] = trc_gen.EnumValue(p[1], p[3])

def p_enum_value_1(p):
	'enum_value : ID'
	p[0] = trc_gen.EnumValue(p[1])

# expressions

def p_expression(p):
	'''expression : SCONST
	              | FCONST
	              | ICONST
	'''
	p[0] = p[1]

# ids

def p_qualified_id(p):
	'''qualified_id : ID
	                | ID QUAL qualified_id
	'''
	if len(p) == 2:
		p[0] = p[1]
	else:
		p[0] = '%s::%s' % (p[1], p[3])


def p_error(p):
	print 'Syntax error on line %d, at or before "%s"' % (p.lineno, p.value)
	sys.exit(1)


import yacc
yacc.yacc()

