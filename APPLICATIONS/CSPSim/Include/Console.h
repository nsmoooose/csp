#ifndef OSGCONSOLE_CONSOLE_
#define OSGCONSOLE_CONSOLE_
#include <osg/Drawable>
#include <osg/State>
#include <osgText/Text>
#include <osgText/Font>

#include <streambuf>
#include <string>
#include <deque>
namespace osgConsole
{

	class Console: public osg::Drawable, public std::basic_streambuf<char> {
	public:
		Console(int x=0, int y=0, int w=1024, int h=768, int border=10);
		Console(const Console &copy, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY);

		META_Object(osgConsole, Console);

		inline float getLeft() const { return left_; }
		inline float getBottom() const { return bottom_; }
		inline float getWidth() const { return width_; }
		inline float getHeight() const { return height_; }
		inline void setRect(float l, float t, float w, float h);
		inline int getTabSize() const { return tab_size_; }
		inline void setTabSize(int s) { tab_size_ = s; buffer_.clear(); dirtyDisplayList(); }
		inline bool getCursorEnabled() const { return cursor_enabled_; }
		inline void setCursorEnabled(bool e) { cursor_enabled_ = e; dirtyDisplayList(); }
		inline char getCursorCharacter() const { return cursor_char_; }
		inline void setCursorCharacter(char c) { cursor_char_ = c; dirtyDisplayList(); }

		inline void setCursorBlinkPhase(float p) { csr_blink_phase_ = p; dirtyDisplayList(); }
		inline float getCursorBlinkPhase() const { return csr_blink_phase_; }

		inline void setFont(osgText::Font *font);
		inline const osgText::Font *getFont() const;
		inline osgText::Font *getFont();

		inline const osg::Vec4 &getTextColor() const;
		inline void setTextColor(const osg::Vec4 &c);

		inline int getMaxBufferLines() const;
		inline void setMaxBufferLines(int n);

		inline void clearBuffer();

		void print(char c);
		void print(const char *s);
		void print(const std::string &s);
		void eat();
		void setline(const std::string &s);
		void setCursor(int pos);

		virtual void drawImplementation(osg::State &state) const;
		inline virtual bool computeBound() const;

	protected:
		virtual ~Console() {}
		Console &operator=(const Console &) { return *this; }

		void draw_text(osg::State &state) const;
		void split_buffer_lines() const;
		std::string expand_tabs(const std::string &s) const;

		inline virtual int_type overflow(int_type c = traits_type::eof());

	private:
		mutable osg::ref_ptr<osgText::Text> text_;	
		mutable osg::ref_ptr<osgText::Text> token_;
		struct Buffer_line {
			std::string line;
			float width;
			Buffer_line(): width(0) {}
			Buffer_line(const std::string &l, float w): line(l), width(w) {}
		};

		typedef std::deque<Buffer_line> Buffer_type;
		mutable Buffer_type buffer_;

		int tab_size_;

		float left_;
		float bottom_;
		float width_;
		float height_;
		float border_;

		bool cursor_enabled_;
		char cursor_char_;
		int cursor_pos_;
		float cursor_x_;
		float csr_blink_phase_;

		int buf_lines_;
	};

	// INLINE METHODS

	inline void Console::setRect(float l, float t, float w, float h)
	{
		left_ = l; 
		bottom_ = t; 
		width_ = w; 
		height_ = h; 
		dirtyBound();
		dirtyDisplayList();
	}

	inline void Console::setFont(osgText::Font *font)
	{ 
		text_->setFont(font);
		buffer_.clear();
		dirtyDisplayList();
	}

	inline bool Console::computeBound() const
	{
		_bbox._min.set(left_, bottom_, 0);
		_bbox._max.set(left_+width_, bottom_+height_, 0);
		_bbox_computed = true;
		return true;
	}

	inline const osgText::Font *Console::getFont() const
	{
		return text_->getFont();
	}

	inline osgText::Font *Console::getFont()
	{
		return text_->getFont();
	}

	inline const osg::Vec4 &Console::getTextColor() const
	{
		return text_->getColor();
	}

	inline void Console::setTextColor(const osg::Vec4 &c)
	{
		text_->setColor(c);
	}

	inline void Console::clearBuffer()
	{
		buffer_.clear();
		dirtyDisplayList();
	}

	inline Console::int_type Console::overflow(int_type c)
	{
		if (c == traits_type::eof()) return traits_type::not_eof(c);
		print(traits_type::to_char_type(c));
		return c;
	}

	inline int Console::getMaxBufferLines() const
	{
		return buf_lines_;
	}

	inline void Console::setMaxBufferLines(int n)
	{
		buf_lines_ = n;
		dirtyDisplayList();
	}

}

#endif

