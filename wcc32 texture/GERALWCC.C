// watcom C
// C standart  (c) 1999 X-prog
#include <graph.h>   //watcom
#include <stdio.h>
#include <string.h>

#define dword unsigned
#define word unsigned short
#define byte char 

typedef struct
{
        int x,y;
}pixel;

pixel pix(word x,word y)
{
        pixel aux;
        aux.x=x;
        aux.y=y;
        return aux;
} 



word X_MAX, Y_MAX;
word X_MAX1, Y_MAX1;
word video_set=0;
int SIZEMEM_SCREEN=-1;

#define VGA_320_200_8   1
#define VESA_320_200_16 2        //not work, for now
#define VESA_640_480_8  3        //not work, for now
#define VESA_640_480_16 4        //not work, for now

double x_max=10.,y_max=10.,x_center=5.,y_center=5.,x_density=32,y_density=20;

void Set320x200(void)
{
   _setvideomode(_MRES256COLOR); //watcom
}

void Set_text(void)
{
   _setvideomode(_TEXTC80);  //watcom
}


void* set_video (word video)
{
        if (video==1)
        {
        Set320x200();
        video_set=VGA_320_200_8;
        X_MAX=320;Y_MAX=200;
        X_MAX1=319;Y_MAX1=199;
        x_density=32;                     //para converter: vec2d -> pixel
        y_density=20;                     //para converter: vec2d -> pixel
        SIZEMEM_SCREEN=X_MAX*Y_MAX;
        return ((char *)0xa0000);
        }
        else return NULL;
}


void unset_video (void)
{
        video_set=0;
        Set_text();
}
//----------------------------virtual SCREEN
byte *VIRTUAL_SCREEN_8;
word *VIRTUAL_SCREEN_16;

int use_VIRTUAL_SCREEN (void)
{
        if(video_set==VGA_320_200_8)
        VIRTUAL_SCREEN_8=(byte*) calloc (X_MAX * Y_MAX,sizeof(byte));
        if(video_set==VESA_320_200_16)
        VIRTUAL_SCREEN_16=(word*) calloc (X_MAX * Y_MAX,sizeof(word));
        if (VIRTUAL_SCREEN_8==NULL && VIRTUAL_SCREEN_16==NULL) 
                return -1;
        return 0;
}

void VS_TO_RS_VGA_320_200_8(void)
{
        memcpy((char*) 0xa0000, VIRTUAL_SCREEN_8, 64000);   
        memset(VIRTUAL_SCREEN_8, 0, 64000); 
}

void unuse_VIRTUAL_SCREEN (void)
{
        free(VIRTUAL_SCREEN_8);
        free(VIRTUAL_SCREEN_16);
}
//-----------------------------------------2d functions
// C standart  (c) 1999 X-prog

typedef struct   //vector 2D
{
        float x, y;
}vec2d;

vec2d vec2(float x,float y)
{
        vec2d aux;
        aux.x=x;
        aux.y=y;
        return aux;
} 

pixel vec2_to_pixel (vec2d v2)
{
        pixel aux;
        aux.x=(int)(v2.x*x_density);
        aux.y=Y_MAX-(int)(v2.y*y_density);
        return aux;
}


//---------------------- 3d functions
typedef struct   //vector 3D
{
        float x, y, z;
}vec3d;

vec3d vec3(float x,float y,float z)
{
        vec3d aux;
        aux.x=x;
        aux.y=y;
        aux.z=z;
        return aux;
}



float dist=5; //prespectiva

pixel vec3_to_pixel (vec3d v3,vec3d cam)
{
        vec2d v2;
        if(cam.z==v3.z) v2=vec2(x_center,y_center);
        else
        {
        v2.x=(dist*((v3.x-cam.x)/(cam.z-v3.z)))+x_center;
        v2.y=(dist*((v3.y-cam.y)/(cam.z-v3.z)))+y_center;
        }
//        printf ("ponto x:%f y:%f \n",v2.x,v2.y);
        
        return vec2_to_pixel(v2);
}


