/*
** r_models.c
**
** models loading and rendering
** (c) 2002 by DarkOne
**
** 10/20/02 - started work
*/
#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "r_gl/glext.h"
#include "WolfDef.h"
#include "r_models.h"

// ------------------------- * defines * -------------------------

#define MD2_MAGIC			('I'+('D'<<8)+('P'<<16)+('2'<<24))
#define MD2_VERSION		8

#define	MD2_MAX_TRIS		4096
#define MD2_MAX_VERTS		2048
#define MD2_MAX_FRAMES	512
#define MD2_MAX_SKINS		32
#define	MD2_SKINNAME		64		// length of skin-name

#define NUMVERTEXNORMALS	162

// vertex normals
float	md2_vertexnormals[NUMVERTEXNORMALS][3]=
{
#include "anorms.h"
};

// ------------------------- * types * -------------------------

#pragma pack(push, 1)

typedef struct md2_s
{
	unsigned magic;
	int version;

	int skinWidth;
	int skinHeight;
	int frameSize;		// byte size of each frame
// sizes
	int numSkins;
	int numVertices;
	int numTexCoords;	// greater than numVertices for seams
	int numTriangles;
	int numGLcmds;		// dwords in strip/fan command list
	int numFrames;
// offsets
	int offSkins;			// each skin is a MAX_SKINNAME string
	int ofsTexCoords;	// byte offset from start for stverts
	int ofsTriangles;	// offset for dtriangles
	int ofsFrames;		// offset for frame data
	int ofsGLcmds;
	int ofsEnd;				// end of file
} md2_header_t;

typedef struct md2_vert_s
{
	byte v[3];				// scaled byte to fit in frame mins/maxs
	byte lightNormalIndex;
} md2_vert_t;

typedef struct md2_frame_s
{
	float scale[3];			// multiply byte verts by this
	float	translate[3];	// then add this
	char name[16];			// frame name
	md2_vert_t verts[1];// variable sized
} md2_frame_t;

#pragma pack(pop)

// ------------------------- * loading/unloading * -------------------------

// TODO: move to fileIO.c (?)

/*
** MD2_BBox
*/
void MD2_BBox(model_t *mdl)
{
	md2_frame_t *frm;
	md2_vert_t *vertex;
	bbox_t *bbox;
	int frame, vertices, n;
	int *glCmds;
	bool first=true;

	bbox=&mdl->bbox;

	for(frame=0; frame<mdl->numFrames; frame++)
	{
		frm=(md2_frame_t *)(mdl->frames+mdl->frameSize*frame);
		glCmds=mdl->glCmds;

		while(*glCmds)
		{
			vertices=*glCmds++;
			if(vertices<0) vertices=-vertices; // triangle fan

			for(n=0; n<vertices; n++)
			{
				glCmds+=2;
				vertex=&frm->verts[*glCmds++];

				if(first)
				{
					first=false;
					bbox->xmax=bbox->xmin=vertex->v[0]*frm->scale[0]+frm->translate[0];
					bbox->ymax=bbox->ymin=vertex->v[1]*frm->scale[1]+frm->translate[1];
					bbox->zmax=bbox->zmin=vertex->v[2]*frm->scale[2]+frm->translate[2];
				}
				else
				{
					if(bbox->xmax<vertex->v[0]*frm->scale[0]+frm->translate[0])
						bbox->xmax=vertex->v[0]*frm->scale[0]+frm->translate[0];
					else if(bbox->xmin>vertex->v[0]*frm->scale[0]+frm->translate[0])
						bbox->xmin=vertex->v[0]*frm->scale[0]+frm->translate[0];
				
					if(bbox->ymax<vertex->v[1]*frm->scale[1]+frm->translate[1])
						bbox->ymax=vertex->v[1]*frm->scale[1]+frm->translate[1];
					else if(bbox->ymin>vertex->v[1]*frm->scale[1]+frm->translate[1])
						bbox->ymin=vertex->v[1]*frm->scale[1]+frm->translate[1];

					if(bbox->zmax<vertex->v[2]*frm->scale[2]+frm->translate[2])
						bbox->zmax=vertex->v[2]*frm->scale[2]+frm->translate[2];
					else if(bbox->zmin>vertex->v[2]*frm->scale[2]+frm->translate[2])
						bbox->zmin=vertex->v[2]*frm->scale[2]+frm->translate[2];
				}
			}
			glEnd();
		}
	}
}

/*
** MD2_Load
**
** loads md2 model
*/
bool MD2_Load(char *filename, char *skin, model_t *mdl)
{
	byte *raw;
	md2_header_t *header;

	FS_LoadFile(filename, &raw, false);
	if(!raw) return false; // file not found
	header=(md2_header_t *)raw;

	if(header->magic!=MD2_MAGIC || header->version!=MD2_VERSION)
	{
		Con_Printf("MD2_Load: %s is not a valid model\n", filename);
		FS_FreeFile(raw);
		return false;
	}

// loading frame information
	mdl->numFrames=header->numFrames;
	mdl->frameSize=header->frameSize;
	mdl->frames=malloc(header->frameSize*header->numFrames);
	if(!mdl->frames)
	{
		FS_FreeFile(raw);
		return false;
	}
	memcpy(mdl->frames, raw+header->ofsFrames, header->frameSize*header->numFrames);

// loading glCommands
	mdl->glCmds=malloc(header->numGLcmds*4);
	if(!mdl->glCmds)
	{
		FS_FreeFile(raw);
		free(mdl->frames);
		return false;
	}
	memcpy(mdl->glCmds, raw+header->ofsGLcmds, header->numGLcmds*4);
	FS_FreeFile(raw);

// bounding box
	MD2_BBox(mdl);

// skin loading
	if(!Img_Read(skin, (image_t *)&mdl->skin)) return false; // fixme
	Vid_UploadTexture(&mdl->skin, true, false);

//	Con_Printf("Model %s loaded (%d frames)\n", filename, mdl->numFrames);
	return true;
}

