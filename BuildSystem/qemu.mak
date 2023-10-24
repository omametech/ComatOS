FAT32_IMG := $(OUTPUT_DIRECTORY)/ComatOS.img
$(FAT32_IMG): all
	@echo -e "  FAT32\tComatOS.img"
	@dd if=/dev/zero of=$(FAT32_IMG) bs=1M count=4 2>/dev/null
	@mformat -i $(FAT32_IMG) ::
	@mcopy -i $(FAT32_IMG) $(OUTPUT_DIRECTORY)/Kernel/kernel.elf ::/kernel.elf
	@mmd -i $(FAT32_IMG) ::/EFI
	@mmd -i $(FAT32_IMG) ::/EFI/BOOT
	@mcopy -i $(FAT32_IMG) $(OUTPUT_DIRECTORY)/Bootloader/bootldr.efi ::/EFI/BOOT/BOOTx64.efi
fat32: $(FAT32_IMG)
.PHONY: fat32

OVMF_PATH := /usr/share/edk2-ovmf
QEMU_ARGS := -m 128M -cpu host -enable-kvm -serial stdio -smp sockets=1,cores=2,threads=2
-include $(PROJECT_ROOT)/.env.mak

qemu: $(FAT32_IMG)
	@echo -e "   QEMU\tComatOS.img"
	qemu-system-x86_64 -M q35 $(QEMU_ARGS) \
		-drive if=pflash,readonly=on,file=$(OVMF_PATH)/OVMF_CODE.fd \
		-hda $(FAT32_IMG)
