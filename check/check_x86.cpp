

#include "conv_3x3.h"
#include <stdio.h>
void Check_x86()
{
/*
        float in[] = {
        0.0f, 1.0f, 2.0f, 3.0f, 4.0f,
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f,
        2.0f, 3.0f, 4.0f, 5.0f, 6.0f,
        3.0f, 4.0f, 5.0f, 6.0f, 7.0f,
        4.0f, 5.0f, 6.0f, 7.0f, 8.0f
    };
*/
	float in[1352];
	for(int i=0; i<1352; i++)
	{
		in[i] = (float)i;
	}


    // weights & bias
    float ker[] = {
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f
    };
    
    	float b[1152]={0.0f};

    	Mat bottom_blob(26,26,2,in);
    	Mat top_blob(24,24,2,b);
    	Mat _kernel(3,3,2,ker);
	conv3x3s1_sse(bottom_blob,top_blob,_kernel);
	float* res = top_blob.data;
	
	for(int i=0; i<2; i++)
	{
		for(int j=0; j<24; j++)
		{
			for(int k=0; k<24; k++)
			{
				printf("%f\t",res[i*24*24+j*24+k]);
			}
			printf("row%d%d\n",i,j);	
		}
		
	}


}


int main()
{
    Check_x86();
    return 0;
}