//------------------------------pixel
void setpixel(long x, long y, char col) 
{
        if (x>=0&&x<320 &&  y>=0 && y <200)
        *((char*) 0xa0000 + (y*320) + x) = col;
}

void setbpixel(long x, long y, char col) 
{
        if (x>=0&&x<320 &&  y>=0 && y <200)
        *(VIRTUAL_SCREEN_8 + (y*320) + x) = col;
}


void clear(void)
{
        memset((char*) 0xa0000, 0, 64000); 
}


void quadrado(void)
{
        int i;
        for(i=0;i<320;i++)
                {setpixel(i,0,14);
                setpixel(i,199,14);
                }
        for(i=0;i<200;i++)
                {setpixel(0,i,14);
                setpixel(319,i,14);
                }

}

void linemem (int xp,int yp,int xq,int yq,char valor)
{
	int x=xp, y=yp, d=0, dx=xq-xp,dy=yq-yp,c , m,
	xinc=1, yinc=1;
	if(dx<0){xinc=-1;dx=-dx;}
	if(dy<0){yinc=-1;dy=-dy;}
	if(dy<dx)
	{
		c=2*dx;m=2*dy;
		while (x!=xq)
		{
                        setpixel(x,y,valor);
			x+=xinc;d+=m;
			if (d>dx){y+=yinc;d-=c;}
		}
	}
	else
	{
		c=2*dy;m=2*dx;
		while (y!=yq)
		{
                        setpixel(x,y,valor);
			y+=yinc;d+=m;
			if (d>dy){x+=xinc;d-=c;}
		}
	}
}
void line (pixel a,pixel b,byte cor)
{
        linemem (a.x,a.y,b.x,b.y,cor);
}

//-------------------------------------poligno
typedef struct           //for fast 3D-graphics
        {
        int x;                      //coordenada x 
        int X_texture,Y_texture;    // coordenadas da textura << 8
        int light;                // only for ground shading...
        }scanline_bar;

scanline_bar *LEFT,*RIGHT;

typedef struct 
        {
        word X_size;  // tamanho x
        word Y_size;  // tamanho y
        byte *map;    // bitmap (cada byte ‚ um pixel, independente das cores)
        }text_map;

typedef struct 
        {
        pixel A,B,C;                //coordenates
        pixel mA,mB,mC;             //texture maping
        char lightA,lightB,lightC;  //ground shading
        char lightT;                //flat and ground shadind
        text_map image;             //texture
        word color;                 //color (if no textutre)
        }triangle;

typedef struct 
        {
        vec3d a,b,c;                //coordenates
        pixel mA,mB,mC;             //texture maping
        char lightA,lightB,lightC;  //ground shading
        char lightT;                //flat and ground shadind
        text_map image;             //texture
        word color;                 //color (if no textutre)
        }triangle3d;


int use_polignos (void)
{
        LEFT=(scanline_bar*)calloc(Y_MAX+1,sizeof(scanline_bar));
        RIGHT=(scanline_bar*)calloc(Y_MAX+1,sizeof(scanline_bar));
        if (LEFT==NULL || RIGHT==NULL)
                {
                free (LEFT);
                free (RIGHT);
                return -1;
                }
        return 0;
}

void unuse_polignos (void)
{
                free (LEFT);
                free (RIGHT);
}


/*----------------------------triangulo-----------------------------------*/

void putxdir(int x,int y)
{
        if ( y>=0 && y<Y_MAX )
                  RIGHT[y].x=x;
}

void putxesq(int x,int y)
{
        if ( y>=0 && y<Y_MAX )
                 LEFT[y].x=x;
}

