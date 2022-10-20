del efi_main.o
del BOOTX64.EFI
del fat.img

cls

clang -target x86_64-unknown-windows -ffreestanding -fshort-wchar -mno-red-zone -I libs/zircon-uefi/include -c -o efimain.o efimain.c
clang -target x86_64-unknown-windows -ffreestanding -fshort-wchar -mno-red-zone -I libs/zircon-uefi/include -c -o shell.o shell.c

clang -target x86_64-unknown-windows -nostdlib -Wl,-entry:efi_main -Wl,-subsystem:efi_application -fuse-ld=lld-link -o BOOTX64.EFI efimain.o shell.o
