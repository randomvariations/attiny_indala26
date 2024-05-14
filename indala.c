#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FLIPPER_CODE    0x12345670

#define CRT1_SIZE 48
// CRT1 + 4 bytes setup + 16 cycles/bit*64 bits*4 bytes 
// + 2 bytes jump to start + 4 bytes exit
#define INDALA_SIZE 4154

const uint8_t crt1_header[CRT1_SIZE] = {
  0x0e, 0xc0, 0x15, 0xc0, 0x14, 0xc0, 0x13, 0xc0, 0x12, 0xc0, 0x11, 0xc0,
  0x10, 0xc0, 0x0f, 0xc0, 0x0e, 0xc0, 0x0d, 0xc0, 0x0c, 0xc0, 0x0b, 0xc0,
  0x0a, 0xc0, 0x09, 0xc0, 0x08, 0xc0, 0x11, 0x24, 0x1f, 0xbe, 0xcf, 0xe5,
  0xd2, 0xe0, 0xde, 0xbf, 0xcd, 0xbf, 0x02, 0xd0, 0x04, 0xc8, 0xe8, 0xcf
};

static uint8_t prog[INDALA_SIZE];

void hl(uint8_t *addr) {
  int cycle;
  printf("1");
  for (cycle=0;cycle<16;cycle++) {
    addr[cycle*4]   = 0x17;
    addr[cycle*4+1] = 0xbb;
    addr[cycle*4+2] = 0x07;
    addr[cycle*4+3] = 0xbb;
  }
}

void lh(uint8_t *addr) {
  int cycle;
  printf("0");
  for (cycle=0;cycle<16;cycle++) {
    addr[cycle*4]   = 0x07;
    addr[cycle*4+1] = 0xbb;
    addr[cycle*4+2] = 0x17;
    addr[cycle*4+3] = 0xbb;
  }
}

void generate_indala_prog(uint16_t id1, uint32_t id2) {
  int i;
  int index = CRT1_SIZE;
  uint32_t bit31 = 1<<31;
  uint32_t indala = id2;

  memset(prog, 0, INDALA_SIZE);
  memcpy(prog, crt1_header, CRT1_SIZE);

  // eor r16,r16
  prog[index++] = 0x00;
  prog[index++] = 0x27;

  // ldi r17, _BV(PINB3) | _BV(PINB4)
  prog[index++] = 0x18;
  prog[index++] = 0xe1;

  printf("%x\n",indala);

  // preamble
  hl(&prog[index]);
  index+=16*4;
  lh(&prog[index]);
  index+=16*4;
  hl(&prog[index]);
  index+=16*4;
  for(i=0; i < 29; i++) {
    lh(&prog[index]);
    index+=16*4;
  }
  printf("\n");
  hl(&prog[index]);
  index+=16*4;

  for (i=0; i < 27; i++) {
    if (bit31 & indala) {
      hl(&prog[index]);
    } else {
      lh(&prog[index]);
    }
    indala = indala<<1;
    index+=16*4;
  }

  // two zeros
  lh(&prog[index]);
  index+=16*4;
  lh(&prog[index]);
  index+=16*4;

  // last two bits
  for (i=0; i < 2; i++) {
    if (bit31 & indala) {
      hl(&prog[index]);
    } else {
      lh(&prog[index]);
    }
    indala = indala<<1;
    index+=16*4;
  }
  printf("\n");

  // rjmp main
  prog[index++] = 0xff;
  prog[index++] = 0xc7;

  // cli (disable interrupts)
  prog[index++] = 0xf8;
  prog[index++] = 0x94;

  // rjmp .-2 (loop forever)
  prog[index++] = 0xff;
  prog[index++] = 0xcf;

  printf("Length: %d\n",index);

}

void main() {
  int ifd;

  ifd = open( "indala.bin",O_WRONLY | O_CREAT | O_TRUNC,0644);
  generate_indala_prog(0, FLIPPER_CODE );
  write(ifd,prog,INDALA_SIZE);
  close(ifd);

}
