/*************************************************************************
** NewWolf
** Copyright (C) 1999-2002 by DarkOne
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**************************************************************************
** image files I/O
*************************************************************************/
#include "common.h"
#include <jpeglib.h>
#include "../Q_fns.h"

// ------------------------- * TGA (Truevision TARGA) * -------------------------
#pragma pack(push, 1)
typedef struct tga_s
{
	byte id_length, colormap_type, image_type;
	word colormap_index, colormap_length;
	byte colormap_size;
	word x_origin, y_origin, width, height;
	byte pixel_size, attributes;
} tga_t;
#pragma pack(pop)

/*
** Img_ReadTGA
**
** Loads TGA image file
** returns true if loading was successful
*/
bool Img_ReadTGA(char *name, image_t *img)
{
	byte *raw, *p, *dest;
	tga_t *header;
	byte pheader;
	bool flip;
	int plen, x, y0, y;

	FS_LoadFile(name, &raw, false);
	if(!raw) return false; // file read error
	header=(tga_t *)raw;

// check image type
	if((header->image_type!=2 && header->image_type!=10) ||
		 (header->pixel_size!=32 && header->pixel_size!=24))
	{
		FS_FreeFile(raw);
		return false; // unsupported type
	}

	img->w=header->width;
	img->h=header->height;
	img->bpp=(header->pixel_size)/8;
	img->data=malloc(img->w*img->h*img->bpp);
	if(!img->data)
	{
		FS_FreeFile(raw);
		return false; // memory allocation error
	}

	flip=!(header->attributes&0x20); // top-bottom orientation
	p=raw+sizeof(tga_t)+header->id_length; // data block
	if(header->colormap_type)
		p+=(header->colormap_size/8)*header->colormap_length; // skip colormap

	switch(header->image_type)
	{
	case 2: // Unmapped RGB images
		for(y0=0; y0<img->h; y0++)
		{
			y=flip?(img->h-(y0+1)):y0;
			dest=img->data+y*img->w*img->bpp;

		// copy scanline
			for(x=0; x<img->w; x++)
			{
				dest[2]=*p++;
				dest[1]=*p++;
				dest[0]=*p++;
				if(img->bpp==4) dest[3]=*p++;
				dest+=img->bpp;
			}
		}
		break;
	case 10: // Run Length Encoded, RGB images
		for(y0=0; y0<img->h; y0++)
		{
			y=flip?(img->h-(y0+1)):y0;
			dest=img->data+y*img->w*img->bpp;
			for(x=0; x<img->w;)
			{
				pheader=*p++;
				plen=(pheader&127)+1;

				if(pheader&128) // RLE packet
				{
					while(plen--)
					{
						dest[2]=p[0];
						dest[1]=p[1];
						dest[0]=p[2];
						if(img->bpp==4) dest[3]=p[3];
						dest+=img->bpp;

						if(++x==img->w) // run spans across rows
						{
							if(++y0==img->h) break; // last scanline decoded
							y=flip?(img->h-(y0+1)):y0;
							dest=img->data+y*img->w*img->bpp;
							x=0;
						}
					}
					p+=img->bpp;
				}
				else
					while(plen--)
					{
						dest[2]=*p++;
						dest[1]=*p++;
						dest[0]=*p++;
						if(img->bpp==4) dest[3]=*p++;
						dest+=img->bpp;

						if(++x==img->w) // pixel packet run spans across rows
						{
							if(++y0==img->h) break; // last scanline decoded
							y=flip?(img->h-(y0+1)):y0;
							dest=img->data+y*img->w*img->bpp;
							x=0;
						}
					}
			}
		}
		break;
	}

// TODO: flip as post-processing ?

	FS_FreeFile(raw);
	return true;
}

// ------------------------- * PCX (Zsoft Paintbrush) * -------------------------
#pragma pack(push, 1)
typedef struct pcx_s
{
	byte manufacturer;
	byte version;
	byte encoding;
	byte bits_per_pixel;
	word xmin, ymin, xmax, ymax; // w=xmax-xmin; h=ymax-ymin
	word hres, vres;
	byte palette[48];
  byte reserved;
	byte color_planes;
	word bytes_per_line;
	word palette_type;
	byte filler[58];
} pcx_t;
#pragma pack(pop)

