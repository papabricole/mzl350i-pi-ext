#include "bcm2835.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/timeb.h>
#include <stdint.h>

#define RGB565(r,g,b)((r >> 3) << 11 | (g >> 2) << 5 | ( b >> 3))
#define BCM2708SPI
#define ROTATE90

#define RGB565_MASK_RED        0xF800
#define RGB565_MASK_GREEN    0x07E0
#define RGB565_MASK_BLUE       0x001F

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

void wr_comm(uint8_t out_data)
{
    LCD_CS_CLR;
    LCD_RS_CLR;

    bcm2835_spi_transfer(out_data);

    LCD_CS_SET;
}

void wr_dat(uint8_t out_data)
{
    LCD_CS_CLR;
    LCD_RS_SET;

    bcm2835_spi_transfer(out_data);

    LCD_CS_SET;
}

void LCD_WR_Data(uint32_t val)
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

    wr_comm(0x0011);
    delay(1);
    wr_comm(0x0013);
    delay(1);

    wr_comm(0x00C0);
    wr_dat(0x0000);
    wr_dat(0x003B);
    wr_dat(0x0000);
    wr_dat(0x0002);
    wr_dat(0x0011);

    wr_comm(0x00C1);
    wr_dat(0x0010);
    wr_dat(0x0010);//0B);
    wr_dat(0x0068);//88);

    wr_comm(0x00C5);
    wr_dat(0x0001);

    wr_comm(0x00C8);
    wr_dat(0x0000);
    wr_dat(0x0030); //30
    wr_dat(0x0036);
    wr_dat(0x0045);
    wr_dat(0x0004);
    wr_dat(0x0016);
    wr_dat(0x0037);
    wr_dat(0x0075);
    wr_dat(0x0077);
    wr_dat(0x0054);
    wr_dat(0x0006);//f);
    wr_dat(0x0012);//00);

    wr_comm(0x00D0);
    wr_dat(0x0007);
    wr_dat(0x0040);//40); //41
    wr_dat(0x001f);//1c);//1e

    
    wr_comm(0x00D1);
    wr_dat(0x0000);
    wr_dat(0x0018);//18);
    wr_dat(0x0006);//1d);

    wr_comm(0x00D2);
    wr_dat(0x0001);
    wr_dat(0x0011);//11

    wr_comm(0x00E4);
    wr_dat(0x00A0);

    wr_comm(0x00F0);
    wr_dat(0x0001);

    wr_comm(0x00F3);
    wr_dat(0x0040);
    wr_dat(0x000A);

    wr_comm(0x00F7);
    wr_dat(0x0080);

    wr_comm(0x0036);
    wr_dat(0x0028);//02);

    wr_comm(0x003A);
    wr_dat(0x0005);

    wr_comm(0x00B3);
    wr_dat(0x0000);
    wr_dat(0x0000);
    wr_dat(0x0000);
    wr_dat(0x0011);

    wr_comm(0x002B);
    wr_dat(0x0000);
    wr_dat(0x0000);
    wr_dat(0x0001);
    wr_dat(0x003F);

    wr_comm(0x002A);
    wr_dat(0x0000);
    wr_dat(0x0000);
    wr_dat(0x0001);
    wr_dat(0x00df);

    delay(1);
    wr_comm(0x0029);
    wr_comm(0x002C);
}

void ili9481_SetCursor(uint32_t x, uint32_t y)
{
    wr_comm(0x002B);
    wr_dat(x>>8);
    wr_dat(0x00FF&x);
    wr_dat(0x0001);
    wr_dat(0x003F);

    wr_comm(0x002A);
    wr_dat(y>>8);
    wr_dat(0x00FF&y);
    wr_dat(0x0001);
    wr_dat(0x00df);
}

void ili9481_Setwindow(uint32_t xs, uint32_t xe, uint32_t ys, uint32_t ye)
{
    wr_comm(0x002a);
    wr_dat(xs>>8);
    wr_dat(xs);
    wr_dat(xe>>8);
    wr_dat(xe);

    wr_comm(0x002b);
    wr_dat(ys>>8);
    wr_dat(ys);
    wr_dat(ye>>8);
    wr_dat(ye);
    wr_comm(0x002C);
}

void TFTSetXY(uint32_t x, uint32_t y)
{
    ili9481_SetCursor(x,y);
    wr_comm(0x002C);
}

