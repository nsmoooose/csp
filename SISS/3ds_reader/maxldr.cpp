#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include "maxldr.h"

Color3DS::Color3DS() {
	m_r = m_g = m_b = 0.0f;
	fl = false;
}

Color3DS::Color3DS( float r, float g, float b ) {
	this->m_r = r;
	this->m_g = g;
	this->m_b = b;
	fl = false;
}

float Color3DS::Red() {
	return( m_r );
}

void Color3DS::Red( float r ) {
	this->m_r = r;
}

float Color3DS::Green() {
	return( m_g );
}

void Color3DS::Green( float g ) {
	this->m_g = g;
}

float Color3DS::Blue() {
	return( m_b );
}

void Color3DS::Blue( float b ) {
	this->m_b = b;
}

istream &operator>>( istream &stream, Color3DS &c ) {
	if( c.fl ) {
		stream.read( (char *)&c.m_r, sizeof(c.m_r) );
		stream.read( (char *)&c.m_g, sizeof(c.m_g) );
		stream.read( (char *)&c.m_b, sizeof(c.m_b) );
	} else {
		unsigned char temp1, temp2, temp3;
		stream.read( (char *)&temp1, sizeof(temp1) );
		stream.read( (char *)&temp2, sizeof(temp2) );
		stream.read( (char *)&temp3, sizeof(temp3) );
		c.m_r = temp1 / 255.0f;
		c.m_g = temp2 / 255.0f;
		c.m_b = temp3 / 255.0f;
	}

	return( stream );
}

void Color3DS::floatFlag( bool f ) {
	fl = f;
}

// Material3DS

Material3DS::Material3DS() {
	mp_matname = NULL;
	mp_texname = NULL;
}

Material3DS::~Material3DS() {
	delete[] mp_matname;
	delete[] mp_texname;
}

void Material3DS::setMatName( char * name ) {
	mp_matname = name;
}

void Material3DS::setAmb( Color3DS *amb ) {
	m_ambient = *amb;
}

void Material3DS::setDif( Color3DS *dif ) {
	m_diffuse = *dif;
}

void Material3DS::setSpc( Color3DS *spc ) {
	m_specular = *spc;
}

void Material3DS::setTexName( char *name ) {
	mp_texname = name;
}

void Material3DS::setTexPct( float pct ) {
	m_pct = pct;
}

void Material3DS::setTexTile( unsigned short tile ) {
	m_tiling = tile;
}

void Material3DS::setTexBlur( float blur ) {
	m_texblur = blur;
}

char * Material3DS::getMatName() {
	return( mp_matname );
}

Color3DS * Material3DS::getAmb() {
	return( &m_ambient );
}

Color3DS * Material3DS::getDif() {
	return( &m_diffuse );
}

Color3DS * Material3DS::getSpc() {
	return( &m_specular );
}

char * Material3DS::getTexName() {
	return( mp_texname );
}

float Material3DS::getTexPct() {
	return( m_pct );
}

unsigned short Material3DS::getTexTile() {
	return( m_tiling );
}

float Material3DS::getTexBlur() {
	return( m_texblur );
}

