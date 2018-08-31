
#include "./check/conv_3x3.h"
#include "conv_wino_3x3.h"
//#include <stdio.h>
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
    	Mat top_blob_n(24,24,2,b);
    	Mat top_blob(24,24,2,b);
    	Mat _kernel(3,3,2,ker);
	Mat kernel_tm;
//	conv3x3s1_sse(bottom_blob,top_blob_n,_kernel);
	conv_winograd_kernel(_kernel, kernel_tm, 2, 2);
//	conv_winograd_comp(bottom_blob, top_blob, kernel_tm);
	float* res1 = top_blob_n.data;
	float* res2 = top_blob.data;
/*	
	for(int i=0; i<2; i++)
	{
		for(int j=0; j<24; j++)
		{
			for(int k=0; k<24; k++)
			{
				if((res1[i*24*24+j*24+k]-res2[i*24*24+j*24+k]) != 0)
				printf("%f\t",res1[i*24*24+j*24+k]-res2[i*24*24+j*24+k]);
			}
			printf("\n");	
		}printf("\n");
		
	}

*/
}


int main()
{
    Check_x86();
    return 0;
}

