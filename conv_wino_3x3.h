#include "mat.h"
#include <stdio.h>
static void conv_winograd_kernel(const Mat kernel, Mat& kernel_tm, int inch, int outch)
{
    kernel_tm.create(8*8, inch, outch);

    const float ktm[8][3] = {
        {   1.0f,     0.0f,     0.0f},
        {-2.0f/9,  -2.0f/9,  -2.0f/9},
        {-2.0f/9,   2.0f/9,  -2.0f/9},
        {1.0f/90,  1.0f/45,  2.0f/45},
        {1.0f/90, -1.0f/45,  2.0f/45},
        {1.0f/45,  1.0f/90, 1.0f/180},
        {1.0f/45, -1.0f/90, 1.0f/180},
        {   0.0f,     0.0f,     1.0f}
    };
    for (int p = 0; p<outch; p++)
    {
        for (int q = 0; q<inch; q++)
        {
//            const float* kernel0 = (const float*)kernel + p*inch * 9 + q * 9;
            const float* kernel0 = kernel.data + p*inch*9 + q * 9;
            float* kernel_tm0 = kernel_tm.channel(p).row(q);

            const float* k0 = kernel0;
            const float* k1 = kernel0 + 3;
            const float* k2 = kernel0 + 6;

            // h
            float tmp[8][3];
            for (int i=0; i<8; i++)
            {
                tmp[i][0] = k0[0] * ktm[i][0] + k0[1] * ktm[i][1] + k0[2] * ktm[i][2];
                tmp[i][1] = k1[0] * ktm[i][0] + k1[1] * ktm[i][1] + k1[2] * ktm[i][2];
                tmp[i][2] = k2[0] * ktm[i][0] + k2[1] * ktm[i][1] + k2[2] * ktm[i][2];
            }

            // v
            for (int j=0; j<8; j++)
            {
                float* tmpp = &tmp[j][0];

                for (int i=0; i<8; i++)
                {
                    kernel_tm0[j*8 + i] = tmpp[0] * ktm[i][0] + tmpp[1] * ktm[i][1] + tmpp[2] * ktm[i][2];
                }
            }
        }
    }



}


