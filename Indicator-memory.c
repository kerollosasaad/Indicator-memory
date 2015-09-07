/*  *vim
    *7-sept-2015
    *This is an Ubuntu appindicator that displays memory usage and cpu.
    *inspired from some functionalities of https://gist.github.com/982939
    *License: This software is free and in public domain so feel free to redistribute it and\or modify it.
    *Author: Kerollos Asaad Gerges.
*/

#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>
#include <glibtop.h>
#include <glibtop/cpu.h>
#include <glibtop/mem.h>
#include <glibtop/netload.h>
#include <glibtop/netlist.h>

int menu_choice = 1, period = 1;
gboolean first_run = TRUE;
AppIndicator *indicator;
GtkWidget *quit_item, *indicator_menu, *memory_item, *sysmon_item, *free_memory_item, *cpu_item, *systemMonitor_item, *help_item, *help_window;

static void system_monitor(GtkWidget *widget, gpointer data){
  GError *err = NULL;
  gchar *argv[] = {"gnome-system-monitor"};
  g_spawn_async(NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &err);
}

static void report_window (GtkWidget *widget, gpointer data)
{
  GtkWidget *window, *table, *label1;
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW (window), 350, 250);
  gtk_window_set_title(GTK_WINDOW(window), "Help!");
  gtk_window_move(GTK_WINDOW(window), 600, 200);
  table = gtk_table_new(1, 1, TRUE);

  label1 = gtk_label_new("mail:  KerollosAsaad@Gmail.com \n\n Send message to report a bug.");

  gtk_table_attach_defaults(GTK_TABLE(table), label1, 0, 1, 0, 1);
  gtk_container_add(GTK_CONTAINER(window), table);  

  gtk_widget_show_all(window);
}


double kb_to_gb(double kb){
  return kb / (1024.0 * 1024 * 1024);
}

double get_used_mem(){
  glibtop_mem mem;
  glibtop_get_mem (&mem);
  return kb_to_gb(mem.total - (mem.free + mem.buffer + mem.cached)); 
}

double get_total_mem(){
  glibtop_mem mem;
  glibtop_get_mem (&mem);
  return kb_to_gb(mem.total);
}

int get_cpu(){
  static int cpu_total_old = 0, cpu_idle_old = 0;
  glibtop_cpu cpu;
  glibtop_get_cpu (&cpu);
  int total_new = cpu.total - cpu_total_old, idle_new = cpu.idle - cpu_idle_old;
  cpu_total_old = cpu.total, cpu_idle_old = cpu.idle;
  return 100 * (total_new - idle_new) / total_new;
}

gboolean update(){
  double mem_used = get_used_mem();
  double mem_all = get_total_mem();
  double mem_free = mem_all - mem_used;
  int cpu_used = get_cpu();
  gchar *memory_label = g_strdup_printf(" mem: %.1f GB  of %.1f GB", mem_used, mem_all);
  gchar *free_memory_label = g_strdup_printf("free: %.1f GB of %.1f GB", mem_free, mem_all);
  gchar *cpu_label = g_strdup_printf("cpu: %d%%", cpu_used);
  app_indicator_set_label(indicator, memory_label, memory_label);
  gtk_menu_item_set_label(GTK_MENU_ITEM(free_memory_item), free_memory_label);
  gtk_menu_item_set_label(GTK_MENU_ITEM(cpu_item), cpu_label);
  g_free(memory_label), g_free(cpu_label), g_free(free_memory_label);
  return TRUE;
}

int main(int argc, char **argv){
  gtk_init(&argc, &argv);
  indicator_menu = gtk_menu_new();
  GtkWidget *sep = gtk_separator_menu_item_new();

  systemMonitor_item = gtk_menu_item_new_with_label("System Monitor");
  gtk_menu_shell_append(GTK_MENU_SHELL(indicator_menu), systemMonitor_item);
  g_signal_connect(systemMonitor_item, "activate", G_CALLBACK(system_monitor), NULL);
  
  indicator = app_indicator_new("sysload", "gtk-execute", APP_INDICATOR_CATEGORY_SYSTEM_SERVICES);
  gtk_menu_shell_append(GTK_MENU_SHELL(indicator_menu), sep);
    
  free_memory_item = gtk_menu_item_new_with_label("");
  gtk_menu_shell_append(GTK_MENU_SHELL(indicator_menu), free_memory_item);

  cpu_item = gtk_menu_item_new_with_label("");
  gtk_menu_shell_append(GTK_MENU_SHELL(indicator_menu), cpu_item);
  
  sep = gtk_separator_menu_item_new();
  gtk_menu_shell_append(GTK_MENU_SHELL(indicator_menu), sep);
  
  help_item = gtk_menu_item_new_with_label("Help");
  gtk_menu_shell_append(GTK_MENU_SHELL(indicator_menu), help_item);
  g_signal_connect(help_item, "activate", G_CALLBACK(report_window), NULL);

  quit_item = gtk_menu_item_new_with_label("Exit");
  gtk_menu_shell_append(GTK_MENU_SHELL(indicator_menu), quit_item);
  g_signal_connect(quit_item, "activate", G_CALLBACK(gtk_main_quit), NULL);
  
  gtk_widget_show_all(indicator_menu);
  app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
  app_indicator_set_label(indicator, "sysload", "sysload");
  app_indicator_set_menu(indicator, GTK_MENU(indicator_menu));

  update();
  g_timeout_add(1000 * period, (GtkFunction) update, NULL);
  
  gtk_main();
  return 0;
}