/*
** MD2_UnLoad
**
** frees memory occupied by md2 model
*/
void MD2_UnLoad(model_t *mdl)
{
	if(mdl->glCmds) free(mdl->glCmds), mdl->glCmds==NULL;
	if(mdl->frames) free(mdl->frames), mdl->frames==NULL;
	Vid_UnLoadTexture(&mdl->skin);
}

// ------------------------- * rendering * -------------------------

/*
** MD2_Render
**
** renders a model
** align:
**  0 - no align
**  1 - align bottom to base
**  2 - align top to base
*/
void MD2_Render(model_t *mdl, int frame, int align, float base)
{
	int *glCmds, vertices, n;
	md2_frame_t *frm;
	md2_vert_t *vertex;

	if(frame>=mdl->numFrames) return;

	if(align==1)
		base-=mdl->bbox.zmin;
	else if(align==2)
		base-=mdl->bbox.zmax;
	else
		base=0;

	frm=(md2_frame_t *)(mdl->frames+mdl->frameSize*frame);
	glCmds=mdl->glCmds;

	Vid_SelectTextureIndirect(&mdl->skin);
	glColor3ub(0xFF, 0xFF, 0xFF);
// translate & scale
	glTranslatef(frm->translate[0], frm->translate[1], frm->translate[2]+base);
	glScalef(frm->scale[0], frm->scale[1], frm->scale[2]);

	while(*glCmds)
	{
		vertices=*glCmds++;
		if(vertices<0) // triangle fan
		{
			glBegin(GL_TRIANGLE_FAN);
			vertices=-vertices;
		}
		else
			glBegin(GL_TRIANGLE_STRIP);

		for(n=0; n<vertices; n++)
		{
			glTexCoord2fv((float *)glCmds);
			glCmds+=2;

			vertex=&frm->verts[*glCmds++];

			glNormal3fv(md2_vertexnormals[vertex->lightNormalIndex]);
			glVertex3f(vertex->v[0], vertex->v[1], vertex->v[2]);
		}
		glEnd();
	}
}

/*
** MD2_RenderLerp
**
** renders a model, linearly interpolating between 2 frames
** align:
**  0 - no align
**  1 - align bottom to base
**  2 - align top to base
*/
void MD2_RenderLerp(model_t *mdl, int frame1, int frame2, float t, int align, float base)
{
	int *glCmds, vertices, n;
	md2_frame_t *frmA, *frmB;
	md2_vert_t *vert1, *vert2;
	float tmp[3];

	if(frame1>=mdl->numFrames || frame2>=mdl->numFrames) return;

	if(align==1)
		base-=mdl->bbox.zmin;
	else if(align==2)
		base-=mdl->bbox.zmax;
	else
		base=0;

	frmA=(md2_frame_t *)(mdl->frames+mdl->frameSize*frame1);
	frmB=(md2_frame_t *)(mdl->frames+mdl->frameSize*frame2);
	glCmds=mdl->glCmds;

	Vid_SelectTextureIndirect(&mdl->skin);
	glColor3ub(0xFF, 0xFF, 0xFF);

// translate & scale
	glTranslatef(lerp(t, frmA->translate[0], frmB->translate[0]),
							 lerp(t, frmA->translate[1], frmB->translate[1]),
							 lerp(t, frmA->translate[2], frmB->translate[2])+base);
	glScalef(lerp(t, frmA->scale[0], frmB->scale[0]),
					 lerp(t, frmA->scale[1], frmB->scale[1]),
					 lerp(t, frmA->scale[2], frmB->scale[2]));

	while(*glCmds)
	{
		vertices=*glCmds++;
		if(vertices<0) // triangle fan
		{
			glBegin(GL_TRIANGLE_FAN);
			vertices=-vertices;
		}
		else
			glBegin(GL_TRIANGLE_STRIP);

		for(n=0; n<vertices; n++)
		{
			glTexCoord2fv((float *)glCmds);
			glCmds+=2;

			vert1=&frmA->verts[*glCmds];
			vert2=&frmB->verts[*glCmds];
			glCmds++;

			tmp[0]=lerp(t, md2_vertexnormals[vert1->lightNormalIndex][0], md2_vertexnormals[vert2->lightNormalIndex][0]);
			tmp[1]=lerp(t, md2_vertexnormals[vert1->lightNormalIndex][1], md2_vertexnormals[vert2->lightNormalIndex][1]);
			tmp[2]=lerp(t, md2_vertexnormals[vert1->lightNormalIndex][2], md2_vertexnormals[vert2->lightNormalIndex][2]);
			glNormal3fv(tmp);
			tmp[0]=lerp(t, vert1->v[0], vert2->v[0]);
			tmp[1]=lerp(t, vert1->v[1], vert2->v[1]);
			tmp[2]=lerp(t, vert1->v[2], vert2->v[2]);
			glVertex3fv(tmp);
		}
		glEnd();
	}
}