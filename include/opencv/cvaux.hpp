/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef __CVAUX_HPP__
#define __CVAUX_HPP__

#ifdef __cplusplus

#include <iosfwd>

/****************************************************************************************\
*                                       CamShiftTracker                                  *
\****************************************************************************************/

class CV_EXPORTS CvCamShiftTracker
{
public:

    CvCamShiftTracker();
    virtual ~CvCamShiftTracker();

    /**** Characteristics of the object that are calculated by track_object method *****/
    float   get_orientation() const // orientation of the object in degrees
    { return m_box.angle; }
    float   get_length() const // the larger linear size of the object
    { return m_box.size.height; }
    float   get_width() const // the smaller linear size of the object
    { return m_box.size.width; }
    CvPoint2D32f get_center() const // center of the object
    { return m_box.center; }
    CvRect get_window() const // bounding rectangle for the object
    { return m_comp.rect; }

    /*********************** Tracking parameters ************************/
    int     get_threshold() const // thresholding value that applied to back project
    { return m_threshold; }

    int     get_hist_dims( int* dims = 0 ) const // returns number of histogram dimensions and sets
    { return m_hist ? cvGetDims( m_hist->bins, dims ) : 0; }

    int     get_min_ch_val( int channel ) const // get the minimum allowed value of the specified channel
    { return m_min_ch_val[channel]; }

    int     get_max_ch_val( int channel ) const // get the maximum allowed value of the specified channel
    { return m_max_ch_val[channel]; }

    // set initial object rectangle (must be called before initial calculation of the histogram)
    bool    set_window( CvRect window)
    { m_comp.rect = window; return true; }

    bool    set_threshold( int threshold ) // threshold applied to the histogram bins
    { m_threshold = threshold; return true; }

    bool    set_hist_bin_range( int dim, int min_val, int max_val );

    bool    set_hist_dims( int c_dims, int* dims );// set the histogram parameters

    bool    set_min_ch_val( int channel, int val ) // set the minimum allowed value of the specified channel
    { m_min_ch_val[channel] = val; return true; }
    bool    set_max_ch_val( int channel, int val ) // set the maximum allowed value of the specified channel
    { m_max_ch_val[channel] = val; return true; }

    /************************ The processing methods *********************************/
    // update object position
    virtual bool  track_object( const IplImage* cur_frame );

    // update object histogram
    virtual bool  update_histogram( const IplImage* cur_frame );

    // reset histogram
    virtual void  reset_histogram();

    /************************ Retrieving internal data *******************************/
    // get back project image
    virtual IplImage* get_back_project()
    { return m_back_project; }

    float query( int* bin ) const
    { return m_hist ? (float)cvGetRealND(m_hist->bins, bin) : 0.f; }

protected:

    // internal method for color conversion: fills m_color_planes group
    virtual void color_transform( const IplImage* img );

    CvHistogram* m_hist;

    CvBox2D    m_box;
    CvConnectedComp m_comp;

    float      m_hist_ranges_data[CV_MAX_DIM][2];
    float*     m_hist_ranges[CV_MAX_DIM];

    int        m_min_ch_val[CV_MAX_DIM];
    int        m_max_ch_val[CV_MAX_DIM];
    int        m_threshold;

    IplImage*  m_color_planes[CV_MAX_DIM];
    IplImage*  m_back_project;
    IplImage*  m_temp;
    IplImage*  m_mask;
};

/****************************************************************************************\
*                                   Adaptive Skin Detector                               *
\****************************************************************************************/

class CV_EXPORTS CvAdaptiveSkinDetector
{
private:
	enum {
		GSD_HUE_LT = 3,
		GSD_HUE_UT = 33,
		GSD_INTENSITY_LT = 15,
		GSD_INTENSITY_UT = 250
	};

	class CV_EXPORTS Histogram
	{
	private:
		enum {
			HistogramSize = (GSD_HUE_UT - GSD_HUE_LT + 1)
		};

	protected:
		int findCoverageIndex(double surfaceToCover, int defaultValue = 0);

	public:
		CvHistogram *fHistogram;
		Histogram();
		virtual ~Histogram();

		void findCurveThresholds(int &x1, int &x2, double percent = 0.05);
		void mergeWith(Histogram *source, double weight);
	};

	int nStartCounter, nFrameCount, nSkinHueLowerBound, nSkinHueUpperBound, nMorphingMethod, nSamplingDivider;
	double fHistogramMergeFactor, fHuePercentCovered;
	Histogram histogramHueMotion, skinHueHistogram;
	IplImage *imgHueFrame, *imgSaturationFrame, *imgLastGrayFrame, *imgMotionFrame, *imgFilteredFrame;
	IplImage *imgShrinked, *imgTemp, *imgGrayFrame, *imgHSVFrame;

protected:
	void initData(IplImage *src, int widthDivider, int heightDivider);
	void adaptiveFilter();

public:

	enum {
		MORPHING_METHOD_NONE = 0,
		MORPHING_METHOD_ERODE = 1,
		MORPHING_METHOD_ERODE_ERODE	= 2,
		MORPHING_METHOD_ERODE_DILATE = 3
	};

	CvAdaptiveSkinDetector(int samplingDivider = 1, int morphingMethod = MORPHING_METHOD_NONE);
	virtual ~CvAdaptiveSkinDetector();

	virtual void process(IplImage *inputBGRImage, IplImage *outputHueMask);
};


/****************************************************************************************\
*                                  Fuzzy MeanShift Tracker                               *
\****************************************************************************************/

class CV_EXPORTS CvFuzzyPoint {
public:
	double x, y, value;

	CvFuzzyPoint(double _x, double _y);
};

class CV_EXPORTS CvFuzzyCurve {
private:
    std::vector<CvFuzzyPoint> points;
	double value, centre;

	bool between(double x, double x1, double x2);

public:
	CvFuzzyCurve();
	~CvFuzzyCurve();

