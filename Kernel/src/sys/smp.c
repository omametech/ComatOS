#include <sys/smp.h>
#include <fw/madt.h>
#include <drivers/timer/hpet.h>
#include <sys/cpuid.h>
#include <sys/apic.h>
#include <printf.h>
#include <libk.h>
#include <sys/gdt.h>

void ap_trampoline(void);

volatile uint8_t aprunning = 0;
uint8_t bspID, bspDone = 0; // free spinlock

void smp_init() {
    cpuid_data_t data = cpuid(1);
    bspID = data.rbx >> 24;

    // copy the AP trampoline to a fixed address in low conventional mem
    memcpy((void*)0x8000, &ap_trampoline, 4096);

    // copy the GDT pointer
    memcpy((void*)0x9000, &gdt_ptr, sizeof(gdt_ptr_t));

    // for each local APIC
    for (int i = 0; i < lapics_length; i++) {
        madt_lapic_t *lapic = lapics[i];
        // no need to bring up the BSP
        if (lapic->apicID == bspID) continue;
        // if the processor isn't enabled nor online capable,
        // don't try to bring it up
        if (
            (lapic->flags & 0b01) == 0
            && (lapic->flags & 0b10) == 0
        ) continue;

        printf("smp: bringing up LAPIC #%d\n", i);

        lapic_write(0x280, 0); // clear APIC errors
        apic_send_ipi(i, 0x00C500); // init IPI
        do { asm("pause"); } while(lapic_read(0x300) &  (1 << 12)); // wait for delivery
        apic_send_ipi(i, 0x008500); // deassert
        do { asm("pause"); } while(lapic_read(0x300) &  (1 << 12)); // wait for delivery
        mdelay(10);

        // send startup IPI twice
        for (int j = 0; j < 2; j++) {
            lapic_write(0x280, 0); // clear APIC errors
            apic_send_ipi(i, 0x000608); // startup IPI for 0800:0000
            udelay(200);
            do { asm("pause"); } while(lapic_read(0x300) &  (1 << 12)); // wait for delivery
        }
    }

    // release the spinlocks
    bspDone = 1;

    printf("smp: APs running=%d\n", aprunning);
}

void ap_startup() {
    while(1);
}
