#if !defined(_COMPONENT_LABELING_H_INCLUDED)
#define _CLASSE_BLOBRESULT_INCLUDED

#include "vector"
#include "BlobContour.h"
#include "Blob.h"


//! definició de que es un vector de blobs
typedef std::vector<CBlob*>	Blob_vector;



bool ComponentLabeling(	IplImage* inputImage,
						IplImage* maskImage,
						unsigned char backgroundColor,
						Blob_vector &blobs );


void contourTracing( IplImage *image, IplImage *mask, CvPoint contourStart, t_labelType *labels, 
					 bool *visitedPoints, t_labelType label,
					 bool internalContour, unsigned char backgroundColor,
					 CBlobContour *currentBlobContour );

CvPoint tracer( IplImage *image, IplImage *mask, CvPoint P, bool *visitedPoints,
				short initialMovement,
				unsigned char backgroundColor, short &movement );
				

#endif	//!_CLASSE_BLOBRESULT_INCLUDED