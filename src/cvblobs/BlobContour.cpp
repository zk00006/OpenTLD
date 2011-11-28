#include "BlobContour.h"
#include "cxcore.h"

CBlobContour::CBlobContour()
{
	m_startPoint.x = 0;
	m_startPoint.y = 0;
	m_area = -1;
	m_perimeter = -1;
	m_contourPoints = NULL;
	m_moments.m00 = -1;
	m_contour = NULL;
	m_parentStorage = NULL;
}
CBlobContour::CBlobContour(CvPoint startPoint, CvMemStorage *storage )
{
	m_startPoint.x = startPoint.x;
	m_startPoint.y = startPoint.y;
	m_area = -1;
	m_perimeter = -1;
	m_moments.m00 = -1;

	m_parentStorage = storage;

	m_contourPoints = NULL;

	// contour sequence: must be compatible with opencv functions
	m_contour = cvCreateSeq( CV_SEQ_ELTYPE_CODE | CV_SEQ_KIND_CURVE | CV_SEQ_FLAG_CLOSED,
			     		 sizeof(CvContour),
			     		 sizeof(t_chainCode),m_parentStorage);

}


//! Copy constructor
CBlobContour::CBlobContour( CBlobContour *source )
{
	if (source != NULL )
	{
		*this = *source;
	}
}

CBlobContour::~CBlobContour()
{
	// let parent blob deallocate all contour and contour point memory
	m_contour = NULL;
	m_contourPoints = NULL;
}


//! Copy operator
CBlobContour& CBlobContour::operator=( const CBlobContour &source )
{
	if( this != &source )
	{		
		m_startPoint = source.m_startPoint;

		m_parentStorage = source.m_parentStorage;
		
		if (m_contour)
		{
			cvClearSeq( m_contour );
		}

		if (source.m_contour)
		{
			m_contour =	cvCloneSeq( source.m_contour, m_parentStorage);
		}
		
		if( source.m_contourPoints )
		{
			if( m_contourPoints )
				cvClearSeq( m_contourPoints );
			m_contourPoints = cvCloneSeq( source.m_contourPoints, m_parentStorage);
		}

		m_area = source.m_area;
		m_perimeter = source.m_area;
		m_moments = source.m_moments;
	}
	return *this;
}


/**
- FUNCIÓ: AddChainCode
- FUNCIONALITAT: Add chain code to contour
- PARÀMETRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACIÓ: 2008/05/06
- MODIFICACIÓ: Data. Autor. Descripció.
*/
void CBlobContour::AddChainCode(t_chainCode chaincode)
{
	cvSeqPush(m_contour, &chaincode);
}

//! Clears chain code contour and points
void CBlobContour::ResetChainCode()
{
	if( m_contour )
	{
		cvClearSeq( m_contour );
		m_contour = NULL;
	}
	if( m_contourPoints )
	{
		cvClearSeq( m_contourPoints );
		m_contourPoints = NULL;
	}
}

/**
- FUNCIÓ: GetPerimeter
- FUNCIONALITAT: Get perimeter from chain code. Diagonals sum sqrt(2) and horizontal and vertical codes 1
- PARÀMETRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACIÓ: 2008/04/30
- MODIFICACIÓ: Data. Autor. Descripció.
- NOTA: Algorithm derived from "Methods to estimate area and perimeters of blob-like objects: A comparison", L.Yang
*/
double CBlobContour::GetPerimeter()
{
	// is calculated?
	if (m_perimeter != -1)
	{
		return m_perimeter;
	}

	if( IsEmpty() )
		return 0;

	m_perimeter = cvContourPerimeter( GetContourPoints() );

	return m_perimeter;
}

/**
- FUNCIÓ: GetArea
- FUNCIONALITAT: Computes area from chain code
- PARÀMETRES:
	- 
- RESULTAT:
	- May give negative areas for clock wise contours
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACIÓ: 2008/04/30
- MODIFICACIÓ: Data. Autor. Descripció.
- NOTA: Algorithm derived from "Properties of contour codes", G.R. Wilson
*/
double CBlobContour::GetArea()
{
	// is calculated?
	if (m_area != -1)
	{
		return m_area;
	}

	if( IsEmpty() )
		return 0;

	m_area = fabs( cvContourArea( GetContourPoints() ));
	
	return m_area;
}

//! Get contour moment (p,q up to MAX_CALCULATED_MOMENTS)
double CBlobContour::GetMoment(int p, int q)
{
	// is a valid moment?
	if ( p < 0 || q < 0 || p > MAX_MOMENTS_ORDER || q > MAX_MOMENTS_ORDER )
	{
		return -1;
	}

	if( IsEmpty() )
		return 0;

	// it is calculated?
	if( m_moments.m00 == -1)
	{
		cvMoments( GetContourPoints(), &m_moments );
	}
		
	return cvGetSpatialMoment( &m_moments, p, q );

	
}

//! Calculate contour points from crack codes
t_PointList CBlobContour::GetContourPoints()
{
	// it is calculated?
	if( m_contourPoints != NULL )
		return m_contourPoints;

	if ( m_contour == NULL || m_contour->total <= 0 )
	{
		return NULL;
	}

	CvSeq *tmpPoints;
	CvSeqReader reader;
	CvSeqWriter writer;
	CvPoint actualPoint;
	CvRect boundingBox;

	// if aproximation is different than simple extern perimeter will not work
	tmpPoints = cvApproxChains( m_contour, m_parentStorage, CV_CHAIN_APPROX_NONE);


	// apply an offset to contour points to recover real coordinates
	
	cvStartReadSeq( tmpPoints, &reader);

	m_contourPoints = cvCreateSeq( tmpPoints->flags, tmpPoints->header_size, tmpPoints->elem_size, m_parentStorage );
	cvStartAppendToSeq(m_contourPoints, &writer );

	// also calculate bounding box of the contour to allow cvPointPolygonTest
	// work correctly on the generated polygon
	boundingBox.x = boundingBox.y = 10000;
	boundingBox.width = boundingBox.height = 0;
	
	for( int i=0; i< tmpPoints->total; i++)
	{
		CV_READ_SEQ_ELEM( actualPoint, reader);

		actualPoint.x += m_startPoint.x;
		actualPoint.y += m_startPoint.y;

		boundingBox.x = MIN( boundingBox.x, actualPoint.x );
		boundingBox.y = MIN( boundingBox.y, actualPoint.y );
		boundingBox.width = MAX( boundingBox.width, actualPoint.x );
		boundingBox.height = MAX( boundingBox.height, actualPoint.y );
		
		CV_WRITE_SEQ_ELEM( actualPoint, writer );
	}
	cvEndWriteSeq( &writer );
	cvClearSeq( tmpPoints );

	// assign calculated bounding box
	((CvContour*)m_contourPoints)->rect = boundingBox;


	return m_contourPoints;
}
