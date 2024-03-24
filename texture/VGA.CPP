
        /*********************************************************
         *                                                       *
         *   Fast VGA(320*200*256) +-+ VGA.CPP made by X-prog    *
         *                                                       *
         *   NOTE:Run in Real Mode and use 32-Bit operations.    *
         *        To compile in BC5 you must do this             *
         *        bcc -3 -B -ml program.cpp                      *
         *                                                       *
         *    Example of program.cpp :                           *
         *    +----------------------------+                     *
         *    |#include "vga.cpp"          | <<---this file      *
         *    |#include <stdio.h>          |                     *
         *    |                            |                     *
         *    |void main (void)       ...  |                     *
         *    +----------------------------+                     *
         *                                                       *
         *    Used for fast 2D and 3D graphics on VGA dispaly    *
         *                                                       *
         *                                                       *
         *  Author:Rui Jorge Pereira Gon‡alves (c) 1998 X-prog   *
         *********************************************************/

#if defined (__TINY__)
#error "Modelo de mem¢ria inv lido."
#endif


#if defined (__SMALL__)
#error "Modelo de mem¢ria inv lido."
#endif

#if defined (__MEDIUM__)
#error "Modelo de mem¢ria inv lido."
#endif

#define word unsigned int
#define byte unsigned char
#define dword unsigned long int


/*--------------------------------------------------------------------------*/
//----------------Static var veis
char far *VIRTUAL_SCREEN; //for fast 2D-graphics
int VS_SEG, VS_OFF; //seguemento e offset de *VIRTUAL_SCREEN
int ON_3D=1, VGA_ON=1;  // (...=0)->ON (...!=0)->OFF


struct scanline_bar          //for fast 3D-graphics
        {
        int x;                      //coordenada x 
        int X_texture,Y_texture;    // coordenadas da textura << 8
        int light;                // only for ground shading...
        };

scanline_bar *LEFT,*RIGHT;

struct text_map
        {
        word X_size;  // tamanho x
        word Y_size;  // tamanho y
        byte *map;    // bitmap (cada byte ‚ um pixel, independente das cores)
        };

struct point
        {
        int x,y;                   //coodenadas no ecrÆ
        point(int xx,int yy) {x=xx;y=yy;}
        point(){x=0;y=0;}
        };


struct triangle
        {
        point A,B,C;                //coordenates
        point mA,mB,mC;             //texture maping
        char lightA,lightB,lightC;  //ground shading
        char lightT;                //flat and ground shadind
        text_map image;             //texture
        char color;                 //color (if no textutre)
        };

/*--------------------------------------------------------------------------*/
// declara‡äes :

//---------------activar / desactivar
int DESACTIVATE_3D(void);
// desactivar modo de acelara‡Æo 3D
int ACTIVATE_3D (void);
// activar modo de acelara‡Æo 3D
int set_vga(void);
// activar modo vga
int set_text(void);
// desactivar modo vga

//---------------trabalhos directos com o ecrÆ
void waitretrace(void);
// esperar pelo retra‡amento vertical (60 FPS)
void putpixel(short int X,short int Y,char Col);
// por um pixel no ecrÆ
void clean(char valor);
// lipar o ecrÆ com a cor valor 
void putmem(char *mem);
// por 64000 bytes no ecrÆ real

//---------------trabalho com o ecrÆ virtual
void back_to_screen(void);
// por back_screen no ecrÆ real 
void back_putmem(char *mem);
// por 64000 bytes em back_screen
void back_clean(char valor);
// limpar back_screen com a cor valor
void setmemo_VS(int where,char valor,int number);
//setar number bytes em VIRTUAL_SCREEN a partir de where 

//---------------trabalho com a palete
void getregister(char color,char &red,char &green,char &blue);
// tirar o R.G.B de uma cor da palete
void getblockregister(int start,int colors,int *pal);
// tirar o R.G.B. de um bloco de cores
void setregister(int color,char green,char red,char blue);
// actualizar o R.G.B de uma cor de palete
void setblockregister(int start,int max_colors,char *pal);
// actualizar o R.G.B de um bloco de cores da palete

