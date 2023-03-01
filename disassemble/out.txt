#include <stdio.h>
#include <stdlib.h>

int main(){
    char buf[1024];

    FILE* out=fopen("out.txt","w+");
    if(out==NULL){
        fprintf(stderr,"open failed. \n");
        exit(-1);
    }
    while(fgets(buf,sizeof(buf),stdin)!=NULL){
        printf("%s",buf);
        fputs(buf,out);
    }

    fclose(out);


    return 0;
}