static void conv_winograd_comp(const Mat& bottom_blob, Mat& top_blob, const Mat& kernel_tm)
{

    int w = bottom_blob.w;
    int h = bottom_blob.h;
    int inch = bottom_blob.c;

    int outw = top_blob.w;
    int outh = top_blob.h;
    int outch = top_blob.c;


//    outw = (outw + 5) / 6 * 6;
//    outh = (outh + 5) / 6 * 6;

//    w = outw + 2;
//    h = outh + 2;
//    copy_make_border(bottom_blob, bottom_blob_bordered, 0, h - bottom_blob.h, 0, w - bottom_blob.w, 0, 0.f);


    Mat bottom_blob_tm;
    
    int w_til = outw / 6;
    int h_til = outh / 6;
    const int tiles = w_til * h_til;
    bottom_blob_tm.create(64, tiles, inch);
//    bottom_blob_tm.create(tiles, 64, inch);



        
    for (int q = 0; q<inch; q++)
    {
        Mat img0 = bottom_blob.channel(q);
        Mat img0_tm = bottom_blob_tm.channel(q);

        float tmp[8][8];

            // tile
            for (int i=0; i<h_til; i++)
            {
                for (int j=0; j<w_til; j++)
                {


                    float* r0 = img0.row(i * 6) + j * 6;
                    for (int m=0; m<8; m++)
                    {
                        tmp[0][m] = r0[0] - r0[6] + (r0[4] - r0[2]) * 5.25f;
                        tmp[7][m] = r0[7] - r0[1] + (r0[3] - r0[5]) * 5.25f;

                        float tmp12a = (r0[2] + r0[6] - r0[4] * 4.25f);
                        float tmp12b = (r0[1] + r0[5] - r0[3] * 4.25f);

                        tmp[1][m] = tmp12a + tmp12b;
                        tmp[2][m] = tmp12a - tmp12b;

                        float tmp34a = (r0[6] + r0[2] * 0.25f - r0[4] * 1.25f);
                        float tmp34b = (r0[1] * 0.5f - r0[3] * 2.5f + r0[5] * 2.f);

                        tmp[3][m] = tmp34a + tmp34b;
                        tmp[4][m] = tmp34a - tmp34b;

                        float tmp56a = (r0[6] + (r0[2] - r0[4] * 1.25f) * 4.f);
                        float tmp56b = (r0[1] * 2.f - r0[3] * 2.5f + r0[5] * 0.5f);

                        tmp[5][m] = tmp56a + tmp56b;
                        tmp[6][m] = tmp56a - tmp56b;

                        r0 += w;
                    }
 
                    float* b0_tm = img0_tm.row(i * w_til + j);


                    for (int m=0; m<8; m++)
                    {
                        const float* tmp0 = tmp[m];

                        b0_tm[0] = tmp0[0] - tmp0[6] + (tmp0[4] - tmp0[2]) * 5.25f;
                        b0_tm[7] = tmp0[7] - tmp0[1] + (tmp0[3] - tmp0[5]) * 5.25f;

                        float tmp12a = (tmp0[2] + tmp0[6] - tmp0[4] * 4.25f);
                        float tmp12b = (tmp0[1] - tmp0[3] * 4.25f + tmp0[5]);

                        b0_tm[1] = tmp12a + tmp12b;
                        b0_tm[2] = tmp12a - tmp12b;

                        float tmp34a = (tmp0[6] + tmp0[2] * 0.25f - tmp0[4] * 1.25f);
                        float tmp34b = (tmp0[1] * 0.5f - tmp0[3] * 2.5f + tmp0[5] * 2.f);

                        b0_tm[3] = tmp34a + tmp34b;
                        b0_tm[4] = tmp34a - tmp34b;

                        float tmp56a = (tmp0[6] + (tmp0[2] - tmp0[4] * 1.25f) * 4.f);
                        float tmp56b = (tmp0[1] * 2.f - tmp0[3] * 2.5f + tmp0[5] * 0.5f);

                        b0_tm[5] = tmp56a + tmp56b;
                        b0_tm[6] = tmp56a - tmp56b;
                        b0_tm += 8;
                    }

                }
            }
        }
    // END transform input

    // BEGIN dot
    Mat top_blob_tm;
    top_blob_tm.create(64, tiles, outch);



    for (int p=0; p<outch; p++)
    {
        Mat out_tm = top_blob_tm.channel(p);
        Mat kernel_tm_0 = kernel_tm.channel(p); 

        for(int q=0; q<inch; q++)
        {
            float* kernel_0 = kernel_tm_0.row(q);
            Mat bottom_tm = bottom_blob_tm.channel(q);

            for(int i=0; i<tiles; i++)
            {
                float* bottom_tm_ti = bottom_tm.row(i);
                float* out0 = out_tm.row(i);

                for(int j=0; j<64; j++)
                {
                    out0[j] += bottom_tm_ti[j] * kernel_0[j];
			
                }

            }
        }
    }
    /*
       	float* res = top_blob_tm.data;
	
		for(int i=0; i<2; i++)
		{
			for(int j=0; j<tiles; j++)
			{
                for(int m=0; m<64; m++)
                {
                    printf("%f\t",res[i*tiles*64+j*64+m]);
                }printf("\n");
 				
 			}
 			printf("\n");	
		} 
    */


    // BEGIN transform output
//         const float otm[6][8] = {
//             {1.0f,  1.0f,   1.0f,   1.0f,   1.0f,  32.0f, 32.0f, 0.0f},
//             {0.0f,  1.0f,  -1.0f,   2.0f,  -2.0f,  16.0f,-16.0f, 0.0f},
//             {0.0f,  1.0f,   1.0f,   4.0f,   4.0f,   8.0f,  8.0f, 0.0f},
//             {0.0f,  1.0f,  -1.0f,   8.0f,  -8.0f,   4.0f, -4.0f, 0.0f},
//             {0.0f,  1.0f,   1.0f,  16.0f,  16.0f,   2.0f,  2.0f, 0.0f},
//             {0.0f,  1.0f,  -1.0f,  32.0f, -32.0f,   1.0f, -1.0f, 1.0f}
//         };

        // 0 = r0 + (r1 + r2) + (r3 + r4)     + (r5 + r6) * 32
        // 1 =      (r1 - r2) + (r3 - r4) * 2 + (r5 - r6) * 16
        // 2 =      (r1 + r2) + (r3 + r4) * 4 + (r5 + r6) * 8
        // 3 =      (r1 - r2) + (r3 - r4) * 8 + (r5 - r6) * 4
        // 4 =      (r1 + r2) + (r3 + r4) * 16+ (r5 + r6) * 2
        // 5 = r7 + (r1 - r2) + (r3 - r4) * 32+ (r5 - r6)




        for (int p = 0; p<outch; p++)
        {
            Mat out0_tm = top_blob_tm.channel(p);
            Mat out = top_blob.channel(p);

            float tmp[6][8];

            // tile
            
            for(int i=0; i<tiles; i++)
            {
                float* output_tm_0 = out0_tm.row(i);
                float* output_tm_1 = (float*)output_tm_0 + 8;
                float* output_tm_2 = (float*)output_tm_1 + 8;
                float* output_tm_3 = (float*)output_tm_2 + 8;
                float* output_tm_4 = (float*)output_tm_3 + 8;
                float* output_tm_5 = (float*)output_tm_4 + 8;
                float* output_tm_6 = (float*)output_tm_5 + 8;
                float* output_tm_7 = (float*)output_tm_6 + 8;
                for(int j=0; j<8; j++)
                {
        // 0 = r0 + (r1 + r2) + (r3 + r4)     + (r5 + r6) * 32
        // 1 =      (r1 - r2) + (r3 - r4) * 2 + (r5 - r6) * 16
        // 2 =      (r1 + r2) + (r3 + r4) * 4 + (r5 + r6) * 8
        // 3 =      (r1 - r2) + (r3 - r4) * 8 + (r5 - r6) * 4
        // 4 =      (r1 + r2) + (r3 + r4) * 16+ (r5 + r6) * 2
        // 5 = r7 + (r1 - r2) + (r3 - r4) * 32+ (r5 - r6)
                    float tmp024a = output_tm_1[0] + output_tm_2[0];
                    float tmp135a = output_tm_1[0] - output_tm_2[0];

                    float tmp024b = output_tm_3[0] + output_tm_4[0];
                    float tmp135b = output_tm_3[0] - output_tm_4[0];

                    float tmp024c = output_tm_5[0] + output_tm_6[0];
                    float tmp135c = output_tm_5[0] - output_tm_6[0];

                    tmp[0][j] = output_tm_0[0] + tmp024a + tmp024b + tmp024c * 32;
                    tmp[2][j] = tmp024a + tmp024b * 4 + tmp024c * 8;
                    tmp[4][j] = tmp024a + tmp024b * 16 + tmp024c + tmp024c;

                    tmp[1][j] = tmp135a + tmp135b + tmp135b + tmp135c * 16;
                    tmp[3][j] = tmp135a + tmp135b * 8 + tmp135c * 4;
                    tmp[5][j] = output_tm_7[0] + tmp135a + tmp135b * 32 + tmp135c;

                    output_tm_0 ++;
                    output_tm_1 ++;
                    output_tm_2 ++;
                    output_tm_3 ++;
                    output_tm_4 ++;
                    output_tm_5 ++;
                    output_tm_6 ++;
                    output_tm_7 ++;
                }

                int ro = i / w_til;
                int cl = i % w_til;
//                printf("tiles=%d\tw_til=%d\tro=%d\tcl=%d\n",tiles,w_til,ro,cl);
                float* output0 = out.row(ro*6) + 6*cl;
//                output0 +=  6*cl;

                for (int m=0; m<6; m++)
                {
                    float* tmp0 = tmp[m];
                
                    float tmp024a = tmp0[1] + tmp0[2];
                    float tmp135a = tmp0[1] - tmp0[2];

                    float tmp024b = tmp0[3] + tmp0[4];
                    float tmp135b = tmp0[3] - tmp0[4];

                    float tmp024c = tmp0[5] + tmp0[6];
                    float tmp135c = tmp0[5] - tmp0[6];

                    output0[0] =  tmp0[0] + tmp024a + tmp024b + tmp024c * 32;
                    output0[2] =  tmp024a + tmp024b * 4 + tmp024c * 8;
                    output0[4] =  tmp024a + tmp024b * 16 + tmp024c + tmp024c;

                    output0[1] =  tmp135a + tmp135b + tmp135b + tmp135c * 16;
                    output0[3] =  tmp135a + tmp135b * 8 + tmp135c * 4;
                    output0[5] =  tmp0[7] + tmp135a + tmp135b * 32 + tmp135c;
/*
                    if(i==0 && p==0){
                        for(int kk=0; kk<outh; kk++)
                        {
                            for(int lll=0; lll<outw; lll++){
                                printf("%f\t",out.data[kk*outw+lll]);
                            }printf("\n");
                                               
                        }
                    }
                    */
                    
/*                    
                    if(i==0 && p==0){
                    
                        for(int kk=0; kk<6; kk++)
                        {
                            printf("%f\t",(out.row(ro*6) + 6*cl)[kk]);                   
                        }printf("\n");
//                    printf("%p\t%p\n",output0,out.row(ro*6) + 6*cl);
                    }*/

                    output0 += outw;
                }
            }
        }
/*
   	float* res = top_blob.data;
	
		for(int i=0; i<24; i++)
		{
			for(int j=0; j<24; j++)
			{
 				printf("%f\t",res[i*24+j]);
 			}
 			printf("\n");	
		} 
*/

}