void line_poly_esq (int x1,int y1,int x2,int y2)
{
        int x, y, d, dx,dy,c , m, xinc, yinc;
        int xp,yp,xq,yq;
        if (x1<x2)
                {
                xp=x2;yp=y2;xq=x1;yq=y1;
                }
        else
                {
                xp=x1;yp=y1;xq=x2;yq=y2;
                }
        x=xp; y=yp; d=0; dx=xq-xp;dy=yq-yp;xinc=1;yinc=1;
	if(dx<0){xinc=-1;dx=-dx;}
	if(dy<0){yinc=-1;dy=-dy;}
	if(dy<dx)
	{
		c=2*dx;m=2*dy;
		while (x!=xq)
                {
                        x+=xinc;d+=m;
                        if (d>dx)
                                {
                                putxesq(x-xinc,y);
                                y+=yinc;d-=c;
                                }
		}
	}
	else
	{
		c=2*dy;m=2*dx;
		while (y!=yq)
		{
                        putxesq(x,y);
			y+=yinc;d+=m;
			if (d>dy){x+=xinc;d-=c;}
		}
	}
        putxesq(xq,yq);
}

void line_poly_dir (int x1,int y1,int x2,int y2)
{
        int x, y, d, dx,dy,c , m, xinc, yinc;
        int xp,yp,xq,yq;
        if (x1<x2) {xp=x1;yp=y1;xq=x2;yq=y2;} else {xp=x2;yp=y2;xq=x1;yq=y1;}

        x=xp;y=yp;d=0;dx=xq-xp;dy=yq-yp;xinc=1;yinc=1;
	if(dx<0){xinc=-1;dx=-dx;}
	if(dy<0){yinc=-1;dy=-dy;}
        if(dy<dx)                           
	{
		c=2*dx;m=2*dy;
		while (x!=xq)
                {
                        x+=xinc;d+=m;
                        if (d>dx)
                                {
                                putxdir(x-xinc,y);
                                y+=yinc;d-=c;
                                }
		}
	}
	else
	{
		c=2*dy;m=2*dx;
		while (y!=yq)
		{
                        putxdir(x,y);
			y+=yinc;d+=m;
			if (d>dy){x+=xinc;d-=c;}
		}
	}
        putxdir(xq,yq);
        
}



void flat_scanline (int i,char light,word color)
{
int aux;
if (LEFT[i].x>X_MAX || RIGHT[i].x<0 ) return;
if (LEFT[i].x<0) LEFT[i].x=0;
if (RIGHT[i].x>X_MAX) RIGHT[i].x=X_MAX;
aux=(X_MAX*i)+LEFT[i].x;
memset (VIRTUAL_SCREEN_8+aux,color,RIGHT[i].x-LEFT[i].x);
}

void fill_flat_triangle (triangle T)
        {
        pixel tri[3],aux;
        long int m1,m2;    //declives
        int lines,i;
        tri[0]=T.A;tri[1]=T.B;tri[2]=T.C;
        if (tri[0].y>tri[1].y) {aux=tri[0];tri[0]=tri[1];tri[1]=aux;}
        if (tri[0].y>tri[2].y) {aux=tri[0];tri[0]=tri[2];tri[2]=aux;}
        lines=tri[2].y;          //organiza (de cima para baixo)
        if (tri[1].y>(m1=tri[2].y)) lines=m1=tri[1].y;
        if (lines>Y_MAX) lines=Y_MAX;       //fim do poligno em y
        i=tri[0].y;
        if (i<0) i=0;                   //inicio do poligno em y
        if (tri[0].y>Y_MAX || m1<0) return; // se est  fora do ecrÆ sai
        m2=(tri[1].y-tri[0].y)*(tri[2].x-tri[0].x);   //maior m:lado direito
        m1=(tri[2].y-tri[0].y)*(tri[1].x-tri[0].x);

        if (m1>m2)
        {
        line_poly_dir(tri[0].x,tri[0].y,tri[1].x,tri[1].y);
        line_poly_esq(tri[0].x,tri[0].y,tri[2].x,tri[2].y);
        if ( tri[1].y < tri[2].y )
                line_poly_dir(tri[1].x,tri[1].y,tri[2].x,tri[2].y);
        else
                line_poly_esq(tri[1].x,tri[1].y,tri[2].x,tri[2].y);
        }        
        else
        {
        line_poly_esq(tri[0].x,tri[0].y,tri[1].x,tri[1].y);
        line_poly_dir(tri[0].x,tri[0].y,tri[2].x,tri[2].y);
        if ( tri[1].y > tri[2].y )
                line_poly_dir(tri[1].x,tri[1].y,tri[2].x,tri[2].y);
        else
                line_poly_esq(tri[1].x,tri[1].y,tri[2].x,tri[2].y);

        }


        do flat_scanline(i,T.lightT,T.color);
        while (i++<lines);
}




