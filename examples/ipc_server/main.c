#include <stdio.h>
#include <libtock/kernel/ipc.h>
#include <libtock/tock.h>

static void ipc_callback(
  int pid,
  __attribute__ ((unused)) int len,
  __attribute__ ((unused)) int buf,
  __attribute__ ((unused)) void* ud) {
  ipc_notify_client(pid);
}

int main(void) {
  int returncode = ipc_register_service_callback("ipc_server", ipc_callback, NULL);
  printf("ipc_register_service_callback returned %d\n", returncode);

// return 0;
  int scheduled_count = 0;
  while (1) {
    scheduled_count++;
    if (scheduled_count > 10000)
      printf("scheduled_count: %d\n", scheduled_count);
    // printf("%d\n", scheduled_count);
    yield();
  }
}