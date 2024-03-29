#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stddef.h>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#ifdef __APPLE__           
#include <OpenCL/opencl.h>           
#else          
#include <CL/cl.h>           
#endif

int devicenumber=0;
int platformnumber=0;

struct grid {
  double phi;
  double c1;
};
struct csle {
  double c1l;
  double c1s;
};
struct pfmpar {
  double E0;
  double surfTen;
  double ee;
  double w;
  double deltax;
  double deltay;
  double deltat;
  double dx;
  double dy;
  double dt;
  double eesqrt;
  double Er;
  double IntMob;
  double IntMobInv;
};
struct pfmval {
  double Tr;
  double sigma;
  double Vm;
  double D11l;
  double D11s;
  double phisolid;
  double philiquid;
  double Rg;
  double T0;
  double lrep;
  double c1l_Initial;
  double c1s_Initial;
  double c1l_1stguess;
  double c1s_1stguess;
  double a2;
  double rad;
  double epsc;
  double intwidth;
  double dxIntfcPoints;
  double dtParam;
  double epsm;
  double InterfaceMobility;
  double RefD;
  double angle;
  double TLiquidus;
  double Toffset;
  double PosOffset;
  double TG;
  double Vp;
  double NoiseFac;
  int   nproc;
  int   jNx;
  int   iNy;
  int   jDimX;
  int   iDimY;
  int   ntimesteps;
  int   savetime;
  int   myrank;
};

int t;

struct grid *gridNew;
struct grid *gridOld;
struct csle *cscl;
struct pfmval pfmdat;
struct pfmpar pfmvar;

double *temp;
int timestep;
int *tstep;

int device_run;

int tstart=1;

int nx;
int ny;
int NX;
int NY;

int nxny;
int NXNY;
int ntNoNoise;

int rank;
int totny;
int nxtotny;
int totsliceny;
int nxtotsliceny;
int istart;
int iend;

int globaldim0;
int globaldim1;

int np;

int i;
int j;
int k;
int i1;

//cl_platform_id platform_id = NULL;
//cl_device_id device_id = NULL;
cl_context context = NULL;
cl_command_queue cmdQ = NULL;
cl_program program;
cl_kernel kernel1;
cl_kernel kernel2;
cl_kernel kernel3;
cl_kernel kernel2a;
cl_kernel kernel2b;
cl_kernel kernel3a;
cl_kernel kernel3b;
cl_kernel kernel4;
cl_kernel kernel5[9];    
cl_kernel kernel6[9];  
cl_kernel kernel7[9];  
cl_kernel kernel8;  
cl_kernel kernel9;
cl_kernel kernel10[3];
cl_kernel kernel11[2];
cl_kernel kernel12;
cl_uint ret_num_devices;
cl_uint ret_num_platforms;
cl_int ret;
cl_ulong local_size;
cl_int cl_local_size;
size_t kernel_code_size;
size_t local_size_size;
cl_event kerndone;
cl_char device_name[1024] = {0}; 
cl_char vendor_name[1024] = {0};
cl_device_type device_type; 
cl_char string[10240] = {0};
cl_uint work_dim=2;

size_t globaldim[2];

cl_mem d_gridNew;
cl_mem d_gridOld;
cl_mem d_cscl;
cl_mem d_pfmdat;
cl_mem d_pfmvar;
cl_mem d_temp;
cl_mem d_tstep;

char *source_str;

void CL_build();
void CL_create_kernels();
void CL_device_selection();
void CL_initialize_variables();
void CL_memory_apis();
void CL_Initialize_domain();
void CL_kernel_init_temperature();
void CL_Update_Temperature();
void CL_Solve_phi_com();
void CL_DeviceToHost();
void CL_Global_Max_Min();

void initialize_domain(struct grid *, struct grid *, struct csle *, struct pfmval *, struct pfmpar *, double *);
void savetimestep(struct grid *, struct pfmval *, struct pfmpar *, double *, int);
void savetimestepcsle(struct csle *, struct pfmval *, int);
int RandRange(int , int );
void copy_New_To_Old(struct grid *, struct grid *, struct pfmval *);
void savetimestepcscl(struct csle *, struct pfmval *, struct pfmpar *, int);
void apply_BC(struct grid *, struct pfmval *);
void read_rank(struct grid *, struct csle *, double *temp, struct pfmval *, struct pfmpar *, int);
void apply_BC_temp_ib_noflux(double *, struct pfmval *);
void apply_BC_temp_it_noflux(double *, struct pfmval *);
