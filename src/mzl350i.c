#include "bcm2835.h"
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

#define	SPICS	RPI_GPIO_P1_24	//GPIO08
#define	SPIRS	RPI_GPIO_P1_22	//GPIO25
#define	SPIRST	RPI_GPIO_P1_10	//GPIO15
#define	SPISCL	RPI_GPIO_P1_23	//GPIO11
#define	SPISCI	RPI_GPIO_P1_19	//GPIO10
#define	LCDPWM	RPI_GPIO_P1_12	//GPIO18

#define LCD_CS_CLR	bcm2835_gpio_clr(SPICS)
#define LCD_RS_CLR	bcm2835_gpio_clr(SPIRS)
#define LCD_RST_CLR	bcm2835_gpio_clr(SPIRST)
#define LCD_SCL_CLR	bcm2835_gpio_clr(SPISCL)
#define LCD_SCI_CLR	bcm2835_gpio_clr(SPISCI)
#define LCD_PWM_CLR	bcm2835_gpio_clr(LCDPWM)

#define LCD_CS_SET	bcm2835_gpio_set(SPICS)
#define LCD_RS_SET	bcm2835_gpio_set(SPIRS)
#define LCD_RST_SET	bcm2835_gpio_set(SPIRST)
#define LCD_SCL_SET	bcm2835_gpio_set(SPISCL)
#define LCD_SCI_SET	bcm2835_gpio_set(SPISCI)
#define LCD_PWM_SET	bcm2835_gpio_set(LCDPWM)

void wr_cmd(uint8_t out_data)
{
    LCD_CS_SET;
    LCD_RS_CLR;

    bcm2835_spi_transfer(out_data);
}

void wr_dat(uint8_t out_data)
{
    LCD_CS_CLR;
    LCD_RS_SET;

    bcm2835_spi_transfer(out_data);
}

void LCD_WR_Data(uint16_t val)
{
    bcm2835_spi_transfer(val>>8);
    bcm2835_spi_transfer(val);
}

void LCD_Init()
{
    LCD_RST_CLR;
    delay (100);
    LCD_RST_SET;
    delay (100);

    wr_cmd(0x11);      /* Exit Sleep Mode */
    delay(1);
    wr_cmd(0x13);      /* Entering Nomal Displaymode */
    delay(1);

    wr_cmd(0xC0);      /* Panel Driving Settings */
    wr_dat(0x00);
    wr_dat(0x3B);
    wr_dat(0x00);
    wr_dat(0x02);
    wr_dat(0x11);

    wr_cmd(0xC1);      /* Display_Timing_Setting for Normal Mode */
    wr_dat(0x10);
    wr_dat(0x10);//0B);
    wr_dat(0x68);//88);

    wr_cmd(0xC5);      /* Frame Rate and Inversion Control */
    wr_dat(0x01);

    wr_cmd(0xC8);      /* Gamma Settings */
    wr_dat(0x00);
    wr_dat(0x30); //30
    wr_dat(0x36);
    wr_dat(0x45);
    wr_dat(0x04);
    wr_dat(0x16);
    wr_dat(0x37);
    wr_dat(0x75);
    wr_dat(0x77);
    wr_dat(0x54);
    wr_dat(0x06);//f);
    wr_dat(0x12);//00);

    wr_cmd(0xD0);      /* Power setting */
    wr_dat(0x07);
    wr_dat(0x40);//40);
    wr_dat(0x1f);//1c);

    
    wr_cmd(0xD1);      /* VCOM Control */
    wr_dat(0x00);
    wr_dat(0x18);//18);
    wr_dat(0x06);//1d);

    wr_cmd(0xD2);      /* Power_Setting for Normal Mode */
    wr_dat(0x01);
    wr_dat(0x11);

    wr_cmd(0xE4);      /* ??? */
    wr_dat(0xA0);

    wr_cmd(0xF0);      /* ??? */
    wr_dat(0x01);

    wr_cmd(0xF3);      /* ??? */
    wr_dat(0x40);
    wr_dat(0x0A);

    wr_cmd(0xF7);      /* ??? */
    wr_dat(0x80);

    wr_cmd(0x36);      /* Set_address_mode */
    wr_dat(0x28);//02); /* BGR-order,Holizontal-Flip */

    wr_cmd(0x3A);      /* Set_pixel_format */
    wr_dat(0x05);       /* RGB565(16dpp) */

    wr_cmd(0xB3);
    wr_dat(0x00);
    wr_dat(0x00);
    wr_dat(0x00);
    wr_dat(0x11);

    wr_cmd(0x2B);
    wr_dat(0x00);
    wr_dat(0x00);
    wr_dat(0x01);
    wr_dat(0x3F);

    wr_cmd(0x2A);
    wr_dat(0x00);
    wr_dat(0x00);
    wr_dat(0x01);
    wr_dat(0xdf);

    delay(1);
    wr_cmd(0x29);      /* Display ON */
    wr_cmd(0x2C);
}

