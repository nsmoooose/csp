// Copyright (C) 1999-2003 Open Source Telecom Corporation.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// As a special exception to the GNU General Public License, permission is 
// granted for additional uses of the text contained in its release 
// of Common C++.
// 
// The exception is that, if you link the Common C++ library with other
// files to produce an executable, this does not by itself cause the
// resulting executable to be covered by the GNU General Public License.
// Your use of that executable is in no way restricted on account of
// linking the Common C++ library code into it.
//
// This exception does not however invalidate any other reasons why
// the executable file might be covered by the GNU General Public License.
// 
// This exception applies only to the code released under the 
// name Common C++.  If you copy code from other releases into a copy of
// Common C++, as the General Public License permits, the exception does
// not apply to the code that you add in this way.  To avoid misleading
// anyone as to the status of such modified files, you must delete
// this exception notice from them.
// 
// If you write modifications of your own for Common C++, it is your choice
// whether to permit this exception to apply to your modifications.
// If you do not wish that, delete this exception notice.

/**
 * @file misc.h
 * @short Memory management, configuration keydata objects and string
 * tokenizer.
 **/

#ifndef	CCXX_MISC_H_
#define	CCXX_MISC_H_

#ifndef CCXX_CONFIG_H_
#include <cc++/config.h>
#endif

#ifndef CCXX_THREAD_H_
#include <cc++/thread.h>
#endif

#include <fstream>
#include <iostream>

#define	KEYDATA_INDEX_SIZE	97
#define	KEYDATA_PAGER_SIZE	512
#define KEYDATA_PATH_SIZE	256

#ifdef	CCXX_NAMESPACES
namespace ost {
#endif


#ifdef WIN32
class CCXX_CLASS_EXPORT MemPager;
class CCXX_CLASS_EXPORT SharedMemPager;
#endif

/**
 * The memory pager is used to allocate cumulative memory pages for
 * storing object specific "persistant" data that is presumed to persist
 * during the life of a given derived object.  When the object is
 * destroyed, all accumulated data is automatically purged.
 * 
 * There are a number of odd and specialized utility classes found in Common
 * C++.  The most common of these is the "MemPager" class.  This is basically
 * a class to enable page-grouped "cumulative" memory allocation; all
 * accumulated allocations are dropped during the destructor.  This class has
 * found it's way in a lot of other utility classes in Common C++.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short Accumulative object memory allocator.
 */
class MemPager
{
private:
	unsigned int pagesize;
	unsigned int pages;

	struct _page
	{
		struct _page *next;
		int used;
	} *page;

protected:
	/**
	 * Allocate first workspace from paged memory.  This method
	 * scans all currently allocated blocks for available space
	 * before adding new pages and hence is both slower and more
	 * efficient.
	 *
	 * @param size size of memory to allocate.
	 * @return pointer to allocated memory.
	 */
	virtual void* first(size_t size);

	/**
	 * Allocate memory from either the currently active page, or
	 * allocate a new page for the object.
	 *
	 * @param size size of memory to allocate.
	 * @return pointer to allocated memory.
	 */
	virtual void* alloc(size_t size);

	/**
	 * Allocate a string from the memory pager pool and copy the
	 * string into it's new memory area.  This method allocates
	 * memory by first searching for an available page, and then
	 * allocating a new page if no space is found.
	 *
	 * @param str string to allocate and copy into paged memory pool.
	 * @return copy of string from allocated memory.
	 */
	char* first(char *str);

	/**
	 * Allocate a string from the memory pager pool and copy the
	 * string inti it's new memory area.  This checks only the
	 * last active page for available space before allocating a
	 * new page.
	 *
	 * @param str string to allocate and copy into paged memory pool.
	 * @return copy of string from allocated memory.
	 */
	char* alloc(char *str);

	/**
	 * Create a paged memory pool for cumulative storage.  This
	 * pool allocates memory in fixed "pagesize" chunks.  Ideal
	 * performance is achived when the pool size matches the
	 * system page size.  This pool can only exist in derived
	 * objects.
	 *
	 * @param pagesize page size to allocate chunks.
	 */
	MemPager(int pagesize = 4096);

	/**
	 * purge the current memory pool.
	 */
	void purge(void);

	/**
	 * Delete the memory pool and all allocated memory.
	 */
	virtual ~MemPager();

public:
	/**
	 * Return the total number of pages that have been allocated
	 * for this memory pool.
	 *
	 * @return number of pages allocated.
	 */
	inline int getPages(void)
		{return pages;};
};

/**
 * The shared mempager uses a mutex to protect key access methods.
 * This class is used when a mempager will be shared by multiple
 * threads.
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short mutex protected memory pager.
 */
class SharedMemPager : public MemPager, public Mutex
{
protected:
	/**
	 * Create a mempager mutex pool.
	 *
	 * @param pagesize page size for allocation.
	 */
	SharedMemPager(int pagesize = 4096);

