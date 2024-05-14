#ifndef __ASSEMBLER__
#define __ASSEMBLER__
#endif
#include <avr/io.h>

#define FLIPPER_CODE	0x12345670

.global main

#define OUT_PINS       _BV(PINB3) | _BV(PINB4)

.macro	togglelh cycles
	.rept \cycles
		out	_SFR_IO_ADDR(DDRB), r16
		out	_SFR_IO_ADDR(DDRB), r17
	.endr
.endm

.macro	togglehl cycles
	.rept \cycles
		out	_SFR_IO_ADDR(DDRB), r17
		out	_SFR_IO_ADDR(DDRB), r16
	.endr
.endm

// 1010 0000 0000 0000 0000 0000 0000 0000
// 1
.macro	preamble
	togglehl 16
	togglelh 16
	togglehl 16
	.rept 29
		togglelh 16
	.endr
	togglehl 16
.endm

.macro	send_bits	value, count
	.if	\count
		.if	\value & 0x80000000
			togglehl 16
		.else
			togglelh 16
		.endif
		send_bits	(\value << 1), (\count - 1)
	.endif
.endm

main:
	eor	r16, r16
	ldi	r17, OUT_PINS

start:
	preamble

	send_bits FLIPPER_CODE, 27

	togglelh 16
	togglelh 16

	send_bits FLIPPER_CODE << 27, 2

	rjmp	start
