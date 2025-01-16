#ifndef IPC_H
#define IPC_H

bool lua_ipc_init(lua_State *L);
void dwl_ipc_send_client_opened_event(Client *b);
void dwl_ipc_send_client_closed_event(Client *b);
void dwl_ipc_send_client_title_changed_event(Client *b);
void dwl_ipc_send_client_state_changed_event(Client *b);
void dwl_ipc_send_frame_event(void);
void dwl_ipc_send_monitor_added_event(Monitor *m);
void dwl_ipc_send_monitor_removed_event(Monitor *m);

#endif
