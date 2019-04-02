#include <stdio.h>
#include <stdlib.h>
#include <lo/lo.h>

#define DISPLAY_STRING

static void printRead(const char const* readString, int rdlen) {
  //void printRead() {
#ifdef DISPLAY_STRING
  printf("Read %d: \"%s\"\n", rdlen, readString);
#else /* display hex */
  unsigned char   *p;
  printf("Read %d:", rdlen);
  for (p = buf; rdlen-- > 0; p++)
    printf(" 0x%x", *p);
  printf("\n");
#endif

}

int encoder_readback(const char *path, const char *types, lo_arg ** argv,
                     int argc, void *data, void *user_data) {
  printf("%s -> i:%d, f:%f\n\n", path, argv[0]->i, argv[1]->f);
  fflush(stdout);
}



/* int main(char *buff[]) { */
/*   init("/dev/ttyS4", &printRead); */
/*   sleep(5); */
/*   writeToSerial("40\n"); */
/*   sleep(5); */
/*   writeToSerial("20\n"); */
/*   sleep(5); */
/*   writeToSerial("0\n"); */
/*   sleep(5); */
/*   close_mcb(); */
/*   printf("Done closing down\n"); */
/*   return 0; */
/* } */

void error(int num, const char *msg, const char *path)
{
  printf("liblo server error %d in path %s: %s\n", num, path, msg);
  fflush(stdout);
}


const char testdata[6] = "ABCDE";

int main(int argc, char *argv[])
{
  lo_server_thread st = lo_server_thread_new("7771", error);
  lo_server_thread_add_method(st, "/cnt/readback", "if", encoder_readback, NULL);
  lo_server_thread_start(st);

  /* build a blob object from some data */
    //lo_blob btest = lo_blob_new(sizeof(testdata), testdata);

    /* an address to send messages to. sometimes it is better to let the server
     * pick a port number for you by passing NULL as the last argument */
//    lo_address t = lo_address_new_from_url( "osc.unix://localhost/tmp/mysocket" );
  lo_address t = lo_address_new(NULL, "7770");

  /* if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'q') { */
  /*     /\* send a message with no arguments to the path /quit *\/ */
  /*     if (lo_send(t, "/quit", NULL) == -1) { */
    /*         printf("OSC error %d: %s\n", lo_address_errno(t), */
    /*                lo_address_errstr(t)); */
    /*     } */
    /* } else { */
  /* send a message to /foo/bar with two float arguments, report any
         * errors */
    /* if (lo_send(t, "/mc/command", "if", 23, 0.12345678f) == -1) { */
    /*   printf("OSC error %d: %s\n", lo_address_errno(t), */
    /*          lo_address_errstr(t)); */
    /* } */
    sleep(5);
    if (lo_send(t, "/mc/register_reader", "i", 1) == -1) {
      printf("OSC error %d: %s\n", lo_address_errno(t),
             lo_address_errstr(t));
    }
    if (lo_send(t, "/mc/command", "if", 23, 40.0) == -1) {
      printf("OSC error %d: %s\n", lo_address_errno(t),
             lo_address_errstr(t));
    }
    sleep(5);
    if (lo_send(t, "/mc/command", "if", 23, 20.0) == -1) {
      printf("OSC error %d: %s\n", lo_address_errno(t),
             lo_address_errstr(t));
    }
    //    writeToSerial("20\n");
    sleep(5);
    if (lo_send(t, "/mc/command", "if", 23, 0.0) == -1) {
      printf("OSC error %d: %s\n", lo_address_errno(t),
             lo_address_errstr(t));
    }

    //writeToSerial("0\n");
    sleep(5);
    //close_mcb();
    printf("Done closing down\n");
    lo_server_thread_free(st);

        /* send a message to /a/b/c/d with a mixtrure of float and string
         * arguments */
        /* lo_send(t, "/a/b/c/d", "sfsff", "one", 0.12345678f, "three", */
        /*         -0.00000023001f, 1.0); */

        /* /\* send a 'blob' object to /a/b/c/d *\/ */
        /* lo_send(t, "/a/b/c/d", "b", btest); */

        /* /\* send a 'blob' object to /blobtest *\/ */
        /* lo_send(t, "/blobtest", "b", btest); */

        /* /\* send a jamin scene change instruction with a 32bit integer argument *\/ */
        /* lo_send(t, "/jamin/scene", "i", 2); */
    /* } */

    return 0;
}