	void setCentre(double _centre);
	double getCentre();
	void clear();
	void addPoint(double x, double y);
	double calcValue(double param);
	double getValue();
	void setValue(double _value);
};

class CV_EXPORTS CvFuzzyFunction {
public:
    std::vector<CvFuzzyCurve> curves;

	CvFuzzyFunction();
	~CvFuzzyFunction();
	void addCurve(CvFuzzyCurve *curve, double value = 0);
	void resetValues();
	double calcValue();
	CvFuzzyCurve *newCurve();
};

class CV_EXPORTS CvFuzzyRule {
private:
	CvFuzzyCurve *fuzzyInput1, *fuzzyInput2;
	CvFuzzyCurve *fuzzyOutput;
public:
	CvFuzzyRule();
	~CvFuzzyRule();
	void setRule(CvFuzzyCurve *c1, CvFuzzyCurve *c2, CvFuzzyCurve *o1);
	double calcValue(double param1, double param2);
	CvFuzzyCurve *getOutputCurve();
};

class CV_EXPORTS CvFuzzyController {
private:
    std::vector<CvFuzzyRule*> rules;
public:
	CvFuzzyController();
	~CvFuzzyController();
	void addRule(CvFuzzyCurve *c1, CvFuzzyCurve *c2, CvFuzzyCurve *o1);
	double calcOutput(double param1, double param2);
};

class CV_EXPORTS CvFuzzyMeanShiftTracker
{
private:
	class FuzzyResizer
	{
	private:
		CvFuzzyFunction iInput, iOutput;
		CvFuzzyController fuzzyController;
	public:
		FuzzyResizer();
		int calcOutput(double edgeDensity, double density);
	};

	class SearchWindow
	{
	public:
		FuzzyResizer *fuzzyResizer;
		int x, y;
		int width, height, maxWidth, maxHeight, ellipseHeight, ellipseWidth;
		int ldx, ldy, ldw, ldh, numShifts, numIters;
		int xGc, yGc;
		long m00, m01, m10, m11, m02, m20;
		double ellipseAngle;
		double density;
		unsigned int depthLow, depthHigh;
		int verticalEdgeLeft, verticalEdgeRight, horizontalEdgeTop, horizontalEdgeBottom;

		SearchWindow();
		~SearchWindow();
		void setSize(int _x, int _y, int _width, int _height);
		void initDepthValues(IplImage *maskImage, IplImage *depthMap);
		bool shift();
		void extractInfo(IplImage *maskImage, IplImage *depthMap, bool initDepth);
		void getResizeAttribsEdgeDensityLinear(int &resizeDx, int &resizeDy, int &resizeDw, int &resizeDh);
		void getResizeAttribsInnerDensity(int &resizeDx, int &resizeDy, int &resizeDw, int &resizeDh);
		void getResizeAttribsEdgeDensityFuzzy(int &resizeDx, int &resizeDy, int &resizeDw, int &resizeDh);
		bool meanShift(IplImage *maskImage, IplImage *depthMap, int maxIteration, bool initDepth);
	};

public:
	enum TrackingState
	{
		tsNone 			= 0,
		tsSearching 	= 1,
		tsTracking 		= 2,
		tsSetWindow 	= 3,
		tsDisabled		= 10
	};

	enum ResizeMethod {
		rmEdgeDensityLinear		= 0,
		rmEdgeDensityFuzzy		= 1,
		rmInnerDensity			= 2
	};

	enum {
		MinKernelMass			= 1000
	};

	SearchWindow kernel;
	int searchMode;

private:
	enum
	{
		MaxMeanShiftIteration 	= 5,
		MaxSetSizeIteration 	= 5
	};

	void findOptimumSearchWindow(SearchWindow &searchWindow, IplImage *maskImage, IplImage *depthMap, int maxIteration, int resizeMethod, bool initDepth);

public:
	CvFuzzyMeanShiftTracker();
	~CvFuzzyMeanShiftTracker();

	void track(IplImage *maskImage, IplImage *depthMap, int resizeMethod, bool resetSearch, int minKernelMass = MinKernelMass);
};


namespace cv
{

class CV_EXPORTS Octree
{
public:    
    struct Node
    {
        Node() {}
        int begin, end;
        float x_min, x_max, y_min, y_max, z_min, z_max;		
        int maxLevels;
        bool isLeaf;
        int children[8];
    };

    Octree();
    Octree( const vector<Point3f>& points, int maxLevels = 10, int minPoints = 20 );
    virtual ~Octree();

    virtual void buildTree( const vector<Point3f>& points, int maxLevels = 10, int minPoints = 20 );
    virtual void getPointsWithinSphere( const Point3f& center, float radius,
                                        vector<Point3f>& points ) const;
    const vector<Node>& getNodes() const { return nodes; }
private:
    int minPoints;
    vector<Point3f> points;
    vector<Node> nodes;
	
	virtual void buildNext(size_t node_ind);
};


class CV_EXPORTS Mesh3D
{
public:
    struct EmptyMeshException {};

    Mesh3D();
    Mesh3D(const vector<Point3f>& vtx);
    ~Mesh3D();

    void buildOctree();
    void clearOctree();
    float estimateResolution(float tryRatio = 0.1f);        
    void computeNormals(float normalRadius, int minNeighbors = 20);
    void computeNormals(const vector<int>& subset, float normalRadius, int minNeighbors = 20);
    
    void writeAsVrml(const String& file, const vector<Scalar>& colors = vector<Scalar>()) const;
    
    vector<Point3f> vtx;
    vector<Point3f> normals;
    float resolution;    
    Octree octree;

    const static Point3f allzero;
};

class CV_EXPORTS SpinImageModel
{
public:
    
    /* model parameters, leave unset for default or auto estimate */
    float normalRadius;
    int minNeighbors;

    float binSize;
    int imageWidth;

    float lambda;                        
    float gamma;

    float T_GeometriccConsistency;
    float T_GroupingCorespondances;

