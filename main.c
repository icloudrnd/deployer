#include "deployer.h"



int main(int argc, char *argv[])
{
 int sz,try = 0;
 int err = 0;
 
 struct globalArgs *gArgs; //create new 
 gArgs = malloc(sizeof(struct globalArgs));
 
  // parse command line and get settings  
  preConfigure(argc, argv, gArgs);
  
  downloadData(gArgs);
 
  
 exit(0); 
}

//
//---------------------getConfig()-----------------------------------
//
size_t getConfig(char *ptr, size_t size, size_t nmemb, struct globalArgs *gArgs){
        
    char *p;
    size_t res;
    //char d[2] = ":";
    
    gArgs->errno = atoi(strtok(ptr, ":"));
    
    switch(gArgs->errno){
       case DP_OK:
             p = strtok(NULL, ":");
             gArgs->image = p;
             p = strtok(NULL, ":");
             gArgs->dest = p;                     
             break;
             
       case DP_ERR:
             return DP_ERR;   
             break;
       default:
          printf("Usage: creator -udc\n");
          exit(1);       
    }
 return size*nmemb;    
}
//
//------------------decompressData--------------------------------
//
size_t decompressData(char *ptr, size_t size, size_t nmemb, FILE *f_out){
    
    size_t res = BZ_OK;
    size_t w; 
    
    strm.next_in = ptr;
    strm.avail_in = size*nmemb;
    char *out = malloc(size*nmemb);
        
    while(strm.avail_in != 0){ 
        
       if( strm.avail_out != (size*nmemb)){
             strm.next_out = out; 
             strm.avail_out = size*nmemb;
       }
        
       res = BZ2_bzDecompress(&strm);
        
       if(res == BZ_OK || res == BZ_STREAM_END )
          if(strm.avail_out < (size*nmemb)){ // if avail_out has been deacriased, it means that data was written to buff 
              w = fwrite(out, size,(size*nmemb) - strm.avail_out, f_out);  
          }
    }
    
    free(out);   
    
 return size*nmemb;   
}


//------------------downloadData-----------------------------------
//
//
size_t downloadData(struct globalArgs *gArgs){

    size_t res;
    double dsize = 0.0;
    double dspeed = 0.0;    
 
    strm.opaque = NULL;
    strm.bzfree = NULL;
    strm.bzalloc = NULL;
    
    FILE *f_out;
    
    
    res = BZ2_bzDecompressInit(&strm,0,0);
    CURL *curl = curl_easy_init();
    
    if(curl){
        
       curl_easy_setopt(curl, CURLOPT_URL, "http://10.10.111.205/cgi-bin/deploycgi.py"); 
       curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "mac=fa:16:3e:5e:53:45&status=preparing");
       curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, getConfig);
       curl_easy_setopt(curl, CURLOPT_WRITEDATA, gArgs);
       res = curl_easy_perform(curl);  
        
       f_out  = fopen(gArgs->dest,"wb");
        
       curl_easy_setopt(curl, CURLOPT_URL, "http://10.10.111.205/cgi-bin/deploycgi.py"); 
       curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "mac=fa:16:3e:5e:53:45&status=imaging");
       res = curl_easy_perform(curl);  
       
       //curl_easy_setopt(curl, CURLOPT_URL, gArgs->url);   
       curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, decompressData);
      // curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "zlib");
       curl_easy_setopt(curl, CURLOPT_WRITEDATA, f_out);
       syslog(LOG_INFO, "Prepearing to download %s\n", gArgs->url);
       res = curl_easy_perform(curl);
       
       if(res != CURLE_OK){
              syslog(LOG_ERR,"Download fail: %s", curl_easy_strerror(res));
              exit(res);
       }
/*
       curl_easy_setopt(curl, CURLOPT_URL, "http://10.10.111.205/cgi-bin/deploycgi.py"); 
       curl_easy_setopt(curl, CURL_POSTFIELDS, "mac=fa:16:3e:5e:53:45&status=localboot");
       res = curl_easy_perform(curl);  
*/
       
       curl_easy_cleanup(curl); 
       curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &dsize);
       curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &dspeed);    
       
       syslog(LOG_INFO, "Total seize of download %i with speed %i\n", (int)dsize, (int)dspeed);
    }
    
   fclose(f_out);
   
 return DP_OK;
}

//
//--------------------getMAC()---------------------------------------
//
size_t getMAC(){
    //gets host mac
    
 return DP_OK;    
}



//---------------------preConfigure()---------------------------------
//
//
size_t preConfigure(int argc, char *argv[],struct globalArgs *gArgs){
 
 int p = 0;

  while((p = getopt(argc, argv, "u:d:c:")) != -1){

   switch(p){
     case'u':
        gArgs->url = optarg;
        break;
     case'd':
        gArgs->dest = optarg;
        break;
    case'c':
        gArgs->md5 = optarg;
        break;
     default:
          printf("Usage: creator -udc\n");
          exit(1);
   };
 };  

return 0;

}
