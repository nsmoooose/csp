

// Initial CSP version by Wolverine69
// Code based on libraries from magic-software.com
// and Game Programming Gems.

#include "stdinc.h"


#include "TypesVector3.h"
#include "TypesVector4.h"
#include "TypesMatrix3.h"
#include "TypesMatrix4.h"

const StandardMatrix4 StandardMatrix4::ZERO(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f);
const StandardMatrix4 StandardMatrix4::IDENTITY(1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f);


#ifndef HALF_PI
#define HALF_PI 1.57f
#endif

#ifndef PI
#define PI 3.1415f
#endif


//----------------------------------------------------------------------------
StandardMatrix4::StandardMatrix4 ()
{
    // For efficiency reasons, do not initialize matrix.
}
//----------------------------------------------------------------------------
StandardMatrix4::StandardMatrix4 (const double aafEntry[4][4])
{
    memcpy(rowcol,aafEntry,16*sizeof(double));
}
//----------------------------------------------------------------------------
StandardMatrix4::StandardMatrix4 (const StandardMatrix4& rkMatrix)
{
    memcpy(rowcol,rkMatrix.rowcol,16*sizeof(double));
}

StandardMatrix4::StandardMatrix4 (const StandardMatrix3& rkMatrix)
{
    rowcol[0][0] = rkMatrix.rowcol[0][0];
    rowcol[0][1] = rkMatrix.rowcol[0][1];
    rowcol[0][2] = rkMatrix.rowcol[0][2];
    rowcol[0][3] = 0;
    rowcol[1][0] = rkMatrix.rowcol[1][0];
    rowcol[1][1] = rkMatrix.rowcol[1][1];
    rowcol[1][2] = rkMatrix.rowcol[1][2];
    rowcol[1][3] = 0;
    rowcol[2][0] = rkMatrix.rowcol[2][0];
    rowcol[2][1] = rkMatrix.rowcol[2][1];
    rowcol[2][2] = rkMatrix.rowcol[2][2];
    rowcol[2][3] = 0;
    rowcol[3][0] = 0;
    rowcol[3][1] = 0;
    rowcol[3][2] = 0;
    rowcol[3][3] = 1;
   

}



StandardMatrix4::StandardMatrix4(double etr1, double etr2, double etr3, double etr4,
                                 double etr5, double etr6, double etr7, double etr8,
                                 double etr9, double etr10, double etr11, double etr12, 
                                 double etr13, double etr14, double etr15, double etr16)
{

    rowcol[0][0] = etr1;
    rowcol[0][1] = etr2;
    rowcol[0][2] = etr3;
    rowcol[0][3] = etr4;
    rowcol[1][0] = etr5;
    rowcol[1][1] = etr6;
    rowcol[1][2] = etr7;
    rowcol[1][3] = etr8;
    rowcol[2][0] = etr9;
    rowcol[2][1] = etr10;
    rowcol[2][2] = etr11;
    rowcol[2][3] = etr12;
    rowcol[3][0] = etr13;
    rowcol[3][1] = etr14;
    rowcol[3][2] = etr15;
    rowcol[3][3] = etr16;

}


//----------------------------------------------------------------------------
StandardMatrix4::StandardMatrix4 (const double entry[16])
{
    rowcol[0][0] = entry[0];
    rowcol[0][1] = entry[1];
    rowcol[0][2] = entry[2];
    rowcol[0][3] = entry[3];
    rowcol[1][0] = entry[4];
    rowcol[1][1] = entry[5];
    rowcol[1][2] = entry[6];
    rowcol[1][3] = entry[7];
    rowcol[2][0] = entry[8];
    rowcol[2][1] = entry[9];
    rowcol[2][2] = entry[10];
    rowcol[2][3] = entry[11];
    rowcol[3][0] = entry[12];
    rowcol[3][1] = entry[13];
    rowcol[3][2] = entry[14];
    rowcol[3][3] = entry[15];
}