/*
** Img_ReadPCX
**
** Loads PCX image file
** returns true if loading was successful
*/
bool Img_ReadPCX(char *name, image_t *img)
{
	byte *raw, *p, *dest, *palette;
	pcx_t *header;
	int data, len, x, y;

	len=FS_LoadFile(name, &raw, false);
	if(!raw) return false; // file read error
	header=(pcx_t *)raw;

// check image type
	if(	header->manufacturer!=0x0A ||
			header->version!=5 ||
			header->encoding!=1 ||
			header->bits_per_pixel!=8)
	{
		FS_FreeFile(raw);
		return false; // unsupported type
	}

	img->w=header->xmax-header->xmin+1;
	img->h=header->ymax-header->ymin+1;
	img->bpp=3;
	img->data=malloc(img->w*img->h*img->bpp);
	if(!img->data)
	{
		FS_FreeFile(raw);
		return false; // memory allocation error
	}

	palette=raw+len-768;
	p=raw+sizeof(pcx_t);

	for(y=0; y<img->h; y++)
	{
		dest=img->data+y*img->w*img->bpp;

		for(x=0; x<img->w;)
		{
			data=*p++;

			if((data&0xC0)==0xC0)
			{
				len=data&0x3F;
				data=*p++;
			}
			else
				len=1;

			while(len--)
			{
				*dest++=palette[data*3+0];
				*dest++=palette[data*3+1];
				*dest++=palette[data*3+2];
				if(++x>=img->w) break;
			}
		}
	}

	FS_FreeFile(raw);
	return true;
}

// ------------------------- * JPEG (Joint Photographic Experts Group) * -------------------------

static void jpg_noop(j_decompress_ptr cinfo)
{
}

static boolean jpg_fill_input_buffer(j_decompress_ptr cinfo)
{
	return true;
}

static void jpg_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
	cinfo->src->next_input_byte+=(size_t)num_bytes;
	cinfo->src->bytes_in_buffer-=(size_t)num_bytes;
}

static void jpeg_mem_src(j_decompress_ptr cinfo, byte *mem, int len)
{
	cinfo->src=(struct jpeg_source_mgr *)(*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_PERMANENT, sizeof(struct jpeg_source_mgr));
	cinfo->src->init_source=jpg_noop;
	cinfo->src->fill_input_buffer=jpg_fill_input_buffer;
	cinfo->src->skip_input_data=jpg_skip_input_data;
	cinfo->src->resync_to_restart=jpeg_resync_to_restart;
	cinfo->src->term_source=jpg_noop;
	cinfo->src->bytes_in_buffer=len;
	cinfo->src->next_input_byte=mem;
}

/*
** Img_ReadJPG
**
** Loads JPG image file
** returns true if loading was successful
*/
bool Img_ReadJPG(char *name, image_t *img)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
  JSAMPARRAY buffer;
	byte *raw, *dest;
	int len;

	len=FS_LoadFile(name, &raw, false);
	if(!raw) return false; // file read error

// decompress
	cinfo.err=jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, raw, len);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

// check image type
	if(cinfo.output_components!=3)
	{
		jpeg_destroy_decompress(&cinfo);
		FS_FreeFile(raw);
		return false; // unsupported type
	}

	img->w=cinfo.output_width;
	img->h=cinfo.output_height;
	img->bpp=3;
	img->data=malloc(img->w*img->h*img->bpp);
	if(!img->data)
	{
		jpeg_destroy_decompress(&cinfo);
		FS_FreeFile(raw);
		return false; // memory allocation error
	}

	while(cinfo.output_scanline<cinfo.output_height)
	{
		dest=img->data+cinfo.output_scanline*img->w*img->bpp;
		buffer=&dest;
		jpeg_read_scanlines(&cinfo, buffer, 1);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	FS_FreeFile(raw);
	return true;
}