	/**
	 * Purge the memory pool while locked.
 	 */
	void purge(void);

	/**
	 * Get the first memory page after locking.
	 *
	 * @return allocated memory space.
	 * @param size of request.
	 */
	void* first(size_t size);

	/**
	 * Get the last memory page after locking.
	 *
	 * @return allocated memory space.
	 * @param size of request.
	 */
	void* alloc(size_t size);
};

/**
 * Keydata objects are used to load and hold "configuration" data for
 * a given application.
 *
 * This class is used to load and then hold "<code>keyword = value</code>" pairs parsed from a text
 * based "config" file that has been divided into "<code>[sections]</code>". The syntax is:
 *
 * <code><pre>
 * [section_name]
 * key1=value1
 * key2=value2</pre></code>
 *
 * Essentially, the "path" is a "keypath" into a theoretical namespace of key
 * pairs, hence one does not use "real" filepaths that may be OS dependent. The "<code>/</code>" path refers
 * to "<code>/etc</code>" prefixed (on UNIX) directories and this is processed within the constructor. It
 * could refer to the <code>/config</code> prefix on QNX, or even, gasp, a "<code>C:\WINDOWS</code>". Hence, a keypath of
 * "<code>/bayonne.d/vmhost/smtp</code>" actually resolves to a "<code>/etc/bayonne.d/vmhost.conf</code>" and loads key
 * value pairs from the <code>[smtp]</code> section of that <code>.conf</code> file. 
 *
 * Similarly, something like "<code>~bayonne/smtp</code>" path refers to a "<code>~/.bayonnerc</code>" and loads key pairs
 * from the <code>[smtp]</code> section. This coercion occurs before the name is passed to the open call. 
 *
 * I actually use derived keydata based classes as global initialized objects, and they hence
 * automatically load and parse config file entries even before "main" has started. 
 *
 * Keydata can hold multiple values for the same key pair.  This can
 * occur either from storing a "list" of data items in a config file,
 * or when overlaying multiple config sources (such as <code>/etc/....conf</code> and
 * <code>~/.confrc</code> segments) into a single object.  The keys are stored as
 * cumulative (read-only/replacable) config values under a hash index
 * system for quick retrieval.
 * 
 * Keydata can
 * also load a table of "initialization" values for keyword pairs that were
 * not found in the external file.
 *
 * One typically derives an application specific keydata class to load a
 * specific portion of a known config file and initialize it's values.  One
 * can then declare a global instance of these objects and have
 * configuration data initialized automatically as the executable is loaded.
 * 
 * Hence, if I have a "[paths]" section in a "<code>/etc/server.conf?</code>" file, I might
 * define something like:
 *
 * <code><pre>
 * class KeyPaths : public Keydata
 * {
 *   public:
 *     KeyPaths() : Keydata("/server/paths")
 *     {
 *       static Keydata::Define *defvalues = {
 * 	  {"datafiles", "/var/server"},
 * 	  {NULL, NULL}};
 * 
 *       // override with [paths] from "~/.serverrc" if avail.
 * 
 *       load("~server/paths");
 *       load(defvalues);
 *     }
 * };
 * 
 * KeyPaths keypaths;
 * </pre></code>
 *
 * @author David Sugar <dyfet@ostel.com>
 * @short load text configuration files into keyword pairs.
 */
class Keydata : protected MemPager
{
public:
#if defined(__GNUC__) || !defined(__hpux)
#pragma pack(1)
#endif

	struct Keyval
	{
		Keyval *next;
		char val[1];
	};

	struct Keysym
	{
		Keysym *next;
		Keyval *data;
		const char **list;
		short count;
		char sym[1];
	};

	struct Define
	{
		char *keyword;
		char *value;
	};

#if defined(__GNUC__) || !defined(__hpux)
#pragma pack()
#endif

private:
	static std::ifstream *cfgFile;
	static char lastpath[KEYDATA_PATH_SIZE + 1];
	static int count, sequence;

	int link;

	Keysym *keys[KEYDATA_INDEX_SIZE];

	/**
	 * Compute a hash key signature id for a symbol name.
	 *
	 * @return key signature index path.
	 * @param sym symbol name.
	 */
	unsigned getIndex(const char *sym);

protected:
	CCXX_MEMBER_EXPORT(Keysym*) getSymbol(const char *sym, bool create);