//----------------------------------------------------------------------------
double* StandardMatrix4::operator[] (int iRow) const
{
    return (double*)&rowcol[iRow][0];
}
//----------------------------------------------------------------------------
StandardMatrix4::operator double* ()
{
    return &rowcol[0][0];
}
//----------------------------------------------------------------------------
StandardVector4 StandardMatrix4::GetColumn (int iCol) const // Checked
{
    assert( 0 <= iCol && iCol < 4 );
    return StandardVector4(rowcol[0][iCol],rowcol[1][iCol],
        rowcol[2][iCol], rowcol[3][iCol]);
}
//----------------------------------------------------------------------------
StandardMatrix4& StandardMatrix4::operator= (const StandardMatrix4& rkMatrix)
{
    memcpy(rowcol,rkMatrix.rowcol,16*sizeof(double));
    return *this;
}
//----------------------------------------------------------------------------
bool StandardMatrix4::operator== (const StandardMatrix4& rkMatrix) const
{
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
        {
            if ( rowcol[iRow][iCol] != rkMatrix.rowcol[iRow][iCol] )
                return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool StandardMatrix4::operator!= (const StandardMatrix4& rkMatrix) const
{
    return !operator==(rkMatrix);
}

bool StandardMatrix4::operator== (const StandardMatrix3& rkMatrix) const
{
	for( int i = 0; i < 3; i++ )
	{
		for( int j = 0; j < 3; j++ )
		{
			if( rowcol[i][j] != rkMatrix.rowcol[i][j] )
			{
				return( false );
			}
		}
	}
	return( true );
}

bool StandardMatrix4::operator!= (const StandardMatrix3& rkMatrix) const
{
	return( !operator==(rkMatrix) );
}


StandardMatrix4 & StandardMatrix4::operator+=(const StandardMatrix4 & rkMatrix)
{
  for (int iRow = 0; iRow < 4; iRow++)
    {
      for (int iCol = 0; iCol < 4; iCol++)
	{
	  rowcol[iRow][iCol] += rkMatrix.rowcol[iRow][iCol];
	}
    }


  return *this;
}

StandardMatrix4 & StandardMatrix4::operator-=(const StandardMatrix4 & rkMatrix)
{
  for (int iRow = 0; iRow < 4; iRow++)
    {
      for (int iCol = 0; iCol < 4; iCol++)
	{
	  rowcol[iRow][iCol] -= rkMatrix.rowcol[iRow][iCol];
	}
    }


  return *this;

}

StandardMatrix4 & StandardMatrix4::operator*=(const StandardMatrix4 & rkMatrix)
{

  StandardMatrix4 t;

  for (int iRow = 0; iRow < 4; iRow++)
    {
      for (int iCol = 0; iCol < 4; iCol++)
	{       
            double f = rowcol[iRow][0]*rkMatrix.rowcol[0][iCol];
            f += rowcol[iRow][1]*rkMatrix.rowcol[1][iCol];
            f += rowcol[iRow][2]*rkMatrix.rowcol[2][iCol];
            f += rowcol[iRow][3]*rkMatrix.rowcol[3][iCol];

			t[iRow][iCol] = f;
	}
    }

  *this = t;
  return *this;

}


//----------------------------------------------------------------------------
StandardMatrix4 StandardMatrix4::operator+ (const StandardMatrix4& rkMatrix) const
{
    StandardMatrix4 kSum;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
        {
            kSum.rowcol[iRow][iCol] = rowcol[iRow][iCol] +
                rkMatrix.rowcol[iRow][iCol];
        }
    }
    return kSum;
}
//----------------------------------------------------------------------------
StandardMatrix4 StandardMatrix4::operator- (const StandardMatrix4& rkMatrix) const
{
    StandardMatrix4 kDiff;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
        {
            kDiff.rowcol[iRow][iCol] = rowcol[iRow][iCol] -
                rkMatrix.rowcol[iRow][iCol];
        }
    }
    return kDiff;
}
//----------------------------------------------------------------------------
StandardMatrix4 StandardMatrix4::operator* (const StandardMatrix4&  rkMatrix) const
{
    StandardMatrix4 kProd;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
        {
            kProd.rowcol[iRow][iCol] =
                rowcol[iRow][0]*rkMatrix.rowcol[0][iCol] +
                rowcol[iRow][1]*rkMatrix.rowcol[1][iCol] +
                rowcol[iRow][2]*rkMatrix.rowcol[2][iCol] +
                rowcol[iRow][3]*rkMatrix.rowcol[3][iCol];
        }
    }
    return kProd;
}
//----------------------------------------------------------------------------
StandardVector4 operator* (const StandardMatrix4 & rkMatrix, const StandardVector4& rkPoint)
{
    StandardVector4 kProd;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        kProd[iRow] =
            rkMatrix.rowcol[iRow][0]*rkPoint[0] +
            rkMatrix.rowcol[iRow][1]*rkPoint[1] +
            rkMatrix.rowcol[iRow][2]*rkPoint[2] +
	    rkMatrix.rowcol[iRow][3]*rkPoint[3];
    }
    return kProd;
}
//----------------------------------------------------------------------------
StandardVector4 operator* (const StandardVector4& rkPoint, const StandardMatrix4& rkMatrix)
{
    StandardVector4 kProd;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        kProd[iRow] =
            rkPoint[0]*rkMatrix.rowcol[0][iRow] +
            rkPoint[1]*rkMatrix.rowcol[1][iRow] +
            rkPoint[2]*rkMatrix.rowcol[2][iRow] +
            rkPoint[3]*rkMatrix.rowcol[3][iRow];
    }
    return kProd;
}
//----------------------------------------------------------------------------
StandardMatrix4 StandardMatrix4::operator- () const
{
    StandardMatrix4 kNeg;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
            kNeg[iRow][iCol] = -rowcol[iRow][iCol];
    }
    return kNeg;
}
//----------------------------------------------------------------------------
StandardMatrix4 StandardMatrix4::operator* (double fScalar) const
{
    StandardMatrix4 kProd;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
            kProd[iRow][iCol] = fScalar*rowcol[iRow][iCol];
    }
    return kProd;
}
//----------------------------------------------------------------------------
StandardMatrix4 operator* (double fScalar, const StandardMatrix4& rkMatrix)
{
    StandardMatrix4 kProd;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
            kProd[iRow][iCol] = fScalar*rkMatrix.rowcol[iRow][iCol];
    }
    return kProd;
}
//----------------------------------------------------------------------------
StandardMatrix4 StandardMatrix4::Transpose () const
{
    StandardMatrix4 kTranspose;
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
            kTranspose[iRow][iCol] = rowcol[iCol][iRow];
    }
    return kTranspose;
}
//----------------------------------------------------------------------------
StandardMatrix4 * StandardMatrix4::Inverse ()
{

  StandardMatrix4 a = *this;
  StandardMatrix4 b = StandardMatrix4::IDENTITY;

  unsigned int r, c;
  unsigned int cc;
  unsigned int rowMax; // Points to max abs value row in this column
  unsigned int row;
  double tmp;

  // Go through columns
  for (c=0; c<4; c++)
  {

    // Find the row with max value in this column
    rowMax = c;

    for (r=c+1; r<4; r++)
    {
      if (fabs(a.rowcol[c][r]) > fabs(a.rowcol[c][rowMax]))
      {
        rowMax = r;
      }
    }

    // If the max value here is 0, we can't invert.  Return identity.
    if (a.rowcol[rowMax][c] == 0.0F)
      return((class StandardMatrix4 *)&StandardMatrix4::IDENTITY);

    // Swap row "rowMax" with row "c"
    for (cc=0; cc<4; cc++)
    {
      tmp = a.rowcol[cc][c];
      a.rowcol[cc][c] = a.rowcol[cc][rowMax];
      a.rowcol[cc][rowMax] = tmp;
      tmp = b.rowcol[cc][c];
      b.rowcol[cc][c] = b.rowcol[cc][rowMax];
      b.rowcol[cc][rowMax] = tmp;
    }

    // Now everything we do is on row "c".
    // Set the max cell to 1 by dividing the entire row by that value
    tmp = a.rowcol[c][c];

    for (cc=0; cc<4; cc++)
    {
      a.rowcol[cc][c] /= tmp;
      b.rowcol[cc][c] /= tmp;
    }

    // Now do the other rows, so that this column only has a 1 and 0's
    for (row = 0; row < 4; row++)
    {
      if (row != c)
      {
        tmp = a.rowcol[c][row];
        for (cc=0; cc<4; cc++)
        {
          a.rowcol[cc][row] -= a.rowcol[cc][c] * tmp;
          b.rowcol[cc][row] -= b.rowcol[cc][c] * tmp;
        }
      }
    }

  }

  *this = b;

  return this;


/*

	StandardVector4		mat2Ident[4];
	StandardVector4		mat2Inv[4];
	StandardVector4		*rows2Ident[4];
	StandardVector4		*rows2Inv[4];
	StandardVector4		*ptRow0, *ptRow1;
	StandardVector4		tvec0, tvec1;
	double mult;


	rows2Ident[0] = &mat2Ident[0];	rows2Inv[0] = &mat2Inv[0];
	rows2Ident[1] = &mat2Ident[1];	rows2Inv[1] = &mat2Inv[1];
	rows2Ident[2] = &mat2Ident[2];	rows2Inv[2] = &mat2Inv[2];
	rows2Ident[3] = &mat2Ident[3];	rows2Inv[3] = &mat2Inv[3];

	mat2Ident[0][0] = this->m_aafEntry[0][0];	mat2Inv[0][0] = 1.0f;
	mat2Ident[0][1] = this->m_aafEntry[0][1];	mat2Inv[0][1] = 0.0f;
	mat2Ident[0][2] = this->m_aafEntry[0][2];	mat2Inv[0][2] = 0.0f;
	mat2Ident[0][3] = this->m_aafEntry[0][3];	mat2Inv[0][3] = 0.0f;

	mat2Ident[1][0] = this->m_aafEntry[1][0];	mat2Inv[1][0] = 0.0f;
	mat2Ident[1][1] = this->m_aafEntry[1][1];	mat2Inv[1][1] = 1.0f;
	mat2Ident[1][2] = this->m_aafEntry[1][2];	mat2Inv[1][2] = 0.0f;
	mat2Ident[1][3] = this->m_aafEntry[1][3];	mat2Inv[1][3] = 0.0f;

	mat2Ident[2][0] = this->m_aafEntry[2][0];	mat2Inv[2][0] = 0.0f;
	mat2Ident[2][1] = this->m_aafEntry[2][1];	mat2Inv[2][1] = 0.0f;
	mat2Ident[2][2] = this->m_aafEntry[2][2];	mat2Inv[2][2] = 1.0f;
	mat2Ident[2][3] = this->m_aafEntry[2][3];	mat2Inv[2][3] = 0.0f;

	mat2Ident[3][0] = this->m_aafEntry[3][0];	mat2Inv[3][0] = 0.0f;
	mat2Ident[3][1] = this->m_aafEntry[3][1];	mat2Inv[3][1] = 0.0f;
	mat2Ident[3][2] = this->m_aafEntry[3][2];	mat2Inv[3][2] = 0.0f;
	mat2Ident[3][3] = this->m_aafEntry[3][3];	mat2Inv[3][3] = 1.0f;

	// Pick a pivot
	if( fabs((*(rows2Ident[3]))[0]) > fabs((*(rows2Ident[2]))[0]) ) {
		ptRow0 = rows2Ident[3];			ptRow1 = rows2Inv[3];
		rows2Ident[3] = rows2Ident[2];	rows2Inv[3] = rows2Inv[2];
		rows2Ident[2] = ptRow0;			rows2Inv[2] = ptRow1;
	}
	if( fabs((*(rows2Ident[2]))[0]) > fabs((*(rows2Ident[1]))[0]) ) {
		ptRow0 = rows2Ident[2];			ptRow1 = rows2Inv[2];
		rows2Ident[2] = rows2Ident[1];	rows2Inv[2] = rows2Inv[1];
		rows2Ident[1] = ptRow0;			rows2Inv[1] = ptRow1;
	}
	if( fabs((*(rows2Ident[1]))[0]) > fabs((*(rows2Ident[0]))[0]) ) {
		ptRow0 = rows2Ident[1];			ptRow1 = rows2Inv[1];
		rows2Ident[1] = rows2Ident[0];	rows2Inv[1] = rows2Inv[0];
		rows2Ident[0] = ptRow0;			rows2Inv[0] = ptRow1;
	}

	// Clean the first column
	mult = (*(rows2Ident[0]))[0];
	*(rows2Ident[0]) /= mult;		*(rows2Inv[0]) /= mult;
	tvec0 = *(rows2Ident[0]);		tvec1 = *(rows2Inv[0]);
	mult = (*(rows2Ident[1]))[0];
	tvec0 *= mult;					tvec1 *= mult;
	*(rows2Ident[1]) -= tvec0;		*(rows2Inv[1]) -= tvec1;
	tvec0 = *(rows2Ident[0]);		tvec1 = *(rows2Inv[0]);
	mult = (*(rows2Ident[2]))[0];
	tvec0 *= mult;					tvec1 *= mult;
	*(rows2Ident[2]) -= tvec0;		*(rows2Inv[2]) -= tvec1;
	tvec0 = *(rows2Ident[0]);		tvec1 = *(rows2Inv[0]);
	mult = (*(rows2Ident[3]))[0];
	tvec0 *= mult;					tvec1 *= mult;
	*(rows2Ident[3]) -= tvec0;		*(rows2Inv[3]) -= tvec1;

	// Pick a new pivot
	if( fabs((*(rows2Ident[3]))[1]) > fabs((*(rows2Ident[2]))[1]) ) {
		ptRow0 = rows2Ident[3];			ptRow1 = rows2Inv[3];
		rows2Ident[3] = rows2Ident[2];	rows2Inv[3] = rows2Inv[2];
		rows2Ident[2] = ptRow0;			rows2Inv[2] = ptRow1;
	}
	if( fabs((*(rows2Ident[2]))[1]) > fabs((*(rows2Ident[1]))[1]) ) {
		ptRow0 = rows2Ident[2];			ptRow1 = rows2Inv[2];
		rows2Ident[2] = rows2Ident[1];	rows2Inv[2] = rows2Inv[1];
		rows2Ident[1] = ptRow0;			rows2Inv[1] = ptRow1;
	}

	// Clean second column
	mult = (*(rows2Ident[1]))[1];
	*(rows2Ident[1]) /= mult;		*(rows2Inv[1]) /= mult;
	tvec0 = *(rows2Ident[1]);		tvec1 = *(rows2Inv[1]);
	mult = (*(rows2Ident[0]))[1];
	tvec0 *= mult;					tvec1 *= mult;
	*(rows2Ident[0]) -= tvec0;		*(rows2Inv[0]) -= tvec1;
	tvec0 = *(rows2Ident[1]);		tvec1 = *(rows2Inv[1]);
	mult = (*(rows2Ident[2]))[1];
	tvec0 *= mult;					tvec1 *= mult;
	*(rows2Ident[2]) -= tvec0;		*(rows2Inv[2]) -= tvec1;
	tvec0 = *(rows2Ident[1]);		tvec1 = *(rows2Inv[1]);
	mult = (*(rows2Ident[3]))[1];
	tvec0 *= mult;					tvec1 *= mult;
	*(rows2Ident[3]) -= tvec0;		*(rows2Inv[3]) -= tvec1;

	// Pick a pivot
	if( fabs((*(rows2Ident[3]))[2]) > fabs((*(rows2Ident[2]))[2]) ) {
		ptRow0 = rows2Ident[3];			ptRow1 = rows2Inv[3];
		rows2Ident[3] = rows2Ident[2];	rows2Inv[3] = rows2Inv[2];
		rows2Ident[2] = ptRow0;			rows2Inv[2] = ptRow1;
	}

	// Clean the third column
	mult = (*(rows2Ident[2]))[2];
	*(rows2Ident[2]) /= mult;		*(rows2Inv[2]) /= mult;
	tvec0 = *(rows2Ident[2]);		tvec1 = *(rows2Inv[2]);
	mult = (*(rows2Ident[0]))[2];
	tvec0 *= mult;					tvec1 *= mult;
	*(rows2Ident[0]) -= tvec0;		*(rows2Inv[0]) -= tvec1;
	tvec0 = *(rows2Ident[2]);		tvec1 = *(rows2Inv[2]);
	mult = (*(rows2Ident[1]))[2];
	tvec0 *= mult;					tvec1 *= mult;
	*(rows2Ident[1]) -= tvec0;		*(rows2Inv[1]) -= tvec1;
	tvec0 = *(rows2Ident[2]);		tvec1 = *(rows2Inv[2]);
	mult = (*(rows2Ident[3]))[2];
	tvec0 *= mult;					tvec1 *= mult;
	*(rows2Ident[3]) -= tvec0;		*(rows2Inv[3]) -= tvec1;

	if( (*(rows2Ident[3]))[3] == 0.0f )
	{
		//Singular
		return( NULL );
	}

	// Clean the fourth column
	mult = (*(rows2Ident[3]))[3];
	*(rows2Ident[3]) /= mult;		*(rows2Inv[3]) /= mult;
	tvec0 = *(rows2Ident[3]);		tvec1 = *(rows2Inv[3]);
	mult = (*(rows2Ident[0]))[3];
	tvec0 *= mult;					tvec1 *= mult;
	*(rows2Ident[0]) -= tvec0;		*(rows2Inv[0]) -= tvec1;
	tvec0 = *(rows2Ident[3]);		tvec1 = *(rows2Inv[3]);
	mult = (*(rows2Ident[1]))[3];
	tvec0 *= mult;					tvec1 *= mult;
	*(rows2Ident[1]) -= tvec0;		*(rows2Inv[1]) -= tvec1;
	tvec0 = *(rows2Ident[3]);		tvec1 = *(rows2Inv[3]);
	mult = (*(rows2Ident[2]))[3];
	tvec0 *= mult;					tvec1 *= mult;
	*(rows2Ident[2]) -= tvec0;		*(rows2Inv[2]) -= tvec1;

	// Copy inverse into this
	this->m_aafEntry[0][0] = (*(rows2Inv[0]))[0];
	this->m_aafEntry[0][1] = (*(rows2Inv[0]))[1];
	this->m_aafEntry[0][2] = (*(rows2Inv[0]))[2];
	this->m_aafEntry[0][3] = (*(rows2Inv[0]))[3];
	this->m_aafEntry[1][0] = (*(rows2Inv[1]))[0];
	this->m_aafEntry[1][1] = (*(rows2Inv[1]))[1];
	this->m_aafEntry[1][2] = (*(rows2Inv[1]))[2];
	this->m_aafEntry[1][3] = (*(rows2Inv[1]))[3];
	this->m_aafEntry[2][0] = (*(rows2Inv[2]))[0];
	this->m_aafEntry[2][1] = (*(rows2Inv[2]))[1];
	this->m_aafEntry[2][2] = (*(rows2Inv[2]))[2];
	this->m_aafEntry[2][3] = (*(rows2Inv[2]))[3];
	this->m_aafEntry[3][0] = (*(rows2Inv[3]))[0];
	this->m_aafEntry[3][1] = (*(rows2Inv[3]))[1];
	this->m_aafEntry[3][2] = (*(rows2Inv[3]))[2];
	this->m_aafEntry[3][3] = (*(rows2Inv[3]))[3];
	
	return(this);
	*/

	/*
	 *	Graphics Gems 4
	 *
	 */
/*
    StandardMatrix4 a = *this;	    // As a evolves from original mat into identity
	StandardMatrix4	b = StandardMatrix4::IDENTITY;  // b evolves from identity into inverse(a)

	int i, j, i1, k;
	double swap[4];

	
    // Loop over cols of a from left to right, eliminating above and below diag
    for(j=0; j<4; j++) {   // Find largest pivot in column j among rows j..3
		i1 = j;		    // Row with largest pivot candidate
		for(i=j+1; i<4; i++) {
			if( fabs(a[i][j]) > fabs(a[i1][j]) ) {
				i1 = i;
			}
		}

		// Swap rows i1 and j in a and b to put pivot on diagonal
		//swap(a.v[i1], a.v[j]);
		memcpy( swap, a[i1], 16 );
		memcpy( a[i1], a[j], 16 );
		memcpy( a[j], swap, 16 );
		//swap(b.v[i1], b.v[j]);
		memcpy( swap, b[i1], 16 );
		memcpy( b[i1], b[j], 16 );
		memcpy( b[j], swap, 16 );

		// Scale row j to have a unit diagonal
		if (a[j][j]==0.0f)
		{
			//Error Singular Matrix
			// Impossible to invert
			return( NULL );
		}

		//b.v[j] /= a.v[j].n[j];
		for( k = 0; k < 4; k++ )
		{
			b[j][k] /= a[j][j];
			//a.v[j] /= a.v[j].n[j];
			a[j][k] /= a[j][j];
		}


		// Eliminate off-diagonal elems in col j of a, doing identical ops to b
		for (i=0; i<3; i++) {
			if (i!=j) {
				for( k = 0; k < 4; k++ )
				{
					//b.v[i] -= a.v[i].n[j]*b.v[j];
					b[i][k] -= a[i][j] * b[j][k];
					//a.v[i] -= a.v[i].n[j]*a.v[j];
					a[i][k] -= a[i][j] * a[j][k];
				}
			}
		}
	}

	for( i = 0; i < 4; i++ )
	{
		for( j = 0; j < 4; j++ )
		{
			if( b[i][j] < 0.000001 && b[i][j] > -0.000001 )
			{
				m_aafEntry[i][j] = 0.0f;
			} else {
				m_aafEntry[i][j] = b[i][j];
			}
		}
	}

	return(this);
*/
}
//----------------------------------------------------------------------------
/*
StandardMatrix4 StandardMatrix4::Inverse (double fTolerance) const
{
    StandardMatrix4 kInverse = StandardMatrix4::ZERO;
    Inverse(kInverse,fTolerance);
    return kInverse;
}
*/
//----------------------------------------------------------------------------
double StandardMatrix4::Determinant () const
{
  // to do


    return 1;
}

