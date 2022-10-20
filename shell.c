#include "shell.h"

#define INPUT_BUFFER_SIZE 128

extern efi_handle g_image_handle;
extern efi_system_table* g_system_table;

extern efi_simple_text_output_protocol* g_cout;
extern efi_simple_text_input_protocol* g_cin;

char16_t buffer[INPUT_BUFFER_SIZE] = {0};
size_t offset;

void* memset(void* dst, uint8_t ch, size_t count) {
  for (uint8_t* ptr = dst; count > 0; count--, ptr++) {
    *ptr = ch;
  }
  return dst;
}

void reverse_string(char16_t* string, int len) {
  for (int i = 0, j = len - 1; i < j; i++, j--) {
    int c = string[i];
    string[i] = string[j];
    string[j] = c;
  }
}

void int_to_string(int value, char16_t* buffer) {
  int i, sign;

  if ((sign = value) < 0) value = -value;
  i = 0;

  do {
    buffer[i++] = value % 10 + '0';
  } while ((value /= 10) > 0);

  if (sign < 0) buffer[i++] = '-';

  reverse_string(buffer, i);
  buffer[i] = '\0';
}

bool string_compare(const char16_t* s1, const char16_t* s2, size_t n) {
  if (n == 0) return true;
  do {
    if (*s1 != *s2++) return false;
    if (*s1++ == 0) break;
  } while (--n != 0);
  return true;
}

bool read_line() {
  while (true) {
    efi_input_key input_key;
    efi_status efi_status = g_cin->ReadKeyStroke(g_cin, &input_key);
    if (EFI_ERROR(efi_status)) {
      break;
    }

    if (input_key.UnicodeChar == L'\b') {
      if (offset > 0) {
        offset -= 1;
        g_cout->OutputString(g_cout, &input_key.UnicodeChar);
      }
      continue;
    }

    g_cout->OutputString(g_cout, &input_key.UnicodeChar);

    if (input_key.UnicodeChar == L'\r' || input_key.UnicodeChar == L'\n') {
      return true;
    }

    buffer[offset++] = input_key.UnicodeChar;
    buffer[offset] = L'\0';
  }

  return false;
}

void help();
void echo();
void color();
void clear();
void exit();
void time();

void shell_run() {
  while (true) {
    bool execute = read_line();

    if (execute) {
      g_cout->OutputString(g_cout, L"\r\n");

      if (string_compare(&buffer[0], L"help", 5)) {
        help();
      } else if (string_compare(&buffer[0], L"echo", 4)) {
        echo();
      } else if (string_compare(&buffer[0], L"color", 5)) {
        color();
      } else if (string_compare(&buffer[0], L"clear", 6)) {
        clear();
      } else if (string_compare(&buffer[0], L"exit", 5)) {
        exit();
      } else if (string_compare(&buffer[0], L"time", 5)) {
        time();
      } else {
        g_cout->OutputString(g_cout, L"Komut Bulunamadi!\r\n");
      }

      g_cout->OutputString(g_cout, L"> ");

      offset = 0;
      memset(&buffer[0], L'0', INPUT_BUFFER_SIZE);
    }
  }
}

void help() {
  g_cout->OutputString(g_cout,
                       L"1. help            komut listesini gosterir.\r\n"
                       L"2. echo <message>  <message> yazisini gosterir.\r\n"
                       L"3. color <R><G><B> yazi rengini degistirir.\r\n"
                       L"4. clear           ekrani temizler.\r\n"
                       L"5. exit            cikis yapar.\r\n"
                       L"6. time            tarih ve saati gosterir.\r\n");
}

void echo() {
  char16_t* message = &buffer[4];
  if (*message == L'\0') {
    g_cout->OutputString(g_cout, L"<message>\r\n");
    return;
  }

  if (*message == L' ') {
    message++;
  }

  g_cout->OutputString(g_cout, message);
  g_cout->OutputString(g_cout, L"\r\n");
}

void color() {
  g_cout->SetAttribute(g_cout, EFI_WHITE);

  char16_t* message = &buffer[5];
  if (*message == L'\0') {
    g_cout->OutputString(g_cout, L"<R> or <G> or <B>\r\n");
    return;
  }

  if (*message == L' ') {
    message++;
  }

  switch (*message) {
    case L'R':
      g_cout->SetAttribute(g_cout, EFI_RED);
      break;
    case L'G':
      g_cout->SetAttribute(g_cout, EFI_GREEN);
      break;
    case L'B':
      g_cout->SetAttribute(g_cout, EFI_BLUE);
      break;
  }
}

void clear() { g_cout->ClearScreen(g_cout); }

void exit() {
  g_system_table->BootServices->Exit(g_image_handle, EFI_SUCCESS, 0, NULL);
}

void time() {
  efi_time efi_time;
  g_system_table->RuntimeServices->GetTime(&efi_time, NULL);

  char16_t buffer[20];
  int_to_string(efi_time.Year, &buffer[0]);
  buffer[4] = L'/';
  int_to_string(efi_time.Month, &buffer[5]);
  buffer[6] = L'/';
  int_to_string(efi_time.Day, &buffer[7]);
  buffer[9] = L' ';
  int_to_string(efi_time.Hour, &buffer[10]);
  buffer[12] = L':';
  int_to_string(efi_time.Minute, &buffer[13]);
  buffer[15] = L':';
  int_to_string(efi_time.Second, &buffer[17]);

  g_cout->OutputString(g_cout, &buffer[0]);
  g_cout->OutputString(g_cout, L"\r\n");
}
