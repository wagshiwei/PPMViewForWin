

#include <stdio.h>
#include <stdlib.h>
void *load_ppm(char *filename,int *w,int *h){
     
    FILE* fp = NULL;
    int line = 0;
	// 读取图片格式(例如:"P6")
   // 高宽在第二行非注释数据
   // getc()
	int len;
	int stat = 0;
	int escape = 0;
	int ptype = 0;
	int max = 0;
	int byteNum = 1;


	int numbreak = 0;

	int wread = 0;
	int hread = 0;
	int maxread = 0;
	unsigned char *pixels = NULL;
	unsigned char *p = NULL;
	int rgbcout = 0;
	char header[1024];

    fp = fopen(filename, "rb");

	*w = 0;
	*h = 0;
   
	int bytesCount=0;
	int isReading = 0;
    // p1\n#\n w h\n 1||0 (pix 1 有换行符) ASCII   p4 binary 无换行
    // p2\n#\n w h\n max  (pix 1 有换行符) ASCII gray p5 binary 无换行
    // p3\n#\n w h\n max   (pix 3 有换行符 RGB) ASCII     p6 binary 无换行
    while((len = fread(header, 1,1024,fp))>0){
        // fgets(header, 1024, fp);
        for(int i=0;i<len;i++){
            if(escape){
               if(header[i]!="\n"){
                  continue;

               }else{
                   escape=0;
               }

            }
            switch(stat){
                case 0:
                    if(header[i]=='#'){
                        escape = 1;
                        
                    }else if(header[i]=='p'|| header[i] == 'P'){
                        i++;
                        switch(header[i]){
                            case '1':
                                
                            case '2':
                                
                            case '3':
                                 
                            case '4':
                                   
                            case '5':
                                 
                            case '6': 
                                 
                              ptype = header[i]-'0';
                              stat++;
                            break;
                            default:
								fclose(fp);
                            return NULL;  

                        }
                         if(ptype==3||ptype==6){
                          byteNum = 3;

                         }else{
                          byteNum = 1;
                          
                          }
                    }else{

                    }

                    break;
                case 1:
                    
                    if(header[i]=='#'){
                        escape = 1;
                        
                    }else if(header[i]>='0'&&header[i]<='9'){
                          if(numbreak){
                            
                             if(*h<(0xffff)){
                                *h=(*h)*10+(header[i]-'0');
                              }
                             hread = 1;
                          }else{
                              if(*w<(0xffff)){
                                *w=(*w)*10+(header[i]-'0');
                              }
                             wread = 1;
                          }

                    }else {
                        if(wread){
                             numbreak = 1;
                        }
                        if(hread&&header[i]=='\n'){
                            if(ptype==1||ptype==4){
                              stat+=2;
                                max = 1;
                            }
                            stat++;
                            /*pixels=malloc((*w)*(*h)*4+10);
							*/
							pixels=(unsigned char*)calloc(sizeof(char), (*w)*(*h) * 4 + 10);
							memset(pixels, 0, (*w)*(*h) * 4);
							p=pixels;
                        }

                    }
                        break;
                case 2:
                    if(ptype==1||ptype==4){
                       stat++;
                       max = 1;
                       break;
                    }
                    if(header[i]=='#'){
                        escape = 1;
                        
                    }else if(header[i]>='0'&&header[i]<='9'){
                        if(max<(0xffff)){
                           max=max*10+(header[i]-'0');
                        }
                          maxread = 1;
                    }else{
                       if(maxread&&header[i]=='\n'){
                            stat++;
                       }
                    }
                    break;
                case 3:
                     if(ptype<4){
                         if(header[i]>='0'&&header[i]<='9'){
                              *(p)=*(p)*10+(unsigned char)(((header[i]-'0')/(float)max)*255);
							  isReading = 1;
                         }else{

							 if (isReading) {
								 isReading = 0;
							 }
							 else {
								 break;
							 }
							 
							 if (p >= (unsigned char*)pixels + *w*(*h) * 4) {
								 break;
							 }
							 if (p-3< pixels) {
								 printf("pppppppppppppppp");
							 }

							 rgbcout = (rgbcout + 1) % 3; //R1 G2 B0 R1
                             if(byteNum== 1){
                                /* *(p+1) = *(p);
                                 *(p+2) = *(p);
                                 *(p+3) = (unsigned char)255;*/
                                 p=(p)+3;
                             }else{
                                  p=(p)+1;
                                  if(rgbcout==0){
									  if (p - 3 < pixels) {
										  printf("pppppppppppppppp");
									  }
									  //unsigned char tmp = 0;
									  
									 // *(p) = *(p - 3);
									  //*(p - 3) = (unsigned char)255;

									 // tmp = *(p - 1);
									 // *(p - 1) = *(p - 2);
									 // *(p - 2) = tmp;

                                      p=(p)+1;
									 
                                  }
                             }
                             
                         }

                     }else{
                         *((unsigned char*)p)=*((unsigned char*) p)*10+(unsigned char)(((header[i])/(float)max*255));
						 rgbcout = (rgbcout + 1) % 3;
                        if(byteNum== 1){
                            *(p+1) = *(p);
                                 *(p+2) = *(p);
                                 *(p+3) = (unsigned char)255;
                                 p=(p)+3;
                        }else{
							if (p >= (unsigned char*)pixels + *w*(*h) * 4) {
								break;
							}
                                  p=(p)+1;
                                  if(rgbcout==0){
                                     
									  //unsigned char tmp = 0;
									 
									  /**(p) = *(p - 3);
									  *(p - 3) = (unsigned char)255;

									  tmp = *(p - 1);
									  *(p - 1) = *(p - 2);
									  *(p - 2) = tmp;*/

                                    p=(p)+1;

                                  }
								  
                        }
                     }
                    break;
                case 5:
                    
                    break;

            }

        }
        // if(header[0] != '#'){
        //     ++line;
        // }
    }
    // 读取宽高
    // sscanf(header,"%d %d\n", w, h);

    // 获取最大像素值
    // fgets(header, 20, fp);

    // get rgb data
    // fread(*data, (*w)*(*h)*3, 1, fp);

    fclose(fp);
    
    return pixels;
}