//----------------------------------------------------------------------------
void StandardMatrix4::TensorProduct (const StandardVector4& rkU, const StandardVector4& rkV,
    StandardMatrix4& rkProduct)
{
    for (int iRow = 0; iRow < 4; iRow++)
    {
        for (int iCol = 0; iCol < 4; iCol++)
            rkProduct[iRow][iCol] = rkU[iRow]*rkV[iCol];
    }
}
//----------------------------------------------------------------------------

// These routines are using right handed transformations and are based
// on row-column based matrices. They will work with OpenGL or DirectX
// because the linear representation of the matrix will be the same.

//StandardMatrix4 TranslationMatrix4( double x, double y, double z )
//{
//  StandardMatrix4 mat = StandardMatrix4::IDENTITY;
//  mat[3][0] = x;
//  mat[3][1] = y;
//  mat[3][2] = z;
//  return mat;
//}

//StandardMatrix4 TranslationMatrix4( StandardVector3 & position)
//{
//  StandardMatrix4 mat = StandardMatrix4::IDENTITY;
//  mat[3][0] = position.x;
//  mat[3][1] = position.y;
//  mat[3][2] = position.z;
//  return mat;   
//}

StandardMatrix4 TranslationMatrix4( double x, double y, double z )
{
  StandardMatrix4 mat = StandardMatrix4::IDENTITY;
  mat[0][3] = x;
  mat[1][3] = y;
  mat[2][3] = z;
  return mat;
}

