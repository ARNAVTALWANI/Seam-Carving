#include "seamcarving.h"
#include "c_img.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

// Part 1
void calc_energy(struct rgb_img *im, struct rgb_img **grad){
  create_img(grad, im->height, im->width);
  double energy_x, energy_y;
  for (int x = 0; x < im->width; x++) {
    for (int y = 0; y < im->height; y++) {
      int x_down = x;
      int x_up = x;
      int y_down = y;
      int y_up = y;
      
      if (x - 1 < 0) {
        x_down = im->width;
      } if (x + 1 >= im->width) {
        x_up = -1;
      } if (y - 1 < 0) {
        y_down = im->height;
      } if (y + 1 >= im->height) {
        y_up = -1;
      }

      int Rx1, Rx2, Ry1, Ry2, Gx1, Gx2, Gy1, Gy2, Bx1, Bx2, By1, By2;
      Rx1 = get_pixel(im, y, x_up + 1, 0);
      Rx2 = get_pixel(im, y, x_down - 1, 0);
      Ry1 = get_pixel(im, y_up + 1, x, 0);
      Ry2 = get_pixel(im, y_down - 1, x, 0);
      Gx1 = get_pixel(im, y, x_up + 1, 1);
      Gx2 = get_pixel(im, y, x_down - 1, 1);
      Gy1 = get_pixel(im, y_up + 1, x, 1);
      Gy2 = get_pixel(im, y_down - 1, x, 1);
      Bx1 = get_pixel(im, y, x_up + 1, 2);
      Bx2 = get_pixel(im, y, x_down - 1, 2);
      By1 = get_pixel(im, y_up + 1, x, 2);
      By2 = get_pixel(im, y_down - 1, x, 2);

      int Rx = Rx1 - Rx2;
      int Ry = Ry1 - Ry2;
      int Gx = Gx1 - Gx2;
      int Gy = Gy1 - Gy2;
      int Bx = Bx1 - Bx2;
      int By = By1 - By2;

      energy_x = ((Rx * Rx) + (Gx * Gx) + (Bx * Bx));
      energy_y = ((Ry * Ry) + (Gy * Gy) + (By * By));
      double energy_tot = sqrt(energy_x + energy_y) / 10;
      uint8_t energy = energy_tot;
      set_pixel(*grad, y, x, energy, energy, energy);
    }
  }
}
// Part 2
void dynamic_seam(struct rgb_img *grad, double **best_arr){
  (*best_arr) = (double *)malloc(sizeof(double) * grad->width * grad->height);
  
  for (int y = 0; y < grad->height; y++) {
    for (int x = 0; x < grad->width; x++) {
      if (y == 0){
        (*best_arr)[y*grad->width+x] = (double)get_pixel(grad, y, x, 0);
      }
      else if (x == 0) {
        (*best_arr)[y * grad->width + x] = fmin((*best_arr)[(y - 1) * grad->width + x],
                          (*best_arr)[(y - 1) * grad->width + (x + 1)]) + get_pixel(grad, y, x, 0);
      }

      else if (x == grad->width - 1) {
        (*best_arr)[y * grad->width + x] = fmin((*best_arr)[(y - 1) * grad->width + x],
                          (*best_arr)[(y - 1) * grad->width + (x - 1)]) + get_pixel(grad, y, x, 0);
      }

      else {
        double xx = fmin((*best_arr)[(y - 1) * grad->width + x],
                         (*best_arr)[(y - 1) * grad->width + x + 1]);
        (*best_arr)[y * grad->width + x] = fmin((*best_arr)[(y - 1) * grad->width + (x - 1)], xx)+get_pixel(grad, y, x, 0);
      }
    }
  }
}


  // Part 3
void recover_path(double *best, int height, int width, int **path)
{ 
  (*path) = (int *)malloc(sizeof(int) * height);
  for (int x = 0; x < width; x++) {
    int min_energy_pixel = 0;
    double minimum_current_pixel = best[x*width];     
    for (int y = 1; y < width; y++){
      if (best[(x*width)+y] < minimum_current_pixel){
        min_energy_pixel = y;
        minimum_current_pixel = best[y+(x*width)];
        
      }
    }
      (*path)[x] = min_energy_pixel;    
  }
}

  // Part 4
void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path)
{
  // Correct
  create_img(dest, src->height, src->width-1);

  int width = src->width;
  int height = src->height;

  *dest = (struct rgb_img *)malloc(sizeof(struct rgb_img));
  (*dest)->width = width - 1;
  (*dest)->height = height;
  (*dest)->raster = (unsigned char *)malloc(3 * height * ((*dest)->width) * sizeof(unsigned char));

  for (int y = 0; y < height; y++) {
    int seam_idx = path[y];
    for (int x = 0; x < width; x++) {
      if (x < seam_idx) {
        int src_idx = 3 * (y * width + x);
        int dest_idx = 3 * (y * ((*dest)->width) + x);
        (*dest)->raster[dest_idx] = src->raster[src_idx];
        (*dest)->raster[dest_idx + 1] = src->raster[src_idx + 1];
        (*dest)->raster[dest_idx + 2] = src->raster[src_idx + 2];
      } 
      else if (x > seam_idx) {
        int src_idx = 3 * (y * width + x);
        int dest_idx = 3 * (y * ((*dest)->width) + x - 1);
        (*dest)->raster[dest_idx] = src->raster[src_idx];
        (*dest)->raster[dest_idx + 1] = src->raster[src_idx + 1];
        (*dest)->raster[dest_idx + 2] = src->raster[src_idx + 2];
      }
    }
  }
}



