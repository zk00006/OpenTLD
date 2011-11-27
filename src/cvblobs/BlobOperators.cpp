#include <limits.h>
#include "BlobOperators.h"

/***************************************************************************
  Implementació de les classes per al càlcul de característiques sobre el blob

  Implementation of the helper classes to perform operations on blobs
/**************************************************************************/

/**
- FUNCTION: Moment
- FUNCTIONALITY: Calculates the pq moment of the blob
- PARAMETERS:
- RESULT:
	- returns the pq moment or 0 if the moment it is not implemented
- RESTRICTIONS:
	- Currently, implemented moments up to 3
- AUTHOR: Ricard Borràs
- CREATION DATE: 20-07-2004.
- MODIFICATION: Date. Author. Description.
*/
double CBlobGetMoment::operator()(CBlob &blob)
{
	return blob.Moment(m_p, m_q);
}

/**
- FUNCIÓ: HullPerimeter
- FUNCIONALITAT: Calcula la longitud del perimetre convex del blob.
			   Fa servir la funció d'OpenCV cvConvexHull2 per a 
			   calcular el perimetre convex.
			   
- PARÀMETRES:
- RESULTAT:
	- retorna la longitud del perímetre convex del blob. Si el blob no té coordenades
	  associades retorna el perímetre normal del blob.
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 20-07-2004.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: CBlobGetHullPerimeter
- FUNCTIONALITY: Calculates the convex hull perimeter of the blob
- PARAMETERS:
- RESULT:
	- returns the convex hull perimeter of the blob or the perimeter if the 
	blob edges could not be retrieved
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double CBlobGetHullPerimeter::operator()(CBlob &blob)
{
	CvSeq *convexHull;
	double perimeter;

	convexHull = blob.GetConvexHull();

	if( convexHull )
		perimeter = fabs(cvArcLength(convexHull,CV_WHOLE_SEQ,1));
	else
		return 0;

	cvClearSeq(convexHull);

	return perimeter;
}

double CBlobGetHullArea::operator()(CBlob &blob)
{
	CvSeq *convexHull;
	double area;

	convexHull = blob.GetConvexHull();

	if( convexHull )
		area = fabs(cvContourArea(convexHull));
	else
		return 0;

	cvClearSeq(convexHull);

	return area;
}

/**
- FUNCTION: CBlobGetMinXatMinY
- FUNCTIONALITY: Calculates the minimum X on the minimum Y
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double CBlobGetMinXatMinY::operator()(CBlob &blob)
{
	double result = LONG_MAX;
	
	CvSeqReader reader;
	CvPoint actualPoint;
	t_PointList externContour;
	
	externContour = blob.GetExternalContour()->GetContourPoints();
	if( !externContour ) return result;
	cvStartReadSeq( externContour, &reader);

	for( int i=0; i< externContour->total; i++)
	{
		CV_READ_SEQ_ELEM( actualPoint, reader);

		if( (actualPoint.y == blob.MinY()) && (actualPoint.x < result) )
		{
			result = actualPoint.x;
		}	
	}

	return result;
}

/**
- FUNCTION: CBlobGetMinXatMinY
- FUNCTIONALITY: Calculates the minimum Y on the maximum X
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double CBlobGetMinYatMaxX::operator()(CBlob &blob)
{
	double result = LONG_MAX;
	
	CvSeqReader reader;
	CvPoint actualPoint;
	t_PointList externContour;
	
	externContour = blob.GetExternalContour()->GetContourPoints();
	if( !externContour ) return result;
	cvStartReadSeq( externContour, &reader);

	for( int i=0; i< externContour->total; i++)
	{
		CV_READ_SEQ_ELEM( actualPoint, reader);

		if( (actualPoint.x == blob.MaxX()) && (actualPoint.y < result) )
		{
			result = actualPoint.y;
		}	
	}

	return result;
}

/**
- FUNCTION: CBlobGetMaxXatMaxY
- FUNCTIONALITY: Calculates the maximum X on the maximum Y
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double CBlobGetMaxXatMaxY::operator()(CBlob &blob)
{
	double result = LONG_MIN;
	
	CvSeqReader reader;
	CvPoint actualPoint;
	t_PointList externContour;
	
	externContour = blob.GetExternalContour()->GetContourPoints();
	if( !externContour ) return result;

	cvStartReadSeq( externContour, &reader);

	for( int i=0; i< externContour->total; i++)
	{
		CV_READ_SEQ_ELEM( actualPoint, reader);

		if( (actualPoint.y == blob.MaxY()) && (actualPoint.x > result) )
		{
			result = actualPoint.x;
		}	
	}

	return result;
}

/**
- FUNCTION: CBlobGetMaxYatMinX
- FUNCTIONALITY: Calculates the maximum Y on the minimum X
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double CBlobGetMaxYatMinX::operator()(CBlob &blob)
{
	double result = LONG_MIN;
	
	CvSeqReader reader;
	CvPoint actualPoint;
	t_PointList externContour;
	
	externContour = blob.GetExternalContour()->GetContourPoints();
	if( !externContour ) return result;

	cvStartReadSeq( externContour, &reader);

	
	for( int i=0; i< externContour->total; i++)
	{
		CV_READ_SEQ_ELEM( actualPoint, reader);

		if( (actualPoint.x == blob.MinX()) && (actualPoint.y > result) )
		{
			result = actualPoint.y;
		}	
	}

	return result;
}


/**
- FUNCTION: CBlobGetElongation
- FUNCTIONALITY: Calculates the elongation of the blob ( length/breadth )
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
	- See below to see how the lenght and the breadth are aproximated
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double CBlobGetElongation::operator()(CBlob &blob)
{
	double ampladaC,longitudC,amplada,longitud;

	double tmp;

	tmp = blob.Perimeter()*blob.Perimeter() - 16*blob.Area();

	if( tmp > 0.0 )
		ampladaC = (double) (blob.Perimeter()+sqrt(tmp))/4;
	// error intrínsec en els càlculs de l'àrea i el perímetre 
	else
		ampladaC = (double) (blob.Perimeter())/4;

	if(ampladaC<=0.0) return 0;
	longitudC=(double) blob.Area()/ampladaC;

	longitud=MAX( longitudC , ampladaC );
	amplada=MIN( longitudC , ampladaC );

	return (double) longitud/amplada;
}

/**
	Retorna la compacitat del blob
*/
/**
- FUNCTION: CBlobGetCompactness
- FUNCTIONALITY: Calculates the compactness of the blob 
			    ( maximum for circle shaped blobs, minimum for the rest)
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double CBlobGetCompactness::operator()(CBlob &blob)
{
	if( blob.Area() != 0.0 )
		return (double) pow(blob.Perimeter(),2)/(4*CV_PI*blob.Area());
	else
		return 0.0;
}

/**
	Retorna la rugositat del blob
*/
/**
- FUNCTION: CBlobGetRoughness
- FUNCTIONALITY: Calculates the roughness of the blob 
			    ( ratio between perimeter and convex hull perimeter)
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double CBlobGetRoughness::operator()(CBlob &blob)
{
	CBlobGetHullPerimeter getHullPerimeter = CBlobGetHullPerimeter();
	
	double hullPerimeter = getHullPerimeter(blob);

	if( hullPerimeter != 0.0 )
		return blob.Perimeter() / hullPerimeter;//HullPerimeter();

	return 0.0;
}

/**
	Retorna la longitud del blob
*/
/**
- FUNCTION: CBlobGetLength
- FUNCTIONALITY: Calculates the lenght of the blob (the biggest axis of the blob)
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
	- The lenght is an aproximation to the real lenght
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double CBlobGetLength::operator()(CBlob &blob)
{
	double ampladaC,longitudC;
	double tmp;

	tmp = blob.Perimeter()*blob.Perimeter() - 16*blob.Area();

	if( tmp > 0.0 )
		ampladaC = (double) (blob.Perimeter()+sqrt(tmp))/4;
	// error intrínsec en els càlculs de l'àrea i el perímetre 
	else
		ampladaC = (double) (blob.Perimeter())/4;

	if(ampladaC<=0.0) return 0;
	longitudC=(double) blob.Area()/ampladaC;

	return MAX( longitudC , ampladaC );
}

/**
	Retorna l'amplada del blob
*/
/**
- FUNCTION: CBlobGetBreadth
- FUNCTIONALITY: Calculates the breadth of the blob (the smallest axis of the blob)
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
	- The breadth is an aproximation to the real breadth
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double CBlobGetBreadth::operator()(CBlob &blob)
{
	double ampladaC,longitudC;
	double tmp;

	tmp = blob.Perimeter()*blob.Perimeter() - 16*blob.Area();

	if( tmp > 0.0 )
		ampladaC = (double) (blob.Perimeter()+sqrt(tmp))/4;
	// error intrínsec en els càlculs de l'àrea i el perímetre 
	else
		ampladaC = (double) (blob.Perimeter())/4;

	if(ampladaC<=0.0) return 0;
	longitudC = (double) blob.Area()/ampladaC;

	return MIN( longitudC , ampladaC );
}

/**
	Calcula la distància entre un punt i el centre del blob
*/
/**
- FUNCTION: CBlobGetDistanceFromPoint
- FUNCTIONALITY: Calculates the euclidean distance between the blob center and 
				 the point specified in the constructor
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double CBlobGetDistanceFromPoint::operator()(CBlob &blob)
{
	double xmitjana, ymitjana;
	CBlobGetXCenter getXCenter;
	CBlobGetYCenter getYCenter;

	xmitjana = m_x - getXCenter( blob );
	ymitjana = m_y - getYCenter( blob );

	return sqrt((xmitjana*xmitjana)+(ymitjana*ymitjana));
}

/**
- FUNCTION: BlobGetXYInside
- FUNCTIONALITY: Calculates whether a point is inside the
    rectangular bounding box of a blob
- PARAMETERS:
- RESULT:
	- returns 1 if it is inside; o if not
- RESTRICTIONS:
- AUTHOR: Francesc Pinyol Margalef
- CREATION DATE: 16-01-2006.
- MODIFICATION: Date. Author. Description.
*/
double CBlobGetXYInside::operator()(CBlob &blob)
{
	if( blob.GetExternalContour()->GetContourPoints() )
	{
		return cvPointPolygonTest( blob.GetExternalContour()->GetContourPoints(), m_p,0) >= 0;
	}

	return 0;
}
#ifdef BLOB_OBJECT_FACTORY