StandardMatrix4 TranslationMatrix4( StandardVector3 & position)
{
  StandardMatrix4 mat = StandardMatrix4::IDENTITY;
  mat[0][3] = position.x;
  mat[1][3] = position.y;
  mat[2][3] = position.z;
  return mat;   
}

StandardMatrix4 RotationXMatrix4( double x )
{
  StandardMatrix4 mat = StandardMatrix4::IDENTITY;
  double cx = (double)cos(x);
  double sx = (double)sin(x);
  mat[1][1] = cx;
  mat[1][2] = -sx;
  mat[2][1] = sx;
  mat[2][2] = cx;
  return mat;
}

StandardMatrix4 RotationYMatrix4( double y )
{
  StandardMatrix4 mat = StandardMatrix4::IDENTITY;
  double cy = (double)cos(y);
  double sy = (double)sin(y);

   	mat[0][0] = cy;
	mat[2][0] = -sy;
    mat[0][2] = sy;
	mat[2][2] = cy;
    return mat;
}

StandardMatrix4 RotationZMatrix4( double z )
{
  StandardMatrix4 mat = StandardMatrix4::IDENTITY;
  double cz = (double)cos(z);
  double sz = (double)sin(z);
  mat[0][0] = cz;
  mat[1][0] = sz;
  mat[0][1] = -sz;
  mat[1][1] = cz;
  return mat;
}