void LCD_test()
{
    uint32_t temp,num,i;
    char n;
    const short color[]={0xf800,0x07e0,0x001f,0xffe0,0x0000,0xffff,0x07ff,0xf81f};

    ili9481_Setwindow(0,480-1,0,320-1);

    LCD_CS_CLR;
    LCD_RS_SET;

    for (n=0;n<8;n++) {
        temp=color[n];
        for (num=40*480;num>0;num--) {
            LCD_WR_Data(temp);
        }
    }
    for (n=0;n<3;n++) {
        ili9481_Setwindow(0,480-1,0,320-1);
        LCD_CS_CLR;
        LCD_RS_SET;

        temp=color[n];
        for (i=0;i<320;i++) {
            for (num=0;num<480;num++) {
                LCD_WR_Data(temp);
            }
        }
    }
    LCD_CS_SET;
}

void LCD_clear(uint32_t p)
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

void write_dot(uint16_t dx, uint16_t dy, uint16_t color)
{
    wr_comm(0x002A);
    wr_dat(dy>>8);
    wr_dat(dy);
    //wr_dat(0x01);
    //wr_dat(0xdF);

    wr_comm(0x002B);
    wr_dat(dx>>8);
    wr_dat(dx);
    //wr_dat(0x01);
    //wr_dat(0x3F);
    wr_comm(0x002C);
    LCD_CS_CLR;
    LCD_RS_SET;
    LCD_WR_Data(color);
    LCD_CS_SET;
}

void loadFrameBuffer_diff_960640()
{
    int  xsize=960, ysize=640;
    unsigned char *buffer;
    FILE *infile=fopen("/dev/fb0","rb");
    int i,j;
    unsigned long offset=0;
    int p;
    int r1,g1,b1;
    int r,g,b;
    int flag;
    int drawmap[2][ysize/2][xsize/2];
    int diffmap[ysize/2][xsize/2];
    int diffsx, diffsy, diffex, diffey;
    int numdiff=0;
    
    buffer = (unsigned char *) malloc(xsize * ysize * 2);

    ili9481_Setwindow(0,480-1,0,320-1);
    LCD_CS_CLR;
    LCD_RS_SET;
    
    for (i=0; i < ysize/2; i++) {
        for (j=0; j< xsize/2; j++) {
            diffmap[i][j]=1;
            drawmap[0][i][j]=0;
            LCD_WR_Data(0);
            drawmap[1][i][j]=255;
        }
    }

    flag=1;
    
    while (1) {
        rewind(infile);
        if (fread (buffer, xsize * ysize *2, sizeof(unsigned char), infile) != 1) {
            printf ("Read < %d chars when loading file %s\n", xsize*ysize*2, "/dev/fb0");
            printf ("config.txt setting error\n") ;
            return;
        }

        numdiff=0;
        flag=1-flag;
        diffex=diffey=0;
        diffsx=diffsy=65535;

        for (i=0; i < ysize; i+=2) {
            for (j=0; j < xsize; j+=2) {
                offset =  (i * xsize+ j)*2;
                p=(buffer[offset+1] << 8) | buffer[offset];
                r = (p & RGB565_MASK_RED) >> 11;
                g = (p & RGB565_MASK_GREEN) >> 5;
                b = (p & RGB565_MASK_BLUE);
                
                r <<= 1;
                b <<= 1;
                
                offset = ( (i+1) * xsize +j )*2;
                p=(buffer[offset+1] << 8) | buffer[offset];
                r1 = (p & RGB565_MASK_RED) >> 11;
                g1 = (p & RGB565_MASK_GREEN) >> 5;
                b1 = (p & RGB565_MASK_BLUE);
                
                r += r1<<1;
                g += g1;
                b += b1 <<1;
                
                offset = ( i*xsize + j+1)*2;
                p=(buffer[offset+1] << 8) | buffer[offset];
                r1 = (p & RGB565_MASK_RED) >> 11;
                g1 = (p & RGB565_MASK_GREEN) >> 5;
                b1 = (p & RGB565_MASK_BLUE);
                
                r += r1<<1;
                g += g1;
                b += b1 <<1;
                
                offset=((i+1)*xsize + j+1)*2;
                p=(buffer[offset+1] << 8) | buffer[offset];
                r1 = (p & RGB565_MASK_RED) >> 11;
                g1 = (p & RGB565_MASK_GREEN) >> 5;
                b1 = (p & RGB565_MASK_BLUE);
                
                r += r1<<1;
                g += g1;
                b += b1 <<1;
                
                p=RGB565(r, g, b);
                
                if (drawmap[1-flag][i>>1][j>>1] != p) {
                    drawmap[flag][i>>1][j>>1] = p;
                    diffmap[i>>1][j>>1]=1;
                    drawmap[1-flag][i>>1][j>>1]=p;
                    numdiff++;
                    if ((i>>1) < diffsx)
                        diffsx = i>>1;
                    if ((i>>1) > diffex)
                        diffex = i >> 1;
                    if ((j>>1)< diffsy)
                        diffsy=j>>1;
                    if ((j>>1)>diffey)
                        diffey = j >>1;
                    
                } else {
                    diffmap[i>>1][j>>1]=0;
                }
            }
            
        }

        if (numdiff< 2) {
            ili9481_Setwindow(0,480-1,0,320-1);
            LCD_CS_CLR;
            LCD_RS_SET;
            for (i=diffsx; i<=diffex; i++) {
                for (j=diffsy;j<=diffey; j++) {
                    if (diffmap[i][j]!=0)
                        write_dot(i,j,drawmap[flag][i][j]);
                }
            }
            usleep(700L);
            
        } else {
            ili9481_Setwindow(diffsy,diffey,diffsx,diffex);
            LCD_CS_CLR;
            LCD_RS_SET;
            for (i=diffsx; i<=diffex; i++) {
                for (j=diffsy;j<=diffey; j++) {
                    LCD_WR_Data(drawmap[flag][i][j]);
                }
            }
        }
    }
}


