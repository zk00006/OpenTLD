
#include "ComponentLabeling.h"

//! Conversion from freeman code to coordinate increments (counterclockwise)
static const CvPoint freemanCodeIncrement[8] =
    { {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1} };



/**
- FUNCIÓ: 
- FUNCIONALITAT: 
- PARÀMETRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACIÓ: 2008/04/29
- MODIFICACIÓ: Data. Autor. Descripció.
*/
inline unsigned char GET_ABOVE_IMAGEPIXEL( unsigned char *currentPixel, IplImage *image )
{
	return *(currentPixel - image->widthStep);
}
inline unsigned char GET_BELOW_IMAGEPIXEL( unsigned char *currentPixel, IplImage *image )
{
	return *(currentPixel + image->widthStep);
}



inline unsigned char GET_IMAGE_PIXEL( IplImage *image, CvPoint p )
{
	return *(image->imageData + p.x + p.y *image->widthStep);
}

inline bool GET_IMAGEMASK_PIXEL( IplImage *mask, CvPoint p )
{
	if( mask != NULL )
		return ((unsigned char)*(mask->imageData + p.x + p.y *mask->widthStep)) > 0;
	else
		return true;
}
inline bool GET_BELOW_VISITEDPIXEL( bool *currentPixel, int imageWidth )
{
	return *( currentPixel + imageWidth );
}

/**
- FUNCIÓ: ASSIGN_LABEL
- FUNCIONALITAT: Assigns label value to label image
- PARÀMETRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACIÓ: 2008/04/29
- MODIFICACIÓ: Data. Autor. Descripció.
*/
inline void ASSIGN_LABEL( CvPoint p, t_labelType *labels, int imageWidth, int newLabel )
{
	*(labels + p.y * imageWidth + p.x) = newLabel;
}


inline void ASSIGN_VISITED( CvPoint p, bool *visitedPoints, int imageWidth  )
{
	*(visitedPoints + p.y * imageWidth + p.x) = true;
}

/**
- FUNCIÓ: ComponentLabeling
- FUNCIONALITAT: Calcula els components binaris (blobs) d'una imatge amb connectivitat a 8
- PARÀMETRES:
	- inputImage: image to segment (pixel values different than blobColor are treated as background)
	- maskImage: if not NULL, all the pixels equal to 0 in mask are skipped in input image
	- backgroundColor: color of background (ignored pixels)
	- blobs: blob vector destination
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACIÓ: 2008/04/21
- MODIFICACIÓ: Data. Autor. Descripció.
- NOTA: Algorithm based on "A linear-time component labeling algorithm using contour tracing technique", 
		F.Chang et al
*/
bool ComponentLabeling(	IplImage* inputImage,
						IplImage* maskImage,
						unsigned char backgroundColor,
						Blob_vector &blobs )
{
	int i,j;
	// row major vector with visited points 
	bool *visitedPoints, *pVisitedPoints, internalContour, externalContour;
	unsigned char *pInputImage, *pMask, *pAboveInputImage, *pBelowInputImage,
				  *pAboveMask, *pBelowMask;
	int imageWidth, imageHeight, currentLabel, contourLabel;
	// row major vector with labelled image 
	t_labelType *labelledImage, *pLabels;
	//! current blob pointer
	CBlob *currentBlob;
	CvSize imageSizes;
	CvPoint currentPoint;

	// verify input image
	if( !CV_IS_IMAGE( inputImage ) )
		return false;

	// verify that input image and mask image has same size
	if( maskImage )
	{
		if( !CV_IS_IMAGE(maskImage) || 
			maskImage->width != inputImage->width || 
			maskImage->height != inputImage->height )
		return false;
	}
	else
	{
		pMask = NULL;
		pAboveMask = NULL;
		pBelowMask = NULL;
	}

	imageSizes = cvSize(inputImage->width,inputImage->height);
	
	imageWidth = inputImage->width;
	imageHeight = inputImage->height;

	// create auxiliary buffers
	labelledImage = (t_labelType*) malloc( inputImage->width * inputImage->height * sizeof(t_labelType) );
	visitedPoints = (bool*) malloc( inputImage->width * inputImage->height * sizeof(bool) );

	// initialize it to 0
	memset(labelledImage, 0, inputImage->width * inputImage->height * sizeof(t_labelType) ) ;
	memset(visitedPoints, false, inputImage->width * inputImage->height * sizeof(bool) ) ;

	// initialize pointers and label counter
	pLabels = labelledImage;
	pVisitedPoints = visitedPoints;
	currentLabel = 1;

	for (j = 0; j < imageHeight; j++ )
	{
		// don't verify if we area on first or last row, it will verified on pointer access
		pAboveInputImage = (unsigned char*) inputImage->imageData + (j-1) * inputImage->widthStep;
		pBelowInputImage = (unsigned char*) inputImage->imageData + (j+1) * inputImage->widthStep;
	
		pInputImage = (unsigned char*) inputImage->imageData + j * inputImage->widthStep;

		if( maskImage )
		{
			pMask = (unsigned char*) maskImage->imageData + j * maskImage->widthStep;
			// don't verify if we area on first or last row, it will verified on pointer access
			pAboveMask = (unsigned char*) maskImage->imageData + (j-1) * maskImage->widthStep;
			pBelowMask = (unsigned char*) maskImage->imageData + (j+1) * maskImage->widthStep;

		}
		
		for (i = 0; i < imageWidth; i++, pInputImage++, pMask++, pAboveInputImage++, pBelowInputImage++,
										 pAboveMask++, pBelowMask++ )
		{
			// ignore background pixels or 0 pixels in mask
			if ( (*pInputImage == backgroundColor) || (maskImage && *pMask == 0 ))
			{
				pLabels++;
				pVisitedPoints++;
				continue;
			}
			
			// new external contour: current label == 0 and above pixel is background
			if( j > 0 )
			{
				externalContour = ((*pAboveInputImage == backgroundColor) || 
								  (maskImage && *pAboveMask == 0)) && 
								  (*pLabels == 0);
			}
			else
				externalContour = (*pLabels == 0);

			// new internal contour: below pixel is background and not visited
			if( !externalContour && j < imageHeight - 1 )
			{
				internalContour = *pBelowInputImage == backgroundColor &&
								  !GET_BELOW_VISITEDPIXEL( pVisitedPoints, imageWidth);
			}
			else
			{
				internalContour = false;
			}
			
			
			if( externalContour )
			{
				currentPoint = cvPoint(i,j);
				// assign label to labelled image
				*pLabels = currentLabel;
				
				// create new blob
				currentBlob = new CBlob(currentLabel, currentPoint, imageSizes );

				// contour tracing with currentLabel
				contourTracing( inputImage, maskImage, currentPoint, 
								labelledImage, visitedPoints, 
								currentLabel, false, backgroundColor, currentBlob->GetExternalContour() );

				// add new created blob
				blobs.push_back(currentBlob);

				currentLabel++;
			}
			else 
			{
				if( internalContour )
				{
					currentPoint = cvPoint(i,j);

					if( *pLabels == 0 )
					{
						// take left neightbour value as current
						if( i > 0 )
							contourLabel = *(pLabels - 1);
					}
					else
					{
						contourLabel = *pLabels;
					}

					if(contourLabel>0)
					{
						currentBlob = blobs[contourLabel-1];
						CBlobContour newContour(currentPoint, currentBlob->GetStorage());
						

						// contour tracing with contourLabel
						contourTracing( inputImage, maskImage, currentPoint, labelledImage, visitedPoints,
										contourLabel, true, backgroundColor, &newContour ); 

						currentBlob->AddInternalContour( newContour );
					}
				}
				// neither internal nor external contour
				else
				{
					// take left neightbour value as current if it is not labelled
					if( i > 0 && *pLabels == 0 )
						*pLabels = *(pLabels - 1);
				}

			}
			
			pLabels++;
			pVisitedPoints++;

		}
	}


	// free auxiliary buffers
	free( labelledImage );
	free( visitedPoints );

	return true;
}