istream &operator>>( istream &stream, Material3DS &mat ) {
	unsigned long chunkLength, cIndex = 6, hLength;
	unsigned short hID;

	stream.read( (char *)&chunkLength, sizeof(chunkLength) );

	while( cIndex < chunkLength ) {
		stream.read( (char *)&hID, sizeof(hID) );
		stream.read( (char *)&hLength, sizeof(hLength) );
		switch( hID ) {
			case(0xA000):
				mat.mp_matname = new char[17];
				memset(mat.mp_matname, 0, 17 );
				stream.getline( mat.mp_matname, 18, '\0' );
				cIndex += stream.gcount() + 6;
				break;
			case(0xA010):
				{
					unsigned short ts;
					unsigned long tl;
					stream.read( (char *)&ts, sizeof(ts) );
					stream.read( (char *)&tl, sizeof(tl) );
					Color3DS tmp;
					if( (ts == 0x0010) || (ts == 0x0013) ) {
						tmp.floatFlag(true);
						cIndex += 24;
					} else {
						cIndex += 15;
					}
					stream >> tmp;
					mat.m_ambient = tmp;
				}
				break;
			case(0xA020):
				{
					unsigned short ts;
					unsigned long tl;
					stream.read( (char *)&ts, sizeof(ts) );
					stream.read( (char *)&tl, sizeof(tl) );
					Color3DS tmp;
					if( (ts == 0x0010) || (ts == 0x0013) ) {
						tmp.floatFlag(true);
						cIndex += 24;
					} else {
						cIndex += 15;
					}
					stream >> tmp;
					mat.m_diffuse = tmp;
				}
				break;
			case(0xA030):
				{
					unsigned short ts;
					unsigned long tl;
					stream.read( (char *)&ts, sizeof(ts) );
					stream.read( (char *)&tl, sizeof(tl) );
					Color3DS tmp;
					if( (ts == 0x0010) || (ts == 0x0013) ) {
						tmp.floatFlag(true);
						cIndex += 24;
					} else {
						cIndex += 15;
					}
					stream >> tmp;
					mat.m_specular = tmp;
				}
				break;
			case(0xa200):
				cIndex += 6;
				break;
			case(0xA300):
				mat.mp_texname = new char[17];
				memset( mat.mp_texname, 0, 17 );
				stream.getline( mat.mp_texname, 18, '\0' );
				cIndex += stream.gcount() + 6;
				break;
			case(0xA040):
				{
					short	ts;
					long	tl;
					stream.read( (char *)&ts, sizeof(ts) );
					stream.read( (char *)&tl, sizeof(tl) );
					switch(ts) {
						case(0x0030):
							short temp;
							stream.read( (char *)&temp, sizeof(temp) );
							mat.m_pct = temp;
							cIndex += 14;
							break;
						case(0x0031):
							stream.read( (char *)&mat.m_pct, sizeof(mat.m_pct) );
							cIndex += 16;
							break;
					}
				}
				break;
			case(0xA351):
				stream.read( (char *)&mat.m_tiling, sizeof(mat.m_tiling) );
				cIndex += 8;
				break;
			case(0xA353):
				stream.read( (char *)&mat.m_texblur, sizeof(mat.m_texblur) );
				cIndex += 10;
				break;
			default:
				stream.seekg(hLength-6, ios::cur);
				cIndex += hLength;
				break;
		}
	}

	return( stream );
}

// Object3DS

Object3DS::Object3DS() {
	mp_objname = NULL;
	mp_vertexArray = NULL;
	mp_faceArray = NULL;
	mp_texcoords = NULL;
	m_transMatrix = StandardMatrix4::IDENTITY;
}

Object3DS::~Object3DS() {
	delete[] mp_objname;
	delete[] mp_vertexArray;
	delete[] mp_faceArray;
	delete[] mp_texcoords;
}

void Object3DS::setName( char *name ) {
	mp_objname = name;
}

void Object3DS::setVArray( StandardVector4 *vec ) {
	mp_vertexArray = vec;
}

void Object3DS::setFArray( Face3DS *faces ) {
	mp_faceArray = faces;
}

void Object3DS::setTCoords( TexCoords3DS *tcs ) {
	mp_texcoords = tcs;
}

char * Object3DS::getName() {
	return( mp_objname );
}

StandardVector4 * Object3DS::getVArray() {
	return( mp_vertexArray );
}

int Object3DS::getNumVertex() {
	return( numVertex );
}

Face3DS * Object3DS::getFArray() {
	return( mp_faceArray );
}

int Object3DS::getNumFaces() {
	return( numFaces );
}

Mat3DS * Object3DS::getMGroup( int i ) {
	if( i < 0 || i > mp_matgroup.num - 1 ) {
		return( NULL );
	}
	return( mp_matgroup[i] );
}

int Object3DS::getNumMats() {
	return( mp_matgroup.num );
}

TexCoords3DS * Object3DS::getTCoords() {
	return( mp_texcoords );
}

StandardMatrix4 * Object3DS::getMatrix() {
	return( &m_transMatrix );
}

