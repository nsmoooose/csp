#ifndef OSGCONSOLE_CONSOLE_
#define OSGCONSOLE_CONSOLE_

#include <osg/Geode>
#include <osgText/Text>

#include <streambuf>
#include <string>
#include <deque>


namespace osgConsole
{

	class Console: public osg::Geode, public std::basic_streambuf<char> {
	public:
		Console(int x=0, int y=0, int w=1024, int h=768, int border=10);
		Console(const Console &copy, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY);

		META_Object(osgConsole, Console);

		inline float getLeft() const { return _left; }
		inline float getBottom() const { return _bottom; }
		inline float getWidth() const { return _width; }
		inline float getHeight() const { return _height; }
		inline void setRect(float l, float t, float w, float h);
		inline bool getCursorEnabled() const { return _cursor_enabled; }
		inline void setCursorEnabled(bool e);
		inline char getCursorCharacter() const { return _cursor_char; }
		void setCursorCharacter(char c);
		void setFont(std::string const &font, int size);
		inline int getTabStop() const { return _tabstop; }
		void setTabStop(int tabstop);

		inline const osg::Vec4 &getTextColor() const;
		inline void setTextColor(const osg::Vec4 &c);

		inline int getMaxBufferLines() const;
		inline void setMaxBufferLines(int n);

		inline void clearBuffer();

		void print(char c, bool layout=true);
		void print(const char *s);
		void print(const std::string &s);
		void eat();
		void setLine(const std::string &s);
		void setCursor(int pos);

	protected:
		virtual ~Console() {}
		Console &operator=(const Console &) { return *this; }

		void splitBufferLines() const;
		std::string expandTabs(const std::string &s) const;
		float getTextWidth(std::string const &text) const;
		float getTextWidth(char c) const;
		void doLayout();

		void _setFont(osg::ref_ptr<osgText::Text> &text, std::string const &font, int size);

	private:
		mutable osg::ref_ptr<osgText::Text> _text;
		mutable osg::ref_ptr<osgText::Text> _cursor;	
		mutable osg::ref_ptr<osgText::Text> _token;

		struct BufferLine {
			std::string line;
			float width;
			BufferLine(): width(0) {}
			BufferLine(const std::string &l, float w): line(l), width(w) {}
		};

		typedef std::deque<BufferLine> BufferType;
		mutable BufferType _buffer;

		int _tabstop;

		float _left;
		float _bottom;
		float _width;
		float _height;
		float _border;

		float _cursor_width;
		bool _cursor_enabled;
		char _cursor_char;
		int _cursor_pos;
		float _cursor_x;
		float _cursor_y;
		float _cursor_blink_phase;

		int _buf_lines;
	};

	// INLINE METHODS

	inline void Console::setRect(float l, float t, float w, float h)
	{
		_left = l; 
		_bottom = t; 
		_width = w; 
		_height = h; 
		doLayout();
	}

	inline const osg::Vec4 &Console::getTextColor() const
	{
		return _text->getColor();
	}

	inline void Console::setTextColor(const osg::Vec4 &c)
	{
		_text->setColor(c);
		_cursor->setColor(c);
	}

	inline void Console::clearBuffer()
	{
		_buffer.clear();
		doLayout();
	}

	inline int Console::getMaxBufferLines() const
	{
		return _buf_lines;
	}

	inline void Console::setMaxBufferLines(int n)
	{
		_buf_lines = n;
		doLayout();
	}

}

#endif