    /* public interface */
    SpinImageModel();
    explicit SpinImageModel(const Mesh3D& mesh);
    ~SpinImageModel();

    void setLogger(std::ostream* log);
    void selectRandomSubset(float ratio);         
    void setSubset(const vector<int>& subset);         
    void compute();

    void match(const SpinImageModel& scene, vector< vector<Vec2i> >& result);    

    Mat packRandomScaledSpins(bool separateScale = false, size_t xCount = 10, size_t yCount = 10) const;
    
    size_t getSpinCount() const { return spinImages.rows; }
    Mat getSpinImage(size_t index) const { return spinImages.row(index); }
    const Point3f& getSpinVertex(size_t index) const { return mesh.vtx[subset[index]]; }
    const Point3f& getSpinNormal(size_t index) const { return mesh.normals[subset[index]]; }

    const Mesh3D& getMesh() const { return mesh; }
    Mesh3D& getMesh() { return mesh; }

    /* static utility functions */
    static bool spinCorrelation(const Mat& spin1, const Mat& spin2, float lambda, float& result);

    static Point2f calcSpinMapCoo(const Point3f& point, const Point3f& vertex, const Point3f& normal);

    static float geometricConsistency(const Point3f& pointScene1, const Point3f& normalScene1,
                                      const Point3f& pointModel1, const Point3f& normalModel1,   
                                      const Point3f& pointScene2, const Point3f& normalScene2,                               
                                      const Point3f& pointModel2, const Point3f& normalModel2);

    static float groupingCreteria(const Point3f& pointScene1, const Point3f& normalScene1,
                                  const Point3f& pointModel1, const Point3f& normalModel1,
                                  const Point3f& pointScene2, const Point3f& normalScene2,                               
                                  const Point3f& pointModel2, const Point3f& normalModel2, 
                                  float gamma);
protected:       
    void defaultParams();

    void matchSpinToModel(const Mat& spin, vector<int>& indeces, 
        vector<float>& corrCoeffs, bool useExtremeOutliers = true) const; 

    void repackSpinImages(const vector<uchar>& mask, Mat& spinImages, bool reAlloc = true) const;
             
    vector<int> subset;
    Mesh3D mesh;
    Mat spinImages;
    std::ostream* out;
};

class CV_EXPORTS TickMeter
{
public:
    TickMeter();
    void start();    
    void stop();

    int64 getTimeTicks() const;
    double getTimeMicro() const;
    double getTimeMilli() const;
    double getTimeSec()   const;
    int64 getCounter() const;

    void reset();
private:
    int64 counter;
    int64 sumTime;
    int64 startTime;
};

CV_EXPORTS std::ostream& operator<<(std::ostream& out, const TickMeter& tm);

/****************************************************************************************\
*            HOG (Histogram-of-Oriented-Gradients) Descriptor and Object Detector        *
\****************************************************************************************/

struct CV_EXPORTS HOGDescriptor
{
public:
    enum { L2Hys=0 };

    HOGDescriptor() : winSize(64,128), blockSize(16,16), blockStride(8,8),
        cellSize(8,8), nbins(9), derivAperture(1), winSigma(-1),
        histogramNormType(L2Hys), L2HysThreshold(0.2), gammaCorrection(true)
    {}

    HOGDescriptor(Size _winSize, Size _blockSize, Size _blockStride,
        Size _cellSize, int _nbins, int _derivAperture=1, double _winSigma=-1,
        int _histogramNormType=L2Hys, double _L2HysThreshold=0.2, bool _gammaCorrection=false)
        : winSize(_winSize), blockSize(_blockSize), blockStride(_blockStride), cellSize(_cellSize),
        nbins(_nbins), derivAperture(_derivAperture), winSigma(_winSigma),
        histogramNormType(_histogramNormType), L2HysThreshold(_L2HysThreshold),
        gammaCorrection(_gammaCorrection)
    {}

    HOGDescriptor(const String& filename)
    {
        load(filename);
    }

    virtual ~HOGDescriptor() {}

    size_t getDescriptorSize() const;
    bool checkDetectorSize() const;
    double getWinSigma() const;

    virtual void setSVMDetector(const vector<float>& _svmdetector);

    virtual bool load(const String& filename, const String& objname=String());
    virtual void save(const String& filename, const String& objname=String()) const;

    virtual void compute(const Mat& img,
                         vector<float>& descriptors,
                         Size winStride=Size(), Size padding=Size(),
                         const vector<Point>& locations=vector<Point>()) const;
    virtual void detect(const Mat& img, vector<Point>& foundLocations,
                        double hitThreshold=0, Size winStride=Size(),
                        Size padding=Size(),
                        const vector<Point>& searchLocations=vector<Point>()) const;
    virtual void detectMultiScale(const Mat& img, vector<Rect>& foundLocations,
                                  double hitThreshold=0, Size winStride=Size(),
                                  Size padding=Size(), double scale=1.05,
                                  int groupThreshold=2) const;
    virtual void computeGradient(const Mat& img, Mat& grad, Mat& angleOfs,
                                 Size paddingTL=Size(), Size paddingBR=Size()) const;

    static vector<float> getDefaultPeopleDetector();

    Size winSize;
    Size blockSize;
    Size blockStride;
    Size cellSize;
    int nbins;
    int derivAperture;
    double winSigma;
    int histogramNormType;
    double L2HysThreshold;
    bool gammaCorrection;
    vector<float> svmDetector;
};


class CV_EXPORTS SelfSimDescriptor
{
public:
    SelfSimDescriptor();
    SelfSimDescriptor(int _ssize, int _lsize,
        int _startDistanceBucket=DEFAULT_START_DISTANCE_BUCKET,
        int _numberOfDistanceBuckets=DEFAULT_NUM_DISTANCE_BUCKETS,
        int _nangles=DEFAULT_NUM_ANGLES);
	SelfSimDescriptor(const SelfSimDescriptor& ss);
	virtual ~SelfSimDescriptor();
    SelfSimDescriptor& operator = (const SelfSimDescriptor& ss);