	/**
	 * Load additional key values into the currrent object from
	 * the specfied config source (a config file/section pair).
	 * These values will overlay the current keywords when matches
	 * are found.  This can be used typically in a derived config
	 * object class constructor to first load a <code>/etc</code> section, and
	 * then load a matching user specific entry from <code>~/.</code> to override
	 * default system values with user specific keyword values.
	 *
	 * @param keypath (filepath/section)
	 * @param environment string
	 */ 
	CCXX_MEMBER_EXPORT(void) load(const char *keypath, 
		  const char *environment = "CONFIG_KEYDATA");

	/**
	 * Load additional key values into the currrent object from
	 * the specfied config source (a config file/section pair).
	 * These values will overlay the current keywords when matches
	 * are found.  This can be used typically in a derived config
	 * object class constructor to first load a <code>/etc</code> section, and
	 * then load a matching user specific entry from <code>~/.</code> to override
	 * default system values with user specific keyword values.
	 * This varient puts a prefix in front of the key name.
	 *
	 * @param prefix
	 * @param keypath (filepath/section)
	 * @param environment optional environment variable that
	 * specifies the home path.
	 */ 
	CCXX_MEMBER_EXPORT(void) loadPrefix(const char *prefix,
		  const char *keypath, const char *environment = "CONFIG_KEYDATA");

	/**
	 * Load default keywords into the current object.  This only
	 * loads keyword entries which have not already been defined
	 * to reduce memory usage.  This form of Load is also commonly
	 * used in the constructor of a derived Keydata class.
	 *
	 * @param pairs list of NULL terminated default keyword/value pairs.
	 */
	CCXX_MEMBER_EXPORT(void) load(Define *pairs);
	
public:
	/**
	 * Create an empty key data object.
	 */
	CCXX_MEMBER_EXPORT(CCXX_EMPTY) Keydata();

	/**
	 * Create a new key data object and use "Load" method to load an
	 * initial config file section into it.
	 *
	 * @param keypath (filepath/section)
	 * @param environment optional environment variable that
	 * specifies the home path.
	 */
	CCXX_MEMBER_EXPORT(CCXX_EMPTY) Keydata(const char *keypath, const char *environment="CONFIG_KEYDATA");

	/**
	 * Destroy the keydata object and all allocated memory.  This
	 * may also clear the "cache" file stream if no other keydata
	 * objects currently reference it.
	 */
	CCXX_MEMBER_EXPORT(virtual) ~Keydata();

	/**
	 * Unlink the keydata object from the cache file stream.  This
	 * should be used if you plan to keepa Keydata object after it
	 * is loaded once all keydata objects have been loaded, otherwise
	 * the cfgFile stream will remain open.  You can also use
	 * endKeydata().
	 */
	CCXX_MEMBER_EXPORT(void) unlink(void);

	/**
	 * Get a count of the number of data "values" that is associated
	 * with a specific keyword.  Each value is from an accumulation of
	 * "<code>load()</code>" requests.
	 *
	 * @param sym keyword symbol name.
	 * @return count of values associated with keyword.
	 */
	CCXX_MEMBER_EXPORT(int) getCount(const char *sym);

	/**
	 * Get the first data value for a given keyword.  This will
	 * typically be the <code>/etc</code> set global default.
	 *
	 * @param sym keyword symbol name.
	 * @return first set value for this symbol.
	 */
	CCXX_MEMBER_EXPORT(const char*) getFirst(const char *sym);

	/**
	 * Get the last (most recently set) value for a given keyword.
	 * This is typically the value actually used.
	 *
	 * @param sym keywork symbol name.
	 * @return last set value for this symbol.
	 */
	CCXX_MEMBER_EXPORT(const char*) getLast(const char *sym);

	/**
	 * Get an index array of ALL keywords that are stored by the
	 * current keydata object.
	 *
	 * @return number of keywords found.
	 * @param data pointer of array to hold keyword strings.
	 * @param max number of entries the array can hold.
	 */
	CCXX_MEMBER_EXPORT(unsigned) getIndex(char **data, unsigned max);

	/**
	 * Get the count of keyword indexes that are actually available
	 * so one can allocate a table to receive getIndex.
	 *
	 * @return number of keywords found.
	 */
	CCXX_MEMBER_EXPORT(unsigned) getCount(void);

	/**
	 * Set (replace) the value of a given keyword.  This new value
	 * will become the value returned from getLast(), while the
	 * prior value will still be stored and found from <code>getList()</code>.
	 *
	 * @param sym keyword name to set.
	 * @param data string to store for the keyword.
	 */
	CCXX_MEMBER_EXPORT(void) setValue(const char *sym, const char *data);