// ------------------------- * common * -------------------------

/*
** Img_Read
**
** Reads image.
** Image type is determined by extension
** if no extension present it tries .tga; .jpg; .pcx /in such order/
*/
bool Img_Read(char *path, image_t *img)
{
	char name[MAX_OSPATH], *src;

	if(!path || !*path) return false;

	strcpy(name, path);

// find extension
	src=name+strlen(name)-1;
	while(*src!='/' && src!=name)
	{
		if(*src=='.') break;
		src--;
	}

	if(*src=='.')
	{
		if(!Q_strcasecmp(src, ".tga"))
			return Img_ReadTGA(name, img);
		else if(!Q_strcasecmp(src, ".jpg"))
			return Img_ReadJPG(name, img);
		else if(!Q_strcasecmp(src, ".pcx"))
			return Img_ReadPCX(name, img);
	}
	else
		src=name+strlen(name);
	
	strcpy(src, ".tga");
	if(Img_ReadTGA(name, img)) return true;
	strcpy(src, ".jpg");
	if(Img_ReadJPG(name, img)) return true;
	strcpy(src, ".pcx");
	if(Img_ReadPCX(name, img)) return true;

	return false;
}

// ------------------------- * Image Manipulation * -------------------------
byte *img_def_palette; // default palette from gfx/palette.lmp

/*
** Img_ExpandPalette
**
** expands palette to RGB(A) color
*/
void Img_ExpandPalette(byte *dst, byte *src, int w, int h, byte *pal, bool transparent)
{
	int x, y, srcp, size;
	byte *ssrc;

	if(!pal) // no palette supplied, use default
	{
		if(!img_def_palette) // loading default palette
			if(FS_LoadFile("gfx/palette.lmp", &img_def_palette, false)!=256*3)
			{
				byte *p;

				Con_Printf("Warning: palette file gfx/palette.lmp not found, using grayscale\n");
				img_def_palette=p=Z_Malloc(256*3);
				for(x=0; x<256; x++, p+=3)
					p[0]=p[1]=p[2]=(byte)x;
			}
		pal=img_def_palette;
	}

	ssrc=src;
	size=w*h;

	for(y=0; y<h; y++)
		for(x=0; x<w; x++)
	{
		srcp=*src++;
		if(transparent && srcp==255)
		{
			int r, g, b, i, c;
			int u, v;

			r=g=b=c=0;
			for(v=y-1; v<=y+1; v++)
				for(u=x-1; u<=x+1; u++)
			{
				if(u>0 && u<w && v>0 && v<h && (i=ssrc[v*w+u])!=255)
				{
					r+=pal[i*3+0];
					g+=pal[i*3+1];
					b+=pal[i*3+2];
					c++;
				}
			}
			if(!c) c=1;
			else
				c=c;
			*dst++=(byte)(r/c);
			*dst++=(byte)(g/c);
			*dst++=(byte)(b/c);
			*dst++=0;
			continue;
		}
		*dst++=pal[srcp*3+0];
		*dst++=pal[srcp*3+1];
		*dst++=pal[srcp*3+2];
		if(transparent) *dst++=255;
	}
}

/*
** Img_HasTransprency
**
** returns true if palettized image has entry 255 /transparent/
*/
bool Img_HasTransprency(byte *data, int size)
{
	while(size--)
		if(*data++==255) return true;
	return false;
}

/*
** Img_Resize
**
** fast pixel resize from src to dst
*/
void Img_Resize(byte *dst, int outwidth, int outheight, byte *data, int width, int height, int bpp)
{
	int x, y, n;
	int xx, yy;
	float sx, sy;
	byte *src;

// Scale the image
	if(outwidth>1)
		sx=(float)(width-1)/(float)(outwidth-1);
	else
		sx=(float)(width-1);
  if(outheight>1)
		sy=(float)(height-1)/(float)(outheight-1);
	else
		sy=(float)(height-1);

	for(y=0; y<outheight; y++)
	{
		yy=(int)(y*sy)*width;

		for(x=0; x<outwidth; x++)
		{
			xx=(int)(x*sx);
			src=data+(yy+xx)*bpp;

			for(n=0; n<bpp; n++)
				*dst++=*src++;
		}
	}
}