    size_t getDescriptorSize() const;
    Size getGridSize( Size imgsize, Size winStride ) const;

    virtual void compute(const Mat& img, vector<float>& descriptors, Size winStride=Size(),
                         const vector<Point>& locations=vector<Point>()) const;
    virtual void computeLogPolarMapping(Mat& mappingMask) const;
    virtual void SSD(const Mat& img, Point pt, Mat& ssd) const;

	int smallSize;
	int largeSize;
    int startDistanceBucket;
    int numberOfDistanceBuckets;
    int numberOfAngles;

    enum { DEFAULT_SMALL_SIZE = 5, DEFAULT_LARGE_SIZE = 41,
        DEFAULT_NUM_ANGLES = 20, DEFAULT_START_DISTANCE_BUCKET = 3,
        DEFAULT_NUM_DISTANCE_BUCKETS = 7 };
};

    
class CV_EXPORTS PatchGenerator
{
public:
    PatchGenerator();
    PatchGenerator(double _backgroundMin, double _backgroundMax,
                   double _noiseRange, bool _randomBlur=true,
                   double _lambdaMin=0.6, double _lambdaMax=1.5,
                   double _thetaMin=-CV_PI, double _thetaMax=CV_PI,
                   double _phiMin=-CV_PI, double _phiMax=CV_PI );
    void operator()(const Mat& image, Point2f pt, Mat& patch, Size patchSize, RNG& rng) const;
    void operator()(const Mat& image, const Mat& transform, Mat& patch,
                    Size patchSize, RNG& rng) const;
    void warpWholeImage(const Mat& image, Mat& _T, Mat& buf,
                        Mat& warped, int border, RNG& rng) const;
    void generateRandomTransform(Point2f srcCenter, Point2f dstCenter,
                                 Mat& transform, RNG& rng, bool inverse=false) const;
    double backgroundMin, backgroundMax;
    double noiseRange;
    bool randomBlur;
    double lambdaMin, lambdaMax;
    double thetaMin, thetaMax;
    double phiMin, phiMax;
};

    
class CV_EXPORTS LDetector
{
public:    
    LDetector();
    LDetector(int _radius, int _threshold, int _nOctaves,
              int _nViews, double _baseFeatureSize, double _clusteringDistance);
    void operator()(const Mat& image, vector<KeyPoint>& keypoints, int maxCount=0, bool scaleCoords=true) const;
    void operator()(const vector<Mat>& pyr, vector<KeyPoint>& keypoints, int maxCount=0, bool scaleCoords=true) const;
    void getMostStable2D(const Mat& image, vector<KeyPoint>& keypoints,
                         int maxCount, const PatchGenerator& patchGenerator) const;
    void setVerbose(bool verbose);
    
    void read(const FileNode& node);
    void write(FileStorage& fs, const String& name=String()) const;
    
    int radius;
    int threshold;
    int nOctaves;
    int nViews;
    bool verbose;
    
    double baseFeatureSize;
    double clusteringDistance;
};

typedef LDetector YAPE;

class CV_EXPORTS FernClassifier
{
public:
    FernClassifier();
    FernClassifier(const FileNode& node);
    FernClassifier(const vector<Point2f>& points,
                   const vector<Ptr<Mat> >& refimgs,
                   const vector<int>& labels=vector<int>(),
                   int _nclasses=0, int _patchSize=PATCH_SIZE,
                   int _signatureSize=DEFAULT_SIGNATURE_SIZE,
                   int _nstructs=DEFAULT_STRUCTS,
                   int _structSize=DEFAULT_STRUCT_SIZE,
                   int _nviews=DEFAULT_VIEWS,
                   int _compressionMethod=COMPRESSION_NONE,
                   const PatchGenerator& patchGenerator=PatchGenerator());
    virtual ~FernClassifier();
    virtual void read(const FileNode& n);
    virtual void write(FileStorage& fs, const String& name=String()) const;
    virtual void trainFromSingleView(const Mat& image,
                                     const vector<KeyPoint>& keypoints,
                                     int _patchSize=PATCH_SIZE,
                                     int _signatureSize=DEFAULT_SIGNATURE_SIZE,
                                     int _nstructs=DEFAULT_STRUCTS,
                                     int _structSize=DEFAULT_STRUCT_SIZE,
                                     int _nviews=DEFAULT_VIEWS,
                                     int _compressionMethod=COMPRESSION_NONE,
                                     const PatchGenerator& patchGenerator=PatchGenerator());
    virtual void train(const vector<Point2f>& points,
                       const vector<Ptr<Mat> >& refimgs,
                       const vector<int>& labels=vector<int>(),
                       int _nclasses=0, int _patchSize=PATCH_SIZE,
                       int _signatureSize=DEFAULT_SIGNATURE_SIZE,
                       int _nstructs=DEFAULT_STRUCTS,
                       int _structSize=DEFAULT_STRUCT_SIZE,
                       int _nviews=DEFAULT_VIEWS,
                       int _compressionMethod=COMPRESSION_NONE,
                       const PatchGenerator& patchGenerator=PatchGenerator());
    virtual int operator()(const Mat& img, Point2f kpt, vector<float>& signature) const;
    virtual int operator()(const Mat& patch, vector<float>& signature) const;
    virtual void clear();
    void setVerbose(bool verbose);
    
    int getClassCount() const;
    int getStructCount() const;
    int getStructSize() const;
    int getSignatureSize() const;
    int getCompressionMethod() const;
    Size getPatchSize() const;    
    
    struct Feature
    {
        uchar x1, y1, x2, y2;
        Feature() : x1(0), y1(0), x2(0), y2(0) {}
        Feature(int _x1, int _y1, int _x2, int _y2)
        : x1((uchar)_x1), y1((uchar)_y1), x2((uchar)_x2), y2((uchar)_y2)
        {}
        template<typename _Tp> bool operator ()(const Mat_<_Tp>& patch) const
        { return patch(y1,x1) > patch(y2, x2); }
    };
    