/**
- FUNCIÓ: RegistraTotsOperadors
- FUNCIONALITAT: Registrar tots els operadors definits a blob.h
- PARÀMETRES:
	- fabricaOperadorsBlob: fàbrica on es registraran els operadors
- RESULTAT:
	- Modifica l'objecte fabricaOperadorsBlob
- RESTRICCIONS:
	- Només es registraran els operadors de blob.h. Si se'n volen afegir, cal afegir-los amb 
	  el mètode Register de la fàbrica.
- AUTOR: rborras
- DATA DE CREACIÓ: 2006/05/18
- MODIFICACIÓ: Data. Autor. Descripció.
*/
void RegistraTotsOperadors( t_OperadorBlobFactory &fabricaOperadorsBlob )
{
	// blob shape
	fabricaOperadorsBlob.Register( CBlobGetArea().GetNom(), Type2Type<CBlobGetArea>());
	fabricaOperadorsBlob.Register( CBlobGetBreadth().GetNom(), Type2Type<CBlobGetBreadth>());
	fabricaOperadorsBlob.Register( CBlobGetCompactness().GetNom(), Type2Type<CBlobGetCompactness>());
	fabricaOperadorsBlob.Register( CBlobGetElongation().GetNom(), Type2Type<CBlobGetElongation>());
	fabricaOperadorsBlob.Register( CBlobGetExterior().GetNom(), Type2Type<CBlobGetExterior>());
	fabricaOperadorsBlob.Register( CBlobGetLength().GetNom(), Type2Type<CBlobGetLength>());
	fabricaOperadorsBlob.Register( CBlobGetPerimeter().GetNom(), Type2Type<CBlobGetPerimeter>());
	fabricaOperadorsBlob.Register( CBlobGetRoughness().GetNom(), Type2Type<CBlobGetRoughness>());

	// blob color
	fabricaOperadorsBlob.Register( CBlobGetMean(NULL).GetNom(), Type2Type<CBlobGetMean>());
	fabricaOperadorsBlob.Register( CBlobGetStdDev(NULL).GetNom(), Type2Type<CBlobGetStdDev>());

	// extern pixels
	fabricaOperadorsBlob.Register( CBlobGetExternPerimeterRatio().GetNom(), Type2Type<CBlobGetExternPerimeterRatio>());
	fabricaOperadorsBlob.Register( CBlobGetExternHullPerimeterRatio().GetNom(), Type2Type<CBlobGetExternHullPerimeterRatio>());
	fabricaOperadorsBlob.Register( CBlobGetExternPerimeter().GetNom(), Type2Type<CBlobGetExternPerimeter>());
	

	// hull 
	fabricaOperadorsBlob.Register( CBlobGetHullPerimeter().GetNom(), Type2Type<CBlobGetHullPerimeter>());
	fabricaOperadorsBlob.Register( CBlobGetHullArea().GetNom(), Type2Type<CBlobGetHullArea>());
	

	// elipse info
	fabricaOperadorsBlob.Register( CBlobGetMajorAxisLength().GetNom(), Type2Type<CBlobGetMajorAxisLength>());
	fabricaOperadorsBlob.Register( CBlobGetMinorAxisLength().GetNom(), Type2Type<CBlobGetMinorAxisLength>());
	fabricaOperadorsBlob.Register( CBlobGetAxisRatio().GetNom(), Type2Type<CBlobGetAxisRatio>());
	fabricaOperadorsBlob.Register( CBlobGetOrientation().GetNom(), Type2Type<CBlobGetOrientation>());
	fabricaOperadorsBlob.Register( CBlobGetOrientationCos().GetNom(), Type2Type<CBlobGetOrientationCos>());
	fabricaOperadorsBlob.Register( CBlobGetAreaElipseRatio().GetNom(), Type2Type<CBlobGetAreaElipseRatio>());

	// min an max
	fabricaOperadorsBlob.Register( CBlobGetMaxX().GetNom(), Type2Type<CBlobGetMaxX>());
	fabricaOperadorsBlob.Register( CBlobGetMaxY().GetNom(), Type2Type<CBlobGetMaxY>());
	fabricaOperadorsBlob.Register( CBlobGetMinX().GetNom(), Type2Type<CBlobGetMinX>());
	fabricaOperadorsBlob.Register( CBlobGetMinY().GetNom(), Type2Type<CBlobGetMinY>());

	fabricaOperadorsBlob.Register( CBlobGetMaxXatMaxY().GetNom(), Type2Type<CBlobGetMaxXatMaxY>());
	fabricaOperadorsBlob.Register( CBlobGetMaxYatMinX().GetNom(), Type2Type<CBlobGetMaxYatMinX>());
	fabricaOperadorsBlob.Register( CBlobGetMinXatMinY().GetNom(), Type2Type<CBlobGetMinXatMinY>());
	fabricaOperadorsBlob.Register( CBlobGetMinYatMaxX().GetNom(), Type2Type<CBlobGetMinYatMaxX>());

	// coordinate info
	fabricaOperadorsBlob.Register( CBlobGetXYInside().GetNom(), Type2Type<CBlobGetXYInside>());
	fabricaOperadorsBlob.Register( CBlobGetDiffY().GetNom(), Type2Type<CBlobGetDiffY>());
	fabricaOperadorsBlob.Register( CBlobGetDiffX().GetNom(), Type2Type<CBlobGetDiffX>());
	fabricaOperadorsBlob.Register( CBlobGetXCenter().GetNom(), Type2Type<CBlobGetXCenter>());
	fabricaOperadorsBlob.Register( CBlobGetYCenter().GetNom(), Type2Type<CBlobGetYCenter>());
	fabricaOperadorsBlob.Register( CBlobGetDistanceFromPoint().GetNom(), Type2Type<CBlobGetDistanceFromPoint>());

	// moments
	fabricaOperadorsBlob.Register( CBlobGetMoment().GetNom(), Type2Type<CBlobGetMoment>());

}

#endif	//BLOB_OBJECT_FACTORY