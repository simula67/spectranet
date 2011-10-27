#include <stdio.h>
#include <unistd.h>
#include "spectranet.h"


#define SLEEP_TIME 5

static char *username = "mnaufal";
static char *password = "123456";
int
main(int argc,char *argv)
{

  while( 1 ) {
    if( check_internet_connection() ) {
      fprintf(stderr,"Connection exists. Sleeping\n");
      sleep( SLEEP_TIME );
      continue;
    }
    else {
      connect_to_internet(username,password);
    }
  }
  fprintf(stderr,"Something wrong\n");
  fprintf(stderr,"Exited out of infinite loop\n");
  return 1;
}
