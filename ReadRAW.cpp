#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define IMAGE_WIDTH (5344)
#define IMAGE_HEIGHT (4016)

typedef struct
{ 
 	char bfType[2];//文件类型，必须是0x4d42，即字符串"BM"。
	long imageSize;//整个文件大小
	long blank;//保留字，为0
	long startPosition;//从文件头到实际的位图图像数据的偏移字节数。
}BmpHead;

/*********************
/*********************
第二部分    位图信息头
该结构的长度也是固定的，为40个字节，各个域的依次说明如下：
    4byte   ：本结构的长度，值为40
    4byte   ：图像的宽度是多少象素。
    4byte   ：图像的高度是多少象素。
    2Byte   ：必须是1。
    2Byte   ：表示颜色时用到的位数，常用的值为1(黑白二色图)、4(16色图)、8(256色图)、24(真彩色图)。
    4byte   ：指定位图是否压缩，有效值为BI_RGB，BI_RLE8，BI_RLE4，BI_BITFIELDS。Windows位图可采用RLE4和RLE8的压缩格式，BI_RGB表示不压缩。
    4byte   ：指定实际的位图图像数据占用的字节数，可用以下的公式计算出来：
     图像数据 = Width' * Height * 表示每个象素颜色占用的byte数(即颜色位数/8,24bit图为3，256色为1）
     要注意的是：上述公式中的biWidth'必须是4的整数倍(不是biWidth，而是大于或等于biWidth的最小4的整数倍)。
     如果biCompression为BI_RGB，则该项可能为0。
    4byte   ：目标设备的水平分辨率。
    4byte   ：目标设备的垂直分辨率。
    4byte   ：本图像实际用到的颜色数，如果该值为0，则用到的颜色数为2的(颜色位数)次幂,如颜色位数为8，2^8=256,即256色的位图
    4byte   ：指定本图像中重要的颜色数，如果该值为0，则认为所有的颜色都是重要的。
***********************************/
typedef struct
{
	long    Length;
	long    width;
	long    height;
	short    colorPlane;
	short    bitColor;
	long    zipFormat;
	long    realSize;
	long    xPels;
	long    yPels;
	long    colorUse;
	long    colorImportant;
}InfoHead;