/**
- FUNCIÓ: 
- FUNCIONALITAT: 
- PARÀMETRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACIÓ: 2008/04/29
- MODIFICACIÓ: Data. Autor. Descripció.
*/
void contourTracing( IplImage *image, 
					 IplImage *maskImage,
					 CvPoint contourStart, t_labelType *labels, bool *visitedPoints, t_labelType label,
					 bool internalContour, unsigned char backgroundColor, CBlobContour *currentBlobcontour )
{
	CvPoint t, tnext, tsecond;
	short initialMovement, movement;

	if( internalContour )
	{
		initialMovement = 7;//3;
	}
	else
	{
		initialMovement = 3;//7;
	}

	tsecond = tracer( image, maskImage, contourStart, visitedPoints, initialMovement, 
					backgroundColor, movement );
	
	// assign current label to tnext
	ASSIGN_LABEL( contourStart, labels, image->width, label );
	

	// contour corresponds to isolated pixel? 
	if( tsecond.x == contourStart.x && tsecond.y == contourStart.y )
	{
		// we are finished with the contour
		return;
	}

	// add chain code to current contour
	currentBlobcontour->AddChainCode(movement);
	
	// assign label to next point 
	ASSIGN_LABEL( tsecond, labels, image->width, label );
	
	tnext.x = tsecond.x;
	tnext.y = tsecond.y;
	t.x = tnext.x;
	t.y = tnext.y;
	
	// while T is different than contourStart and Tnext is different than T
	// follow contour until start point is reached again
	while ( t.x != contourStart.x || t.y != contourStart.y || 
			tsecond.x != tnext.x || tsecond.y != tnext.y )
	{
		
		t.x = tnext.x;
		t.y = tnext.y;
		initialMovement = (movement + 5) % 8;
		
		// search for next contour point
		tnext = tracer( image, maskImage, t, visitedPoints, initialMovement, 
						backgroundColor, movement );
		
		// assign label to contour point
		ASSIGN_LABEL( tnext, labels, image->width, label );

		// add chain code to current contour
		currentBlobcontour->AddChainCode(movement);
	}

}

/**
- FUNCIÓ: tracer
- FUNCIONALITAT: Searches for next point of a contour
- PARÀMETRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACIÓ: 2008/04/30
- MODIFICACIÓ: Data. Autor. Descripció.
*/
CvPoint tracer( IplImage *image, IplImage *maskImage, CvPoint P, bool *visitedPoints,
				short initialMovement,
				unsigned char backgroundColor, short &movement )
{
	int d;
	CvPoint pNext;
	
	for (d = 0; d <= 7; d++ )
	{
		movement = (initialMovement + d) % 8;
		
		pNext.x = P.x + freemanCodeIncrement[movement].x;
		pNext.y = P.y + freemanCodeIncrement[movement].y;

		// the point is inside image ?
		if( pNext.x < 0 || pNext.x >= image->width || 
			pNext.y < 0 || pNext.y >= image->height )
		{
			// try other movement
			continue;
		}
		
		// image has blobColor value in the new point?
		if( (GET_IMAGE_PIXEL( image, pNext ) != backgroundColor ) && GET_IMAGEMASK_PIXEL(maskImage, pNext ) )
		{
			return pNext;
		}
		else
		{
			// mark point as visited
			ASSIGN_VISITED( pNext, visitedPoints, image->width );
		}
	}

	// no possible movement was found
	movement = -1;
	pNext.x = P.x;
	pNext.y = P.y;

	return pNext;
}


