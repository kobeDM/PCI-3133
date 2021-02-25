//runPCI-3133_read based on AdInputAD Version 1.00-01 
// Kentaro Miuchi
// ver 1.1 for rotary SW selection
// ver 2.0 assignment change (to default)
// ver 2.1 dynamic range selection from command line parameters
// ver 2.2 delete "writing" message, set +-10V as default range

/* **************************************************************
   inputad.c -Source Code of AdInputAD sample program
   --------------------------------------------------------------
   Version 1.00-01
   --------------------------------------------------------------
   Date 2000/10/12
   --------------------------------------------------------------
   Copyright 2000,2002 Interface Corporation. All rights reserved.
   ************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
//#include <iomanip.h>
#include <fstream>
#include <iostream>
#include "fbiad.h"
#include "PCI-3133.h"
#define TIMESTEP 1
#define MAXDEPTH 4096
#define DEPTH_DEFAULT 22
#define DNUM_DEFAULT 1
#define DYNAMIC_RANGE_DEFAULT 20.0
#define DC_OFFSET_DEFAULT 0.0
using namespace std;

int main(int argc,char **argv)
{
  int  i, j, k, ptr, strtptr, ret, dnum, timestep, depth;
  unsigned short FullRange;
  ADSMPLCHREQ Conf[16];
  ADBOARDSPEC Spec;
  time_t    now_t;
  struct tm *t;
  char      message[24];
  char      nowdate[24];
  int       ii, jj, num, show_flag, start_day, current_day;
  char      nmon[24], nday[24];
  double    val_ave[16][100], ave[16];
  double    val_tmp;
  double dynamic_range,DC_offset;
  double range[2];
  char      fname[256];
  ofstream  fp;
  
  unsigned short AdData[16];
  double dataV[16];
  double datastore[16][MAXDEPTH];
  int    look_ch[] = { 1, 1, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0, 0 };
  if( argc > 1 ) dnum = atoi( argv[1] );
  else dnum=DNUM_DEFAULT;
  if( argc > 3 ) {
    range[0]=atof( argv[2] );
    range[1]=atof( argv[3] );
    dynamic_range = range[1]-range[0];
    DC_offset=(range[0]+range[1])/2.0;
  }
  else {
  dynamic_range = DYNAMIC_RANGE_DEFAULT; 
  DC_offset=DC_OFFSET_DEFAULT;
  range[0]=DC_offset-0.5*dynamic_range;
  range[1]=DC_offset+0.5*dynamic_range;
  //  ULRANGE=ULRANGE_DEFAULT;
  }
  /**  if( argc >= 4 ) strcpy( fname, argv[3] );
       else{**/
    now_t = time( NULL );
    t     = (struct tm *)localtime( &now_t );
    start_day = t->tm_mday;
    if( ( t->tm_mon  + 1 ) < 10 ) sprintf( nmon, "0%d", t->tm_mon  + 1 );
    else sprintf( nmon, "%d", t->tm_mon + 1 );
    if( t->tm_mday < 10 ) sprintf( nday, "0%d", t->tm_mday );
    else sprintf( nday, "%d", t->tm_mday );
    sprintf( nowdate, "%d%s%s_%d",
           t->tm_year + 1900,
           nmon,
	     nday,dnum
         );
    strcpy( fname, nowdate );
    //  }

    //  if( argc >= 3 ) timestep = atoi( argv[2] );
    //else
 timestep = TIMESTEP;
 //  if( argc >= 2 ) depth = atoi( argv[1] );
 // else 

