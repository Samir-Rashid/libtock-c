#include <stdio.h>
#include <libtock-sync/services/alarm.h>
#include <libtock/kernel/ipc.h>

size_t service_id = -1;

char buf[64] __attribute__((aligned(64)));

bool done = false;

static void ipc_callback(__attribute__ ((unused)) int   pid,
                         __attribute__ ((unused)) int   len,
                         __attribute__ ((unused)) int   arg2,
                         __attribute__ ((unused)) void* ud) {
  done = true;
}

static uint32_t get_time_ms(void) {
  struct timeval tv;
  libtock_alarm_gettimeasticks(&tv, NULL);
  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

int main(void) {
  int ret;

  ret = ipc_discover("ipc_server", &service_id);
  while (ret != RETURNCODE_SUCCESS) {
    printf("No server, retry in 1 second.\n");
    libtocksync_alarm_delay_ms(1000);
  }

  printf("Server discovered.\n");

  ipc_register_client_callback(service_id, ipc_callback, NULL);
  ipc_share(service_id, buf, 64);

  int num_pongs = 10000;
  // printf("Sed.\n");
  uint32_t start_time = get_time_ms();

  for (int i = 0; i < num_pongs; ++i) {
  // printf("Serd.\n");
    done = false;
    ipc_notify_service(service_id);
  // printf("Seasdfd.\n");
    yield_for(&done);
  // printf("Seasdfrd.\n");
  }

  uint32_t end_time = get_time_ms();

  printf("Ping-pong %d times take %lu ms.\n", num_pongs,  end_time - start_time);

  return 0;
}