//RGB2BMP
int saveRGBtoBMP(unsigned char *pRGB, unsigned long RGBWidth, unsigned long RGBHeight, unsigned char *pBMP)
{
	unsigned long RGBStride;
	unsigned long RGBSize;
	RGBStride = RGBWidth*3;
	RGBSize = RGBStride*RGBHeight;

	unsigned long RealRGBSize;
	RealRGBSize = (RGBStride+RGBWidth%4)*RGBHeight;
	unsigned char *pRealRGB = (unsigned char *)malloc(RealRGBSize);

	//RGB to BGR
	unsigned char tmp;
	for(unsigned long i=0; i<=RGBSize-3; i+=3)
	{
		tmp = pRGB[i];
		pRGB[i] = pRGB[i+2];
		pRGB[i+2] = tmp;
	}

	//line1 to line n
	unsigned char *pRealRGBIdx = pRealRGB;
	for(unsigned long i=1;i<=RGBHeight;i++)
	{
		memcpy(pRealRGBIdx,&pRGB[RGBSize-RGBStride*i],RGBStride);
		pRealRGBIdx += RGBStride;
		memset(pRealRGBIdx,0x00,RGBWidth%4);
		pRealRGBIdx += RGBWidth%4;
	}
	pRealRGBIdx = 0;
	
	BmpHead m_BMPHeader;
	m_BMPHeader.bfType[0]='B';
	m_BMPHeader.bfType[1]='M';
	m_BMPHeader.imageSize=RealRGBSize+0x36;
	m_BMPHeader.blank=0;
	m_BMPHeader.startPosition=0x36;

	InfoHead  m_BMPInfoHeader;
	m_BMPInfoHeader.Length=0x28; 
	m_BMPInfoHeader.width=RGBWidth;
	m_BMPInfoHeader.height=RGBHeight;
	m_BMPInfoHeader.colorPlane=1;
	m_BMPInfoHeader.bitColor=0x18;
	m_BMPInfoHeader.zipFormat=0;
	m_BMPInfoHeader.realSize=RealRGBSize;
	m_BMPInfoHeader.xPels=0;
	m_BMPInfoHeader.yPels=0;
	m_BMPInfoHeader.colorUse=0;
	m_BMPInfoHeader.colorImportant=0;

	unsigned char *pBMPIndex = pBMP;

	memcpy(pBMPIndex,m_BMPHeader.bfType,sizeof(m_BMPHeader.bfType));
	pBMPIndex += sizeof(m_BMPHeader.bfType);
	memcpy(pBMPIndex,&m_BMPHeader.imageSize,sizeof(m_BMPHeader.imageSize));
	pBMPIndex += sizeof(m_BMPHeader.imageSize);	
	memcpy(pBMPIndex,&m_BMPHeader.blank,sizeof(m_BMPHeader.blank));
	pBMPIndex += sizeof(m_BMPHeader.blank);	
	memcpy(pBMPIndex,&m_BMPHeader.startPosition,sizeof(m_BMPHeader.startPosition));
	pBMPIndex += sizeof(m_BMPHeader.startPosition);	
	
	memcpy(pBMPIndex,&m_BMPInfoHeader.Length,sizeof(m_BMPInfoHeader.Length));
	pBMPIndex += sizeof(m_BMPInfoHeader.Length);
	memcpy(pBMPIndex,&m_BMPInfoHeader.width,sizeof(m_BMPInfoHeader.width));
	pBMPIndex += sizeof(m_BMPInfoHeader.width);
	memcpy(pBMPIndex,&m_BMPInfoHeader.height,sizeof(m_BMPInfoHeader.height));
	pBMPIndex += sizeof(m_BMPInfoHeader.height);
	memcpy(pBMPIndex,&m_BMPInfoHeader.colorPlane,sizeof(m_BMPInfoHeader.colorPlane));
	pBMPIndex += sizeof(m_BMPInfoHeader.colorPlane);
	memcpy(pBMPIndex,&m_BMPInfoHeader.bitColor,sizeof(m_BMPInfoHeader.bitColor));
	pBMPIndex += sizeof(m_BMPInfoHeader.bitColor);
	memcpy(pBMPIndex,&m_BMPInfoHeader.zipFormat,sizeof(m_BMPInfoHeader.zipFormat));
	pBMPIndex += sizeof(m_BMPInfoHeader.zipFormat);
	memcpy(pBMPIndex,&m_BMPInfoHeader.realSize,sizeof(m_BMPInfoHeader.realSize));
	pBMPIndex += sizeof(m_BMPInfoHeader.realSize);
	memcpy(pBMPIndex,&m_BMPInfoHeader.xPels,sizeof(m_BMPInfoHeader.xPels));
	pBMPIndex += sizeof(m_BMPInfoHeader.xPels);
	memcpy(pBMPIndex,&m_BMPInfoHeader.yPels,sizeof(m_BMPInfoHeader.yPels));
	pBMPIndex += sizeof(m_BMPInfoHeader.yPels);
	memcpy(pBMPIndex,&m_BMPInfoHeader.colorUse,sizeof(m_BMPInfoHeader.colorUse));
	pBMPIndex += sizeof(m_BMPInfoHeader.colorUse);
	memcpy(pBMPIndex,&m_BMPInfoHeader.colorImportant,sizeof(m_BMPInfoHeader.colorImportant));
	pBMPIndex += sizeof(m_BMPInfoHeader.colorImportant);

	memcpy(pBMPIndex,pRealRGB,RealRGBSize);
	pBMPIndex =0;

	free(pRealRGB);
	return 0;
}