void ili9481_SetCursor(uint32_t x, uint32_t y)
{
    wr_cmd(0x2B);
    wr_dat(x>>8);
    wr_dat(0xFF&x);
    wr_dat(0x01);
    wr_dat(0x3F);

    wr_cmd(0x2A);
    wr_dat(y>>8);
    wr_dat(0xFF&y);
    wr_dat(0x01);
    wr_dat(0xdf);
}

void ili9481_Setwindow(uint32_t xs, uint32_t xe, uint32_t ys, uint32_t ye)
{
    wr_cmd(0x2a);
    wr_dat(xs>>8);
    wr_dat(xs);
    wr_dat(xe>>8);
    wr_dat(xe);

    wr_cmd(0x2b);
    wr_dat(ys>>8);
    wr_dat(ys);
    wr_dat(ye>>8);
    wr_dat(ye);
    wr_cmd(0x2C);
}

void TFTSetXY(uint32_t x, uint32_t y)
{
    ili9481_SetCursor(x,y);
    wr_cmd(0x2C);
}

void LCD_test()
{
    uint32_t num,i;
    char n;
    const uint16_t color[]={0xf800,0x07e0,0x001f,0xffe0,0x0000,0xffff,0x07ff,0xf81f};

    ili9481_Setwindow(0,480-1,0,320-1);

    LCD_CS_CLR;
    LCD_RS_SET;

    for (n=0;n<8;n++) {
        for (num=40*480;num>0;num--) {
            LCD_WR_Data(color[n]);
        }
    }
    for (n=0;n<3;n++) {
        ili9481_Setwindow(0,480-1,0,320-1);
        LCD_CS_CLR;
        LCD_RS_SET;

        for (i=0;i<320;i++) {
            for (num=0;num<480;num++) {
                LCD_WR_Data(color[n]);
            }
        }
    }
    LCD_CS_SET;
}

void LCD_clear(uint16_t p)
{
    uint32_t i,j;
    TFTSetXY(0,0);
    LCD_CS_CLR;
    LCD_RS_SET;

    for (i=0;i<320;i++) {
        for (j=0;j<480;j++) {
            LCD_WR_Data(p);
        }
    }
    LCD_CS_SET;
}

void write_dot(int dx,int dy, uint16_t color)
{
    LCD_CS_SET; LCD_RS_CLR;
    bcm2835_spi_transfer(0x2A);
    LCD_RS_SET; LCD_CS_CLR;
    bcm2835_spi_transfer(dx>>8);
    bcm2835_spi_transfer(dx);
    bcm2835_spi_transfer(0x01);
    bcm2835_spi_transfer(0xdF);

    LCD_CS_SET; LCD_RS_CLR;
    bcm2835_spi_transfer(0x2B);
    LCD_RS_SET; LCD_CS_CLR;
    bcm2835_spi_transfer(dy>>8);
    bcm2835_spi_transfer(dy);
    bcm2835_spi_transfer(0x01);
    bcm2835_spi_transfer(0x3F);

    LCD_CS_SET; LCD_RS_CLR;
    bcm2835_spi_transfer(0x2C);
    LCD_RS_SET; LCD_CS_CLR;
    bcm2835_spi_transfer(color>>8);
    bcm2835_spi_transfer(color);
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

    ili9481_Setwindow(0, xsize-1, 0, ysize-1);
    LCD_clear(0);

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
                        write_dot(i,j,drawmap[i][j]);
                }
            }
            usleep(700L);
            
        } else {
            ili9481_Setwindow(diffsy, diffey, diffsx, diffex);
            LCD_CS_CLR;
            LCD_RS_SET;
            
            for (i=diffsx; i<=diffex; i++) {
                for (j=diffsy;j<=diffey; j++) {
                    LCD_WR_Data(drawmap[i][j]);
                }
            }
        }
    }
}

int main (void)
{
    printf("bcm2835 init now\n");
    if (!bcm2835_init()) {
        printf("bcm2835 init error\n");
        return 1;
    }
    bcm2835_gpio_fsel(SPICS, BCM2835_GPIO_FSEL_OUTP) ;
    bcm2835_gpio_fsel(SPIRS, BCM2835_GPIO_FSEL_OUTP) ;
    bcm2835_gpio_fsel(SPIRST, BCM2835_GPIO_FSEL_OUTP) ;
    bcm2835_gpio_fsel(LCDPWM, BCM2835_GPIO_FSEL_OUTP) ;
    
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE3);
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_2);

    LCD_PWM_CLR;
    printf ("Raspberry Pi MZL350I LCD...\n") ;
    
    LCD_Init();
    //LCD_test();
    loadFrameBuffer_diff_480320();

    return 0 ;
}