depth=DEPTH_DEFAULT;
  FullRange = FULLRANGE;




  system("clear");
  /**  printf("Enter the device number: ");
       scanf("%d", &dnum);**/
  //  dnum=1;
  
  ret = AdOpen(dnum);
  if(ret){
    printf("Open error: ret=%Xh\n", ret);
    exit(EXIT_FAILURE);
  }
  fprintf(stderr,"reading PCI-3133(id=%d)\n",dnum);
  fprintf(stderr,"dynamic range:%.2lf to %.2lf\n",range[0],range[1]);
  ret = AdGetDeviceInfo(dnum, &Spec);
  if(ret){
    printf("DaGetDeviceInfo error: ret=%Xh\n", ret);
    AdClose(dnum);
    exit(EXIT_FAILURE);
  }
  /* printf(" board type :%d\n",Spec.ulBoardType); */

  for(i=0;i<16;i++){
    Conf[i].ulChNo = i+1;
    if(range[0]==-10&&range[1]==10) {
      //bipolar 10V
      Conf[i].ulRange = AD_10V;
      fprintf(stderr,"setting AD_10V");

    }
    //    if(range[0]==-10&&range[1]==10){
    // Conf[i].ulRange = AD_10V;
          //}
    else if(range[0]==-2.5&&range[1]==2.5){
      Conf[i].ulRange = AD_2P5V;
      // fprintf(stderr,"setting: AD_2P5V");
    }
    else if(range[0]==0){
      //unipolar 5V
      if(range[1]==5){
      Conf[i].ulRange = AD_0_5V;
      //fprintf(stderr,"setting: AD_0_5V");
    }
      //unipolar 10
      if(range[1]==10){
      Conf[i].ulRange = AD_0_10V;
      //fprintf(stderr,"setting: AD_0_10V");
      }

    }

    //default setting
    else{
    Conf[i].ulRange = AD_5V;
      fprintf(stderr,"setting: AD_5V");
    }
    AdData[i]=0;
    for(j=0;j<MAXDEPTH;j++){
      datastore[i][j]=0;
    }
  }

  ptr = 0;
  ii = 0;

  jj = 0;
  for( i = 0; i < 16; i ++ ){
    for( j = 0; j < 100; j ++ ){
      val_ave[i][j] = 0;
    }
  }
  fp.open( fname, ios_base::app );

  while( 1 ){
    show_flag = 0;
    now_t = time( NULL );
    t     = (struct tm *)localtime( &now_t );
    current_day = t->tm_mday;
    if( current_day != start_day ){
      start_day = current_day;
      fp.close();
      if( ( t->tm_mon  + 1 ) < 10 ) sprintf( nmon, "0%d", t->tm_mon  + 1 );
      else sprintf( nmon, "%d", t->tm_mon  + 1 );
      if( t->tm_mday < 10 ) sprintf( nday, "0%d", t->tm_mday );
      else sprintf( nday, "%d", t->tm_mday );
      sprintf( nowdate, "%d%s%s_%d",
	       t->tm_year + 1900,
	       nmon,
	       nday,dnum
	     );
      strcpy( fname, nowdate );
      fp.open( fname );
    }
    sprintf( message, "%d/%d/%d/%d:%d:%d",
           t->tm_year + 1900,
           t->tm_mon  + 1,
           t->tm_mday,
           t->tm_hour,
           t->tm_min,
           t->tm_sec
         );

    for( i = 0; i < 8; i ++ ){
      //    for( i = 0; i < 16; i ++ ){
      ret = AdInputAD(dnum,16,AD_INPUT_SINGLE,&Conf[i],AdData );
      //      dataV[i]=AdData[i]*10.0/FullRange-0.0;
      //     dataV[i]=AdData[i]*5.0/FullRange-2.5;
      dataV[i]=AdData[i]*dynamic_range/FullRange+range[0];
      ret = AdInputAD(dnum,16,AD_INPUT_SINGLE,&Conf[i+8],AdData );
      //      dataV[i+8]=AdData[i+8]*10.0/FullRange-0.0;
      //      dataV[i+8]=AdData[i+8]*5.0/FullRange-2.5;
      dataV[i+8]=AdData[i+8]*dynamic_range/FullRange+range[0];
      //version1.?
      //      datastore[i*2][ptr]=dataV[i];
      //datastore[i*2+1][ptr]=dataV[i+8];
      //version 2.?
      datastore[i][ptr]=dataV[i];
      datastore[i+8][ptr]=dataV[i+8];
      //}
    }
    strtptr=ptr-depth+1;
      
    if( ptr < depth ){
    for( j = MAXDEPTH + strtptr; j < MAXDEPTH;j ++ ){
      for( i = 0; i < 16; i ++ ){
	/* printf( "%.2lf\t", datastore[i][j] ); */
      }
      /* printf("\n"); */

    }
    strtptr=0;
    }

    num = 0;
    for( i = 0; i < 16; i ++ ){
      ave[i] = 0;
    }
    for( j = strtptr; j < ptr; j ++ ){
      if( j == ( ptr-1 ) ){
	show_flag = 0;
        if( ( ii % 10 ) == 0 ){ 
	  //fprintf( stderr, "\r                  " );
	  fprintf( stderr, "%s\t", message );
        }
	for( i = 0; i < 16; i ++ ){

	  if( jj >= 100 ){ jj = 0; }
          val_ave[i][jj] = datastore[i][j];
	  ave[i] = 0;
	  for( k = 0; k < 100; k ++ ){
	    ave[i] += val_ave[i][k] / 100.; 
	  }
	  val_tmp = datastore[i][j];
          if( ( ii % 10 ) == 0 ){ 
	    fprintf( stderr, "%.3lf\t", val_tmp );
          }
	  if( fabs( val_tmp ) > ( fabs(ave[i]*1.3 ) ) || 
              fabs( val_tmp ) < ( fabs(ave[i]*0.7 ) )){
	    if( fabs( val_tmp ) > 0.1 ){
              if( look_ch[i] ){
	        show_flag = 1;
		//	        fprintf( stderr, "Writing\n" );
              }
	    }
	  }else{
	  }
	  if( i  == 7 &&ii %10 == 0){ 
	    fprintf( stderr, "\n\t\t\t");
	    }
	}
	  if(ii % 10 ==0){ 
	    fprintf( stderr, "\n");
	    }
	jj ++;
	ii ++;
      }
    }
    if( ( ii % 600 ) == 0 ){ show_flag = 1;}
    if( ii > 65536 ){ ii = 0; }
    if( show_flag ){
      if( ii > 0 ){ 
	fp << message << " "; 
	//fprintf( fp, "%s\t", message ); 
      }
      for( j = strtptr; j < ptr; j ++ ){
	//	for( i = 0; i < 16; i ++ ){
	for( i = 0; i < 16; i ++ ){
	  if( j == ( ptr-1 ) ){
	    fp << datastore[i][j] << " ";
	    //    fprintf( fp, "%.4lf\t", datastore[i][j] );

	  }
	}
      }
      fp << std::endl << std::flush;
      // fprintf( fp, "\n");
    }

    usleep(100000);
    
    ptr++;
    if(ptr>MAXDEPTH)ptr=0;
  }
  fp.close();

  ret = AdClose(dnum);

  return(0);
}