    enum
    {
        PATCH_SIZE = 31,
        DEFAULT_STRUCTS = 50,
        DEFAULT_STRUCT_SIZE = 9,
        DEFAULT_VIEWS = 5000,
        DEFAULT_SIGNATURE_SIZE = 176,
        COMPRESSION_NONE = 0,
        COMPRESSION_RANDOM_PROJ = 1,
        COMPRESSION_PCA = 2,
        DEFAULT_COMPRESSION_METHOD = COMPRESSION_NONE
    };
    
protected:
    virtual void prepare(int _nclasses, int _patchSize, int _signatureSize,
                         int _nstructs, int _structSize,
                         int _nviews, int _compressionMethod);
    virtual void finalize(RNG& rng);
    virtual int getLeaf(int fidx, const Mat& patch) const;
    
    bool verbose;
    int nstructs;
    int structSize;
    int nclasses;
    int signatureSize;
    int compressionMethod;
    int leavesPerStruct;
    Size patchSize;
    vector<Feature> features;
    vector<int> classCounters;
    vector<float> posteriors;
};

class CV_EXPORTS PlanarObjectDetector
{
public:
    PlanarObjectDetector();
    PlanarObjectDetector(const FileNode& node);
    PlanarObjectDetector(const vector<Mat>& pyr, int _npoints=300,
                         int _patchSize=FernClassifier::PATCH_SIZE,
                         int _nstructs=FernClassifier::DEFAULT_STRUCTS,
                         int _structSize=FernClassifier::DEFAULT_STRUCT_SIZE,
                         int _nviews=FernClassifier::DEFAULT_VIEWS,
                         const LDetector& detector=LDetector(),
                         const PatchGenerator& patchGenerator=PatchGenerator()); 
    virtual ~PlanarObjectDetector();
    virtual void train(const vector<Mat>& pyr, int _npoints=300,
                       int _patchSize=FernClassifier::PATCH_SIZE,
                       int _nstructs=FernClassifier::DEFAULT_STRUCTS,
                       int _structSize=FernClassifier::DEFAULT_STRUCT_SIZE,
                       int _nviews=FernClassifier::DEFAULT_VIEWS,
                       const LDetector& detector=LDetector(),
                       const PatchGenerator& patchGenerator=PatchGenerator());
    virtual void train(const vector<Mat>& pyr, const vector<KeyPoint>& keypoints,
                       int _patchSize=FernClassifier::PATCH_SIZE,
                       int _nstructs=FernClassifier::DEFAULT_STRUCTS,
                       int _structSize=FernClassifier::DEFAULT_STRUCT_SIZE,
                       int _nviews=FernClassifier::DEFAULT_VIEWS,
                       const LDetector& detector=LDetector(),
                       const PatchGenerator& patchGenerator=PatchGenerator());
    Rect getModelROI() const;
    vector<KeyPoint> getModelPoints() const;
    const LDetector& getDetector() const;
    const FernClassifier& getClassifier() const;
    void setVerbose(bool verbose);
    
    void read(const FileNode& node);
    void write(FileStorage& fs, const String& name=String()) const;
    bool operator()(const Mat& image, Mat& H, vector<Point2f>& corners) const;
    bool operator()(const vector<Mat>& pyr, const vector<KeyPoint>& keypoints,
                    Mat& H, vector<Point2f>& corners, vector<int>* pairs=0) const;
    
protected:
    bool verbose;
    Rect modelROI;
    vector<KeyPoint> modelPoints;
    LDetector ldetector;
    FernClassifier fernClassifier;
};


////////////////////////////////////////////////////////////////////////////////////////////////////    
//                                        One-Way Descriptor                                      //
////////////////////////////////////////////////////////////////////////////////////////////////////    

class AffinePose;
    
// OneWayDescriptor: incapsulates a descriptor for a single point 
class CV_EXPORTS OneWayDescriptor
{
public:
    OneWayDescriptor();
    ~OneWayDescriptor();
    
    // allocates memory for given descriptor parameters
    void Allocate(int pose_count, Size size, int nChannels);
    
    // GenerateSamples: generates affine transformed patches with averaging them over small transformation variations.
    // If external poses and transforms were specified, uses them instead of generating random ones
    // - pose_count: the number of poses to be generated
    // - frontal: the input patch (can be a roi in a larger image)
    // - norm: if nonzero, normalizes the output patch so that the sum of pixel intensities is 1
    void GenerateSamples(int pose_count, IplImage* frontal, int norm = 0);
    
    // GenerateSamplesFast: generates affine transformed patches with averaging them over small transformation variations.
    // Uses precalculated transformed pca components.
    // - frontal: the input patch (can be a roi in a larger image)
    // - pca_hr_avg: pca average vector
    // - pca_hr_eigenvectors: pca eigenvectors
    // - pca_descriptors: an array of precomputed descriptors of pca components containing their affine transformations
    //   pca_descriptors[0] corresponds to the average, pca_descriptors[1]-pca_descriptors[pca_dim] correspond to eigenvectors
    void GenerateSamplesFast(IplImage* frontal, CvMat* pca_hr_avg, 
                             CvMat* pca_hr_eigenvectors, OneWayDescriptor* pca_descriptors);
    
    // sets the poses and corresponding transforms
    void SetTransforms(AffinePose* poses, CvMat** transforms);
    
    // Initialize: builds a descriptor. 
    // - pose_count: the number of poses to build. If poses were set externally, uses them rather than generating random ones
    // - frontal: input patch. Can be a roi in a larger image
    // - feature_name: the feature name to be associated with the descriptor
    // - norm: if 1, the affine transformed patches are normalized so that their sum is 1 
    void Initialize(int pose_count, IplImage* frontal, const char* feature_name = 0, int norm = 0);
    