void loadFrameBuffer_diff_480320()
{
    int  xsize=480, ysize=320;
    unsigned char *buffer;
    FILE *infile=fopen("/dev/fb0","rb");
    int i,j;
    unsigned long offset=0;
    int p;
    int flag;
    int drawmap[2][ysize][xsize];
    int diffmap[ysize][xsize];
    int diffsx, diffsy, diffex, diffey;
    int numdiff=0;
    
    buffer = (unsigned char *) malloc(xsize * ysize * 2);

    ili9481_Setwindow(0,480-1,0,320-1);
    LCD_CS_CLR;
    LCD_RS_SET;

    for (i=0; i < ysize; i++) {
        for (j=0; j< xsize; j++) {
            diffmap[i][j]=1;
            drawmap[0][i][j]=0;
            LCD_WR_Data(0);
            drawmap[1][i][j]=255;
        }
    }
    
    flag=1;
    
    while (1) {
        rewind(infile);
        if (fread (buffer, xsize * ysize *2, sizeof(unsigned char), infile) != 1) {
            printf ("Read < %d chars when loading file %s\n", xsize*ysize*2, "/dev/fb0");
            printf ("config.txt setting error\n") ;
            return;
        }

        numdiff=0;
        flag=1-flag;
        diffex=diffey=0;
        diffsx=diffsy=65535;
        
        for (i=0; i < ysize; i++) {
            for(j=0; j < xsize; j++) {
                offset =  (i * xsize+ j)*2;
                p=(buffer[offset+1] << 8) | buffer[offset];
                
                if (drawmap[1-flag][i][j] != p) {
                    drawmap[flag][i][j] = p;
                    diffmap[i][j]=1;
                    drawmap[1-flag][i][j]=p;
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
            ili9481_Setwindow(0,480-1,0,320-1);
            LCD_CS_CLR;
            LCD_RS_SET;
            for (i=diffsx; i<=diffex; i++) {
                for (j=diffsy;j<=diffey; j++) {
                    if (diffmap[i][j]!=0)
                        write_dot(i,j,drawmap[flag][i][j]);
                }
            }
            usleep(700L);
            
        } else {
            ili9481_Setwindow(diffsy,diffey,diffsx,diffex);
            LCD_CS_CLR;
            LCD_RS_SET;
            
            for (i=diffsx; i<=diffex; i++) {
                for (j=diffsy;j<=diffey; j++) {
                    LCD_WR_Data(drawmap[flag][i][j]);
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
    //loadFrameBuffer_diff_960640();
    return 0 ;
}
