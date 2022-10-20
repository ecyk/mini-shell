#ifndef EFI_H
#define EFI_H

#include <efi/boot-services.h>
#include <efi/runtime-services.h>
#include <efi/system-table.h>
#include <efi/types.h>

void reverse_string(char16_t* string, int len);
void int_to_string(int value, char16_t* buffer);
bool string_compare(const char16_t* s1, const char16_t* s2, size_t n);

#endif
