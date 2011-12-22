#include "spectranet.h"

int
check_internet_connection(void)
{
  char *externhost = "74.125.236.81";
  char *externport ="80";
  char *request = "GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
  int sock;
  if( (sock = connect_to_server(externhost,externport)) < 0 ) {
    return FALSE;
  }
  if( send_data(sock,request,strlen(request)) < 0 ) {
	  close(sock);
	  return FALSE;
  }
  char match_string[13],ch;
  match_string[12] = '\0';
  int i,count =0;
  while( count < 500 ) {
	  count++;
	  i = 0;
	  if( read(sock,&ch,1) < 0 ) {
		  close(sock);
		  return FALSE;
	  }
	  while( i < 11 ) {
		  match_string[i] = match_string[i+1];
		  i++;
	  }
	  match_string[i] = ch;
	  //	  printf("%s\n",match_string);
	  if( ! strcmp(match_string,"203.122.18.8") ) {
	    close(sock);
	    return FALSE;
	  }
  }
  return TRUE;
}
void
connect_to_internet(char *username,char *password)
{
  fprintf(stderr,"Connecting to Internet\n");
  char request[500];
  /*
    1. Make a GET / to 1.254.254.254 as though to facebook
  */
  
  strcpy(request,"GET / HTTP/1.1\r\nHost: www.facebook.com\r\n\r\n");
  int sock;
  if( (sock = connect_to_server("1.254.254.254","80")) < 0 ) {
    return;
  }
  if( send_data(sock,request,strlen(request)) < 0 ) {
    return;
  }

  /*
    2. Take the part after "/userportal" in the META REFRESH tag in html.
  */
  char *match_string = "/userportal";
  int i=0,len = strlen(match_string);
  char ch,path[200];
  while( 1 ) {

    /* 
       Umm. I did some obfuscation :( This is rather a little complicated.
       First read one byte, if error just return.
    */
    if( read(sock,&ch,1) < 0 ) {  /* eh ? */
      close(sock);
      return;
    }

    if( i >= len ) {
      /* The substring "/userportal" has been matched. Just copy until you find a quote */
      if( ch == '\"' ) {
	close(sock);
	path[i] = '\0';
	break;
      }
      else {
	path[i++] = ch;
      }
    }
    else {
      /* Try and match substring "/userportal" */
      if( match_string[i] == ch )
	path[i++] = ch;
      else
	i = 0;
    }
  }

  /*
    3. Make a request to 203.92.63.70 with the path from the above URL
  */

  sprintf(request,"GET %s HTTP/1.1\r\nHost: 203.92.63.70\r\n\r\n",path);
  if( (sock = connect_to_server("203.92.63.70","80")) < 0 ) {
    return;
  }
  if( send_data(sock,request,strlen(request)) < 0 ) {
    return;
  }


  /*
    4. From the response extract the cookie for and use it for subsequent requests
  */
  char *resp,cookie[100],*start,*stop;
  strcpy(cookie,"");
  while( 1 ) {
    resp = recv_data(sock);

    if( (start = strstr(resp,"JSESSIONID=")) != NULL ) {
      i=0;
      stop = strstr(resp,";");
      if( stop == NULL ) { /* eh? */
        close(sock);
	return;
      }
      while(start != stop)
        cookie[i++] = *(start++);
      cookie[i] = '\0';
	  free(resp);
      resp = NULL;
      close(sock);
      break;
    }

    if(!strcmp(resp,"Content-Type: text/html;charset=ISO-8859-1\r\n") ) {
      free(resp);
      resp = NULL;
      close(sock);
      break;
    }
    
  }
  /*
    5. Now connect to 203.92.63.70 and POST to /userportal/login.do?requesturi=http%3A%2F%2Fwww.facebook.com%2F
  */
  strcpy(request,"");
  strcat(request,"POST /userportal/login.do?requesturi=http%3A%2F%2Fwww.facebook.com%2F HTTP/1.1\r\nHost: 203.92.63.70\r\nCookie: ");
  strcat(request,cookie);
  strcat(request,"\r\n\r\n");
  if( (sock = connect_to_server("203.92.63.70","80")) < 0 ) {
    return;
  }
  if( send_data(sock,request,strlen(request)) < 0 ) {
    return;
  }

  close(sock);

  /*
    6. Now connect to 203.92.63.70 and POST to /userportal/newlogin.do?phone=0 the data "type=2&username=<username>&password=<password>
  */
  int postlen = 26 + strlen(username) + strlen(password);
  char content_len[5];
  sprintf(content_len,"%d",postlen);
  strcpy(request,"");
  strcat(request,"POST /userportal/newlogin.do?phone=0 HTTP/1.1\r\nContent-Length: ");
  strcat(request,content_len);
  strcat(request,"\r\nHost: 203.92.63.70\r\nContent-Type: application/x-www-form-urlencoded\r\nReferer: http://203.92.63.70/userportal/login.do?requesturi=http%3A%2F%2Fwww.facebook.com%2F\r\nCookie: ");
  strcat(request,cookie);
  strcat(request,"\r\n\r\ntype=2&username=");
  strcat(request,username);
  strcat(request,"&password=");
  strcat(request,password);
  if( (sock = connect_to_server("203.92.63.70","80")) < 0 ) {
    return;
  }
  if( send_data(sock,request,strlen(request)) < 0 ) {
    return;
  }

  close(sock);

}