//RAW2RGB
int convertRAWtoRGB(unsigned short *pRAW, unsigned long RAWWidth, unsigned long RAWHeight, unsigned char *pRGB)
{
	#define WIDTH RAWWidth

	#define RAW_IDX(i,j) (i*WIDTH+j)

	#define M_R_IDX(i,j) ((i*WIDTH+j)*3+0)
	#define M_G_IDX(i,j) ((i*WIDTH+j)*3+1)
	#define M_B_IDX(i,j) ((i*WIDTH+j)*3+2)

	#define L_R_IDX(i,j) ((i*WIDTH+j-1)*3+0)
	#define L_G_IDX(i,j) ((i*WIDTH+j-1)*3+1)
	#define L_B_IDX(i,j) ((i*WIDTH+j-1)*3+2)

	#define U_R_IDX(i,j) (((i-1)*WIDTH+j)*3+0)
	#define U_G_IDX(i,j) (((i-1)*WIDTH+j)*3+1)
	#define U_B_IDX(i,j) (((i-1)*WIDTH+j)*3+2)

	#define R_R_IDX(i,j) ((i*WIDTH+j+1)*3+0)
	#define R_G_IDX(i,j) ((i*WIDTH+j+1)*3+1)
	#define R_B_IDX(i,j) ((i*WIDTH+j+1)*3+2)

	#define D_R_IDX(i,j) (((i+1)*WIDTH+j)*3+0)
	#define D_G_IDX(i,j) (((i+1)*WIDTH+j)*3+1)
	#define D_B_IDX(i,j) (((i+1)*WIDTH+j)*3+2)

	#define W_R_IDX(i,j) (((i-1)*WIDTH+j-1)*3+0)
	#define W_G_IDX(i,j) (((i-1)*WIDTH+j-1)*3+1)
	#define W_B_IDX(i,j) (((i-1)*WIDTH+j-1)*3+2)

	#define X_R_IDX(i,j) (((i-1)*WIDTH+j+1)*3+0)
	#define X_G_IDX(i,j) (((i-1)*WIDTH+j+1)*3+1)
	#define X_B_IDX(i,j) (((i-1)*WIDTH+j+1)*3+2)

	#define Y_R_IDX(i,j) (((i+1)*WIDTH+j-1)*3+0)
	#define Y_G_IDX(i,j) (((i+1)*WIDTH+j-1)*3+1)
	#define Y_B_IDX(i,j) (((i+1)*WIDTH+j-1)*3+2)

	#define Z_R_IDX(i,j) (((i+1)*WIDTH+j+1)*3+0)
	#define Z_G_IDX(i,j) (((i+1)*WIDTH+j+1)*3+1)
	#define Z_B_IDX(i,j) (((i+1)*WIDTH+j+1)*3+2)


#if 0//把RAW转成灰度图
	for(unsigned long i=0;i<RAWHeight;i++)
	{
		for(unsigned long j=0;j<RAWWidth;j++)
		{
			pRGB[M_R_IDX(i,j)] = pRAW[RAW_IDX(i,j)]>>2;
			pRGB[M_G_IDX(i,j)] = pRAW[RAW_IDX(i,j)]>>2;
			pRGB[M_B_IDX(i,j)] = pRAW[RAW_IDX(i,j)]>>2;
		}
	}
#endif



#if 1//把RAW转成带空穴的RGB

	for(unsigned long i=0;i<RAWHeight;i++)
	{
		for(unsigned long j=0;j<RAWWidth;j++)
		{
			//使用前对内存数据清零
			pRGB[M_R_IDX(i,j)] = 0x00;
			pRGB[M_G_IDX(i,j)] = 0x00;
			pRGB[M_B_IDX(i,j)] = 0x00;

			if(i%2==0)//偶数行
			{
				if(j%2==0)//偶数列
				{
					pRGB[M_R_IDX(i,j)] = pRAW[RAW_IDX(i,j)]>>2;//R
				}
				else//奇数列
				{
					pRGB[M_G_IDX(i,j)] = pRAW[RAW_IDX(i,j)]>>2;//G
				}
			}
			else//奇数行
			{
				if( j%2==0)//偶数列
				{
					pRGB[M_G_IDX(i,j)] = pRAW[RAW_IDX(i,j)]>>2;//G
				}
				else//奇数列
				{
					pRGB[M_B_IDX(i,j)] = pRAW[RAW_IDX(i,j)]>>2;//B
				}	
			}
			
		}
	}
#endif


#if 0//白平衡
	unsigned int rGain = 1.0;
	unsigned int gGain = 1.0;
	unsigned int bGain = 1.0;

	for(unsigned long i=0;i<RAWHeight;i++)
	{
		for(unsigned long j=0;j<RAWWidth;j++)
		{
			pRGB[M_R_IDX(i,j)] = (pRGB[M_R_IDX(i,j)]*rGain >255) ? 255 : pRGB[M_R_IDX(i,j)]*rGain;
			pRGB[M_G_IDX(i,j)] = (pRGB[M_G_IDX(i,j)]*gGain >255) ? 255 : pRGB[M_G_IDX(i,j)]*gGain;
			pRGB[M_B_IDX(i,j)] = (pRGB[M_B_IDX(i,j)]*bGain >255) ? 255 : pRGB[M_B_IDX(i,j)]*bGain;
		}
	}
#endif



#if 1//使用双次线性差值法解马赛克

#if 1//对G通道做双线性差值
	for(unsigned long i=0;i<RAWHeight;i++)
	{
		for(unsigned long j=0;j<RAWWidth;j++)
		{
			if(i%2==0)//偶数行
			{
				if(j%2==0)//偶数列
				{
					if(i==0 && j==0)
					{
						pRGB[M_G_IDX(i,j)] = (pRGB[R_G_IDX(i,j)]+pRGB[D_G_IDX(i,j)])/2;
					}
					else if(i==0)
					{
						pRGB[M_G_IDX(i,j)] = (pRGB[L_G_IDX(i,j)]+pRGB[R_G_IDX(i,j)]+pRGB[D_G_IDX(i,j)])/3;
					}
					else if(j==0)
					{
						pRGB[M_G_IDX(i,j)] = (pRGB[U_G_IDX(i,j)]+pRGB[R_G_IDX(i,j)]+pRGB[D_G_IDX(i,j)])/3;
					}
					else
					{
						pRGB[M_G_IDX(i,j)] = (pRGB[L_G_IDX(i,j)]+pRGB[U_G_IDX(i,j)]+pRGB[R_G_IDX(i,j)]+pRGB[D_G_IDX(i,j)])/4;
					}
				}
			}
			else//奇数行
			{
				if(j%2!=0)//奇数列
				{
					if(i==RAWHeight-1 && j==RAWWidth-1)
					{
						pRGB[M_G_IDX(i,j)] = (pRGB[L_G_IDX(i,j)]+pRGB[U_G_IDX(i,j)])/2;
					}
					else if(i==RAWHeight-1)
					{
						pRGB[M_G_IDX(i,j)] = (pRGB[L_G_IDX(i,j)]+pRGB[U_G_IDX(i,j)]+pRGB[R_G_IDX(i,j)])/3;
					}
					else if(j==RAWWidth-1)
					{
						pRGB[M_G_IDX(i,j)] = (pRGB[L_G_IDX(i,j)]+pRGB[U_G_IDX(i,j)]+pRGB[D_G_IDX(i,j)])/3;
					}
					else
					{
						pRGB[M_G_IDX(i,j)] = (pRGB[L_G_IDX(i,j)]+ pRGB[U_G_IDX(i,j)]+ pRGB[R_G_IDX(i,j)]+ pRGB[D_G_IDX(i,j)])/4;
					}					
				}
			}
		}
	}
#endif

#if 1//对R通道做双线性差值
	for(unsigned long i=0;i<RAWHeight;i++)
	{
		for(unsigned long j=0;j<RAWWidth;j++)
		{
			if(i%2!=0)
			{
				if(j%2!=0)
				{
					if(i==RAWHeight-1 && j==RAWWidth-1)
					{
						pRGB[M_R_IDX(i,j)] = pRGB[W_R_IDX(i,j)];
					}
					else if(i==RAWHeight-1)
					{
						pRGB[M_R_IDX(i,j)] = (pRGB[W_R_IDX(i,j)]+ pRGB[X_R_IDX(i,j)])/2;
					}
					else if(j==RAWWidth-1)
					{
						pRGB[M_R_IDX(i,j)] = (pRGB[W_R_IDX(i,j)]+ pRGB[Y_R_IDX(i,j)])/2;		
					}
					else
					{
						pRGB[M_R_IDX(i,j)] = (pRGB[W_R_IDX(i,j)]+ pRGB[X_R_IDX(i,j)]+ pRGB[Y_R_IDX(i,j)]+ pRGB[Z_R_IDX(i,j)])/4;					
					}
				}
			}
		}
	}
	for(unsigned long i=0;i<RAWHeight;i++)
	{
		for(unsigned long j=0;j<RAWWidth;j++)
		{
			if(i%2==0)
			{
				if(j%2!=0)
				{
					if(i==0 && j==RAWWidth-1)
					{
						pRGB[M_R_IDX(i,j)] = (pRGB[L_R_IDX(i,j)]+pRGB[D_R_IDX(i,j)])/2;
					}
					else if(i==0)
					{
						pRGB[M_R_IDX(i,j)] = (pRGB[L_R_IDX(i,j)]+pRGB[R_R_IDX(i,j)]+pRGB[D_R_IDX(i,j)])/3;
					}
					else if(j==RAWWidth-1)
					{
						pRGB[M_R_IDX(i,j)] = (pRGB[L_R_IDX(i,j)]+pRGB[U_R_IDX(i,j)]+pRGB[D_R_IDX(i,j)])/3;
					}
					else
					{
						pRGB[M_R_IDX(i,j)] = (pRGB[L_R_IDX(i,j)]+ pRGB[U_R_IDX(i,j)]+ pRGB[R_R_IDX(i,j)]+ pRGB[D_R_IDX(i,j)])/4;
					}
				}
			}
			else
			{
				if(j%2==0)
				{
					if(i==RAWHeight-1 && j==0)
					{
						pRGB[M_R_IDX(i,j)] = (pRGB[U_R_IDX(i,j)]+pRGB[R_R_IDX(i,j)])/2;
					}
					else if(i==RAWHeight-1)
					{
						pRGB[M_R_IDX(i,j)] = (pRGB[L_R_IDX(i,j)]+pRGB[U_R_IDX(i,j)]+pRGB[R_R_IDX(i,j)])/3;
					}
					else if(j==0)
					{
						pRGB[M_R_IDX(i,j)] = (pRGB[U_R_IDX(i,j)]+pRGB[U_R_IDX(i,j)]+pRGB[D_R_IDX(i,j)])/3;
					}
					else
					{
						pRGB[M_R_IDX(i,j)] = (pRGB[L_R_IDX(i,j)]+ pRGB[U_R_IDX(i,j)]+ pRGB[R_R_IDX(i,j)]+ pRGB[D_R_IDX(i,j)])/4;
					}
				}
			}
		}
	}
#endif



#if 1//对B通道做双线性差值
	for(unsigned long i=0;i<RAWHeight;i++)
	{
		for(unsigned long j=0;j<RAWWidth;j++)
		{
			if(i%2==0)
			{
				if(j%2==0)
				{
					if(i==0 && j==0)
					{
						pRGB[M_B_IDX(i,j)] = pRGB[Z_B_IDX(i,j)];
					}
					else if(i==0)
					{
						pRGB[M_B_IDX(i,j)] = (pRGB[Y_B_IDX(i,j)]+ pRGB[Z_B_IDX(i,j)])/2;
					}
					else if(j==0)
					{
						pRGB[M_B_IDX(i,j)] = (pRGB[X_B_IDX(i,j)]+ pRGB[Z_B_IDX(i,j)])/2;		
					}
					else
					{
						pRGB[M_B_IDX(i,j)] = (pRGB[W_B_IDX(i,j)]+ pRGB[X_B_IDX(i,j)]+ pRGB[Y_B_IDX(i,j)]+ pRGB[Z_B_IDX(i,j)])/4;					
					}
				}
			}
		}
	}
	for(unsigned long i=0;i<RAWHeight;i++)
	{
		for(unsigned long j=0;j<RAWWidth;j++)
		{
			if(i%2==0)
			{
				if(j%2!=0)
				{
					if(i==0 && j==RAWWidth-1)
					{
						pRGB[M_B_IDX(i,j)] = (pRGB[L_B_IDX(i,j)]+pRGB[D_B_IDX(i,j)])/2;
					}
					else if(i==0)
					{
						pRGB[M_B_IDX(i,j)] = (pRGB[L_B_IDX(i,j)]+pRGB[R_B_IDX(i,j)]+pRGB[D_B_IDX(i,j)])/3;
					}
					else if(j==RAWWidth-1)
					{
						pRGB[M_B_IDX(i,j)] = (pRGB[L_B_IDX(i,j)]+pRGB[U_B_IDX(i,j)]+pRGB[D_B_IDX(i,j)])/3;
					}
					else
					{
						pRGB[M_B_IDX(i,j)] = (pRGB[L_B_IDX(i,j)]+pRGB[U_B_IDX(i,j)]+pRGB[R_B_IDX(i,j)]+pRGB[D_B_IDX(i,j)])/4;
					}
				}
			}
			else
			{
				if(j%2==0)
				{
					if(i==RAWHeight-1 && j==0)
					{
						pRGB[M_B_IDX(i,j)] = (pRGB[U_B_IDX(i,j)]+pRGB[R_B_IDX(i,j)])/2;
					}
					else if(i==RAWHeight-1)
					{
						pRGB[M_B_IDX(i,j)] = (pRGB[L_B_IDX(i,j)]+pRGB[U_B_IDX(i,j)]+pRGB[R_B_IDX(i,j)])/3;
					}
					else if(j==0)
					{
						pRGB[M_B_IDX(i,j)] = (pRGB[U_B_IDX(i,j)]+pRGB[U_B_IDX(i,j)]+pRGB[D_B_IDX(i,j)])/3;
					}
					else
					{
						pRGB[M_B_IDX(i,j)] = (pRGB[L_B_IDX(i,j)]+pRGB[U_B_IDX(i,j)]+pRGB[R_B_IDX(i,j)]+pRGB[D_B_IDX(i,j)])/4;
					}
				}
			}
		}
	}

#endif

#endif

	return 0;
}


