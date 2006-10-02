# Combat Simulator Project
# Copyright 2004 Mark Rose <mkrose@users.sourceforge.net>
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
TRC symbol module: symbol definitions for ply.lex
"""

import string
import copy


# symbols
symbols = (
	'SEMI',
	'COLON',
	'COMMA',
	'SHARP',
	'QUAL',
#	'QUOTE',
	'EQ',
#	'LANGLE',
#	'RANGLE',
	'LBRACE',
	'RBRACE',
	'LBRACKET',
	'RBRACKET',
	'SETTING',
)

# reserved words
reserved = (
	'DEPRECATED',
	'REQUIRED',
	'namespace',
	'using',
	'include',
	'header',
	'type',
	'typedef',
	'enum',
)

# simple and compound types
type_categories = {
	'BOOL_TYPE': ('bool',),
	'INT_TYPE': ('int8', 'int16', 'int32', 'int64', 'uint8', 'uint16', 'uint32'),
	'FLOAT_TYPE': ('float', 'double'),
	'STRING_TYPE': ('std::string',),
	'CSP_TYPE': (
		'csp::Matrix3',
		'csp::Vector3',
		'csp::Quat',
		'csp::Path',
		'csp::Key',
	),
	'GROUP_TYPE': ('group',),
	'BITSET_TYPE': ('bitset',),
	'MESSAGE_TYPE': ('message',),
}

# literals
literals = (
	'ID', 'FCONST', 'SCONST', 'ICONST', #'CCONST',
)

# include paths ("a.h", <a.h>)
include_paths = (
	'FILEPATH_Q',
	'FILEPATH_A'
)

# simple regex's
t_SEMI = r';'
t_COLON = r':'
t_COMMA = r','
t_QUAL = r'::'
#t_QUOTE = r'"'
t_SHARP = r'\#'
t_EQ = r'='
#t_LANGLE = r'<'
#t_RANGLE = r'>'
t_LBRACE = r'{'
t_RBRACE = r'}'
t_LBRACKET = r'\['
t_RBRACKET = r'\]'
t_SETTING = r'@[a-zA-Z_][a-zA-Z_0-9]*'


# Variable id
#t_ID = r'[a-zA-Z_]([a-zA-Z_0-9]*)'

t_ignore = " \t\x0c\r"

# Integer literal
t_ICONST = r'\d+([uU]|[lL]|[uU][lL]|[lL][uU])?'

# Floating literal
t_FCONST = r'((\d+)(\.\d+)(e(\+|-)?(\d+))? | (\d+)e(\+|-)?(\d+))([lL]|[fF])?'

# String literal
t_SCONST = r'\"([^\\\n]|(\\.))*?\"'

# Include file paths
t_FILEPATH_Q = r'\"[a-zA-Z\.\\/0-9_-]+\"'
t_FILEPATH_A = r'\<[a-zA-Z\.\\/0-9_-]+\>'

# Character constant 'c' or L'c'
#t_CCONST = r'(L)?\'([^\\\n]|(\\.))*?\''

# Comments (// style only)
def t_comment(t):
	r' //[^\n]*'
	t.lineno += t.value.count('\n')

priorities = (
	('left', 'FILEPATH_Q', 'FILEPATH_A'),
)

def t_newline(t):
	r'\n+'
	t.lineno += t.value.count("\n")

def t_error(t):
	print "Illegal character '%s'" % t.value[0]
	t.skip(1)

tokens = symbols + literals + reserved + include_paths
tokens = map(string.upper, tokens)

typedefs = {}

all_types = {}
for cat, typelist in type_categories.items():
	tokens.append(cat)
	for typename in typelist:
		all_types[typename] = cat

all_words = copy.copy(all_types)
for word in reserved:
	all_words[word] = word.upper()

def t_ID(t):
	r'[a-zA-Z_]([a-zA-Z_0-9]*)'
	t.type = all_words.get(t.value, 'ID')
	return t


class ParserState:
	def __init__(self, types):
		self.types = types
		self.messages = {}
		self.typedefs = {}

state = ParserState(all_types)


# Build the lexer
import lex
lex.lex()
