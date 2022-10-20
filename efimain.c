#include "shell.h"

#define SHELL_ASCII_TEXT                           \
  L"  #####  #     # ####### #       #       \r\n" \
  L" #     # #     # #       #       #       \r\n" \
  L" #       #     # #       #       #       \r\n" \
  L"  #####  ####### #####   #       #       \r\n" \
  L"       # #     # #       #       #       \r\n" \
  L" #     # #     # #       #       #       \r\n" \
  L"  #####  #     # ####### ####### ####### \r\n" \
  L"                                         \r\n" \
  L"                                         \r\n" \
  L"                                         \r\n"

efi_handle g_image_handle;
efi_system_table* g_system_table;

efi_simple_text_output_protocol* g_cout;
efi_simple_text_input_protocol* g_cin;

efi_status efi_main(efi_handle image_handle, efi_system_table* system_table) {
  g_image_handle = image_handle;
  g_system_table = system_table;

  // Console Output
  g_cout = system_table->ConOut;

  efi_status efi_status = g_cout->Reset(g_cout, true);
  if (EFI_ERROR(efi_status)) {
    return efi_status;
  }

  // Console Input
  g_cin = system_table->ConIn;

  efi_status = g_cin->Reset(g_cin, true);
  if (EFI_ERROR(efi_status)) {
    g_cout->SetAttribute(g_cout, EFI_RED);
    g_cout->OutputString(g_cout,
                         L"The input device is not functioning correctly and "
                         L"could not be reset.\r\n");
    return efi_status;
  }

  g_cout->EnableCursor(g_cout, true);

  g_cout->SetAttribute(g_cout, EFI_WHITE);
  g_cout->OutputString(g_cout, SHELL_ASCII_TEXT);

  g_cout->OutputString(g_cout, g_system_table->FirmwareVendor);
  g_cout->OutputString(g_cout, L"\r\n\r\n");

  g_cout->SetAttribute(g_cout, EFI_WHITE);
  g_cout->OutputString(g_cout, L"> ");

  shell_run();

  return EFI_SUCCESS;
}
