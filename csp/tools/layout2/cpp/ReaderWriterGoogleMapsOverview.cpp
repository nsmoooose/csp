#include <sstream>
#include <osgDB/Registry>
#include <csp/tools/layout2/cpp/ReaderWriterTiledMapServiceOverview.h>

namespace csp
{
namespace layout
{

class ReaderWriterGoogleMapsOverview : public ReaderWriterTiledMapServiceOverview
{
	public:
		ReaderWriterGoogleMapsOverview()
		{
			supportsExtension( getExtension(), "GoogleMaps overview pseudo-format" );
		}

		virtual const char* className() const { return "GoogleMaps overview pseudo-loader"; }

	protected:
		virtual unsigned int getMaxLevel() const
		{
			return 20;
		}

		virtual std::string getExtension() const
		{
			return "googlemaps_overview";
		}

		virtual void getFileNameAndUrl(unsigned int level, unsigned int tileColumn, unsigned int tileRow, std::string & fileName, std::string & url) const
		{
			std::ostringstream ossfileName;
			ossfileName << "googlemaps_" << level << "_" << tileColumn << "_" << tileRow << ".jpg";
			fileName = ossfileName.str();

			static unsigned int serverId = 0;
			serverId = ++serverId % 4;

			const unsigned int version = 41;

			std::ostringstream ossurl;
			ossurl	<< "http://khm" << serverId << ".google.com/kh"
					<< "/v=" << version
					<< "&x=" << tileColumn
					<< "&y=" << tileRow
					<< "&z=" << level
					<< "&s=Ga&.jpg";
			url = ossurl.str();
		}
};

// now register with Registry to instantiate the above
// reader/writer.
REGISTER_OSGPLUGIN(googlemaps_overview, ReaderWriterGoogleMapsOverview)

} // namespace layout
} // namespace csp
