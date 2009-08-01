#include <sstream>
#include <osgDB/Registry>
#include <csp/tools/layout2/cpp/ReaderWriterTiledMapServiceOverview.h>

namespace csp
{
namespace layout
{

class ReaderWriterVirtualEarthOverview : public ReaderWriterTiledMapServiceOverview
{
	public:
		ReaderWriterVirtualEarthOverview()
		{
			supportsExtension( getExtension(), "VirtualEarth overview pseudo-format" );
		}

		virtual const char* className() const { return "VirtualEarth overview pseudo-loader"; }

	protected:
		virtual unsigned int getMaxLevel() const
		{
			return 20;
		}

		virtual std::string getExtension() const
		{
			return "virtualearth_overview";
		}

		std::string urlSuffix(int level, int tileColumn, int tileRow) const
		{
			std::string suffix;

			int nbTilesInLevel = int( ::pow( 2.0, level ) );

			for ( ; level > 0; --level )
			{
				nbTilesInLevel /= 2;
				std::div_t divResult;

				divResult = std::div( tileColumn, nbTilesInLevel );
				int right = divResult.quot;
				tileColumn = divResult.rem;

				divResult = std::div( tileRow, nbTilesInLevel );
				int bottom = divResult.quot;
				tileRow = divResult.rem;

				if ( bottom )
					if ( right )
						suffix += '3';
					else
						suffix += '2';
				else
					if ( right )
						suffix += '1';
					else
						suffix += '0';
			}

			return suffix;
		}

		virtual void getFileNameAndUrl(unsigned int level, unsigned int tileColumn, unsigned int tileRow, std::string & fileName, std::string & url) const
		{
			std::string showLabels = "h"; // "a" for no, "h" for yes

			std::ostringstream ossfileName;
			ossfileName << "virtualearth_" << showLabels << "_" << level << "_" << tileColumn << "_" << tileRow << ".jpg";
			fileName = ossfileName.str();

			static unsigned int serverId = 0;
			serverId = ++serverId % 4;

			const unsigned int version = 41;

			std::ostringstream ossurl;
			ossurl	<< "http://ecn.t" << serverId << ".tiles.virtualearth.net/tiles/"
					<< showLabels
					<< urlSuffix(level, tileColumn, tileRow)
					<< ".jpeg?g=304&.jpg";
			url = ossurl.str();
		}
};

// now register with Registry to instantiate the above
// reader/writer.
REGISTER_OSGPLUGIN(virtualearth_overview, ReaderWriterVirtualEarthOverview)

} // namespace layout
} // namespace csp