/*------------------------------texture-----------------------------------*/

void putxesqt(int tx,int ty,int y)
{
        if ( y>=0 && y<Y_MAX )
        {
                LEFT[y].X_texture=tx;
                LEFT[y].Y_texture=ty;
        }
}

void putxdirt(int tx,int ty,int y)
{
        if ( y>=0 && y<Y_MAX )
        {
                RIGHT[y].X_texture=tx;
                RIGHT[y].Y_texture=ty;
        }
}

void line_polytex_esq (int x1,int y1,int x2,int y2,
           int ftx1,int fty1,int ftx2,int fty2)
{
        int tx,ty;
        int tdx;
        int tdy;
        int x, y, d, dx,dy,c , m, xinc, yinc;
        int xp,yp,xq,yq;
        int tx1,ty1,tx2,ty2;   //long
        if (x1<x2)
        {
                tx1=ftx2;ty1=fty2;tx2=ftx1;ty2=fty1;
                xp=x2;yp=y2;xq=x1;yq=y1;
        }
        else
        {
                tx1=ftx1,ty1=fty1,tx2=ftx2,ty2=fty2;
                xp=x1;yp=y1;xq=x2;yq=y2;
        }

        tx1=(tx1<<8);  // 2-bytes,2-bytes
        ty1=(ty1<<8);  //  pixel , aproxima‡Æo
        tx2=(tx2<<8);
        ty2=(ty2<<8);

        x=xp; y=yp; d=0; dx=xq-xp; dy=yq-yp;xinc=1; yinc=1;
	if(dx<0){xinc=-1;dx=-dx;}
	if(dy<0){yinc=-1;dy=-dy;}
	if(dy<dx)
	{
		c=2*dx;m=2*dy;
		while (x!=xq)
                {
                        x+=xinc;d+=m;
                        if (d>dx)
                                {
                                putxesq(x-xinc,y);
                                y+=yinc;d-=c;
                                }
		}
	}
	else
	{
		c=2*dy;m=2*dx;
		while (y!=yq)
		{
                        putxesq(x,y);
			y+=yinc;d+=m;
			if (d>dy){x+=xinc;d-=c;}
		}
	}
        putxesq(xq,yq);

        if (yp<yq) d=1; else d=-1;
        tx=tx1;ty=ty1;
        tdx=tx2-tx1;
        tdy=ty2-ty1;
        if (dy!=0) 
        {
                tdx=tdx/dy;
                tdy=tdy/dy;
                for (;dy>0;dy--)
                {
                   putxesqt(tx,ty,yp);
                   yp+=d;
                   ty+=tdy;
                   tx+=tdx;
                   
                }
        }
        putxesqt(tx2,ty2,yq);
}

void line_polytex_dir (int x1,int y1,int x2,int y2,
           int ftx1,int fty1,int ftx2,int fty2)
{
        int tx,ty;
        int tdx;
        int tdy;
        int x, y, d, dx,dy,c , m, xinc, yinc;
        int xp,yp,xq,yq;
        int tx1,ty1,tx2,ty2;   //long
        if (x1<x2)
        {
                tx1=ftx1,ty1=fty1,tx2=ftx2,ty2=fty2;
                xp=x1;yp=y1;xq=x2;yq=y2;
        }
        else
        {
                tx1=ftx2;ty1=fty2;tx2=ftx1;ty2=fty1;
                xp=x2;yp=y2;xq=x1;yq=y1;
        }

        tx1=(tx1<<8);  // 2-bytes,2-bytes
        ty1=(ty1<<8);  //  pixel , aproxima‡Æo
        tx2=(tx2<<8);
        ty2=(ty2<<8);

        x=xp; y=yp; d=0; dx=xq-xp;dy=yq-yp;xinc=1; yinc=1;
	if(dx<0){xinc=-1;dx=-dx;}
	if(dy<0){yinc=-1;dy=-dy;}
	if(dy<dx)
	{
		c=2*dx;m=2*dy;
		while (x!=xq)
                {
                        x+=xinc;d+=m;
                        if (d>dx)
                                {
                                putxdir(x-xinc,y);
                                y+=yinc;d-=c;
                                }
		}
	}
	else
	{
		c=2*dy;m=2*dx;
		while (y!=yq)
		{
                        putxdir(x,y);
			y+=yinc;d+=m;
			if (d>dy){x+=xinc;d-=c;}
		}
	}
        putxdir(xq,yq);

        if (yp<yq) d=1; else d=-1;
        tx=tx1;ty=ty1;
        tdx=tx2-tx1;
        tdy=ty2-ty1;
        if (dy!=0) 
        {
                tdx=tdx/dy;
                tdy=tdy/dy;
                for (;dy>0;dy--)
                {
                   putxdirt(tx,ty,yp);
                   yp+=d;
                   ty+=tdy;
                   tx+=tdx;
                   
                }
        }
        putxdirt(tx2,ty2,yq);
}