StandardMatrix4 RotationAxisMatrix4(double angle, double _x, double _y, double _z)
{


  double x, y, z;
  x=_x; y=_y; z=_z;

  StandardMatrix4 ret = StandardMatrix4::IDENTITY;
  double sinA, cosA;
  double invCosA;
  StandardVector3 nrm(x,y,z);
  nrm.Normalize();

  double xSq, ySq, zSq;

  sinA = (double)sin(angle);
  cosA = (double)cos(angle);
  invCosA = 1.0F - cosA;

  xSq = x * x;
  ySq = y * y;
  zSq = z * z;

  ret[0][0] = (invCosA * xSq) + (cosA);
  ret[0][1] = (invCosA * x * y) - (sinA * z );
  ret[0][2] = (invCosA * x * z) + (sinA * y );

  ret[1][0] = (invCosA * x * y) + (sinA * z);
  ret[1][1] = (invCosA * ySq) + (cosA);
  ret[1][2] = (invCosA * y * z) - (sinA * x);

  ret[2][0] = (invCosA * x * z) - (sinA * y);
  ret[2][1] = (invCosA * y * z) + (sinA * x);
  ret[2][2] = (invCosA * zSq) + (cosA);

  return ret;
}

StandardMatrix4 ScaleMatrix4( double xscale, double yscale, double zscale )
{
	StandardMatrix4 mat4 = StandardMatrix4::IDENTITY;
	mat4[0][0] = xscale;
	mat4[1][1] = yscale;
	mat4[2][2] = zscale;

	return( mat4 );
}