/*
** Img_Resample
**
** quality image resample from src to dst
*/
void Img_Resample(byte *dst, int outwidth, int outheight, byte *data, int width, int height, int bpp)
{
	int x, y, n;
	float sx, sy;
	float *tempin;

// allocate storage for intermediate image
	tempin=(float *)Z_Malloc(width*height*bpp*sizeof(float));
// convert the pixel data to floating point
	for(n=0; n<width*height*bpp; n++)
		tempin[n]=(float)data[n];

// Scale the image
	if(outwidth>1)
		sx=(float)(width-1)/(float)(outwidth-1);
	else
		sx=(float)(width-1);
  if(outheight>1)
		sy=(float)(height-1)/(float)(outheight-1);
	else
		sy=(float)(height-1);

	if(sx<=1.0 && sy<=1.0)
	{// magnify both width and height:  use weighted sample of 4 pixels
		int x0, x1, y0, y1;
		float alpha, beta;
		float *src00, *src01, *src10, *src11;
		float s1, s2;

		for(y=0; y<outheight; y++)
		{
			y0=(int)(y*sy);
			y1=y0+1;
			if(y1>=height) y1=height-1;
			alpha=y*sy-y0;
			y0*=width;
			y1*=width;
			for(x=0; x<outwidth; x++)
			{
				x0=(int)(x*sx);
				x1=x0+1;
				if(x1>=width) x1=width-1;
				beta=x*sx-x0;

			// compute weighted average of pixels in rect (x0, y0) - (x1, y1)
				src00=tempin+(y0+x0)*bpp;
				src01=tempin+(y0+x1)*bpp;
				src10=tempin+(y1+x0)*bpp;
				src11=tempin+(y1+x1)*bpp;

				for(n=0; n<bpp; n++)
				{
					s1=(*src00++)*(1.0f-beta)+(*src01++)*beta;
					s2=(*src10++)*(1.0f-beta)+(*src11++)*beta;
					*dst++=(byte)(s1*(1.0f-alpha)+s2*alpha);
				}
			}
		}
	}
	else
	{// shrink width and/or height:  use an unweighted box filter
		int x0, x1, y0, y1;
		int xx, yy;
		float sum;

		for(y=0; y<outheight; y++)
		{
			y0=(int)(y*sy);
			y1=y0+1;
			if(y1>=height) y1=height-1;

			for(x=0; x<outwidth; x++)
			{
				x0=(int)(x*sx);
				x1=x0+1;
				if(x1>=width) x1=width-1;

			// compute average of pixels in the rectangle (x0, y0) - (x1, y1)
				for(n=0; n<bpp; n++)
				{
					sum=0;
					for(yy=y0; yy<=y1; yy++)
						for(xx=x0; xx<=x1; xx++)
							sum+=*(tempin+(yy*width+xx)*bpp+n);
					sum/=(x1-x0+1)*(y1-y0+1);
					*dst++=(byte)sum;
				}
			}
		}
	}

	Z_Free(tempin);
}

/*
** Img_MipMap
**
** creates next level mip-map image /uses the same data space/
** returns false if no mip mapping required (1x1 rexture)
** TODO: make it less resource expensive /without malloc and resample/
*/
bool Img_MipMap(byte *src, int *width, int *height, int bpp)
{
	int nw, nh;
	byte *dst;

	if(*width==1 && *height==1) return false;

	nw=(*width<2)?1:*width>>1;
	nh=(*height<2)?1:*height>>1;
	dst=Z_Malloc(nw*nh*bpp);
		Img_Resample(dst, nw, nh, src, *width, *height, bpp);
		memcpy(src, dst, nw*nh*bpp);
	Z_Free(dst);
	*width=nw;
	*height=nh;

	return true;
}

