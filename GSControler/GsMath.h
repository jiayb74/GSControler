
#define PI				3.14159265358979l
#define ALMOST_NULL		0.000000000000000001l
#define NULL            0
int fnGsMath(void);

bool fnGsCubicRoot(double* pdInCoefficient,		//ax^3+bx^2+cx+d=0,pdIntCoefficient[0] = d, pdIntCoefficient[3] = a;
						int* pnReRootCnt,				//count of real root outputed
						int* pnImRootCnt,				//count of imaginary root outputed
						double* pdOutReRoot,			//point to array of real roots
						double* pdOutImRootReal,		//point to real part of the imaginary root
						double* pdOutImRootImag);		//point to imaginary part
double GetDiscrimination(double* pdInCoefficient);
bool GetKeyRatio(double* pdInCoefficient,double dDiscrimination,double *dDAbsSqrt,double *dKeyRatio);

bool fnGsBubbleSort(double* pdInValue,	//data need sorting
						//double* pdOutValue,	//data after sorting, 0->n,getting bigger
						int	nValueCnt);		//number of data
void fnFloat2Bytes(float fData, char *byDat);//convert fDate to 4-bytes

float fnBytes2Float(char *byDat);//convert 4-bytes to float