StandardMatrix4 RotationQuatMatrix4( double X, double Y, double Z, double W )
{
	StandardMatrix4 mat4 = StandardMatrix4::IDENTITY;

	mat4[0][0] = 1 - 2 * ( Y*Y + Z*Z );
	mat4[0][1] =     2 * ( X*Y - Z*W );
	mat4[0][2] =     2 * ( X*Z + Y*W );

	mat4[1][0] =     2 * ( X*Y + Z*W );
	mat4[1][1] = 1 - 2 * ( X*X + Z*Z );
	mat4[1][2] =     2 * ( Y*Z - X*W );

	mat4[2][0] =     2 * ( X*Z - Y*W );
	mat4[2][1] =     2 * ( Y*Z + X*W );
	mat4[2][2] = 1 - 2 * ( X*X + Y*Y );

	mat4.Transpose();

	return( mat4 );
}

// Return an orthographic StandardMatrix4 given the width, height,
//   near, and far values for the frustum boundaries.
StandardMatrix4 OrthoMatrix4(double width, double height, 
                        double nearview, double farview) 
{
  StandardMatrix4 ret=StandardMatrix4::ZERO;
  double depth = farview-nearview;

  ret[0][0] = 2.0F / width;

  ret[1][1] = 2.0F / height;

  ret[2][2] = -(2.0F) / depth;

  ret[3][0] = -1;
  ret[3][1] = -1;
  ret[3][2] = -(farview + nearview) / depth;
  ret[3][3] = 1.0F;

  return ret;
}