    // InitializeFast: builds a descriptor using precomputed descriptors of pca components
    // - pose_count: the number of poses to build
    // - frontal: input patch. Can be a roi in a larger image
    // - feature_name: the feature name to be associated with the descriptor
    // - pca_hr_avg: average vector for PCA
    // - pca_hr_eigenvectors: PCA eigenvectors (one vector per row)
    // - pca_descriptors: precomputed descriptors of PCA components, the first descriptor for the average vector
    // followed by the descriptors for eigenvectors
    void InitializeFast(int pose_count, IplImage* frontal, const char* feature_name, 
                        CvMat* pca_hr_avg, CvMat* pca_hr_eigenvectors, OneWayDescriptor* pca_descriptors);
    
    // ProjectPCASample: unwarps an image patch into a vector and projects it into PCA space
    // - patch: input image patch
    // - avg: PCA average vector
    // - eigenvectors: PCA eigenvectors, one per row
    // - pca_coeffs: output PCA coefficients
    void ProjectPCASample(IplImage* patch, CvMat* avg, CvMat* eigenvectors, CvMat* pca_coeffs) const;
    
    // InitializePCACoeffs: projects all warped patches into PCA space
    // - avg: PCA average vector
    // - eigenvectors: PCA eigenvectors, one per row    
    void InitializePCACoeffs(CvMat* avg, CvMat* eigenvectors);
    
    // EstimatePose: finds the closest match between an input patch and a set of patches with different poses
    // - patch: input image patch
    // - pose_idx: the output index of the closest pose
    // - distance: the distance to the closest pose (L2 distance)
    void EstimatePose(IplImage* patch, int& pose_idx, float& distance) const;
    
    // EstimatePosePCA: finds the closest match between an input patch and a set of patches with different poses. 
    // The distance between patches is computed in PCA space
    // - patch: input image patch
    // - pose_idx: the output index of the closest pose
    // - distance: distance to the closest pose (L2 distance in PCA space)
    // - avg: PCA average vector. If 0, matching without PCA is used
    // - eigenvectors: PCA eigenvectors, one per row
    void EstimatePosePCA(IplImage* patch, int& pose_idx, float& distance, CvMat* avg, CvMat* eigenvalues) const;
    
    // GetPatchSize: returns the size of each image patch after warping (2 times smaller than the input patch) 
    Size GetPatchSize() const
    {
        return m_patch_size;
    }
    
    // GetInputPatchSize: returns the required size of the patch that the descriptor is built from 
    // (2 time larger than the patch after warping)
    Size GetInputPatchSize() const
    {
        return cvSize(m_patch_size.width*2, m_patch_size.height*2);
    }
    
    // GetPatch: returns a patch corresponding to specified pose index
    // - index: pose index
    // - return value: the patch corresponding to specified pose index
    IplImage* GetPatch(int index);
    
    // GetPose: returns a pose corresponding to specified pose index
    // - index: pose index
    // - return value: the pose corresponding to specified pose index
    AffinePose GetPose(int index) const;
    
    // Save: saves all patches with different poses to a specified path
    void Save(const char* path);
    
    // ReadByName: reads a descriptor from a file storage
    // - fs: file storage
    // - parent: parent node
    // - name: node name
    // - return value: 1 if succeeded, 0 otherwise
    int ReadByName(CvFileStorage* fs, CvFileNode* parent, const char* name);
    
    // Write: writes a descriptor into a file storage
    // - fs: file storage
    // - name: node name
    void Write(CvFileStorage* fs, const char* name);
    
    // GetFeatureName: returns a name corresponding to a feature
    const char* GetFeatureName() const;
    
    // GetCenter: returns the center of the feature
    Point GetCenter() const;
    
    void SetPCADimHigh(int pca_dim_high) {m_pca_dim_high = pca_dim_high;};
    void SetPCADimLow(int pca_dim_low) {m_pca_dim_low = pca_dim_low;};
    
protected:
    int m_pose_count; // the number of poses
    Size m_patch_size; // size of each image
    IplImage** m_samples; // an array of length m_pose_count containing the patch in different poses 
    CvMat** m_pca_coeffs; // an array of length m_pose_count containing pca decomposition of the patch in different poses
    AffinePose* m_affine_poses; // an array of poses
    CvMat** m_transforms; // an array of affine transforms corresponding to poses
    
    String m_feature_name; // the name of the feature associated with the descriptor
    Point m_center; // the coordinates of the feature (the center of the input image ROI)
    
    int m_pca_dim_high; // the number of descriptor pca components to use for generating affine poses
    int m_pca_dim_low; // the number of pca components to use for comparison
};

CV_EXPORTS void findOneWayDescriptor(int desc_count, const OneWayDescriptor* descriptors,
                                     IplImage* patch, int& desc_idx, int& pose_idx, float& distance, 
                                     CvMat* avg = 0, CvMat* eigenvalues = 0);

CV_EXPORTS void findOneWayDescriptor(int desc_count, const OneWayDescriptor* descriptors, IplImage* patch, 
                                     float scale_min, float scale_max, float scale_step,
                                     int& desc_idx, int& pose_idx, float& distance, float& scale, 
                                     CvMat* avg, CvMat* eigenvectors);
    
    
// OneWayDescriptorBase: encapsulates functionality for training/loading a set of one way descriptors
// and finding the nearest closest descriptor to an input feature
class CV_EXPORTS OneWayDescriptorBase
{
public:
    
    // creates an instance of OneWayDescriptor from a set of training files
    // - patch_size: size of the input (large) patch
    // - pose_count: the number of poses to generate for each descriptor
    // - train_path: path to training files
    // - pca_config: the name of the file that contains PCA for small patches (2 times smaller
    // than patch_size each dimension
    // - pca_hr_config: the name of the file that contains PCA for large patches (of patch_size size)
    // - pca_desc_config: the name of the file that contains descriptors of PCA components
    OneWayDescriptorBase(Size patch_size, int pose_count, const char* train_path = 0, const char* pca_config = 0, 
                         const char* pca_hr_config = 0, const char* pca_desc_config = 0, int pyr_levels = 2, 
                         int pca_dim_high = 100, int pca_dim_low = 100);
    
