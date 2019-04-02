#include <lo/lo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <signal.h>


static int done = 0;


void error(int num, const char *m, const char *path);

int command_handler(const char *path, const char *types, lo_arg ** argv,
                      int argc, void *data, void *user_data);

typedef void(*ReadHandler)(const unsigned char const*, const int);
extern int init(const char portname[], ReadHandler func);
extern int writeToSerial(const char * const toWrite);
extern int close_mcb();
static lo_address reader = NULL;

void quit_handler(int signum)
{
  done = 1;
  printf("quiting\n\n");
  close_mcb();
  fflush(stdout);
}

void handle_read(const unsigned char const* readString, const int rdlen) {
  printf("Read %d: \"%s\"\n", rdlen, readString);

  if(reader != NULL) {
    if (lo_send(reader, "/cnt/readback", "if", 1, 4.0) == -1) {
      printf("OSC error %d: %s\n", lo_address_errno(reader),
             lo_address_errstr(reader));
    }
  }
}

void start_read_handler() {
  init("/dev/ttyS4", &handle_read);
  sleep(5);
}

int register_handler(const char *path, const char *types, lo_arg ** argv,
                      int argc, void *data, void *user_data) {
  printf("Entering register handler\n");
  if(reader == NULL) {
    lo_message m = (lo_message)data;
    lo_address a = lo_message_get_source(m);
    const char *host = lo_address_get_hostname(a);

    printf("Register host: %s\n", host);
    fflush(stdout);
    reader = lo_address_new(host, "7771");

  }
  return 0;
}


int main()
{
  /* start a new server on port 7770 */
  lo_server_thread st = lo_server_thread_new("7770", error);

  /* add method that will match any path and args */
  //lo_server_thread_add_method(st, NULL, NULL, generic_handler, NULL);

  /* add method that will match the path /foo/bar, with two numbers, coerced
   * to float and int */
  lo_server_thread_add_method(st, "/mc/command", "if", command_handler, NULL);
  lo_server_thread_add_method(st, "/mc/register_reader", "i", register_handler, NULL);

  /* /\* add method that will match the path /blobtest with one blob arg *\/ */
  /* lo_server_thread_add_method(st, "/blobtest", "b", blobtest_handler, NULL); */

  /* /\* add method that will match the path /quit with no args *\/ */
  /* lo_server_thread_add_method(st, "/quit", "", quit_handler, NULL); */

  signal(SIGINT, quit_handler);
  start_read_handler();
  lo_server_thread_start(st);

  while (!done) {
#ifdef WIN32
    Sleep(1);
#else
    usleep(1000);
#endif
  }

  lo_server_thread_free(st);

  return 0;
}

void error(int num, const char *msg, const char *path)
{
  printf("liblo server error %d in path %s: %s\n", num, path, msg);
  fflush(stdout);
}


int command_handler(const char *path, const char *types, lo_arg ** argv,
                    int argc, void *data, void *user_data)
{
  char buffer[1000];

  sprintf(buffer, "%d\n", (int)argv[1]->f);
  writeToSerial(buffer);
  /* example showing pulling the argument values out of the argv array */
  printf("%s -> i:%d, f:%f\n\n", path, argv[0]->i, argv[1]->f);
  fflush(stdout);

  return 0;
}