void putpixelt (int x,int y,text_map texture,int tx,int ty,char light)
{
        int xt,yt;
        xt=tx>>8;
        yt=ty>>8;
        xt=xt & (texture.X_size-1);
        yt=yt & (texture.Y_size-1);
        if (x<X_MAX && x>=0)
 VIRTUAL_SCREEN_8[(X_MAX*y)+x]=(texture.map[((texture.X_size*yt)+xt)]);
}

void flat_tex_scanline (text_map texture,int i,char light)
{
     int dif;
     int tx1;
     int ty1;
     int tx2;
     int ty2;
     int tx,ty;
     int tdx;
     int tdy;

     dif=RIGHT[i].x-LEFT[i].x;
        if (dif!=0) {
        tx1=LEFT[i].X_texture;
        ty1=LEFT[i].Y_texture;
        tx2=RIGHT[i].X_texture;
        ty2=RIGHT[i].Y_texture;
        tx=tx1;ty=ty1;
        tdx=tx2-tx1;
        tdy=ty2-ty1;
        tdx=tdx/dif;tdy=tdy/dif;
        for (dif=LEFT[i].x;dif<RIGHT[i].x;dif++)
        {
                putpixelt(dif,i,texture,tx,ty,light);
                ty+=tdy;
                tx+=tdx;
        }
        }
//    putpixelt(LEFT[i].x,i,texture,LEFT[i].X_texture,LEFT[i].Y_texture,light);
    putpixelt(RIGHT[i].x,i,texture,RIGHT[i].X_texture,RIGHT[i].Y_texture,light);
} 


