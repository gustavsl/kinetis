/*******************************************************************************
*
* kinetis.h
*
*******************************************************************************/
#if !defined(KINETIS_H)
#define KINETIS_H

#include "globalDefs.h"

/*******************************************************************************
* SIM
*******************************************************************************/
#define SIM_SDID (*(volatile uint32_t *) 0x40048024)

#define SIM_REVID_SHIFT 12
#define SIM_REVID_MASK (0x4 << SIM_REVID_SHIFT)     /* Device Revision Number */

#define SIM_FAMID_SHIFT 4
#define SIM_FAMID_MASK (0x7 << SIM_FAMID_SHIFT)          /* Kinetis Family Id */
#define SIM_FAMID_STRINGS "K10", "K20", "K30", "K40", \
                          "K60", "K70", "K50 and K52", "K51 and K53"

#define SIM_PINID_SHIFT 0
#define SIM_PINID_MASK (0xf << SIM_PINID_SHIFT)                 /* Pincout Id */

                                       /* System Clock Gate Control Registers */
#define SIM_SCFC5 (*(volatile uint32_t *) 0x40048038)
#define SIM_PORTA_ENABLE BIT_9
#define SIM_PORTE_ENABLE BIT_13

/*******************************************************************************
* PORT CONTROLLER
*******************************************************************************/
#define PORT_ISF BIT_24                              /* Interrupt Status Mask */

enum {                                             /* Interrupt Configuration */
    PORT_IRQC_DISABLED         =  0 << 16,
    PORT_IRQC_DMA_RISING_EDGE  =  1 << 16,
    PORT_IRQC_DMA_FALLING_EDGE =  2 << 16,
    PORT_IRQC_DMA_EITHER_EDGE  =  3 << 16,
    PORT_IRQC_INT_WHEN_LOW     =  8 << 16,
    PORT_IRQC_INT_RISING_EDGE  =  9 << 16,
    PORT_IRQC_INT_FALLING_EDGE = 10 << 16,
    PORT_IRQC_INT_EITHER_EDGE  = 11 << 16,
    PORT_IRQC_INT_WHEN_HIGH    = 12 << 16,
};

#define PORT_LK BIT_15                                       /* Lock Register */

enum {                                                     /* Pin Mux Control */
    PORT_MUX_DISABLED = 0 << 8,
    PORT_MUX_ANALOG   = 0 << 8,
    PORT_MUX_GPIO     = 1 << 8,
    PORT_MUX_ALT1     = 1 << 8,              /* ALT1-7 Chip Specific Function */
    PORT_MUX_ALT2     = 2 << 8,
    PORT_MUX_ALT3     = 3 << 8,
    PORT_MUX_ALT4     = 4 << 8,
    PORT_MUX_ALT5     = 5 << 8,
    PORT_MUX_ALT6     = 6 << 8,
    PORT_MUX_ALT7     = 7 << 8,
};

#define PORT_DSE BIT_6                               /* Drive Strength Enable */
#define PORT_ODE BIT_5                                   /* Open Drain Enable */
#define PORT_PFE BIT_4                               /* Passive Filter Enable */
#define PORT_SRE BIT_2                    /* Slew Rate Enable (0:fast,1:slow) */
#define PORT_PE  BIT_1                                         /* Pull Enable */
#define PORT_PS  BIT_0                           /* Pull Select (0:down,1:up) */

#define PORT_PULLUP_ENABLE   (PORT_PE | PORT_PS)
#define PORT_PULLDOWN_ENABLE (PORT_PE)

#define PORTA_BASE_ADDR 0x40049000
#define PORTB_BASE_ADDR 0x4004a000
#define PORTC_BASE_ADDR 0x4004b000
#define PORTD_BASE_ADDR 0x4004c000
#define PORTE_BASE_ADDR 0x4004d000

#define PORTA PORTA_BASE_ADDR
#define PORTB PORTB_BASE_ADDR
#define PORTC PORTC_BASE_ADDR
#define PORTD PORTD_BASE_ADDR
#define PORTE PORTE_BASE_ADDR

#define PORT_PCR(port, pin) (*(volatile uint32_t *) (port + (4 * pin)))

/*******************************************************************************
* GPIO
*
* x_PDOR = Port Data Output Register
* x_PSOR = Port Set Output Register
* x_PCOR = Port Clear Output Register
* x_PTOR = Port Toggle Register
* x_PDIR = Port Data Input Register
* x_PDDR = Port Data Direction Register
*
*******************************************************************************/
#define GPIOA_BASE_ADDR 0x400ff000
#define GPIOA_PDOR (*(volatile uint32_t *) (GPIOA_BASE_ADDR + 0x00))
#define GPIOA_PSOR (*(volatile uint32_t *) (GPIOA_BASE_ADDR + 0x04))
#define GPIOA_PCOR (*(volatile uint32_t *) (GPIOA_BASE_ADDR + 0x08))
#define GPIOA_PTOR (*(volatile uint32_t *) (GPIOA_BASE_ADDR + 0x0c))
#define GPIOA_PDIR (*(volatile uint32_t *) (GPIOA_BASE_ADDR + 0x10))
#define GPIOA_PDDR (*(volatile uint32_t *) (GPIOA_BASE_ADDR + 0x14))

#endif
