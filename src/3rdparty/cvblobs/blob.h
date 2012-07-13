/************************************************************************
  			Blob.h
  			
FUNCIONALITAT: Definició de la classe CBlob
AUTOR: Inspecta S.L.
MODIFICACIONS (Modificació, Autor, Data):

FUNCTIONALITY: Definition of the CBlob class and some helper classes to perform
			   some calculations on it
AUTHOR: Inspecta S.L.
MODIFICATIONS (Modification, Author, Date):

**************************************************************************/

//! Disable warnings referred to 255 character truncation for the std:map
#pragma warning( disable : 4786 ) 

#ifndef CBLOB_INSPECTA_INCLUDED
#define CBLOB_INSPECTA_INCLUDED

#include <opencv/cxcore.h>
#include "BlobLibraryConfiguration.h"
#include "BlobContour.h"


#ifdef BLOB_OBJECT_FACTORY
	//! Object factory pattern implementation
	#include "..\inspecta\DesignPatterns\ObjectFactory.h"
#endif


//! Type of labelled images
typedef unsigned int t_labelType;


//! Blob class
class CBlob
{
	typedef std::list<CBlobContour> t_contourList;

public:
	CBlob();
	CBlob( t_labelType id, CvPoint startPoint, CvSize originalImageSize );
	~CBlob();

	//! Copy constructor
	CBlob( const CBlob &src );
	CBlob( const CBlob *src );

	//! Operador d'assignació
	//! Assigment operator
	CBlob& operator=(const CBlob &src );
	
	//! Adds a new internal contour to the blob
	void AddInternalContour( const CBlobContour &newContour );
	
	//! Retrieves contour in Freeman's chain code
	CBlobContour *GetExternalContour()
	{
		return &m_externalContour;
	}

	//! Retrieves blob storage
	CvMemStorage *GetStorage()
	{
		return m_storage;
	}

	//! Get label ID
	t_labelType GetID()
	{
		return m_id;
	}
	//! > 0 for extern blobs, 0 if not
	int	  Exterior( IplImage *mask, bool xBorder = true, bool yBorder = true );
	//! Compute blob's area
	double Area();
	//! Compute blob's perimeter
	double Perimeter();
	//! Compute blob's moment (p,q up to MAX_CALCULATED_MOMENTS)
	double Moment(int p, int q);

	//! Compute extern perimeter 
	double ExternPerimeter( IplImage *mask, bool xBorder  = true, bool yBorder = true );
	
	//! Get mean grey color
	double Mean( IplImage *image );

	//! Get standard deviation grey color
	double StdDev( IplImage *image );

	//! Indica si el blob està buit ( no té cap info associada )
	//! Shows if the blob has associated information
	bool IsEmpty();

	//! Retorna el poligon convex del blob
	//! Calculates the convex hull of the blob
	t_PointList GetConvexHull();

	//! Pinta l'interior d'un blob d'un color determinat
	//! Paints the blob in an image
	void FillBlob( IplImage *imatge, CvScalar color, int offsetX = 0, int offsetY = 0 );

	//! Join a blob to current one (add's contour
	void JoinBlob( CBlob *blob );

	//! Get bounding box
	CvRect GetBoundingBox();
	//! Get bounding ellipse
	CvBox2D GetEllipse();

	//! Minimun X	
	double MinX()
	{
		return GetBoundingBox().x;
	}
	//! Minimun Y
	double MinY()
	{
		return GetBoundingBox().y;
	}
	//! Maximun X
	double MaxX()
	{
		return GetBoundingBox().x + GetBoundingBox().width;
	}
	//! Maximun Y
	double MaxY()
	{
		return GetBoundingBox().y + GetBoundingBox().height;
	}
private:
	
	//! Deallocates all contours
	void ClearContours();
	//////////////////////////////////////////////////////////////////////////
	// Blob contours
	//////////////////////////////////////////////////////////////////////////


	//! Contour storage memory
	CvMemStorage *m_storage;
	//! External contour of the blob (crack codes)
	CBlobContour m_externalContour;
	//! Internal contours (crack codes)
	t_contourList m_internalContours;

	//////////////////////////////////////////////////////////////////////////
	// Blob features
	//////////////////////////////////////////////////////////////////////////
	
	//! Label number
	t_labelType m_id;
	//! Area
	double m_area;
	//! Perimeter
	double m_perimeter;
	//! Extern perimeter from blob
	double m_externPerimeter;
	//! Mean gray color
	double m_meanGray;
	//! Standard deviation from gray color blob distribution
	double m_stdDevGray;
	//! Bounding box
	CvRect m_boundingBox;
	//! Bounding ellipse
	CvBox2D m_ellipse;
	//! Sizes from image where blob is extracted
	CvSize m_originalImageSize;
};

#endif //CBLOB_INSPECTA_INCLUDED