//---------------trabalho com a mem¢ria
void cpymem(char *dest,char *from,int bytes);
// copia mem¢ria a 32-bits (int bytes -deve ser multiplo de 4)
void setmemo(char *p,char valor,int number);
// setar mem¢ria a 32-bits (int number -deve ser multiplo de 4)
void setmemodb(char *p,char valor,int number);
// setar mem¢ria a 8-bits (Mas bastante r pido!!)
void cpymemdb(char *dest,char *from,int bytes);
// copiar mem¢ria a 8-bits (Mas bastante r pido!!)
void cpymemnoz (char *dest,char *from,int bytes);
// copiar mem¢ria a 8-bits avan‡ando nos zeros
int pointer_OFF(void *p);
// tirar offset de um apontador
int pointer_SEG(void *p);
// tirar segmento de um apontador

//---------------Janelas
void putwindow(char *p,int tx,int ty,int xs,int ys);
// por uma janela directamente e rapidamente no ecrÆ.
// (s¢ funciona bem com tamanhos multiplos de 4 e com toda a
// janela dendro do ecrÆ)
void putwindowmemnoz(char *dest,int dtx,int dty,char *from,int ftx,int fty,int wherex,int wherey);
// no fundo ‚: por um "sprit" numa …rea de mem¢ria.
void putwindowmem(char *dest,int dtx,int dty,char *from,int ftx,int fty,int wherex,int wherey);
// por uma janela numa …rea de mem¢ria

//----------------Triƒngulos
void ver_lados(void);
int orientation (point &P,point &Q,point &R);

// em flat e sem textura
void putxdir(int x,int y);
void putxesq(int x,int y);
void line_poly_esq (int x1,int y1,int x2,int y2);
void line_poly_dir (int x1,int y1,int x2,int y2);
void fill_flat_triangle (triangle &T);  // principal
void flat_scanline(int i,char color);

// em flat e com textura
void putxdirt(int x,int y,long int tx,long int ty);
void putxesqt(int x,int y,long int tx,long int ty);
void line_polytex_dir (int x1,int y1,int x2,int y2,
                        int ftx1,int fty1,int ftx2,int fty2);
void line_polytex_dir (int x1,int y1,int x2,int y2,
                        int ftx1,int fty1,int ftx2,int fty2);
void putxesqt(int tx,int ty,int y);
void putxdirt(int tx,int ty,int y);
void flat_tex_scanline (text_map &texture,int i,char light);
void putpixelt (int x,int y,text_map &texture,int tx,int ty,char light);
void fill_flat_tex_triangle (triangle &T);

//em ground shading, sem textutra

//em ground shading, com textutra

/*--------------------------------------------------------------------------*/
// Implementa‡Æo

int DESACTIVATE_3D(void)
{
if (ON_3D) return -1;
delete LEFT;
delete RIGHT;
ON_3D=1;
return 0;
}  


int ACTIVATE_3D (void)
{
if (VGA_ON) return ON_3D;
if (!ON_3D) return ON_3D;
LEFT=new struct scanline_bar[200];
RIGHT=new struct scanline_bar[200];
ON_3D=0;
if (!LEFT) {DESACTIVATE_3D();return -1;}
if (!RIGHT) {DESACTIVATE_3D();return -1;}
for (int i=0;i<200;i++)
        {RIGHT[i].x=319;LEFT[i].x=0;}
return ON_3D;
}


/*slow*/
int pointer_SEG(void *p)
{
        int *w=(int*)&p;
        return *(w+1);
}

/*slow*/
int pointer_OFF(void *p)
{
        int *w=(int*)&p;
        return *w;
}

int set_vga(void)
{
        if(!VGA_ON) return 0;
        VIRTUAL_SCREEN=new char far [64000];
        if (!VIRTUAL_SCREEN) return -1;
        VS_SEG=pointer_SEG(VIRTUAL_SCREEN);
        VS_OFF=pointer_OFF(VIRTUAL_SCREEN);
        back_clean(0);
        asm
        {
           mov ax,13h
	   int 010h
         }

         VGA_ON=0;
         return VGA_ON;
}

int set_text(void)
{
        if(VGA_ON) return -1;
        VGA_ON=-1;
        delete VIRTUAL_SCREEN;
        DESACTIVATE_3D();
        asm
        {
           mov ax,3h
	   int 010h
        }
        return 0;
}

void waitretrace(void)
{
asm{
    mov dx,3DAh
@l1:
    in al,dx
    and al,08h
    jnz @l1
@l2:
    in al,dx
    and al,08h
    jz  @l2
  }
}

