// MIT License
//
// Copyright (c) 2019 Filip Björklund
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "alflib/core/dialog.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

// Project headers
#include "alflib/platform/platform.hpp"

// Library headers
#if defined(ALFLIB_TAREGT_LINUX)
#include <gtk/gtk.h>
#endif

// ========================================================================== //
// Functions
// ========================================================================== //

namespace alflib {

void
ShowInfoDialog(const String& title, const String& message)
{
#if defined(ALFLIB_TARGET_WINDOWS)
  MessageBoxW(nullptr,
              message.GetUTF16().Get(),
              title.GetUTF16().Get(),
              MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
#elif defined(ALFLIB_TARGET_LINUX)
  if (!gtk_init_check(0, nullptr)) {
    return;
  }

  // Create a show dialog
  GtkWidget* dialog = gtk_message_dialog_new(nullptr,
                                             GTK_DIALOG_MODAL,
                                             GTK_MESSAGE_INFO,
                                             GTK_BUTTONS_OK,
                                             "%s",
                                             message.GetUTF8());
  gtk_window_set_title(GTK_WINDOW(dialog), title.GetUTF8());
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(GTK_WIDGET(dialog));

  // Run event loop
  while (g_main_context_iteration(nullptr, false)) {
  }
#endif
}

// -------------------------------------------------------------------------- //

void
ShowWarningDialog(const String& title, const String& message)
{
#if defined(ALFLIB_TARGET_WINDOWS)
  MessageBoxW(nullptr,
              message.GetUTF16().Get(),
              title.GetUTF16().Get(),
              MB_OK | MB_APPLMODAL | MB_ICONWARNING);
#elif defined(ALFLIB_TARGET_LINUX)
  if (!gtk_init_check(0, nullptr)) {
    return;
  }

  // Create a show dialog
  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                             GTK_DIALOG_MODAL,
                                             GTK_MESSAGE_WARNING,
                                             GTK_BUTTONS_OK,
                                             "%s",
                                             message.GetUTF8());
  gtk_window_set_title(GTK_WINDOW(dialog), title.GetUTF8());
  gtk_dialog_run(GTK_DIALOG(dialog));

  // Cleanup
  gtk_widget_destroy(GTK_WIDGET(window));
  gtk_widget_destroy(GTK_WIDGET(dialog));

  // Run event loop
  while (g_main_context_iteration(nullptr, false)) {
  }
#endif
}

// -------------------------------------------------------------------------- //

void
ShowErrorDialog(const String& title, const String& message)
{
#if defined(ALFLIB_TARGET_WINDOWS)
  MessageBoxW(nullptr,
              message.GetUTF16().Get(),
              title.GetUTF16().Get(),
              MB_OK | MB_APPLMODAL | MB_ICONERROR);
#elif defined(ALFLIB_TARGET_LINUX)
  if (!gtk_init_check(0, nullptr)) {
    return;
  }

  // Create a show dialog
  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                             GTK_DIALOG_MODAL,
                                             GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_CANCEL,
                                             "%s",
                                             message.GetUTF8());
  gtk_window_set_title(GTK_WINDOW(dialog), title.GetUTF8());
  gtk_dialog_run(GTK_DIALOG(dialog));

  // Cleanup
  gtk_widget_destroy(GTK_WIDGET(window));
  gtk_widget_destroy(GTK_WIDGET(dialog));

  // Run event loop
  while (g_main_context_iteration(nullptr, false)) {
  }
#endif
}

}
