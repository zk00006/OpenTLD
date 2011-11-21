#ifndef BLOB_PROPERTIES_H_INCLUDED
#define BLOB_PROPERTIES_H_INCLUDED

//! Max order of calculated moments
#define MAX_MOMENTS_ORDER		3

class CBlobProperties
{
public:
	CBlobProperties();
	virtual ~CBlobProperties();

	//! Get blob area
	double GetArea();

	//! Get blob perimeter
	double GetPerimeter();

	//! Get contour moment (p,q up to MAX_CALCULATED_MOMENTS)
	double GetMoment(int p, int q);
	
private:

	//! Computed area from blob
	double m_area;
	//! Computed perimeter from blob
	double m_perimeter;
	// Computed moment from the blob
	double m_moment[MAX_MOMENTS_ORDER*MAX_MOMENTS_ORDER];

};

#endif	//!BLOB_PROPERTIES_H_INCLUDED