    ~OneWayDescriptorBase();
    
    // Allocate: allocates memory for a given number of descriptors
    void Allocate(int train_feature_count);
    
    // AllocatePCADescriptors: allocates memory for pca descriptors
    void AllocatePCADescriptors();
    
    // returns patch size
    Size GetPatchSize() const {return m_patch_size;};
    // returns the number of poses for each descriptor
    int GetPoseCount() const {return m_pose_count;};
    
    // returns the number of pyramid levels
    int GetPyrLevels() const {return m_pyr_levels;};
    
    // CreateDescriptorsFromImage: creates descriptors for each of the input features
    // - src: input image
    // - features: input features
    // - pyr_levels: the number of pyramid levels
    void CreateDescriptorsFromImage(IplImage* src, const vector<KeyPoint>& features);
    
    // CreatePCADescriptors: generates descriptors for PCA components, needed for fast generation of feature descriptors
    void CreatePCADescriptors();
    
    // returns a feature descriptor by feature index
    const OneWayDescriptor* GetDescriptor(int desc_idx) const;
    
    // FindDescriptor: finds the closest descriptor
    // - patch: input image patch
    // - desc_idx: output index of the closest descriptor to the input patch
    // - pose_idx: output index of the closest pose of the closest descriptor to the input patch
    // - distance: distance from the input patch to the closest feature pose
    void FindDescriptor(IplImage* patch, int& desc_idx, int& pose_idx, float& distance) const;
    
    // FindDescriptor: finds the closest descriptor
    // - src: input image 
    // - pt: center of the feature
    // - desc_idx: output index of the closest descriptor to the input patch
    // - pose_idx: output index of the closest pose of the closest descriptor to the input patch
    // - distance: distance from the input patch to the closest feature pose
    void FindDescriptor(IplImage* src, Point2f pt, int& desc_idx, int& pose_idx, float& distance) const;
    
    // InitializePoses: generates random poses
    void InitializePoses();
    
    // InitializeTransformsFromPoses: generates 2x3 affine matrices from poses (initializes m_transforms)
    void InitializeTransformsFromPoses();
    
    // InitializePoseTransforms: subsequently calls InitializePoses and InitializeTransformsFromPoses
    void InitializePoseTransforms();
    
    // InitializeDescriptor: initializes a descriptor
    // - desc_idx: descriptor index
    // - train_image: image patch (ROI is supported)
    // - feature_label: feature textual label
    void InitializeDescriptor(int desc_idx, IplImage* train_image, const char* feature_label);
    
    // InitializeDescriptors: load features from an image and create descriptors for each of them 
    void InitializeDescriptors(IplImage* train_image, const vector<KeyPoint>& features, 
                               const char* feature_label = "", int desc_start_idx = 0);
    
    // LoadPCADescriptors: loads PCA descriptors from a file
    // - filename: input filename
    int LoadPCADescriptors(const char* filename);
    
    // SavePCADescriptors: saves PCA descriptors to a file
    // - filename: output filename
    void SavePCADescriptors(const char* filename);
    
    // SetPCAHigh: sets the high resolution pca matrices (copied to internal structures)
    void SetPCAHigh(CvMat* avg, CvMat* eigenvectors);
    
    // SetPCALow: sets the low resolution pca matrices (copied to internal structures)
    void SetPCALow(CvMat* avg, CvMat* eigenvectors);
    
    
protected:
    Size m_patch_size; // patch size
    int m_pose_count; // the number of poses for each descriptor
    int m_train_feature_count; // the number of the training features
    OneWayDescriptor* m_descriptors; // array of train feature descriptors
    CvMat* m_pca_avg; // PCA average vector for small patches
    CvMat* m_pca_eigenvectors; // PCA eigenvectors for small patches
    CvMat* m_pca_hr_avg; // PCA average vector for large patches
    CvMat* m_pca_hr_eigenvectors; // PCA eigenvectors for large patches
    OneWayDescriptor* m_pca_descriptors; // an array of PCA descriptors
    
    AffinePose* m_poses; // array of poses
    CvMat** m_transforms; // array of affine transformations corresponding to poses
    
    int m_pca_dim_high;
    int m_pca_dim_low;
    
    int m_pyr_levels;
};

class CV_EXPORTS OneWayDescriptorObject : public OneWayDescriptorBase
{
public:
    // creates an instance of OneWayDescriptorObject from a set of training files
    // - patch_size: size of the input (large) patch
    // - pose_count: the number of poses to generate for each descriptor
    // - train_path: path to training files
    // - pca_config: the name of the file that contains PCA for small patches (2 times smaller
    // than patch_size each dimension
    // - pca_hr_config: the name of the file that contains PCA for large patches (of patch_size size)
    // - pca_desc_config: the name of the file that contains descriptors of PCA components
    OneWayDescriptorObject(Size patch_size, int pose_count, const char* train_path, const char* pca_config, 
                           const char* pca_hr_config = 0, const char* pca_desc_config = 0, int pyr_levels = 2);
    
    ~OneWayDescriptorObject();
    
    // Allocate: allocates memory for a given number of features
    // - train_feature_count: the total number of features
    // - object_feature_count: the number of features extracted from the object 
    void Allocate(int train_feature_count, int object_feature_count);
    
    
    void SetLabeledFeatures(const vector<KeyPoint>& features) {m_train_features = features;};
    vector<KeyPoint>& GetLabeledFeatures() {return m_train_features;};
    const vector<KeyPoint>& GetLabeledFeatures() const {return m_train_features;};
    
    // IsDescriptorObject: returns 1 if descriptor with specified index is positive, otherwise 0
    int IsDescriptorObject(int desc_idx) const;
    
    // MatchPointToPart: returns the part number of a feature if it matches one of the object parts, otherwise -1
    int MatchPointToPart(Point pt) const;
    
