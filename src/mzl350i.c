#include "ili9481.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/timeb.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <errno.h>
#include <sys/ioctl.h>

void LCD_test()
{
    uint32_t num,i;
    char n;
    const uint16_t color[]={0xf800,0x07e0,0x001f,0xffe0,0x0000,0xffff,0x07ff,0xf81f};

    ili9481_setwindow(0,480-1,0,320-1);

    for (n=0;n<8;n++) {
        for (num=40*480;num>0;num--) {
            ili9481_writedata(color[n]);
        }
    }
    for (n=0;n<3;n++) {
        ili9481_setwindow(0,480-1,0,320-1);

        for (i=0;i<320;i++) {
            for (num=0;num<480;num++) {
                ili9481_writedata(color[n]);
            }
        }
    }
}

void loadFrameBuffer_diff_480320()
{
    const int xsize=480;
    const int ysize=320;

    uint16_t *framebuffer;
    int i,j;
    unsigned long offset=0;
    uint16_t p;
    uint16_t drawmap[ysize][xsize];
    int diffmap[ysize][xsize];
    int diffsx, diffsy, diffex, diffey;
    int numdiff=0;
    struct fb_fix_screeninfo fix_info;
    struct fb_var_screeninfo var_info;

    int fbdev = open("/dev/fb0", O_RDONLY);
    if (fbdev < 0) {
        printf("Unable to open /dev/fb0.\n");
        return;
    }

    // Retrieve fixed screen info.
    if (ioctl(fbdev, FBIOGET_FSCREENINFO, &fix_info) < 0) {
    	printf("Unable to retrieve fixed screen info: %s\n", strerror(errno));
    	close(fbdev);
    	return;
    }

    // Retrieve the variable screen info.
    if (ioctl(fbdev, FBIOGET_VSCREENINFO, &var_info) < 0) {
        printf("Unable to retrieve variable screen info: %s\n", strerror(errno));
        close(fbdev);
        return;
    }

    printf("Bits per pixel:     %i\n", var_info.bits_per_pixel);
    printf("Resolution:         %ix%i\n", var_info.xres, var_info.yres);

    if (var_info.xres != xsize || var_info.yres != ysize) {
        printf("WARNING: Expected framebuffer resolution %ix%i got %ix%i\n", xsize, ysize, var_info.xres, var_info.yres);
        printf("Edit /boot/config.txt and change framebuffer_width to 480 and framebuffer_height to 320\n");
    }

    framebuffer = mmap(NULL, fix_info.smem_len, PROT_READ,MAP_SHARED, fbdev, 0);
    if (framebuffer == MAP_FAILED) {
        printf("mmap failed.\n");
        close(fbdev);
        return;
    }

    ili9481_setwindow(0, xsize-1, 0, ysize-1);
    ili9481_clear(0);

    for (i=0; i < ysize; i++) {
        for (j=0; j< xsize; j++) {
            diffmap[i][j]=1;
            drawmap[i][j]=0;
        }
    }
    
    while (1) {
        numdiff=0;
        diffex=diffey=0;
        diffsx=diffsy=65535;
        
        for (i=0; i < ysize; i++) {
            for(j=0; j < xsize; j++) {
                offset =  i * var_info.xres + j;
                p=framebuffer[offset];
                
                if (drawmap[i][j] != p) {
                    drawmap[i][j] = p;
                    diffmap[i][j]=1;
                    numdiff++;
                    if ((i) < diffsx)
                        diffsx = i;
                    if ((i) > diffex)
                        diffex = i ;
                    if ((j)< diffsy)
                        diffsy=j;
                    if ((j)>diffey)
                        diffey = j ;
                    
                } else {
                    diffmap[i][j]=0;
                }
            }
            
        }

        if (numdiff< 2) {
            for (i=diffsx; i<=diffex; i++) {
                for (j=diffsy;j<=diffey; j++) {
                    if (diffmap[i][j]!=0)
                        ili9481_pset(i,j,drawmap[i][j]);
                }
            }
            usleep(700L);
            
        } else {
            ili9481_setwindow(diffsy, diffey, diffsx, diffex);
            
            for (i=diffsx; i<=diffex; i++) {
                for (j=diffsy;j<=diffey; j++) {
                    ili9481_writedata(drawmap[i][j]);
                }
            }
        }
    }
}

int main (void)
{
    printf ("Raspberry Pi MZL350I LCD...\n") ;
    
    if (ili9481_init())
        return 1;

    //LCD_test();
    loadFrameBuffer_diff_480320();

    return 0 ;
}