void putpixel(short int X,short int Y,char Col)
{
        asm
        {
    mov      ax,0a000h
    mov      es,ax
    mov      bx,[X]
    mov      dx,[Y]
    mov      di,bx
    mov      bx, dx
    shl      dx, 8
    shl      bx, 6
    add      dx, bx
    add      di, dx
    mov      al, [Col]
    mov es:[di],al

        }
}

void putpixel_back(short int X,short int Y,char Col)
{
    VIRTUAL_SCREEN[(320*Y)+X]=Col;
    /*
        asm
        {
    mov      ax,VS_SEG
    mov      es,ax
    mov      bx,[X]
    mov      dx,[Y]
    mov      di,bx
    mov      bx, dx
    shl      dx, 8
    shl      bx, 6
    add      dx, bx
    add      di, dx
    mov      al, [Col]
    mov es:[di],al

        }*/
}


void back_clean(char valor)
{
asm{
     push es
     push di             
     mov ax,VS_SEG
     mov es,ax       
     mov ax,VS_OFF
     mov di,ax       
     mov al,valor
     shl EAX,08h
     mov al,valor
     shl EAX,08h
     mov al,valor
     shl EAX,08h
     mov al,valor
     mov cx,03e80h 
     rep stosd    
     pop di       
     pop es       
    }
}

void back_to_screen(void)
{
     asm
        {
    mov dx,3DAh
@l7:
    in al,dx
    and al,08h
    jnz @l7
@l8:
    in al,dx
    and al,08h
    jz  @l8
    push si
    push di
    push ds
    push es
    mov SI,VS_OFF
    mov DS,VS_SEG
    mov BX,0a000h
    mov ES,BX
    mov BX,00000h
    mov DI,Bx
    mov CX,03e80h
    rep movsd
    pop es
    pop ds
    pop di
    pop si
	}

}
//por 64000 bytes em VIRTUAL_SCREEN
void back_putmem(char *mem)
{
     asm
        {             
    push si       // segurar o programa
    push di
    push ds
    push es

    mov AX,VS_OFF // SEG e OFF de *VIRTUAL_SCREEN
    mov DI,AX
    mov ES,VS_SEG

    mov EAX,mem   // SEG e OFF de *mem
    mov SI,AX;
    sar EAX,010h
    mov DS,AX;


    mov CX,03e80h //c¢pia de mem¢ria
    rep movsd

    pop es        // repor o programa 
    pop ds
    pop di
    pop si
	}

}



/*Very fast and perfect*/
void clean(char valor)
{
asm{
     mov dx,3DAh
@l5:
     in al,dx
     and al,08h
     jnz @l5
@l6:
     in al,dx
     and al,08h
     jz  @l6
     push es
     push di             
     mov ax,0A000h
     mov es,ax       
     mov ax,00000h
     mov di,ax       
     mov al,valor
     shl EAX,08h
     mov al,valor
     shl EAX,08h
     mov al,valor
     shl EAX,08h
     mov al,valor
     mov cx,03e80h 
     rep stosd    
     pop di       
     pop es       
    }
}

/* very fast and perfect*/
/* P|r 64000 bytes de *mem no ecra*/
void putmem(char *mem)
{
     asm
	{
    mov dx,3DAh
@l3:
    in al,dx
    and al,08h
    jnz @l3
@l4:
    in al,dx
    and al,08h
    jz  @l4
    push si
    push di
    push ds
    push es
    mov EAX,mem
    mov SI,AX;
    sar EAX,010h
    mov DS,AX;
    mov BX,0a000h
    mov ES,BX
    mov BX,00000h
    mov DI,Bx
    mov CX,03e80h
    rep movsd
    pop es
    pop ds
    pop di
    pop si
	}

}

void getregister(char color,char &red,char &green,char &blue)
{
asm {
    mov ax,01015h
    mov bl,color
    int 010h
    mov red,dh
    mov green,ch
    mov blue,cl
    }

}

void getblockregister(int start,int colors,int *pal)
{
asm {
    mov bx,start
    mov cx,colors
    mov EAX,pal
    mov dx,AX
    sar EAX,010h
    mov es,AX
    mov ax,01017h
    int 010h
    }

}