void fill_flat_tex_triangle (triangle T){
        int lines;
        int i,m1;
        pixel tri[3],aux;   // coordenadas do ecrÆ
        pixel mtri[3]; // coordenadas da textura
        tri[0]=T.A;tri[1]=T.B;tri[2]=T.C;
        mtri[0]=T.mA;mtri[1]=T.mB;mtri[2]=T.mC;
        if (tri[0].y>tri[1].y)
        {
        aux=tri[0];tri[0]=tri[1];tri[1]=aux;
        aux=mtri[0];mtri[0]=mtri[1];mtri[1]=aux;
        }
        if (tri[0].y>tri[2].y)
        {
        aux=tri[0];tri[0]=tri[2];tri[2]=aux;
        aux=mtri[0];mtri[0]=mtri[2];mtri[2]=aux;
        }
        lines=tri[2].y;          //organiza (de cima para baixo)
        if (tri[1].y>(m1=tri[2].y)) lines=m1=tri[1].y;
        if (lines>Y_MAX1) lines=Y_MAX1;       //fim do poligno em y
        i=tri[0].y;
        if (i<0) i=0;                   //inicio do poligno em y
        if (tri[0].y>Y_MAX || m1<0) return; // se est  fora do ecrÆ sai
        if ((long)((tri[2].y-tri[0].y)*(tri[1].x-tri[0].x)) >
            (long)((tri[1].y-tri[0].y)*(tri[2].x-tri[0].x)) )
        {
        line_polytex_dir(tri[0].x,tri[0].y,tri[1].x,tri[1].y,
                               mtri[0].x,mtri[0].y,mtri[1].x,mtri[1].y );
        line_polytex_esq(tri[0].x,tri[0].y,tri[2].x,tri[2].y,
                               mtri[0].x,mtri[0].y,mtri[2].x,mtri[2].y);
        if ( tri[1].y < tri[2].y )
                line_polytex_dir(tri[1].x,tri[1].y,tri[2].x,tri[2].y,
                               mtri[1].x,mtri[1].y,mtri[2].x,mtri[2].y );
        else
                line_polytex_esq(tri[1].x,tri[1].y,tri[2].x,tri[2].y,
                              mtri[1].x,mtri[1].y,mtri[2].x,mtri[2].y );
        }        
        else
        {
        line_polytex_esq(tri[0].x,tri[0].y,tri[1].x,tri[1].y,
                               mtri[0].x,mtri[0].y,mtri[1].x,mtri[1].y );
        line_polytex_dir(tri[0].x,tri[0].y,tri[2].x,tri[2].y,
                               mtri[0].x,mtri[0].y,mtri[2].x,mtri[2].y);
        if ( tri[1].y > tri[2].y )
                line_polytex_dir(tri[1].x,tri[1].y,tri[2].x,tri[2].y,
                               mtri[1].x,mtri[1].y,mtri[2].x,mtri[2].y );
        else
                line_polytex_esq(tri[1].x,tri[1].y,tri[2].x,tri[2].y,
                              mtri[1].x,mtri[1].y,mtri[2].x,mtri[2].y );

        }
        do flat_tex_scanline(T.image,i,T.lightT);
        while (i++<lines);
} 

/*-----------------------------------------------------------*/

void unload_map (text_map texture)
{
        free (texture.map);
}

text_map load_map (char name[]) //*.tga
{
        int x,y,total;
        text_map text;
        FILE *Data=fopen (name,"rb");
        int a[2];
        if (Data==NULL) printf ("n abrio o ficheiro\n"); 
        fseek (Data,12,SEEK_SET);
        fread (a,sizeof(int),2,Data);
        text.X_size=a[0];
        text.Y_size=a[1];
       /* x=a[0];
        y=a[1];*/
       
        x=64;
        y=128;


        printf("x:%d\n",x);
        printf("y:%d\n",y);

        total=x*y;
        fseek (Data,16+768,SEEK_SET);
        text.map=(byte*)calloc(total,sizeof(byte));
        printf ("total:%d\n",total);
        getch();
        fread ((byte*)text.map,1,total,Data);
        fclose (Data);
        return text;
}

text_map uma (void)
{
        int tam=128;
        text_map t;
        int i,j;
        t.map=(byte*)calloc(tam*tam,sizeof(byte));
        for (i=0;i<tam;i++)
                for (j=0;j<tam;j++)
                        t.map [((i*tam) + j)] = j+30;
        for (i=tam/4;i<tam/3;i++)
                for (j=10;j<tam/3;j++)
                        t.map [((i*tam) + j)] = 4;
        
        t.Y_size=tam;
        t.X_size=tam;
        return t;
}

void put_image (text_map t,pixel pos)
{
        int i,j;
        for (j=0;j<t.Y_size;j++)
         for (i=0;i<t.X_size;i++)
          {
          setbpixel(i,j,t.map [((i*t.X_size) + j)]);
          }

} 

triangle tri3d_to_tripix (triangle3d t,vec3d camera)
{
        triangle aux;
        aux.A=vec3_to_pixel(t.a,camera);
        aux.B=vec3_to_pixel(t.b,camera);
        aux.C=vec3_to_pixel(t.c,camera);
        aux.mA=t.mA;
        aux.mB=t.mB;
        aux.mC=t.mC;
        aux.lightA=t.lightA;
        aux.lightB=t.lightB;
        aux.lightC=t.lightC;
        aux.lightT=t.lightT;
        aux.image=t.image;
        aux.color=t.color;
        return aux;
}

