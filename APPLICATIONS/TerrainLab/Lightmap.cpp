// Lightmap.cpp: Implementierung der Klasse CLightmap.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Lightmap.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CLightmap::CLightmap()
{

}

CLightmap::~CLightmap()
{

}

#include "basecode.h"
/*
static char* CLightmap::computeNormals(Image *hmap)
{
        char *n;
        int i,j,w;

        static vector3 tmp,v0,v1,v2,v3,v4,v5,v6,v7,v8;
        static vector3 f0,f1,f2,f3,f4,f5,f6,f7;
        char cfname[512];
        /////////////////////////////////////////////////////////

        w = hmap->w;
        n = new char[3*w*w];
        if (!n) 
		{
			printf("nomem for normals!\n");
			return NULL;
		}

        memset(n,0,3*sizeof(char)*w*w);

        /*
              v2
        v1*---*---*v3
          |0 /|2 /|
          | / | / |
          |/ 1|/ 3|
        v8*---v0--*v4
          |7 /|4 /|
          | / | / |
          |/ 6|/ 5|
        v7*---*--*v5
              v6
        */

 /*       printf("û building normal map ");
        for (j=1; j<w-1; j++)
        {
                for (i=1; i<w-1; i++)
                {
                        v0 = vector3((i  )-w/2,hmap->data[(j  )*w+i  ],(j  )-w/2);
                        v1 = vector3((i-1)-w/2,hmap->data[(j-1)*w+i-1],(j-1)-w/2);
                        v2 = vector3((i  )-w/2,hmap->data[(j-1)*w+i  ],(j-1)-w/2);
                        v3 = vector3((i+1)-w/2,hmap->data[(j-1)*w+i+1],(j-1)-w/2);
                        v4 = vector3((i+1)-w/2,hmap->data[(j  )*w+i+1],(j  )-w/2);
                        v5 = vector3((i+1)-w/2,hmap->data[(j+1)*w+i+1],(j+1)-w/2);
                        v6 = vector3((i  )-w/2,hmap->data[(j+1)*w+i  ],(j+1)-w/2);
                        v7 = vector3((i-1)-w/2,hmap->data[(j+1)*w+i-1],(j+1)-w/2);
                        v8 = vector3((i-1)-w/2,hmap->data[(j  )*w+i-1],(j  )-w/2);

                        f1 = CrossProduct(Normalize(v0-v2),Normalize(v2-v8));
                        f2 = CrossProduct(Normalize(v3-v2),Normalize(v2-v0));
                        f3 = CrossProduct(Normalize(v4-v3),Normalize(v3-v0));
                        f4 = CrossProduct(Normalize(v5-v4),Normalize(v4-v6));
                        f6 = CrossProduct(Normalize(v6-v0),Normalize(v0-v7));
                        f7 = CrossProduct(Normalize(v0-v8),Normalize(v8-v7));

                        tmp = 128.0f* Normalize(f1 + f2 + f3 + f5 + f6 + f7);

                        n[0+3*(j*w+i)] = tmp.x;
                        n[1+3*(j*w+i)] = tmp.y;
                        n[2+3*(j*w+i)] = tmp.z;
                }
        }
        printf("done!\n");
        return n;
}

static int CLightmap::intersect_map(const vector3& iv,const ray& r,Image* hm,float fHeightScale)
{
  int w,hits;
  float d,h,D;
  vector3 v,dir;

  v = iv + r.direction;
  w = hm->w;

  hits = 0;

  while (!(( v.x >= w-1 ) || ( v.x <= 0 ) || ( v.z >= w-1 ) || ( v.z <= 0 )))
  {
    D = Magnitude(vector3(v.x,0,v.z)-vector3(r.origin.x,0,r.origin.z)); // length of lightdir's projection
    d = Magnitude(iv-v);
    h = iv.y + (d*r.origin.y) / D;
    if (hm->data[ifloor(v.z)* w + ifloor(v.x)] * fHeightScale > h)
	{
        hits++;
        break;
    }
    dir = r.direction;
    dir.y = 0;
    v += Normalize(dir);
  }
  return hits;
}

static CTexture* CLightmap::genLightmap(char* normal,Image* hm,vector3 fSunDir,int w,float fAmbient)
{
    int i,j,hits;
    float f,dot;
    vector3 n,fVertex;
    Image* lmap;
    ray r;
    float fHeightScale = 10.0f / 255.0f;
    lmap = new Image(w,w,1);
    if (!lmap)
	{
		printf("(!) Error: cannot alloc lightmap!\n");
		return 0;
	}

    for (j=0; j<w; j++)
	{
        for (i=0; i<w; i++)
		{
            fVertex.x = i;
            fVertex.y = hm->data[j*w+i] * fHeightScale;
            fVertex.z = j;

            f = fAmbient ;

            r.origin = fVertex + fSunDir * 2000.0f;
            r.direction = fSunDir;

            if (!intersect_map(fVertex,r,hm,fHeightScale))
			{
                n.x = (float)(normal[3*(j*w+i)+0]);
                n.y = (float)(normal[3*(j*w+i)+1]);
                n.z = (float)(normal[3*(j*w+i)+2]);
                f += 0.5f*(1.0f+DotProduct(Normalize(n),Normalize(fSunDir)));
                if (f>1.0f) f = 1.0f;
            }

            dot = f * 255.0f;
            lmap->data[j*w+i] = (unsigned char)dot;
        }
    }
    return lmap;
}

/*int main(int argc, char** argv){
    char* normalMap=NULL;
    vector3 fSunDir(0.01,0,0.01);
    Image* hmap;
    Image* shadowmap;

    if (argc<=1) {printf("Error: need input file!\n");return false;};

    hmap = img_load(argv[1]);
    if (!hmap){
        printf("error: cannot load %s\n",argv[1]);
        return -1;
    };

    printf("heightmap is %dx%dx%d\n",hmap->w,hmap->h,hmap->bpp);
    normalMap = computeNormals(hmap);
    printf("generating shadowmap...");
    shadowmap = genLightmap(normalMap,hmap,fSunDir,hmap->w,.7f);
    printf("done.\n");
    img_save(hmap,"hmap.tga");

    printf("shadowmap is %dx%dx%d\n",shadowmap->w,shadowmap->h,shadowmap->bpp);
    printf("saving shadowmap as shadow.tga...");
    if(!img_save(shadowmap,"shadow.tga")) printf("done.\n");
    else printf("failed.\n");
    if (normalMap) delete(normalMap);
    if (shadowmap) delete shadowmap;
    return true;
};*/