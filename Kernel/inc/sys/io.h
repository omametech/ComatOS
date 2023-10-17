#ifndef __KERNEL_IO_H
#define __KERNEL_IO_H

#include "stdint.h"

void outb(uint16_t port, uint8_t val);
void outw(uint16_t port, uint16_t val);
void outd(uint16_t port, uint32_t val);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t ind(uint16_t port);

#endif // __KERNEL_IO_H