/*!
 * \fn StandardMatrix4 PerspectiveMatrix4(double fov, double aspect, double nearz, double farz)
 * \param fov  field of view angle in degrees
 * \param aspect  aspect ratio
 * \param nearz   front clipping plane
 * \param farz    back clipping plane
 * \return  The perspective matrix.
 * \brief Returns a right handed perspective matrix. Based on the gluPerspective function.
 *
 * This function calculates a right handed field of view perspective matrix.
 * It is based on the gluPerspective function from openGL.
 */


StandardMatrix4 PerspectiveMatrix4(double fov, double aspect, double nearz, double farz)
{
	double ymax = nearz*(double)tan(fov*PI/360);
	double ymin = -ymax;
	double xmin = ymin*aspect;
    double xmax = ymax*aspect;
	return FrustumMatrix4(xmin,xmax,ymin,ymax,nearz,farz);

}

StandardMatrix4 FrustumMatrix4(double left, double right, 
    double bottom, double top, double nearview, double farview) 
{
  StandardMatrix4 ret=StandardMatrix4::ZERO;
  double width = right-left;
  double height = top-bottom;
  double depth = farview-nearview;

  ret[0][0] = 2.0f*nearview/width;
  ret[1][1] = 2.0f*nearview/(height);
  ret[0][2] = (right+left)/width;
  ret[1][2] = (top+bottom)/height;
  ret[2][2] = -(farview+nearview)/depth;
  ret[3][2] = -1.0f;
  ret[2][3] = -2.0f*farview*nearview/depth;

  return ret;

}