/* Slow */
void setregister(int color,char red,char green,char blue)
{
asm {
    mov bx,color
    mov ch,green
    mov cl,blue
    mov dh,red
    mov ax,01010h
    int 010h
    }
}

void setblockregister(int start,int max_colors,char *pal)
{
asm {
    mov EAX,pal
    mov dx,AX;
    sar EAX,010h
    mov es,AX;
    mov bx,start
    mov cx,max_colors
    mov ax,01012h
    int 010h
    }
}

/*-------------------------------------------------------------------*/
/* As fun‡äes seguintes nÆo sÆo de confian‡a...                      */
/* A vantagem em utilizalas ‚ a super rapidez que possuem !!!        */
/*-------------------------------------------------------------------*/

/* Very fast BUT NOT perfect*/
/* ATEN€ÇO esta fun‡Æo s¢ trabalha bem se a janela estiver
   toda dentro do ecra e os tamanhos forem multiplos de 4*/
void putwindow(char *p,int tx,int ty,int xs,int ys)
{
asm {
    push si
    push di
    push ds
    push es
    mov EAX,p         // por segmento de p em ds e offset em si
    mov si,AX;
    sar EAX,010h
    mov ds,AX;
    mov BX,0a000h     // ; por A000 em es e (xs+(ys*320)) em di
    mov ES,BX
    mov dX,00000h+xs
    mov CX,ys
@label2:
    add dX,320
    loop @label2
    mov DI,dX
    mov bx,tx         // ; quantos DWORDS em cada linha  => bx
    sar bx,02h        // ; { bx=(bx / 4) }
    mov cx,ty         // ; Colunas a copiar
@label1:              // ; ciclo para a linhas
    push cx
    mov cx,bx
    rep movsd         // ; por a linha que tem bx DWORDS
    pop cx
    add dx,320        // ;avan‡ar uma linha
    mov DI,dx
    loop @label1      // ; repete ate por toda a janela
    pop es
    pop ds
    pop di
    pop si
    }
}

/*Very fast but not perfect*/
/* Esta fun‡Æo nÆo olha a erros.
   ‚ aconselhavel que -int bytes- seja um numero multiplo de 4.*/   
void cpymem(char *dest,char *from,int bytes)
{
     asm
	{
    push si
    push di
    push ds
    push es
    mov EAX,from
    mov SI,AX;
    sar EAX,010h
    mov DS,AX;

    mov EAX,dest
    mov DI,AX;
    sar EAX,010h
    mov ES,AX;

    mov CX,bytes
    sar CX,02h
    rep movsd
    pop es
    pop ds
    pop di
    pop si
	}

}

/*Very Fast but not perfect*/
/* ‚ aconselhavel que -int number- seja um numero par*/
void setmemo(char *p,char valor,int number)
{
asm{
     push es
     push di             
     mov EAX,p
     mov DI,AX;
     sar EAX,010h
     mov ES,AX;
     mov al,valor
     shl EAX,08h
     mov al,valor
     shl EAX,08h
     mov al,valor
     shl EAX,08h
     mov al,valor
     mov cx,number
     sar CX,02h
     rep stosd    
     pop di       
     pop es       
    }
}

/*Fast and perfect*/
void setmemodb(char *p,char valor,int number)
{
asm{
     push es
     push di             
     mov EAX,p
     mov DI,AX
     sar EAX,010h
     mov ES,AX
     mov al,valor
     shl EAX,08h
     mov al,valor
     shl EAX,08h
     mov al,valor
     shl EAX,08h
     mov al,valor
     mov dx,number
     mov cx,dx
     sar CX,02h
     rep stosd    
     and dx,00003h
     mov cx,dx
     rep stosb
     pop di       
     pop es       
    }
}

/*Fast and perfect*/
void cpymemdb(char *dest,char *from,int bytes)
{
     asm
	{
    push si
    push di
    push ds
    push es
    mov EAX,from
    mov SI,AX
    sar EAX,010h
    mov DS,AX
    mov EAX,dest
    mov DI,AX
    shr EAX,010h
    mov ES,AX
    mov dx,bytes
    mov CX,dx
    shr cx,02h
    rep movsd
    and dx,00003h
    mov cx,dx
    rep movsb
    pop es
    pop ds
    pop di
    pop si
	}
}

