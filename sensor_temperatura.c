#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <mysql/mysql.h>

int main (void) {
 //DIR *dir;
 //struct dirent *dirent;
 //char dev[16];      // Dev ID
 //char path[] = "/sys/bus/w1/devices";
 char devPath[128]; // Path to device
 char buf[256];     // Data from device
 char tmpData[6];   // Temp C * 1000 reported by device
 
 ssize_t numRead;

/* detectar sensor
 dir = opendir (path);
 if (dir != NULL) {
  while ((dirent = readdir (dir)))
   // 1-wire devices are links beginning with 28-
   if (dirent->d_type == DT_LNK && strstr(dirent->d_name, "28-") != NULL) {
    strcpy(dev, dirent->d_name);
    printf("\nDispositivo detectado: %s\n", dev);
   }
        (void) closedir (dir);
        } else {
  perror ("problemas no sensor");
  return 1;
 }
*/


 // Assemble path to OneWire device
 //sprintf(devPath, "%s/%s/w1_slave", path, dev);
 
  sprintf(devPath, "/sys/bus/w1/devices/28-3c01e0765161/w1_slave");


//MYSQL

 MYSQL *con = mysql_init(NULL);
  
  if (con == NULL)
  {
  fprintf(stderr, "%s\n", mysql_error(con));
  exit(1);
  }
  
  if (mysql_real_connect(con, "localhost", "peter", "password", "temperatura", 0, NULL, 0) == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
    
  } else {
    
  
    
  fprintf(stderr, "connectado!");
  
  }
  






 
 // Read temp continuously
 // Opening the device's file triggers new reading
 while(1) {
  int fd = open(devPath, O_RDONLY);
  
  if(fd == -1) {
   perror ("erro ao abrir dispositivo w1.");
   return 1;
  }
  
  while((numRead = read(fd, buf, 256)) > 0) {
   strncpy(tmpData, strstr(buf, "t=") + 2, 5);
   float tempC = strtof(tmpData, NULL);
   //printf("Device: %s  - ", dev);
   printf("Temperatura: %.2f C  \n", tempC / 1000);
   // printf("%.3f F\n\n", (tempC / 1000) * 9 / 5 + 32);
  
    char sql[100];
    sprintf(sql,"INSERT INTO temperaturas (temp) VALUES (%.2f)", tempC / 1000);
    sleep(10);
  
  
    if (mysql_query(con, sql) != 0){
    fprintf(stderr,"query failure\n");
    return EXIT_FAILURE;
    }
  
  }
  close(fd);
 }
        /* return 0; --never called due to loop */
}