    // GetDescriptorPart: returns the part number of the feature corresponding to a specified descriptor  
    // - desc_idx: descriptor index
    int GetDescriptorPart(int desc_idx) const;
    
    // GetTrainFeatures: returns a set of training features
    const vector<KeyPoint>& GetTrainFeatures() const {return m_train_features;};
    vector<KeyPoint> _GetTrainFeatures() const;
    
    void InitializeObjectDescriptors(IplImage* train_image, const vector<KeyPoint>& features, 
                                     const char* feature_label, int desc_start_idx = 0, float scale = 1.0f);
    
protected:
    int* m_part_id; // contains part id for each of object descriptors
    vector<KeyPoint> m_train_features; // train features
    int m_object_feature_count; // the number of the positive features
};


// detect corners using FAST algorithm
CV_EXPORTS void FAST( const Mat& image, vector<KeyPoint>& keypoints, int threshold, bool nonmax_supression=true );


class CV_EXPORTS LevMarqSparse
{
public:
    LevMarqSparse();
    LevMarqSparse(int npoints, // number of points
            int ncameras, // number of cameras
            int nPointParams, // number of params per one point  (3 in case of 3D points)
            int nCameraParams, // number of parameters per one camera
            int nErrParams, // number of parameters in measurement vector
                            // for 1 point at one camera (2 in case of 2D projections)
            Mat& visibility, // visibility matrix. rows correspond to points, columns correspond to cameras
                             // 1 - point is visible for the camera, 0 - invisible
            Mat& P0, // starting vector of parameters, first cameras then points
            Mat& X, // measurements, in order of visibility. non visible cases are skipped 
            TermCriteria criteria, // termination criteria
            
            // callback for estimation of Jacobian matrices
            void (CV_CDECL * fjac)(int i, int j, Mat& point_params,
                                   Mat& cam_params, Mat& A, Mat& B, void* data),
            // callback for estimation of backprojection errors
            void (CV_CDECL * func)(int i, int j, Mat& point_params,
                                   Mat& cam_params, Mat& estim, void* data),
            void* data // user-specific data passed to the callbacks
            );
    virtual ~LevMarqSparse();
    
    virtual void run( int npoints, // number of points
            int ncameras, // number of cameras
            int nPointParams, // number of params per one point  (3 in case of 3D points)
            int nCameraParams, // number of parameters per one camera
            int nErrParams, // number of parameters in measurement vector
                            // for 1 point at one camera (2 in case of 2D projections)
            Mat& visibility, // visibility matrix. rows correspond to points, columns correspond to cameras
                             // 1 - point is visible for the camera, 0 - invisible
            Mat& P0, // starting vector of parameters, first cameras then points
            Mat& X, // measurements, in order of visibility. non visible cases are skipped 
            TermCriteria criteria, // termination criteria
            
            // callback for estimation of Jacobian matrices
            void (CV_CDECL * fjac)(int i, int j, Mat& point_params,
                                   Mat& cam_params, Mat& A, Mat& B, void* data),
            // callback for estimation of backprojection errors
            void (CV_CDECL * func)(int i, int j, Mat& point_params,
                                   Mat& cam_params, Mat& estim, void* data),
            void* data // user-specific data passed to the callbacks
            );

    virtual void clear();
    
    // useful function to do simple bundle adjastment tasks
    static void bundleAdjust(vector<Point3d>& points, //positions of points in global coordinate system (input and output)
                             const vector<vector<Point2d> >& imagePoints, //projections of 3d points for every camera 
                             const vector<vector<int> >& visibility, //visibility of 3d points for every camera 
                             vector<Mat>& cameraMatrix, //intrinsic matrices of all cameras (input and output)
                             vector<Mat>& R, //rotation matrices of all cameras (input and output)
                             vector<Mat>& T, //translation vector of all cameras (input and output)
                             vector<Mat>& distCoeffs, //distortion coefficients of all cameras (input and output)
                             const TermCriteria& criteria=
                             TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, DBL_EPSILON));
    
protected:
    virtual void optimize(); //main function that runs minimization
    
    //iteratively asks for measurement for visible camera-point pairs
    void ask_for_proj();                                        
    //iteratively asks for Jacobians for every camera_point pair
    void ask_for_projac();    
        
    CvMat* err; //error X-hX
    double prevErrNorm, errNorm;
    double lambda;
    CvTermCriteria criteria;
    int iters;
    
    CvMat** U; //size of array is equal to number of cameras
    CvMat** V; //size of array is equal to number of points
    CvMat** inv_V_star; //inverse of V*

    CvMat* A;
    CvMat* B;
    CvMat* W; 

    CvMat* X; //measurement 
    CvMat* hX; //current measurement extimation given new parameter vector 
    
    CvMat* prevP; //current already accepted parameter. 
    CvMat* P; // parameters used to evaluate function with new params
              // this parameters may be rejected 
    
    CvMat* deltaP; //computed increase of parameters (result of normal system solution )

    CvMat** ea; // sum_i  AijT * e_ij , used as right part of normal equation
                // length of array is j = number of cameras  
    CvMat** eb; // sum_j  BijT * e_ij , used as right part of normal equation
                // length of array is i = number of points

    CvMat** Yj; //length of array is i = num_points

    CvMat* S; //big matrix of block Sjk  , each block has size num_cam_params x num_cam_params 

    CvMat* JtJ_diag; //diagonal of JtJ,  used to backup diagonal elements before augmentation

    CvMat* Vis_index; // matrix which element is index of measurement for point i and camera j
               
    int num_cams;
    int num_points;
    int num_err_param;
    int num_cam_param;
    int num_point_param;

    //target function and jacobian pointers, which needs to be initialized 
    void (*fjac)(int i, int j, Mat& point_params, Mat& cam_params, Mat& A, Mat& B, void* data);
    void (*func)(int i, int j, Mat& point_params, Mat& cam_params, Mat& estim, void* data );

    void* data;
};


}

#endif /* __cplusplus */

#endif /* __CVAUX_HPP__ */

/* End of file. */
