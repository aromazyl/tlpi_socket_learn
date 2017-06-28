#include <event2/event.h>
#include <stdio.h>

int replace_callback(struct event* ev,
    event_callback_fn new_callback,
    void *new_callback_arg
    ) {
  struct event_back *base;
  evutil_socket_t fd;
  short events;
  int pending;
  pending = event_pending(ev, EV_READ|EV_WRITE|EV_SIGNAL|EV_TIMEOUT, NULL);

  if (pending) {
    return -1;
  }
  event_get_assignment(ev, &base, &fd, &events, NULL, NULL);
  event_assign(ev, base, fd, events, new_callback, new_callback_arg);
  return 0;
}