void cpymemnoz (char *dest,char *from,int bytes)
{
asm {
    push si
    push di
    push ds
    push es
    mov EAX,from
    mov SI,AX
    sar EAX,010h
    mov DS,AX
    mov EAX,dest
    mov DI,AX
    shr EAX,010h
    mov ES,AX
    mov cx,bytes
@label2rest:
    lodsb
    cmp al,000h
    jz @zero2
    stosb
    jmp @no2zero2
    @zero2:
    inc di
    @no2zero2:
    loop @label2rest 
    pop es
    pop ds
    pop di
    pop si
    }
}


/*Fast and perfect*/
void putwindowmemnoz(char *dest,int dtx,int dty,char *from,int ftx,int fty,int wherex,int wherey)
{
        if (wherex>dtx || wherey>dty || wherey+fty<0 || wherex+ftx<0)
                return;
        int in=-wherey,inx=-wherex,aux,byteline;
        byteline=dtx-wherex;
        if (byteline>ftx)
                byteline=ftx;
        if (in<0)
                in=0; 
        if (inx<0)
                inx=0;
        in--;
        aux=wherey+in;
        for (int i=in+1;i<fty && aux<dty;i++)
                {
                aux++;
                cpymemnoz (dest+(aux*dtx)+wherex+inx,from+(i*ftx)+inx,byteline-inx);
                }
}

/*Fast and perfect*/
void putwindowmem(char *dest,int dtx,int dty,char *from,int ftx,int fty,int wherex,int wherey)
{
        if (wherex>dtx || wherey>dty || wherey+fty<0 || wherex+ftx<0)
                return;
        int in=-wherey,inx=-wherex,aux,byteline;
        byteline=dtx-wherex;
        if (byteline>ftx)
                byteline=ftx;
        if (in<0)
                in=0; 
        if (inx<0)
                inx=0;
        in--;
        aux=wherey+in;
        for (int i=in+1;i<fty && aux<dty;i++)
                {
                aux++;
                cpymemdb (dest+(aux*dtx)+wherex+inx,from+(i*ftx)+inx,byteline-inx);
                }
}


/*--------------------------------------------------------------------------*/

void putxdir(int x,int y)
{
        if ( y>=0 && y<200 )
                  RIGHT[y].x=x;
}

void putxesq(int x,int y)
{
        if ( y>=0 && y<200 )
                 LEFT[y].x=x;
}

