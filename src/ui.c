#include "ui.h"
#include "bootctrl.h"
#include "ipc.h"
#include "utility.h"

const struct boot_control_module *ctl = &bootctl;

GtkWidget *lbl_curSlot;

void on_activate_slot(gpointer user_data)
{
    int id = GPOINTER_TO_INT(user_data);
    char *name = get_slot_name(id);

    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Are you sure you want to activate slot %s?", name);
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    if (response != GTK_RESPONSE_YES)
        return;

    char str[32];
    snprintf(str, sizeof(str), "Current Slot: <b>%s</b>", name);
    gtk_label_set_markup(GTK_LABEL(lbl_curSlot), str);

    char cmd[32];
    snprintf(cmd, sizeof(cmd), "activate %d", id);
    ipc_write(g_daemon_fd, cmd);

    int res = -1;
    ipc_recv_int(g_daemon_fd, &res);
    if (res < 0)
    {
        dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Failed to activate slot %s", name);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "Do you want to restart now?");
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    if (response == GTK_RESPONSE_YES)
    {
        ipc_write(g_daemon_fd, "reboot");
    }
}

void build_device_info(GtkWidget *container, int curSlot)
{
    // Device Detail
    GtkWidget *card = gtk_frame_new(NULL);
    gtk_box_pack_start(GTK_BOX(container), card, FALSE, FALSE, 0);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_add(GTK_CONTAINER(card), box);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);

    GtkWidget *lbl_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lbl_title), "<span font_size='large'>Device</span>");
    gtk_label_set_xalign(GTK_LABEL(lbl_title), 0.0f);
    gtk_widget_set_margin_bottom(lbl_title, 10);
    gtk_box_pack_start(GTK_BOX(box), lbl_title, FALSE, FALSE, 0);

    lbl_curSlot = gtk_label_new(NULL);
    char m_curSlotStr[32];
    snprintf(m_curSlotStr, sizeof(m_curSlotStr), "Current Slot: <b>%s</b>", get_slot_name(curSlot));
    gtk_label_set_markup(GTK_LABEL(lbl_curSlot), m_curSlotStr);
    gtk_label_set_xalign(GTK_LABEL(lbl_curSlot), 0.0f);
    gtk_box_pack_start(GTK_BOX(box), lbl_curSlot, FALSE, FALSE, 0);
}

void build_slot_card(GtkWidget *container, int id, struct slot_info *slot)
{
    GtkWidget *card = gtk_frame_new(NULL);
    gtk_box_pack_start(GTK_BOX(container), card, FALSE, FALSE, 0);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_container_add(GTK_CONTAINER(card), box);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);

    GtkWidget *lbl_title = gtk_label_new(NULL);
    char m_title[64];
    snprintf(m_title, sizeof(m_title), "<span font_size='large'>Slot %s</span>", get_slot_name(id));
    gtk_label_set_markup(GTK_LABEL(lbl_title), m_title);
    gtk_label_set_xalign(GTK_LABEL(lbl_title), 0.0f);
    gtk_widget_set_margin_bottom(lbl_title, 10);
    gtk_box_pack_start(GTK_BOX(box), lbl_title, FALSE, FALSE, 0);

    GtkWidget *lbl_bootable = gtk_label_new(NULL);
    char m_bootableStr[128];
    const char *m_bootableValue = slot->bootable ? "<span foreground='green'>Yes</span>" : "<span foreground='red'>No</span>";
    snprintf(m_bootableStr, sizeof(m_bootableStr), "Bootable: <b>%s</b>", m_bootableValue);
    gtk_label_set_markup(GTK_LABEL(lbl_bootable), m_bootableStr);
    gtk_label_set_xalign(GTK_LABEL(lbl_bootable), 0.0f);
    gtk_box_pack_start(GTK_BOX(box), lbl_bootable, FALSE, FALSE, 0);

    GtkWidget *lbl_successful = gtk_label_new(NULL);
    char m_successfulStr[128];
    const char *m_successfulValue = slot->successful ? "<span foreground='green'>Yes</span>" : "<span foreground='red'>No</span>";
    snprintf(m_successfulStr, sizeof(m_successfulStr), "Successful: <b>%s</b>", m_successfulValue);
    gtk_label_set_markup(GTK_LABEL(lbl_successful), m_successfulStr);
    gtk_label_set_xalign(GTK_LABEL(lbl_successful), 0.0f);
    gtk_box_pack_start(GTK_BOX(box), lbl_successful, FALSE, FALSE, 0);

    GtkWidget *lbl_active = gtk_label_new(NULL);
    char m_activeStr[128];
    const char *m_activeValue = slot->active ? "<span foreground='green'>Yes</span>" : "<span foreground='red'>No</span>";
    snprintf(m_activeStr, sizeof(m_activeStr), "Active: <b>%s</b>", m_activeValue);
    gtk_label_set_markup(GTK_LABEL(lbl_active), m_activeStr);
    gtk_label_set_xalign(GTK_LABEL(lbl_active), 0.0f);
    gtk_box_pack_start(GTK_BOX(box), lbl_active, FALSE, FALSE, 0);

    GtkWidget *btn_activate = gtk_button_new_with_label("Activate");
    g_signal_connect_swapped(btn_activate, "clicked", G_CALLBACK(on_activate_slot), GINT_TO_POINTER(id));
    gtk_widget_set_margin_top(btn_activate, 16);
    gtk_box_pack_start(GTK_BOX(box), btn_activate, FALSE, FALSE, 0);
}

void build_main_window(GtkApplication *app, gpointer user_data)
{

    // Get current slot data
    ipc_write(g_daemon_fd, "get_slots");
    struct slot_info slots[2] = {{0}};
    ipc_recv_struct(g_daemon_fd, &slots, sizeof(slots));

    int m_curSlot = -1;
    for (int i = 0; i < 2; i++)
    {
        m_curSlot = slots[i].active ? i : m_curSlot;
    }

    // Main window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 16);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Title
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span font_size='xx-large'>Boot Control</span>");
    gtk_widget_set_margin_top(title, 16);
    gtk_widget_set_margin_bottom(title, 16);
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);

    // Content
    build_device_info(vbox, m_curSlot);
    for (int i = 0; i < 2; i++)
    {
        build_slot_card(vbox, i, &slots[i]);
    }

    gtk_widget_show_all(window);
    gtk_main();
}

int init_ui(int argc, char *argv[])
{
    GtkApplication *app;
    int status;

    printf("Initializing UI\n");

    app = gtk_application_new("sh.rul.qbootctl-gtk", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(build_main_window), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return 0;
}