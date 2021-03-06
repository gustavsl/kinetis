/*******************************************************************************
*
* demoUart.c
*
* Simple uart demo.
* This demo supports the following serial interface
*
* Byte 0: Framing Byte
*       0x41, 'A' Valid frame
* Byte 1: Command Byte
*       0x30, '0' Do nothing
*       0x31, '1' Echo help message
*       0x32, '2' Toggle LED status
*       0x32, '3' Quit
* Byte 2: Select language for echo
*       0x30, '0' English
*       0x31, '1' Drunken English
* Byte 3: CRC
*       XOR Bytes 0-2.
*       e.g. A,1,0 would be 0x41 ^ 0x31 ^ 0x30 = 0x40 ('@')
*       e.g. A,2,0 would be 0x41 ^ 0x32 ^ 0x30 = 0x43 ('C')
*       e.g. A,3,0 would be 0x41 ^ 0x33 ^ 0x30 = 0x42 ('B')
*
*
*
*
* James McAnanama
*
* Copyright (C) 2012 www.laswick.net
*
* This program is free software.  It comes without any warranty, to the extent
* permitted by applicable law.  You can redistribute it and/or modify it under
* the terms of the WTF Public License (WTFPL), Version 2, as published by
* Sam Hocevar.  See http://sam.zoy.org/wtfpl/COPYING for more details.
* Canada Day 2012
*
*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "kinetis.h"
#include "hardware.h"
#include "globalDefs.h"
#include "util.h"

#define COLOUR_STRINGS "ORANGE\t", "YELLOW\t", "GREEN\t", "BLUE\r\n"
enum {
    ORANGE,
    YELLOW,
    GREEN,
    BLUE,
};

enum {
    UPDATE_NONE,
    UPDATE_CMD,
    UPDATE_GARBAGE,
};

static int updateFlags;
static int fd;

enum {
    SERIAL_CMD_NOTHING    = '0',
    SERIAL_CMD_HELP       = '1',
    SERIAL_CMD_TOGGLE_LED = '2',
    SERIAL_CMD_QUIT       = '3',
};

enum {
    HELP_ENGLISH = '0',
    HELP_DRUNK   = '1',
};

typedef struct {
    int32_t cmd;
    int32_t style;
    char *helpStringEng;
    char *helpStringDEng;
    char garbage[256];
} msg_t;

static msg_t msg = {
    .cmd           = SERIAL_CMD_HELP,
    .style         = HELP_ENGLISH,
    .helpStringEng =     "\r\n"
                         "Serial Interface: \r\n"
                         "Escape message by hitting the tab, then: \r\n"
                         "Byte 0: Framing byte 'A', \r\n"
                         "Byte 1: Cmd: \r\n"
                         "          '1' Echo this help message, \r\n"
                         "          '2' Toggle LED flashing. \r\n"
                         "          '3' Quit. \r\n"
                         "Byte 2: Style: '0' English, '1' Drunken English \r\n"
                         "Byte 3: CRC: XOR Bytes 0 - 2 \r\n"
                         "Terminate with a carriage return \r\n"
                         "Try these: \r\n"
                         "A10@, A11A, A20C, A30B \r\n",

    .helpStringDEng =    "\r\n"
                         "Whaaaa? ... \r\n"
                         "I you say 'A' Eh?  HAAAAA! \r\n"
                         "FnuI'll go.   .?  \r\n"
                         "What?  \r\n"
                         "I love you man... ZZZZzzzzz \r\n",
};

/******************************************************************************
* uartCallBackHandler
*
* Parse RX messages.
*
******************************************************************************/
static void uartCallBackHandler(uint8_t const *buf, int len)
{
    if (len >= 4 && buf[0] == 'A') {
        int i;
        int crc = 0;
        msg.cmd = SERIAL_CMD_NOTHING;
        for (i = 0; i < 3; i++) {
            crc ^= buf[i];
        }
        if (crc == buf[3]) {
            msg.cmd   = buf[1];
            msg.style = buf[2];
            updateFlags |= UPDATE_CMD;
        }
    }
    else {
        len = len > 255 ? 255 : len;
        memcpy(msg.garbage, buf, len);
        msg.garbage[len] = '\0';
        updateFlags |= UPDATE_GARBAGE;
    }

   return;
}


static void setClock(void)
{
    /* -------- 100 MHz (external clock) -----------
     * Configure the Multipurpose Clock Generator output to use the external
     * clock locked with a PLL at the maximum frequency of 100MHZ
     *
     * For PLL, the dividers must be set first.
     *
     * System:  100 MHz
     * Bus:      50 MHz
     * Flexbus:  50 MHz
     * Flash:    25 MHz
     */
    clockSetDividers(DIVIDE_BY_1, DIVIDE_BY_2, DIVIDE_BY_4, DIVIDE_BY_4);
    clockConfigMcgOut(MCG_PLL_EXTERNAL_100MHZ);

    return;


}

