@ LCD.s
@ Student names: change this to your names or look very silly
@ Last modification date: change this to the last modification date or look very silly

@ Runs on LM4F120/TM4C123
@ Use SSI0 to send an 8-bit code to the ST7735 160x128 pixel LCD.

@ As part of Lab 7, students need to implement these LCD_WriteCommand and LCD_WriteData
@ This driver assumes two low-level LCD functions

@ Backlight (pin 10) connected to +3.3 V
@ MISO (pin 9) unconnected
@ SCK (pin 8) connected to PA2 (SSI0Clk)
@ MOSI (pin 7) connected to PA5 (SSI0Tx)
@ TFT_CS (pin 6) connected to PA3 (SSI0Fss)
@ CARD_CS (pin 5) unconnected
@ Data/Command (pin 4) connected to PA6 (GPIO)
@ RESET (pin 3) connected to PA7 (GPIO)
@ VCC (pin 2) connected to +3.3 V
@ Gnd (pin 1) connected to ground

.syntax unified
.equ DC                      ,    0x40004100
.equ DC_COMMAND              ,    0
.equ DC_DATA                 ,    0x40
.equ GPIO_PORTA_DATA_R       ,    0x400043FC
.equ SSI0_DR_R               ,    0x40008008
.equ SSI0_SR_R               ,    0x4000800C
.equ SSI_SR_RNE              ,    0x00000004  @ SSI Receive FIFO Not Empty
.equ SSI_SR_BSY              ,    0x00000010  @ SSI Busy Bit
.equ SSI_SR_TNF              ,    0x00000002  @ SSI Transmit FIFO Not Full

.global writecommand
.global writedata

.text
.THUMB
.ALIGN

@ The Data/Command pin must be valid when the eighth bit is
@ sent.  The SSI module has hardware input and output FIFOs
@ that are 8 locations deep.  Based on the observation that
@ the LCD interface tends to send a few commands and then a
@ lot of data, the FIFOs are not used when writing
@ commands, and they are used when writing data.  This
@ ensures that the Data/Command pin status matches the byte
@ that is actually being transmitted.
@ The write command operation waits until all data has been
@ sent, configures the Data/Command pin for commands, sends
@ the command, and then waits for the transmission to
@ finish.
@ The write data operation waits until there is room in the
@ transmit FIFO, configures the Data/Command pin for data,
@ and then adds the data to the transmit FIFO.
@ NOTE: These functions will crash or stall indefinitely if
@ the SSI0 module is not initialized and enabled.

@ This is a helper function that sends an 8-bit command to the LCD.
@ Input: R0  8-bit command to transmit
@ Output: none
@ Assumes: SSI0 and port A have already been initialized and enabled
@1) Read SSI0_SR_R and check bit 4, 
@2) If bit 4 is high, loop back to step 1 (wait for BUSY bit to be low)
@3) Clear D/C=PA6 to zero
@4) Write the command to SSI0_DR_R
@5) Read SSI0_SR_R and check bit 4, 
@6) If bit 4 is high, loop back to step 5 (wait for BUSY bit to be low)
writecommand:	LDR R1,=SSI0_SR_R @ get address of SSI0_SR_R in R1
bsyw1:	LDR R2,[R1] @ get value of SSI0_SR_R in R2
	AND R2,#SSI_SR_BSY @ mask SSI0_SR_R with bit 4
	CMP R2,#0
	BNE bsyw1 @ if busy bit is 1, go back to bsyw1 and keep checking
	LDR R1,=DC @ load portA data register address into R1
	LDR R2,[R1] @ load port A data register value into R2
	BIC R2,#DC_DATA @ clear DC_DATA bit (PA6 = 0x40)
	STR R2,[R1] @ store it back
	LDR R1,=SSI0_DR_R @ get address of SSI0_DR_R in R1
	STR R0,[R1] @ store command into SSI0_DR_R
	LDR R1,=SSI0_SR_R @ get address of SSI0_SR_R in R1
bsyw2:	LDR R2,[R1] @ get value of it in R2
	AND R2,#SSI_SR_BSY @ mask it with bit 4
	CMP R2,#0
	BNE bsyw2 @ if busy bit is 1, go back to bsyw2 and keep checking
    BX  LR                          @   return

@ This is a helper function that sends an 8-bit data to the LCD.
@ Input: R0  8-bit data to transmit
@ Output: none
@ Assumes: SSI0 and port A have already been initialized and enabled
@1) Read SSI0_SR_R and check bit 1, 
@2) If bit 1 is low loop back to step 1 (wait for TNF bit to be high)
@3) Set D/C=PA6 to one
@4) Write the 8-bit data to SSI0_DR_R
writedata:	PUSH {R1, R2, R3, LR}
loop:
	LDR R1, =SSI0_SR_R
	LDR R1, [R1]	@R2 has the data of SSIO_SR_R
	AND R1, R1, #0x02 @get bit 1
	CMP R1, #0
	BEQ loop
	LDR R2, =DC
	LDR R3, [R2]
	ORR R3, #DC_DATA	@setting PA6 to one
	STR R3, [R2]
	
	LDR R1, =SSI0_DR_R
	@R0 contains the data
	STR R0, [R1] 
	POP {R1, R2, R3, PC}	

@***************************************************
@ This is a library for the Adafruit 1.8" SPI display.
@ This library works with the Adafruit 1.8" TFT Breakout w/SD card
@ ----> http://www.adafruit.com/products/358
@ as well as Adafruit raw 1.8" TFT display
@ ----> http://www.adafruit.com/products/618
@
@ Check out the links above for our tutorials and wiring diagrams
@ These displays use SPI to communicate, 4 or 5 pins are required to
@ interface (RST is optional)
@ Adafruit invests time and resources providing this open source code,
@ please support Adafruit and open-source hardware by purchasing
@ products from Adafruit!
@
@ Written by Limor Fried/Ladyada for Adafruit Industries.
@ MIT license, all text above must be included in any redistribution
@****************************************************

ALIGN:                           @ make sure the end of this section is aligned
END:                             @ end of file
