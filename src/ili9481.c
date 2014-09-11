#include "bcm2835.h"
#include "ili9481.h"

#include <stdio.h>

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

void ili9481_writedata(uint16_t val)
{
    LCD_CS_CLR;
    LCD_RS_SET;

    bcm2835_spi_transfer(val>>8);
    bcm2835_spi_transfer(val);
}

void ili9481_writedatan(uint32_t len, const uint16_t* data)
{
    uint32_t i;
    LCD_CS_CLR;
    LCD_RS_SET;

    uint16_t* ptr = data;
    for (i=0; i<len; i++) {
        bcm2835_spi_transfer(*ptr>>8);
        bcm2835_spi_transfer(*ptr);
        ptr++;
    }
}

int ili9481_init()
{
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

    return 0;
}

void ili9481_setwindow(uint32_t xs, uint32_t xe, uint32_t ys, uint32_t ye)
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

void ili9481_clear(uint16_t p)
{
    uint32_t i;
    ili9481_setwindow(0, 0, 0, 0);

    for (i=0;i<320*480;i++) {
        ili9481_writedata(p);
    }
}

void ili9481_pset(int dx,int dy, uint16_t color)
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
