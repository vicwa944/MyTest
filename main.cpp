/**

This file contains an extremely poorly written and buggy program that
attempts to create, draw, filter and combine single channel
images. This program contains several faults, some of which are easy
to spot and some which are hard to find and to understand. They are
all plausible faults that can be found in any type of software.

To see the resulting image run
 display -size 200x200 -depth 32 -define quantum:format=floating-point gray:data.raw

190221 --- TESTING TESTING
*/

#include <math.h>
#include <string.h>

#include <fstream>
#include <iostream>

#define DATA_WIDTH  200
#define DATA_HEIGHT 200

#define FILT_WIDTH 20
#define FILT_HEIGHT 20


void fill_gaussian_filter_data(float *filt, int filt_width, int filt_height);

void apply_filter(float *dst, float *src, int width, int height,
                  float *filt, int filt_width, int filt_height);

void copy_max_value(float *dst, float *src1, float *src2, int width, int height);

void fill_circle(float *img, int width, int height, int cx, int cy, int radius);


int main(int argc, char *argv[]){
  
  // Initialize
  
  std::ofstream fout("data.raw",std::ios::binary);
  if( fout.fail() ){ return -1; }
  
  int width = DATA_WIDTH;
  int height = DATA_HEIGHT;
  
  // Create image data1 and draw circles
  
  float* data1 = new float[width*height];
  
  fill_circle( data1, width, height, (1*width)/3, (5*height)/8, width/4 );
  fill_circle( data1, width, height, (4*width)/5, (6*height)/8, width/5 );
  fill_circle( data1, width, height, (2*width)/3, (1*height)/4, width/6 );
  
  // Create image data2 and draw circles
  
  float* data2 = new float[width*height];

  fill_circle( data2, width, height, width/2, height/2, width/5 );
  
  // Create a Gaussian filter
  
  int filt_width = FILT_WIDTH;
  int filt_height = FILT_HEIGHT;
  
  float* pfilt = new float[filt_width*filt_height];
  fill_gaussian_filter_data(pfilt,filt_width,filt_height);
  
  // Create result image data
  // TODO: why do I need sizeof(float) when float is used as type?
  float* data_res = new float[sizeof(float)*width*height];
  
  // Apply Gaussian filter to data1 -> data_res
  apply_filter(data_res, data1, width, height, pfilt, filt_width, filt_height);
  
  // Copy result (data_res) to data1
  memcpy(data1,data_res,width*height*sizeof(data_res));
  
  // Take larges pixel value from data1 and data2 and store in data_res
  copy_max_value(data_res, data1, data2, width, height);
  
  // Save result to file and clean up
  
  fout.write(reinterpret_cast<char*>(data_res),width*height*sizeof(data_res));
  if( fout.fail() ){ return -1; }
  fout.close();
  
  delete[] data1;
  delete[] data2;
  delete[] pfilt;
  delete[] data_res;
  
  return 0;
}

void fill_gaussian_filter_data(float *pfilt, int filt_width, int filt_height){

  int cx = filt_width/2;
  int cy = filt_height/2;
  double c = 0.5*cx;
  double sum = 0.0;

  for( int idx_y = 0 ; idx_y < filt_height ; idx_y++ ){
    for( int idx_x = 0 ; idx_x < filt_width ; idx_x++ ){
      float x = (idx_x-cx)/filt_width;
      float y = (idx_y-cy)/filt_height;
      float r2 = x*x + y*y;
      double val = exp( -r2/(2*c*c) );
      pfilt[idx_x + filt_width*idx_y] = val;
      sum += val;
    }
  }
  for( int idx = 0 ; idx < filt_width*filt_height ; idx++ ){
    pfilt[idx] /= sum; }
}


void apply_filter(float *dst, float *src, int width, int height,
                  float *pfilt, int filt_width, int filt_height){
  
  for( int idx_y = 0 ; idx_y < height ; idx_y++ ){
    for( int idx_x = 0 ; idx_x < width ; idx_x++ ){
      
      float sum = 0;
      for( int idx2_y = 0 ; idx2_y < filt_height ; idx2_y++ ){
        for( int idx2_x = 0 ; idx2_x < filt_width ; idx2_x++ ){
          sum += src[(idx_x+idx2_x) + (idx_y+idx2_y)*width]
            * pfilt[idx2_x + idx2_y * filt_width];
        }}
      dst[idx_x + width*idx_y] = sum;
    }}
}

void copy_max_value(float *dst, float *src1, float *src2, int width, int height){
  
  for( int idx_y = 0 ; idx_y < width ; idx_y++ ){
    for( int idx_x = 0 ; idx_x < width ; idx_x++ ){
      
      int idx_z = idx_x + idx_y * width;

      if( src1[idx_x] > src2[idx_x] ){
        dst[idx_x] = src1[idx_x];
      }
      else {
        dst[idx_x] = src2[idx_x];
      }
    }}
}

void fill_circle(float *img, int width, int height, int cx, int cy, int radius){

  for( int idx_y = 0 ; idx_y < width ; idx_y++ ){
    for( int idx_x = 0 ; idx_x < width ; idx_x++ ){
      if( (idx_x-cx)*(idx_x-cx) + (idx_y-cy)*(idx_y-cy) < radius*radius )
        img[idx_x + width*idx_y] = 1.0;
    }
  }
}