istream &operator>>( istream &stream, Object3DS &object ) {
	unsigned long chunkLength = 0, cIndex = 0, hLength = 0;
	unsigned short cID = 0;
	StandardMatrix4	transm;
	
	stream.read( (char *)&chunkLength, sizeof(chunkLength) );

	object.mp_objname = new char[17];
	memset( object.mp_objname, 0, 17 );
	stream.getline( object.mp_objname, 18, '\0' );
	cIndex += stream.gcount() + 6;

	while( cIndex < chunkLength ) {
		stream.read( (char *)&cID, sizeof(cID) );
		stream.read( (char *)&hLength, sizeof(hLength) );
		switch(cID) {
			case(0x4100):
				cIndex += 6;
				break;
			case(0x4110):
				{
					unsigned short numVertex, tidx = 0;
					stream.read( (char *)&numVertex, sizeof(numVertex) );
					object.mp_vertexArray = new StandardVector4[numVertex];
					object.numVertex = numVertex;
					while( tidx < numVertex ) {
						stream.read( (char *)&object.mp_vertexArray[tidx].x, 4 );
						stream.read( (char *)&object.mp_vertexArray[tidx].y, 4 );
						stream.read( (char *)&object.mp_vertexArray[tidx].z, 4 );
						
						object.mp_vertexArray[tidx].w = 1.0f;
						tidx++;
						cIndex += ( 3 * sizeof(object.mp_vertexArray[tidx].x) );
					}
				}
				cIndex += 8;
				break;
			case(0x4120):
				{
					unsigned short v1, v2, v3, flags, faces, tidx = 0;
					stream.read( (char *)&faces, sizeof(faces) );
					object.mp_faceArray = new Face3DS[faces];
					object.numFaces = faces;
					while( tidx < faces ) {
						stream.read( (char *)&v1, sizeof(v1) );
						stream.read( (char *)&v2, sizeof(v2) );
						stream.read( (char *)&v3, sizeof(v3) );
						stream.read( (char *)&flags, sizeof(flags) );
						// faces winding here
						object.mp_faceArray[tidx].vertex1 = v1;
						object.mp_faceArray[tidx].vertex2 = v2;
						object.mp_faceArray[tidx].vertex3 = v3;
						object.mp_faceArray[tidx].flags = flags;
						tidx++;
						cIndex += ( 4 * sizeof(v1) );
					}
				}
				cIndex += 8;
				break;
			case(0x4130):
				{
					unsigned short faces, tidx = 0;
					Mat3DS *tmat = new Mat3DS;
					tmat->name = new char[17];
					memset( tmat->name, 0, 17 );
					stream.getline(tmat->name, 18, '\0');
					cIndex += stream.gcount();
					stream.read( (char *)&faces, sizeof(faces) );
					cIndex += sizeof(faces);
					tmat->numFaces = faces;
					tmat->faces = new short[faces];
					while( tidx < faces ) {
						stream.read( (char *)&tmat->faces[tidx], sizeof(tmat->faces[tidx]) );
						tidx++;
						cIndex += sizeof(tmat->faces[tidx]);
					}
					object.mp_matgroup.Add(tmat);
				}
				cIndex += 6;
				break;
			case(0x4140):
				{
					unsigned short nverts, tidx = 0;
					stream.read( (char *)&nverts, sizeof(nverts) );
					object.mp_texcoords = new TexCoords3DS[nverts];
					while( tidx < nverts ) {
						stream.read( (char *)&object.mp_texcoords[tidx].u, sizeof(object.mp_texcoords[tidx].u) );
						stream.read( (char *)&object.mp_texcoords[tidx].v, sizeof(object.mp_texcoords[tidx].v) );
						cIndex += ( 2 * sizeof(object.mp_texcoords[tidx].u ) );
						tidx++;
					}
				}
				cIndex += 8;
				break;
			case(0x4160):
				{
					
					for( int i = 0; i < 4; i++ ) {
						stream.read( (char *)(object.m_transMatrix[i]), sizeof(float) );
						stream.read( (char *)(object.m_transMatrix[i]+1), sizeof(float) );
						stream.read( (char *)(object.m_transMatrix[i]+2), sizeof(float) );
					}
				}
				cIndex += hLength;
				break;
			default:
				stream.seekg(hLength-6, ios::cur);
				cIndex += hLength;
				break;
		}
	}
/*
	StandardMatrix4 tmat = *(object.getMatrix());
	tmat.Inverse();
	for( int i = 0; i < object.getNumVertex(); i++ )
	{
		*(object.getVArray()+i) = tmat * *(object.getVArray()+i);
	}
*/
	return( stream );
}

Animation3DS::Animation3DS() {
	objName = new char[13];
	memset( objName, 0, 13 );
}

Animation3DS::~Animation3DS() {
	delete[] objName;
}

char * Animation3DS::getObjectName() {
	return( objName );
}

float * Animation3DS::getPivot() {
	return( pivot );
}

float * Animation3DS::getGlobalPos() {
	return( globalPos );
}

float * Animation3DS::getRotationAngle() {
	return( &rotAngle );
}

float * Animation3DS::getRotX() {
	return( &rotX );
}

