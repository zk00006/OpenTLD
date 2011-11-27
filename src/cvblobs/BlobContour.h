#ifndef BLOBCONTOUR_H_INCLUDED
#define BLOBCONTOUR_H_INCLUDED


#include "list"
#include <opencv/cv.h>
//#include "cxtypes.h"  //AO
#include <opencv/cxcore.h>   //

//! Type of chain codes
typedef unsigned char t_chainCode;
//! Type of list of chain codes
typedef CvSeq* t_chainCodeList;
//! Type of list of points
typedef CvSeq* t_PointList;


//! Max order of calculated moments
#define MAX_MOMENTS_ORDER		3


//! Blob contour class (in crack code)
class CBlobContour
{
	friend class CBlob;
	friend class CBlobProperties; //AO
	
public:
	//! Constructors
	CBlobContour();
	CBlobContour(CvPoint startPoint, CvMemStorage *storage );
	//! Copy constructor
	CBlobContour( CBlobContour *source );

	~CBlobContour();
	//! Assigment operator
	CBlobContour& operator=( const CBlobContour &source );

	//! Add chain code to contour
	void AddChainCode(t_chainCode code);

	//! Return freeman chain coded contour
	t_chainCodeList GetChainCode()
	{
		return m_contour;
	}

	bool IsEmpty()
	{
		return m_contour == NULL || m_contour->total == 0;
	}

	//! Return all contour points
	t_chainCodeList GetContourPoints();

protected:	

	CvPoint GetStartPoint() const
	{
		return m_startPoint;
	}

	//! Clears chain code contour
	void ResetChainCode();
	

	
	//! Computes area from contour
	double GetArea();
	//! Computes perimeter from contour
	double GetPerimeter();
	//! Get contour moment (p,q up to MAX_CALCULATED_MOMENTS)
	double GetMoment(int p, int q);

	//! Crack code list
	t_chainCodeList m_contour; 	

private:
	//! Starting point of the contour
	CvPoint m_startPoint;
	//! All points from the contour
	t_PointList m_contourPoints;



	//! Computed area from contour
	double m_area;
	//! Computed perimeter from contour
	double m_perimeter;
	//! Computed moments from contour
	CvMoments m_moments;

	//! Pointer to storage
	CvMemStorage *m_parentStorage;
};

#endif	//!BLOBCONTOUR_H_INCLUDED