void main (void)
{

        int t_on_off=1;
        vec3d cam=vec3(-10.,-10.,20.);     //camera (visto de cima)
        char ch=0;
        triangle t;
        triangle3d t1,t2,t3,t4;
        text_map text;
        text=uma ();

        // T-coord.         // M-coord.
        t1.a=vec3 (0,0,0);   t1.mA=pix (0,0);
        t1.b=vec3 (10,0,0);  t1.mB=pix (0,6*(text.Y_size-1));
        t1.c=vec3 (0,10,0);  t1.mC=pix (6*(text.Y_size-1),0);

        t1.image=text; //texture
        t1.lightT=0x0; // luz recebida
        t1.color=11; // cor

        // T-coord.         // M-coord.
        t2.a=vec3 (10,10,0);   t2.mA=pix (6*(text.X_size-1),6*(text.Y_size-1));
        t2.b=vec3 (10,0,0);  t2.mB=pix (0,6*(text.X_size-1));
        t2.c=vec3 (0,10,0);  t2.mC=pix (6*(text.Y_size-1),0); 

        t2.image=text; //texture
        t2.lightT=0x0; // luz recebida
        t2.color=12; // cor


        // T-coord.         // M-coord.
        t3.a=vec3 (10,10,0);   t3.mA=pix (text.X_size-1,0);
        t3.b=vec3 (10,0,0);  t3.mB=pix (0,text.Y_size-1);
        t3.c=vec3 (10,10,-10);  t3.mC=pix (text.X_size-1,text.Y_size-1);

        t3.image=text; //texture
        t3.lightT=0x0; // luz recebida
        t3.color=13; // cor

        // T-coord.         // M-coord.
        t4.a=vec3 (0,10,0);   t4.mA=pix (0,5*(text.X_size-1));
        t4.b=vec3 (0,0,0);  t4.mB=pix (0,0);
        t4.c=vec3 (0,10,-10);  t4.mC=pix (5*(text.X_size-1),5*(text.Y_size-1));

        t4.image=text; //texture
        t4.lightT=0x0; // luz recebida
        t4.color=14; // cor


      set_video (VGA_320_200_8);  //ligando tudo 
      use_VIRTUAL_SCREEN ();
      use_polignos ();

        while (ch!='q')
        {

              put_image (text,pix(10,10));
              if (t_on_off>0)
              {
              fill_flat_triangle (tri3d_to_tripix (t4,cam));   // encher o triangulo
              fill_flat_triangle (tri3d_to_tripix (t3,cam));   // encher o triangulo
              fill_flat_triangle (tri3d_to_tripix (t1,cam));   // encher o triangulo
              fill_flat_triangle (tri3d_to_tripix (t2,cam));   // encher o triangulo
              }
              else
              {
              fill_flat_tex_triangle (tri3d_to_tripix (t4,cam));   // encher o triangulo
              fill_flat_tex_triangle (tri3d_to_tripix (t3,cam));   // encher o triangulo
              fill_flat_tex_triangle (tri3d_to_tripix (t1,cam));   // encher o triangulo
              fill_flat_tex_triangle (tri3d_to_tripix (t2,cam));   // encher o triangulo
              }


              VS_TO_RS_VGA_320_200_8();  // passar de ecra virtual para ecra real

        ch=getch();
        switch (ch)
		{
//                case 'e' : t1.A.y--;t1.B.y--;t1.C.y--;break;
//                case 's' : t1.A.y++;t1.B.y++;t1.C.y++;break;

                case '5' :  cam.y--;/*y++;*/ break;
                case '8' :  cam.y++;/*y--;*/ break;
                case '4' :  cam.x--;/*x--;*/ break;
                case '6' :  cam.x++;/*x++;*/ break;
                case '-' :  cam.z--;/*x--;*/ break;
                case '+' :  cam.z++;/*x++;*/ break;
                case 't' :  t_on_off*=-1;/*x++;*/ break;

                }
        }
      

      unuse_polignos ();          // desligando tudo
      unuse_VIRTUAL_SCREEN ();
      unset_video ();

}