float * Animation3DS::getRotY() {
	return( &rotY );
}

float * Animation3DS::getRotZ() {
	return( &rotZ );
}

float * Animation3DS::getScaleX() {
	return( &scaleX );
}

float * Animation3DS::getScaleY() {
	return( &scaleY );
}

float * Animation3DS::getScaleZ() {
	return( &scaleZ );
}

istream &operator>>( istream &stream, Animation3DS &object ) {
	unsigned long	cLength = 0, cIndex = 6, tLength = 0, junk;
	unsigned short	cID;


	stream.read( (char *)&cLength, 4 );

	while( cIndex < cLength ) {
		stream.read( (char *)&cID, sizeof(cID) );
		switch( cID ) {
			case(0xB00A):
			{
				/*
				short	version;
				char	*tname = new char[13];
				long	numFrames;
				stream.read( (char *)&cLength, sizeof(cLength) );
				stream.read( (char *)&version, sizeof(version) );
				stream.getline( tname, 14, '\0' );
				stream.read( (char *)&numFrames, sizeof(numFrames) );
				delete[] tname;
				*/
				stream.read( (char *)&tLength, 4 );
				stream.seekg( tLength - 6, ios::cur );
				cIndex += tLength;
			}
			break;
			case(0xB008):
			{
				/*
				stream.read( (char *)&cLength, sizeof(cLength) );
				//  Start frame
				stream.read( (char *)&cLength, sizeof(cLength) );
				//  End frame
				stream.read( (char *)&cLength, sizeof(cLength) );
				*/
				stream.read( (char *)&tLength, 4 );
				stream.seekg( tLength - 6, ios::cur );
				cIndex += tLength;
			}
			break;
			case(0xB009):
			{
				/*
				stream.read( (char *)&cLength, sizeof(cLength) );
				// Current frame
				stream.read( (char *)&cLength, sizeof(cLength) );
				*/
				stream.read( (char *)&tLength, 4 );
				stream.seekg( tLength - 6, ios::cur );
				cIndex += tLength;
			}
			break;
			case(0xB002):
			{
				// Animation Object
				stream.read( (char *)&tLength, 4 );
				cIndex += 6;
			}
			break;
			case(0xB030):
			{
				
				short nodeID;

				stream.read( (char *)&tLength, 4 );
				// Node Identification Number
				stream.read( (char *)&nodeID, sizeof(nodeID) );
				cIndex += tLength;
			}
			break;
			case(0xB010):
			{
				/*
				unsigned short	flag1, flag2;
				short			pIndex;
				char *tname = new char[12];
				stream.read( (char *)&cLength, sizeof(cLength) );
				// Name of animation object
				stream.getline( tname, 13, '\0' );
				// Flags
				stream.read( (char *)&flag1, sizeof(flag1) );
				stream.read( (char *)&flag2, sizeof(flag2) );
				stream.read( (char *)&pIndex, sizeof(pIndex) );
				delete[] tname;
				*/
				object.objName = new char[12];
				memset( object.objName, 0, 12 );
				stream.read( (char *)&tLength, 4 );
				stream.getline( object.objName, 13, '\0' );
				stream.seekg( tLength - 6 - stream.gcount(), ios::cur );
				cIndex += tLength;
			}
			break;
			case(0xB013):
			{
				//float p0, p1, p2;
				stream.read( (char *)&tLength, 4 );
				//  Pivot Point
				stream.read( (char *)(object.pivot    ), 4 );
				stream.read( (char *)(object.pivot + 1), 4 );
				stream.read( (char *)(object.pivot + 2), 4 );

				cIndex += tLength;
			}
			break;
			case(0xB020):
			{
				unsigned short flag;
				stream.read( (char *)&tLength, 4 );
				// Track Header
				stream.read( (char *)&flag, 2 );
				stream.read( (char *)&junk, 4 );
				stream.read( (char *)&junk, 4 );
				// Number of keys in a track
				stream.read( (char *)&junk, 4 );

				//  Key Header
				stream.read( (char *)&junk, 4 );
				stream.read( (char *)&flag, 2 );
				// Position in Global Coordinates
				stream.read( (char *)(object.globalPos    ), 4 );
				stream.read( (char *)(object.globalPos + 1), 4 );
				stream.read( (char *)(object.globalPos + 2), 4 );

				cIndex += tLength;
			}
			break;
			case(0xB021):
			{
				unsigned short flag;
				stream.read( (char *)&tLength, 4 );

				// Track Header
				stream.read( (char *)&flag, 2 );
				stream.read( (char *)&junk, 4 );
				stream.read( (char *)&junk, 4 );
				// Number of keys in a track
				stream.read( (char *)&junk, 4 );

				//  Key Header
				stream.read( (char *)&junk, 4 );
				stream.read( (char *)&flag, 2 );

				// Rotation
				stream.read( (char *)&object.rotAngle, 4 );
				stream.read( (char *)&object.rotX, 4 );
				stream.read( (char *)&object.rotY, 4 );
				stream.read( (char *)&object.rotZ, 4 );

				cIndex += tLength;
			}
			break;
			case(0xB022):
			{
				unsigned short flag;
				stream.read( (char *)&tLength, 4 );
				// Track Header
				stream.read( (char *)&flag, 2 );
				stream.read( (char *)&junk, 4 );
				stream.read( (char *)&junk, 4 );
				// Number of keys in a track
				stream.read( (char *)&junk, 4 );

				//  Key Header
				stream.read( (char *)&junk, 4 );
				stream.read( (char *)&flag, 2 );
				//  Scaling
				stream.read( (char *)&object.scaleX, 4 );
				stream.read( (char *)&object.scaleY, 4 );
				stream.read( (char *)&object.scaleZ, 4 );

				cIndex += tLength;
			}
			break;
			default:
				stream.read( (char *)&tLength, 4 );
				stream.seekg(tLength-6, ios::cur);
				cIndex += tLength;
				break;
		}
	}

	return( stream );
}