void line_poly_esq (int x1,int y1,int x2,int y2)
{       int xp,yp,xq,yq;
        if (x1<x2) {xp=x2;yp=y2;xq=x1;yq=y1;} else {xp=x1;yp=y1;xq=x2;yq=y2;}

        int x=xp, y=yp, d=0, dx=xq-xp,dy=yq-yp,c , m, xinc=1, yinc=1;
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
{       int xp,yp,xq,yq;
        if (x1<x2) {xp=x1;yp=y1;xq=x2;yq=y2;} else {xp=x2;yp=y2;xq=x1;yq=y1;}

        int x=xp, y=yp, d=0, dx=xq-xp,dy=yq-yp,c , m, xinc=1, yinc=1;
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

void setmemo_VS(int where,char valor,int number)
{
asm{
     push es
     push di             
     mov DI,VS_OFF
     add DI,where
     mov ES,VS_SEG
     mov al,valor
     shl EAX,08h
     mov al,valor
     shl EAX,08h
     mov al,valor
     shl EAX,08h
     mov al,valor
     mov dx,number
     inc dx
     mov cx,dx
     sar CX,02h
     rep stosd    
     and dx,00003h
     mov cx,dx
     rep stosb
     pop di       
     pop es       
    }
}


void flat_scanline (int i,char light,char color)
{
if (LEFT[i].x>320 || RIGHT[i].x<0 ) return; 
if (LEFT[i].x<0) LEFT[i].x=0;
if (RIGHT[i].x>320) RIGHT[i].x=320;
int aux=(320*i)+LEFT[i].x;
setmemo_VS(aux,(color<<4)+(light&0x0F),RIGHT[i].x-LEFT[i].x);
}

int orientation (point &P,point &Q,point &R)
{
        const double EPS=1e-6;
        point A,B;
        long int d;
        A.x=Q.x-P.x;A.y=Q.y-P.y;
        B.x=R.x-P.x;B.y=R.y-P.y;
        d=A.x*B.y-A.y*B.x;
        return (d < -EPS ? -1 : d > EPS);
}

void fill_flat_triangle (triangle &T){
        point tri[3],aux;
        long int m1,m2;    //declives
        int lines;
        tri[0]=T.A;tri[1]=T.B;tri[2]=T.C;
        if (tri[0].y>tri[1].y) {aux=tri[0];tri[0]=tri[1];tri[1]=aux;}
        if (tri[0].y>tri[2].y) {aux=tri[0];tri[0]=tri[2];tri[2]=aux;}
        lines=tri[2].y;          //organiza (de cima para baixo)
        if (tri[1].y>tri[2].y) lines=tri[1].y;
        if (lines>200) lines=200;       //fim do poligno em y
        int i=tri[0].y;
        if (i<0) i=0;                   //inicio do poligno em y
        if (tri[0].y>200 || tri[2].y<0) return; // se est  fora do ecrÆ sai
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


void ver_lados(void)
{
int i;
for (i=0;i<200;i++)
        {
        putpixel_back(LEFT[i].x,i,14);
        putpixel_back(RIGHT[i].x,i,15);
        }
}



/*------------------------------texture-----------------------------------*/

void putxesqt(int tx,int ty,int y)
{
        if ( y>=0 && y<200 )
        {
                LEFT[y].X_texture=tx;
                LEFT[y].Y_texture=ty;
        }
}

void putxdirt(int tx,int ty,int y)
{
        if ( y>=0 && y<200 )
        {
                RIGHT[y].X_texture=tx;
                RIGHT[y].Y_texture=ty;
        }
}

void line_polytex_esq (int x1,int y1,int x2,int y2,
           int ftx1,int fty1,int ftx2,int fty2)
{
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

        int x=xp, y=yp, d=0, dx=xq-xp,dy=yq-yp,c , m, xinc=1, yinc=1;
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
        int tx=tx1,ty=ty1;
        int tdx=tx2-tx1;
        int tdy=ty2-ty1;
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

        int x=xp, y=yp, d=0, dx=xq-xp,dy=yq-yp,c , m, xinc=1, yinc=1;
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
        int tx=tx1,ty=ty1;
        int tdx=tx2-tx1;
        int tdy=ty2-ty1;
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

void putpixelt (int x,int y,text_map &texture,int tx,int ty,char light)
{
        int xt,yt;
        xt=tx>>8;
        yt=ty>>8;
        xt=xt & (texture.X_size-1);
        yt=yt & (texture.Y_size-1);
        if (x<320 && x>=0)
 VIRTUAL_SCREEN[(320*y)+x]=(texture.map[((texture.X_size*yt)+xt)]<<4)+(light&0x0F);
}

void flat_tex_scanline (text_map &texture,int i,char light)
{
     int dif;
     dif=RIGHT[i].x-LEFT[i].x;
        if (dif!=0) {
        int tx1=LEFT[i].X_texture;
        int ty1=LEFT[i].Y_texture;
        int tx2=RIGHT[i].X_texture;
        int ty2=RIGHT[i].Y_texture;
        int tx=tx1,ty=ty1;
        int tdx=tx2-tx1;
        int tdy=ty2-ty1;
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


void fill_flat_tex_triangle (triangle &T){
        point tri[3],aux;   // coordenadas do ecrÆ
        point mtri[3]; // coordenadas da textura
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
        int lines=tri[2].y;          //organiza (de cima para baixo)
        if (tri[1].y>tri[2].y) lines=tri[1].y;
        if (lines>200) lines=200;       //fim do poligno em y
        int i=tri[0].y;
        if (i<0) i=0;                   //inicio do poligno em y
        if (tri[0].y>200 || tri[2].y<0) return; // se est  fora do ecrÆ sai

/*        long int m2=(tri[1].y-tri[0].y)*(tri[2].x-tri[0].x);   //maior m:lado direito
        long int m1=(tri[2].y-tri[0].y)*(tri[1].x-tri[0].x);

        if (m1>m2)*/
        if (long((tri[2].y-tri[0].y)*(tri[1].x-tri[0].x)) >
            long((tri[1].y-tri[0].y)*(tri[2].x-tri[0].x)) )
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

/*-------------------ground shading ---------------------------------------*/