int main(void)
{
    static char *colourStrings[] = { COLOUR_STRINGS };
    int quit  = FALSE;
    int blink = FALSE;

    int32_t len;
    char readString[2048] = {'\0'};

    setClock();


    updateFlags = UPDATE_CMD; /* Launch w help msg */


    /* Install uart into the device table before using it */
    uart_install();

    /*
     * Register the standard I/O streams with a particular deivce driver.
     */

    int fd0 = fdevopen(stdin, "uart3", 0, 0);
    ioctl(fd0, IO_IOCTL_UART_BAUD_SET, 115200);
    assert(fd0 != -1);
    int fd2 = fdevopen(stderr, "uart3", 0, 0);
    ioctl(fd0, IO_IOCTL_UART_BAUD_SET, 115200);
    assert(fd0 != -1);



    fd = fdevopen(stdout, "uart3", 0, 0);
    ioctl(fd, IO_IOCTL_UART_BAUD_SET, 115200);
    assert(fd != -1);


    /*
     * Casting the callback down the (int) flag shoot feels dirty,
     * but the leprechaun that screams in my ear told me to do it...
     */
    ioctl(fd, IO_IOCTL_UART_CALL_BACK_SET, (int)uartCallBackHandler);

              /* Don't bother listening until you hear a tab key.  I mean it! */
    ioctl(fd, IO_IOCTL_UART_ESCAPE_SET, '\t');

                                          /* Call me when the user hits enter */
    ioctl(fd, IO_IOCTL_UART_TERMINATOR_SET, '\r');
    ioctl(fd, IO_IOCTL_UART_BAUD_SET, 115200);

//    printf("The start of something good...\r\n"); /* StdOut is uart3 */

    gpioConfig(N_LED_ORANGE_PORT, N_LED_ORANGE_PIN, GPIO_OUTPUT | GPIO_LOW);
    gpioConfig(N_LED_YELLOW_PORT, N_LED_YELLOW_PIN, GPIO_OUTPUT | GPIO_LOW);
    gpioConfig(N_LED_GREEN_PORT,  N_LED_GREEN_PIN,  GPIO_OUTPUT | GPIO_LOW);
    gpioConfig(N_LED_BLUE_PORT,   N_LED_BLUE_PIN,   GPIO_OUTPUT | GPIO_LOW);

    while (!quit) {
        if (blink) {
            delay();
            gpioClear(N_LED_ORANGE_PORT, N_LED_ORANGE_PIN);
            write(fd, colourStrings[ORANGE], strlen(colourStrings[ORANGE]));
            delay();
            gpioClear(N_LED_YELLOW_PORT, N_LED_YELLOW_PIN);
            write(fd, colourStrings[YELLOW], strlen(colourStrings[YELLOW]));
            delay();
            gpioClear(N_LED_GREEN_PORT, N_LED_GREEN_PIN);
            write(fd, colourStrings[GREEN], strlen(colourStrings[GREEN]));
            delay();
            gpioClear(N_LED_BLUE_PORT, N_LED_BLUE_PIN);
            write(fd, colourStrings[BLUE], strlen(colourStrings[BLUE]));

            delay();
            gpioSet(N_LED_ORANGE_PORT, N_LED_ORANGE_PIN);
            delay();
            gpioSet(N_LED_YELLOW_PORT, N_LED_YELLOW_PIN);
            delay();
            gpioSet(N_LED_GREEN_PORT, N_LED_GREEN_PIN);
            delay();
            gpioSet(N_LED_BLUE_PORT, N_LED_BLUE_PIN);
        }



        if (updateFlags & UPDATE_CMD) {
            updateFlags &= ~UPDATE_CMD;
            switch (msg.cmd) {
            case SERIAL_CMD_HELP:
                if (msg.style == HELP_ENGLISH) {
                    write(fd, msg.helpStringEng, strlen(msg.helpStringEng));
                }
                else {
                    write(fd, msg.helpStringDEng, strlen(msg.helpStringDEng));
                }
                break;
            case SERIAL_CMD_QUIT:
                write(fd, "\r\n BuBye \r\n \r\n",
                   strlen("\r\n BuBye \r\n \r\n"));
                quit = TRUE;
                break;
            case SERIAL_CMD_TOGGLE_LED:
                blink = !blink;
                break;
            }
        }

        if (updateFlags & UPDATE_GARBAGE) {
            updateFlags &= ~UPDATE_GARBAGE;
            write(fd, "\r\n GARBAGE: ",
                strlen("\r\n GARBAGE:"));
            write(fd, msg.garbage, strlen(msg.garbage));
            write(fd, "\r\n",
                strlen("\r\n"));

        }

    }


    gpioSet(N_LED_ORANGE_PORT, N_LED_ORANGE_PIN);
    gpioSet(N_LED_YELLOW_PORT, N_LED_YELLOW_PIN);
    gpioSet(N_LED_GREEN_PORT, N_LED_GREEN_PIN);
    gpioSet(N_LED_BLUE_PORT, N_LED_BLUE_PIN);


    /* 'Quiting' to echo mode.  Turn off callback and use reads */
    ioctl(fd, IO_IOCTL_UART_CALL_BACK_SET, (int)NULL);
#if 1
        printf("Whoop...\n\n");
        printf("\tWhoop...\n\n");
        printf("\t\tWhoop...\n\n");
        printf("\t\t\t  Gangnam Style!\n\n");
#endif

    for (;;) {
        delay();
        gpioClear(N_LED_BLUE_PORT, N_LED_BLUE_PIN);
        delay();
        gpioSet(N_LED_BLUE_PORT, N_LED_BLUE_PIN);

#if 1
        /* TODO scanf doesn't work */
        int d[1024];
        printf("Enter a value:");
        scanf("%d", d);
        printf("/n Sorry, %d is wrong... \n", d[0]);
#endif


        printf("Enter 10 chars: \n");
                                                /* read 10 bytes */
        len = read(fd, (uint8_t *)readString, 10);
        if (len < 254) {
            readString[len + 1] = '\0';
        }
        printf("Sorry, %s is wrong. \n", readString);
    }

    close(fd);
    return 0;
}
