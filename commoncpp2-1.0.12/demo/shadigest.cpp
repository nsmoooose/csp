// Copyright (C) 2002 Elizabeth Barham
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

#include <cc++/digest.h>
#include <cstdio>

#define BUFF_SIZE 1024

int main(int argc, char * argv[])
{
	unsigned char buff[1024];
	ost::SHA256Digest d256;
	unsigned length;

	for(int i = 1 ; i < argc; i++) {
		FILE * fp = fopen(argv[i], "rb");
		d256.initDigest();

		if(fp) {
			while((length = 
			      fread(buff, sizeof(unsigned char), BUFF_SIZE, fp))) {
				d256.putDigest(buff, length);
			}
			
			std::cout << d256 << "  " << argv[i] << std::endl;
			
			fclose(fp);
		}
	}
}