	/**
	 * Return a list of all values set for the given keyword
	 * returned in order.
	 *
	 * @return list pointer of array holding all keyword values.
	 * @param sym keyword name to fetch.
	 */
	CCXX_MEMBER_EXPORT(const char * const*) getList(const char *sym);

	/**
	 * Clear all values associated with a given keyword.  This does
	 * not de-allocate the keyword from memory, however.
	 *
	 * @return keyword name to clear.
	 */
	CCXX_MEMBER_EXPORT(void) clrValue(const char *sym);

	/**
	 * A convient notation for accessing the keydata as an associative
	 * array of keyword/value pairs through the [] operator.
	 */
	inline const char *operator[](const char *keyword)
		{return getLast(keyword);};

	/**
	 * Shutdown the file stream cache.  This should be used before
	 * detaching a deamon, <code>exec()</code>, <code>fork()</code>, etc.
	 */
	friend CCXX_EXPORT(void) endKeydata(void);	
};

/**
 * Splits delimited string into tokens.
 *
 * The StringTokenizer takes a pointer to a string and a pointer
 * to a string containing a number of possible delimiters.
 * The StringTokenizer provides an input forward iterator which allows
 * to iterate through all tokens. An iterator behaves like a logical
 * pointer to the tokens, i.e. to shift to the next token, you've 
 * to increment the iterator, you get the token by dereferencing the
 * iterator.
 *
 * Memory consumption:
 * This class operates on the original string and only allocates memory
 * for the individual tokens actually requested, so this class 
 * allocates at maximum the space required for the longest token in the 
 * given string.
 * Since for each iteration, memory is reclaimed for the last token,
 * you MAY NOT store pointers to them; if you need them afterwards,
 * copy them. You may not modify the original string while you operate
 * on it with the StringTokenizer; the behaviour is undefined in that
 * case.
 *
 * The iterator has one special method 'nextDelimiter()' which returns
 * a character containing the next delimiter following this
 * tokenization process or '\0', if there are no following delimiters. In
 * case of skipAllDelim, it returns the FIRST delimiter.
 *
 * With the method 'setDelimiters(const char*)' you may change the
 * set of delimiters. It affects all running iterators.
 *
 * Example:
 * <code><pre>
 *  StringTokenizer st("mary had a little lamb;its fleece was..", " ;");
 *  StringTokenizer::iterator i;
 *  for (i = st.begin() ; i != st.end() ; ++i) {
 *        cout << "Token: '" << *i << "'\t";
 *        cout << " next Delim: '" << i.nextDelimiter() << "'" << endl;
 *  }
 *  </pre></code>
 *
 * @author Henner Zeller <H.Zeller@acm.org>
 * @license LGPL
 */
class CCXX_CLASS_EXPORT StringTokenizer {
public:
	/**
	 * a delimiter string containing all usual whitespace delimiters.
	 * These are space, tab, newline, carriage return,
	 * formfeed and vertical tab. (see isspace() manpage).
	 */
	static const char * const SPACE;

	/**
	 * Exception thrown, if someone tried to read beyond the
	 * end of the tokens.
	 * Will not happen if you use it the 'clean' way with comparison
	 * against end(), but if you skip some tokens, because you 'know'
	 * they are there. Simplifies error handling a lot, since you can
	 * just read your tokens the way you expect it, and if there is some
	 * error in the input this Exception will be thrown.
	 */
	// maybe move more global ?
	class NoSuchElementException { };

	/**
	 * The input forward iterator for tokens.
	 * @author Henner Zeller
	 */
	class CCXX_CLASS_EXPORT iterator {
		friend class StringTokenizer;  // access our private constructors
	private:
		const StringTokenizer *myTok; // my StringTokenizer
		const char *start;      // start of current token
		const char *tokEnd;     // end of current token (->nxDelimiter)
		const char *endp;       // one before next token
		char *token;            // allocated token, if requested

		// for initialization of the itEnd iterator
		iterator(const StringTokenizer &tok, const char *end) 
			: myTok(&tok),tokEnd(0),endp(end),token(0) {}

		iterator(const StringTokenizer &tok)
			: myTok(&tok),tokEnd(0),endp(myTok->str-1),token(0) {
			++(*this); // init first token.
		}

	public:
		iterator() : myTok(0),start(0),tokEnd(0),endp(0),token(0) {}

		// see also: comment in implementation of operator++
		virtual ~iterator() { if (token) *token='\0'; delete [] token; }
		
		/**
		 * copy constructor.
		 */
		// everything, but not responsible for the allocated token.
		iterator(const iterator& i) :
			myTok(i.myTok),start(i.start),tokEnd(i.tokEnd),
			endp(i.endp),token(0) {}

