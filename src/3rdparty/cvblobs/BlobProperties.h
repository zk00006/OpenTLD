
//! Disable warnings referred to 255 character truncation for the std:map
#pragma warning( disable : 4786 ) 

#ifndef BLOB_PROPERTIES_H_INCLUDED
#define BLOB_PROPERTIES_H_INCLUDED

#include <opencv/cxcore.h>
#include "BlobLibraryConfiguration.h"
#include "BlobContour.h"


#ifdef BLOB_OBJECT_FACTORY
	//! Object factory pattern implementation
	#include "..\inspecta\DesignPatterns\ObjectFactory.h"
#endif


//! Type of labelled images
typedef unsigned int t_labelType;

//! Max order of calculated moments
#define MAX_MOMENTS_ORDER               3


//! Blob class
class CBlobProperties
{
	typedef std::list<CBlobContour> t_contourList;

public:

        CBlobProperties();
        virtual ~CBlobProperties();

        //! Get blob area
        double GetArea();

        //! Get blob perimeter
        double GetPerimeter();

        //! Get contour moment (p,q up to MAX_CALCULATED_MOMENTS)
        double GetMoment(int p, int q);
        

	//////////////////////////////////////////////////////////////////////////
	// Blob contours
	//////////////////////////////////////////////////////////////////////////


	//! Contour storage memory
	CvMemStorage *m_storage;
	//! External contour of the blob (crack codes)
	CBlobContour m_externalContour;
	//! Internal contours (crack codes)
	t_contourList m_internalContours;

private:
	
        //! Computed area from blob
        double m_area;
        //! Computed perimeter from blob
        double m_perimeter;
        // Computed moment from the blob
        double m_moment[MAX_MOMENTS_ORDER*MAX_MOMENTS_ORDER];

};

#endif //!BLOB_PROPERTIES_H_INCLUDED