StandardMatrix4 PerspectiveFOVMatrix4(double angle, double aspect, 
    double nearview, double farview) 
{

  StandardMatrix4 ret=StandardMatrix4::ZERO;
  double depth = farview-nearview;
  angle = (double)(PI * (0.5f * angle)/360.0f);
  double cot = (double)cos(angle)/(double)sin(angle);

  ret[0][0] = cot/aspect;
  ret[1][1] = cot;

  ret[2][2] = 2.0f*farview*nearview/depth;
  ret[2][3] = 1.0f;
  ret[3][2] = -(farview*nearview)/depth;

  return ret;

}

StandardMatrix4 LookAtMatrix4(StandardVector3 & Locate,
		                          StandardVector3 & Target,
								  StandardVector3 & UP)
{
	StandardMatrix4 ret = StandardMatrix4::IDENTITY;
//	StandardVector3 F = Target - Locate;
//	F.Normalize();

//	StandardVector3 S = Cross(F, Normalized(UP));
//	S.Normalize();

//	StandardVector3 U = Cross(S, F);
//	U.Normalize();

	StandardVector3 Z = Locate - Target;
	Z.Normalize();

	StandardVector3 Y = UP;

	StandardVector3 X = Cross(Y,Z);
	Y = Cross(Z,X);
	X.Normalize();
	Y.Normalize();

//	ret[0][0] = S.x;
//	ret[0][1] = U.x;
//	ret[0][2] = -F.x;

//	ret[1][0] = S.y;
//	ret[1][1] = U.y;
//	ret[1][2] = -F.y;

//	ret[2][0] = S.z;
//	ret[2][1] = U.z;
//	ret[2][2] = -F.y;

	ret[0][0] = X[0];
	ret[1][0] = X[1];
	ret[2][0] = X[2];
	//ret[3][0] = -Locate[0];

	ret[0][1] = Y[0];
	ret[1][1] = Y[1];
	ret[2][1] = Y[2];
	//ret[3][1] = -Locate[1];

	ret[0][2] = Z[0];
	ret[1][2] = Z[1];
	ret[2][2] = Z[2];
	//ret[3][2] = -Locate[2];

	StandardMatrix4 trans = TranslationMatrix4(-Locate[0], -Locate[1], -Locate[2]);
    ret = trans*ret;

	return ret;
}

void StandardMatrix4::Print(FILE * stream)
{
  fprintf(stream, "[%f, %f, %f, %f]\n", rowcol[0][0], rowcol[0][1], rowcol[0][2], rowcol[0][3]);
  fprintf(stream, "[%f, %f, %f, %f]\n", rowcol[1][0], rowcol[1][1], rowcol[1][2], rowcol[1][3]);
  fprintf(stream, "[%f, %f, %f, %f]\n", rowcol[2][0], rowcol[2][1], rowcol[2][2], rowcol[2][3]);
  fprintf(stream, "[%f, %f, %f, %f]\n", rowcol[3][0], rowcol[3][1], rowcol[3][2], rowcol[3][3]);

}

void StandardMatrix4::Print(char *String)
{
  sprintf(String, "[%+f, %+f, %+f, %+f]\n[%+f, %+f, %+f, %+f]\n[%+f, %+f, %+f, %+f]\n[%+f, %+f, %+f, %+f]",
    rowcol[0][0], rowcol[0][1], rowcol[0][2], rowcol[0][3],
    rowcol[1][0], rowcol[1][1], rowcol[1][2], rowcol[1][3],
    rowcol[2][0], rowcol[2][1], rowcol[2][2], rowcol[2][3],
    rowcol[3][0], rowcol[3][1], rowcol[3][2], rowcol[3][3]);

}