		/**
		 * assignment operator.
		 */
		// everything, but not responsible for the allocated token.
		iterator &operator = (const iterator &i) {
			myTok = i.myTok; 
			start = i.start; endp = i.endp; tokEnd = i.tokEnd;
			if ( token )
				delete [] token;
			token = 0;
			return *this;
		}

		/**
		 * shifts this iterator to the next token in the string.
		 */
		iterator &operator ++ () THROWS (NoSuchElementException);

		/**
		 * returns the immutable string this iterator
		 * points to or '0' if no token is available (i.e.
		 * i == end()).
		 * Do not store pointers to this token, since it is
		 * invalidated for each iteration. If you need the token,
		 * copy it (e.g. with strdup());
		 */
		const char*  operator *  () THROWS (NoSuchElementException);
		
		/**
		 * returns the next delimiter after the current token or
		 * '\0', if there are no following delimiters.
		 * It returns the very next delimiter (even if 
		 * skipAllDelim=true).
		 */
		inline char nextDelimiter() const {
			return (tokEnd) ? *tokEnd : '\0';
		}
		
		/**
		 * compares to other iterator. Usually used to
		 * compare against the end() iterator.
		 */
		// only compare the end-position. speed.
		inline bool operator == (const iterator &other) const { 
			return (endp == other.endp);
		}

		/**
		 * compares to other iterator. Usually used to
		 * compare against the end() iterator.
		 */
		// only compare the end position. speed.
		inline bool operator != (const iterator &other) const { 
			return (endp != other.endp);
		}
	};
private:
	friend class StringTokenizer::iterator;
	const char *str;
	const char *delim;
	bool skipAll, trim;
	iterator itEnd;

public:
	/**
	 * creates a new StringTokenizer for a string
	 * and a given set of delimiters.
	 *
	 * @param  str          String to be split up. This string will
	 *                      not be modified by this StringTokenizer,
	 *                      but you may as well not modfiy this string
	 *                      while tokenizing is in process, which may
	 *                      lead to undefined behaviour.
	 *
	 * @param  delim        String containing the characters
	 *                      which should be regarded as delimiters.
	 *
	 * @param  skipAllDelim OPTIONAL. 
	 *                      true, if subsequent
	 *                      delimiters should be skipped at once
	 *                      or false, if empty tokens should
	 *                      be returned for two delimiters with
	 *                      no other text inbetween. The first
	 *                      behaviour may be desirable for whitespace
	 *                      skipping, the second for input with
	 *                      delimited entry e.g. /etc/passwd like files
	 *                      or CSV input.
	 *                      NOTE, that 'true' here resembles the 
	 *                      ANSI-C strtok(char *s,char *d) behaviour.
	 *                      DEFAULT = false
	 *
	 * @param trim          OPTIONAL. 
	 *                      true, if the tokens returned
	 *                      should be trimmed, so that they don't have
	 *                      any whitespaces at the beginning or end.
	 *                      Whitespaces are any of the characters 
	 *                      defined in StringTokenizer::SPACE.
	 *                      If delim itself is StringTokenizer::SPACE,
	 *                      this will result in a behaviour with 
	 *                      skipAllDelim = true.
	 *                      DEFAULT = false
	 */
	StringTokenizer (const char *str,
			 const char *delim,
			 bool skipAllDelim = false,
			 bool trim = false);
	
	/**
	 * create a new StringTokenizer which splits the input
	 * string at whitespaces. The tokens are stripped from
	 * whitespaces. This means, if you change the set of
	 * delimiters in either the 'begin(const char *delim)' method
	 * or in 'setDelimiters()', you then get whitespace
	 * trimmed tokens, delimited by the new set.
	 * Behaves like StringTokenizer(s, StringTokenizer::SPACE,false,true);
	 */
	StringTokenizer (const char *s);

	/**
	 * returns the begin iterator
	 */
	iterator begin() const { 
		return iterator(*this);
	}
	
	/**
	 * changes the set of delimiters used in subsequent
	 * iterations.
	 */
	void setDelimiters (const char *d) {
		delim = d;
	}
	
	/**
	 * returns a begin iterator with an alternate set of 
	 * delimiters.
	 */
	iterator begin(const char *d) { 
		delim = d;
		return iterator(*this);
	}

	/**
	 * the iterator marking the end.
	 */
	const iterator& end() const { return itEnd; }
};

#ifdef	CCXX_NAMESPACES
};
#endif

#endif

/** EMACS **
 * Local variables:
 * mode: c++
 * c-basic-offset: 8
 * End:
 */
