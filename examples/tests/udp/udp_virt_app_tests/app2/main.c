#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <button.h>
#include <timer.h>

#include <udp.h>

#define DEBUG 0

static unsigned char BUF_BIND_CFG[2 * sizeof(sock_addr_t)];

void print_ipv6(ipv6_addr_t *);

int main(void) {

  printf("[App2] Starting App2 Test App.\n");

  static char packet[70];
  ipv6_addr_t dest_addr = {
    {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
     0x1c, 0x1d, 0x1e, 0x1f}
  };
  sock_addr_t destination = {
    dest_addr,
    16124
  };

  ipv6_addr_t ifaces[10];
  udp_list_ifaces(ifaces, 10);

  sock_handle_t handle;
  sock_addr_t addr = {
    ifaces[0],
    22222
  };

  int len = snprintf(packet, sizeof(packet), "Hello World - App2\n");
  ssize_t result = udp_send_to(packet, len, &destination);
  assert(result < 0); // should fail because we have not bound

  if (DEBUG) {
    printf("Opening socket on ");
    print_ipv6(&ifaces[0]);
    printf(" : %d\n", addr.port);
  }
  int bind_return = udp_bind(&handle, &addr, BUF_BIND_CFG);
  assert(bind_return >= 0); //bind should succeed

  if (bind_return < 0) {
    printf("Bind failed. Error code: %d\n", bind_return);
    return -1;
  }

  //bound, now try sending a too-long packet
  int max_len = 0;
  udp_get_max_tx_len(&max_len);
  result = udp_send_to(packet, max_len + 1, &destination);
  assert(result < 0); //should fail bc too long

  if (DEBUG) {
    printf("Sending packet (length %d) --> ", len);
    print_ipv6(&(destination.addr));
    printf(" : %d\n", destination.port);
  }

  // We want this app to attempt to bind to addr2 after app1 has made the same
  // attempt. However, udp_send can take more than 10ms for a multi-fragment packet,
  // so putting the delay after the send still makes it possible for this app to
  // bind first. Accordingly, put the delay before the send to ensure it sends second.
  delay_ms(10);
  result = udp_send_to(packet, len, &destination);
  assert(result == RETURNCODE_SUCCESS); //finally, a valid send attempt

  //of the two apps, app2 binds to port 80 second and should fail
  sock_addr_t addr2 = {
    ifaces[0],
    80
  };
  bind_return = udp_bind(&handle, &addr2, BUF_BIND_CFG);
  assert(bind_return < 0); //bind should fail bc this app binds second to port 80

  delay_ms(90); //to re-sync with other app
  sock_addr_t addr3 = {
    ifaces[0],
    81
  };
  bind_return = udp_bind(&handle, &addr3, BUF_BIND_CFG);
  assert(bind_return >= 0); //bind should succeed now


  result = udp_send_to(packet, len, &destination);
  assert(result == RETURNCODE_SUCCESS); // should succeed, both apps should be bound to different ports
  printf("App2 test success!\n");
}

void print_ipv6(ipv6_addr_t *ipv6_addr) {
  for (int j = 0; j < 14; j += 2) {
    printf("%02x%02x:", ipv6_addr->addr[j], ipv6_addr->addr[j + 1]);
  }
  printf("%02x%02x", ipv6_addr->addr[14], ipv6_addr->addr[15]);
}