int main()
{
  unsigned long RAWWidth = IMAGE_WIDTH;
  unsigned long RAWHeight = IMAGE_HEIGHT;
  unsigned long RAWSize = RAWWidth*RAWHeight*2;
  unsigned short *pRAW = (unsigned short *)malloc(RAWSize);
  memset(pRAW, 0x00, RAWSize);

  unsigned long RGBWidth = IMAGE_WIDTH;
  unsigned long RGBHeight = IMAGE_HEIGHT;
  unsigned long RGBSize = RGBWidth*RGBHeight*3;
  unsigned char *pRGB = (unsigned char *)malloc(RGBSize);
  memset(pRGB, 0x00, RGBSize);

  unsigned long BMPSize = (RGBWidth*3+RGBWidth%4)*RGBHeight+0x36;
  unsigned char *pBMP = (unsigned char *)malloc(BMPSize);
  memset(pBMP, 0x00, BMPSize);
  
	FILE *fp_raw = fopen("input.raw", "rb");
	if (fp_raw == NULL)
	{
		return -1;
	}
	fread (pRAW, sizeof(unsigned char), RAWSize, fp_raw) ;
	fclose(fp_raw);
  
	convertRAWtoRGB(pRAW, RAWWidth, RAWHeight, pRGB);
	saveRGBtoBMP(pRGB, RGBWidth, RGBHeight, pBMP);

	FILE *fp_bmp = fopen("save.bmp", "wb");
	if (fp_bmp == NULL)
	{
		return -1;
	}
	fwrite(pBMP,1,BMPSize,fp_bmp);
	fclose(fp_bmp);

	free(pRAW);
	free(pRGB);
	free(pBMP);
	//getchar();
  
	return 0;
}