Object3DS * Reader3DS::getObject( int i ) {
	if( i < 0 || i > objects.num - 1 ) {
		return(NULL);
	}
	return( objects[i] );
}

int Reader3DS::getNumObjects() {
	return( objects.num );
}

Material3DS * Reader3DS::getMaterial( int i ) {
	if( i < 0 || i > materials.num - 1 ) {
		return(NULL);
	}
	return( materials[i] );
}

int Reader3DS::getNumMaterials() {
	return( materials.num );
}

Animation3DS * Reader3DS::getAnimation( int i ) {
	if( i < 0 || i > animations.num - 1 ) {
		return( NULL );
	}
	return( animations[i] );
}

int Reader3DS::getNumAnimations() {
	return( animations.num );
}

bool Reader3DS::read3DS( const char *filename ) {
	ifstream ifs( filename, ios::in | ios::binary | ios::nocreate );
	unsigned short ID;
	unsigned long cLength;

	if( !ifs ) {
		return( false );
	}

	while( !ifs.eof() ) {
		ifs.read( (char *)&ID, sizeof(ID) );
		switch( ID ) {
			case(0x4d4d):
				ifs.read( (char *)&cLength, sizeof(cLength) );
				break;
			case(0x0002):
				ifs.read( (char *)&cLength, sizeof(cLength) );
				ifs.read( (char *)&cLength, sizeof(cLength) );
				break;
			case(0x3d3d):
				ifs.read( (char *)&cLength, sizeof(cLength) );
				break;
			case(0x3d3e):
				ifs.read( (char *)&cLength, sizeof(cLength) );
				ifs.read( (char *)&cLength, sizeof(cLength) );
				break;
			case(0xafff):
				{
					Material3DS *tm = new Material3DS;
					ifs >> *tm;
					materials.Add(tm);
				}
				break;
			case(0x4000):
				{
					Object3DS *to = new Object3DS;
					ifs >> *to;
					objects.Add(to);
				}
				break;
			case(0xB000):
				{
					//  Animation Chunk
					ifs.read( (char *)&cLength, sizeof(cLength) );
				}
				break;
			case(0xB002):
				{
					Animation3DS *ta = new Animation3DS;
					ifs >> *ta;
					animations.Add(ta);
				}
				break;
			default:
				ifs.read( (char *)&cLength, sizeof(cLength) );
				ifs.seekg(cLength-6, ios::cur);
				break;
		}
	}
	ifs.close();
	return( true );
}

void Reader3DS::scaleVertex( float scale ) {
	StandardVector4 *vp;
	for( int i = 0; i < getNumObjects(); i++ ) {
		vp = objects[i]->getVArray();
		for( int j = 0; j < objects[i]->getNumVertex(); j++ ) {
			(vp + j)->x *= scale;
			(vp + j)->y *= scale;
			(vp + j)->z *= scale;
		}
	}
}