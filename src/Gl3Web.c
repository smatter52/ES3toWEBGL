// GL3 library to convert OPENGL ES 3 to WebGL2 script
// Copyright OXONICS 2024
#include <stdio.h>
#include <string.h>

#ifdef __BORLANDC__
 #include "gl3.h"
#else
 #include <GLES3/gl3.h>   // __RPI__
#endif

struct __GLsync {
  int val ;
}  sync ;

// 3 deferals for vertex setup
#define MAX_BUF_INDX 6
#define BIGBUF 8192
#define SMALLBUF 512

char scrp_pak[BIGBUF] ;
char scrp_buf[MAX_BUF_INDX][SMALLBUF] ;
int buf_indx = 0;
static int attrib_size ;  // components per vertex and texture attribute
static int attrib_type ;   // Component type, usually GL_FLOAT (currently assumed)
const void *attrib_pointer[2] ; // Pointer to attrib vertex or texture data

FILE *ofp = NULL ;

void send_wgl(void)
{
  if (ofp == NULL)
    printf("%s\n", scrp_pak) ;
  else
    { fprintf(ofp,"%s\n", scrp_pak) ;
      fflush(ofp) ;
    }
}

// Define js for location object array. These MUST be unique
/* Loc order is :
   position_loc
   texc_loc
   tflag_loc
   color4_loc
   texs_loc
   angle_loc
   radius_loc
   centre_ loc
   locm
   loct
*/

void init_jslocobj(int *loc)
{  char *locstr[] = {"position_loc","texc_loc","tflag_loc","color4_loc",
						  "texs_loc","angle_loc","radius_loc","centre_loc",
                    "locm","loct"} ;

   sprintf(scrp_pak, "const myloc = [%s,%s,%s,%s,%s,%s,%s,%s,%s,%s];",
                      locstr[*loc++],locstr[*loc++],locstr[*loc++],
                      locstr[*loc++],locstr[*loc++],locstr[*loc++],
                      locstr[*loc++],locstr[*loc++],locstr[*loc++],
                      locstr[*loc]) ;

   send_wgl() ;
}


void set_webgl_mapping(int width, int height)
{  char *script = "gl.uniformMatrix4fv" ;
   char *bools = "false" ;
   float mgl[16] = {1.0, 0.0, 0.0, 0.0,
                    0.0, 1.0, 0.0, 0.0,
                    0.0, 0.0, 1.0, 0.0,
                    -1.0, -1.0, 1.0, 1.0 };
   mgl[0] = 2.0 / (float)width ;
   mgl[5] = 2.0 / (float)height ;

   sprintf(scrp_pak,"var mgl = new Float32Array([%2.4f,%2.1f,%2.1f,%2.1f,%2.1f,%2.4f,%2.1f,%2.1f,\
%2.1f,%2.1f,%2.1f,%2.1f, %2.1f,%2.1f,%2.1f,%2.1f]);"
  ,mgl[0],mgl[1],mgl[2],mgl[3],mgl[4],mgl[5],mgl[6],mgl[7]
  ,mgl[8],mgl[9],mgl[10],mgl[11],mgl[12],mgl[13],mgl[14],mgl[15]) ;
   send_wgl() ;
// Matrix multiplication done in vertex shader
   sprintf(scrp_pak,"%s(myloc[0],%s,mgl);", script, bools) ;
   send_wgl() ;
}



// This function creates one texture. This is all I need
GL_APICALL void GL_APIENTRY glGenTextures(GLsizei n, GLuint *textures)
{  char *script = "gl.createTexture();" ;

   strcpy(scrp_pak, "var mytex;") ;
   send_wgl() ;
   sprintf(scrp_pak, "mytex = %s", script) ;
   send_wgl() ;
}

// This function creates one texture. This is all I need
GL_APICALL void GL_APIENTRY glDeleteTextures(GLsizei n, const GLuint *textures)
{  char *script = "gl.deleteTexture" ;

   sprintf(scrp_pak, "%s(mytex);",script) ;
   send_wgl() ;
}

GL_APICALL void GL_APIENTRY glBindTexture(GLenum target, GLuint texture)
{ char *script = "gl.bindTexture" ;

  sprintf(scrp_pak,"%s(%d, mytex);", script, target) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glActiveTexture(GLenum texture)
{ char *script = "gl.activeTexture" ;

  sprintf(scrp_pak,"%s(%d);", script, texture) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glClearColor(GLclampf red,GLclampf green,
                GLclampf blue,GLclampf alpha)
{ char *script = "gl.clearColor" ;

  sprintf(scrp_pak,"%s(%2.2f,%2.2f,%2.2f,%2.2f);", script, red, green, blue, alpha) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{ char *script = "gl.viewport" ;

  sprintf(scrp_pak,"%s(%d,%d,%d,%d);", script, x, y, width, height) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glClear(GLbitfield mask)
{ char *script = "gl.clear" ;

  sprintf(scrp_pak,"%s(%d);", script, mask) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glClearStencil(GLint st)
{ char *script = "gl.clearStencil" ;

  sprintf(scrp_pak,"%s(%d);", script, st) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glUniformMatrix4fv(GLint location, GLsizei count,
                                GLboolean transpose, const GLfloat *val)
{ char *script = "gl.uniformMatrix4fv" ;
  char *bools = "false" ;

  sprintf(scrp_pak,"var mgu = new Float32Array([%2.4f,%2.2f,%2.2f,%2.2f,%2.2f,%2.4f,%2.2f,%2.2f,\
%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f]);"
  ,val[0],val[1],val[2],val[3],val[4],val[5],val[6],val[7]
  ,val[8],val[9],val[10],val[11],val[12],val[13],val[14],val[15]) ;
  send_wgl() ;
  sprintf(scrp_pak,"%s(myloc[%d],%s,mgu);", script, location, bools) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glEnable(GLenum cap)
{ char *script = "gl.enable" ;

  sprintf(scrp_pak,"%s(%d);", script, cap) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glDisable(GLenum cap)
{ char *script = "gl.disable" ;

  sprintf(scrp_pak,"%s(%d);", script, cap) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glTexParameteri(GLenum target, GLenum pname, GLint param)
{ char *script = "gl.texParameteri" ;

  sprintf(scrp_pak,"%s(%d,%d,%d);", script, target, pname, param) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glUniform1i(GLint loc, GLint v0)
{ char *script = "gl.uniform1i" ;

  sprintf(scrp_pak,"%s(myloc[%d],%d);", script, loc, v0) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glUniform4f(GLint loc, GLfloat v0, GLfloat v1,
                                        GLfloat v2, GLfloat v3)
{ char *script = "gl.uniform4f" ;

  sprintf(scrp_pak,"%s(myloc[%d],%2.2f,%2.2f,%2.2f,%2.2f);", script, loc, v0, v1, v2, v3) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glUniform4fv(GLint loc, GLsizei count, const GLfloat *vals)
{ char *script = "gl.uniform4fv" ;
  int i ;
  char *s_p ;
  float *val_p ;

  val_p = (float *)vals ;
  strcpy(scrp_pak, "var uniformvar = new Float32Array([") ;
  for (i=0; i < count*4; i++)
   { s_p = scrp_pak + strlen(scrp_pak) ;
     sprintf(s_p,"%2.4f,", *(val_p+i)) ;
   }
  s_p = scrp_pak + strlen(scrp_pak) - 1 ;
  strcpy(s_p,"]);") ;
  send_wgl() ;

  sprintf(scrp_pak,"%s(myloc[%d],uniformvar);", script, loc) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei cnt)
{ char *script = "gl.drawArrays" ;
  char *s_p ;
  float *val_p ;
  int i ;
  char *limit ;

  // cnt gives the number of indices to be rendered ie cnt*attrib_size so
  limit = &scrp_pak[BIGBUF-32] ;
  val_p = (float *)attrib_pointer[0] ;
  strcpy(scrp_pak, "var vertices = new Float32Array([") ;
  for (i=0; i < attrib_size*cnt; i++)
   { s_p = scrp_pak + strlen(scrp_pak) ;
     if (s_p >= limit)
      { printf("String buffer length exceeded in DrawArrays with %d\n", attrib_size*cnt) ;
        break ;
      }
     else
      sprintf(s_p,"%2.1f,", *(val_p+i)) ;
   }
  s_p = scrp_pak + strlen(scrp_pak) - 1 ;
  strcpy(s_p,"]);") ;
  send_wgl() ;

  if (attrib_pointer[1] != NULL)
   { val_p = (float *)attrib_pointer[1] ;
     strcpy(scrp_pak, "var textures = new Float32Array([") ;
     for (i=0; i < attrib_size*cnt; i++)
      { s_p = scrp_pak + strlen(scrp_pak) ;
        if (s_p >= limit)
         { printf("String buffer length exceeded in DrawArrays with %d\n", attrib_size*cnt) ;
           break ;
         }
     else
      sprintf(s_p,"%2.1f,", *(val_p+i)) ;
      }
     s_p = scrp_pak + strlen(scrp_pak) - 1 ;
     strcpy(s_p,"]);") ;
     send_wgl() ;
   }

// Now flush the bombers
  buf_indx = 0 ;
  while (scrp_buf[buf_indx][0] != '\000' && buf_indx < MAX_BUF_INDX)
   { strcpy(scrp_pak, &scrp_buf[buf_indx][0]) ;
     send_wgl() ;
     buf_indx++ ;
   }
// Finally draw arrays
  sprintf(scrp_pak,"%s(%d,%d,%d);", script, mode, first, cnt) ;
  send_wgl() ;
// and reset
  buf_indx = 0 ;
  memset(scrp_buf, 0, MAX_BUF_INDX*256) ;
  attrib_pointer[0] = NULL ;
  attrib_pointer[1] = NULL ;
}

GL_APICALL void GL_APIENTRY glVertexAttribPointer(GLuint indx, GLint size,
                            GLenum type, GLboolean norm, GLsizei stride,
                            const void *pointer)
{ char *script = "gl.vertexAttribPointer" ;
  int i ;

// Don't you just love globals
  attrib_size = size ;
  attrib_type = type ;
  if (buf_indx == 0)
    { attrib_pointer[0] = pointer ;
      memset(scrp_buf, 0, MAX_BUF_INDX*256) ;
    }
  else if (buf_indx = 3)
    attrib_pointer[1] = pointer ;

// Need to buffer these strings UNTIL drawarrays can set up vertex array
// and textures array (textures required).

  if (attrib_pointer[0] != NULL && buf_indx == 0)
   { strcpy(&scrp_buf[buf_indx][0], "gl.bindBuffer(gl.ARRAY_BUFFER, gl.createBuffer());") ;
     buf_indx++ ;
     strcpy(&scrp_buf[buf_indx][0], "gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.DYNAMIC_DRAW);") ;
     buf_indx++ ;
// indx is a position_loc or texc_loc value
     sprintf(&scrp_buf[buf_indx][0],"%s(myloc[%d],%d,%d, false, 0, 0);", script, indx, size, type) ;
     buf_indx++ ;
   }

  if (attrib_pointer[1] != NULL && buf_indx == 3)
   { strcpy(&scrp_buf[buf_indx][0], "gl.bindBuffer(gl.ARRAY_BUFFER, gl.createBuffer());") ;
     buf_indx++ ;
     strcpy(&scrp_buf[buf_indx][0], "gl.bufferData(gl.ARRAY_BUFFER, textures, gl.DYNAMIC_DRAW);") ;
     buf_indx++ ;
     sprintf(&scrp_buf[buf_indx][0],"%s(myloc[%d],%d,%d, false, 0, 0);", script, indx, size, type) ;
   }

}


GL_APICALL void GL_APIENTRY glEnableVertexAttribArray(GLuint indx)
{ char *script = "gl.enableVertexAttribArray" ;

   sprintf(scrp_pak,"%s(myloc[%d]);", script, indx) ;
   send_wgl() ;
}


GL_APICALL void GL_APIENTRY glDisableVertexAttribArray(GLuint indx)
{ char *script = "gl.disableVertexAttribArray" ;

  sprintf(scrp_pak,"%s(myloc[%d]);", script, indx) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glPixelStorei(GLenum pnam, GLint param)
{ char *script = "gl.pixelStorei" ;

  sprintf(scrp_pak,"%s(%d,%d);", script, pnam, param) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glBlendEquationSeparate(GLenum mRGB, GLenum mAlpha)
{ char *script = "gl.blendEquationSeparate" ;

  sprintf(scrp_pak,"%s(%d,%d);", script, mRGB, mAlpha) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glBlendEquation(GLenum mode)
{ char *script = "gl.blendEquation" ;

  sprintf(scrp_pak,"%s(%d);", script, mode) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glBlendFunc(GLenum sfactor, GLenum dfactor)
{ char *script = "gl.blendFunc" ;

  sprintf(scrp_pak,"%s(%d,%d);", script, sfactor, dfactor) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glBlendFuncSeparate(GLenum sRGB, GLenum dRGB,
 																GLenum sAlpha, GLenum dAlpha)
{ char *script = "gl.blendFuncSeparate" ;

  sprintf(scrp_pak,"%s(%d,%d,%d,%d);", script, sRGB, dRGB, sAlpha, dAlpha) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glTexImage2D(GLenum tgt, GLint lvl, GLint iFmt,
                   GLsizei width, GLsizei height, GLint bdr, GLenum fmt,
                   GLenum typ, const void *data)
{ char *script = "gl.texImage2D" ;
  int i ;
  char *s_p ;
  unsigned char *val_p ;
  char *limit ;

// Assume iFMT and typ is GL_RGBA
  val_p = (unsigned char *)data ;
  limit = &scrp_pak[BIGBUF-8] ;
  strcpy(scrp_pak, "var texdat = new Uint8Array([") ;
  for (i=0; i < width*height*4; i++)
   { s_p = scrp_pak + strlen(scrp_pak) ;
     if (s_p >= limit)
      { printf("String buffer length exceeded in TexImage with %d\n", width*height) ;
        break ;
      }
     else
      sprintf(s_p,"%u,", *(val_p+i)) ;
   }
  s_p = scrp_pak + strlen(scrp_pak) - 1 ;
  strcpy(s_p,"]);") ;
  send_wgl() ;

  sprintf(scrp_pak,"%s(%d,%d,%d,%d,%d,%d,%d,%d,texdat);", script, tgt, lvl,
                iFmt, width, height, bdr, fmt, typ) ;
// sprintf(scrp_pak,"%s(%d,%d,gl.RGBA,%d,%d,%d,gl.RGBA,gl.UNSIGNED_BYTE,texdat);", script, tgt, lvl,
//                 width, height, bdr) ;
   send_wgl() ;
}

GL_APICALL void GL_APIENTRY glTexSubImage2D(GLenum tgt, GLint lvl, GLint iFmt,
                   GLsizei width, GLsizei height, GLint bdr, GLenum fmt,
                   GLenum typ, const void *data)
{ char *script = "gl.texSubImage2D" ;
  int i ;
  char *s_p ;
  unsigned long *val_p ;

// Assume iFMT and typ is GL_RGBA
  val_p = (unsigned long *)data ;
  strcpy(scrp_pak, "var texdat_s = new Uint32Array([") ;
  for (i=0; i < width*height; i++)
   { s_p = scrp_pak + strlen(scrp_pak) ;
     sprintf(s_p,"%lu,", *(val_p+i)) ;
   }
  s_p = scrp_pak + strlen(scrp_pak) - 1 ;
  strcpy(s_p,"]);") ;
  send_wgl() ;

  sprintf(scrp_pak,"%s(%d,%d,%d,%d,%d,%d,%d,%d,texdat_s);", script, tgt, lvl,
                iFmt, width, height, bdr, fmt, typ) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{ char *script = "gl.scissor" ;

  sprintf(scrp_pak,"%s(%d,%d,%d,%d);", script, x, y, width, height) ;
  send_wgl() ;
}

// Dummy function. If called from C the entire c function will need to
// be emulated in js.
GL_APICALL void GL_APIENTRY glReadPixels(GLint x, GLint y, GLsizei width,
                      GLsizei height, GLenum fmt, GLenum type, void *data)
{ char *script = "gl.readPixels" ;

}

GL_APICALL void GL_APIENTRY glColorMask(GLboolean red, GLboolean green,
													 GLboolean blue, GLboolean alpha)
{ char *script = "gl.colorMask" ;
  char *r_s, *g_s, *b_s, *a_s ;
  char *true_s = "true" ; char *false_s = "false" ;
  r_s = (red) ? true_s : false_s ;
  g_s = (green) ? true_s : false_s ;
  b_s = (blue) ? true_s : false_s ;
  a_s = (alpha) ? true_s : false_s ;

  sprintf(scrp_pak,"%s(%s,%s,%s,%s);", script, r_s, g_s, b_s, a_s) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glFlush()
{ char *script = "gl.flush()" ;

  sprintf(scrp_pak,"%s;", script) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glFinish()
{ char *script = "gl.finish()" ;

  sprintf(scrp_pak,"%s;", script) ;
  send_wgl() ;
}

GL_APICALL GLboolean GL_APIENTRY glIsTexture(GLuint tex)
{ char *script = "gl.isTexture" ;

  strcpy(scrp_pak, "var mybool;") ;
  send_wgl() ;

  sprintf(scrp_pak,"mybool = %s(%d);", script, tex) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glBlitFramebuffer(GLint sx0, GLint sy0, GLint sx1, GLint sy1,
										GLint dx0, GLint dy0, GLint dx1, GLint dy1,
                              GLbitfield mask, GLenum filter)
{ char *script = "gl.blitFramebuffer" ;

  sprintf(scrp_pak,"%s(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d);",
                   sx0,sy0,sx1,sy1,dx0,dy0,dx1,dy1,mask,filter) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glStencilMask(GLuint mask)
{ char *script = "gl.stencilMask" ;

  sprintf(scrp_pak,"%s(%d);", script, mask) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glStencilFunc(GLenum func, GLint ref, GLuint mask)
{ char *script = "gl.stencilFunc" ;

  sprintf(scrp_pak,"%s(%d,%d,%d);", script, func, ref, mask) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass)
{ char *script = "gl.stencilOp" ;

  sprintf(scrp_pak,"%s(%d,%d,%d);", script, sfail, dpfail, dppass) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glUniform1f(GLint loc, GLfloat v0)
{ char *script = "gl.Uniform1F" ;

  sprintf(scrp_pak,"%s(%d,%2.2f);", script, loc, v0) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glUniform2f(GLint loc, GLfloat v0, GLfloat v1)
{ char *script = "gl.Uniform2F" ;

  sprintf(scrp_pak,"%s(%d,%2.2f,%2.2f);", script, loc, v0, v1) ;
  send_wgl() ;
}

GL_APICALL void GL_APIENTRY glDepthMask(GLboolean mask)
{ char *script = "gl.depthMask" ;

  if (mask != 0)
   sprintf(scrp_pak,"%s(true);", script) ;
  else
   sprintf(scrp_pak,"%s(false);", script) ;

  send_wgl() ;

}

GL_APICALL GLenum GL_APIENTRY glGetError()
{ char *script = "gl.getError" ;

   strcpy(scrp_pak, "var err;") ;
   send_wgl() ;
   sprintf(scrp_pak, "err = %s()", script) ;
   send_wgl() ;

  send_wgl() ;

}

#ifndef __RPI__
void main()
{ GLfloat mgu[16] = {1.0,0.0,0.0,0.0 ,0.0,1.0,0.0,0.0, 0.0,0.0,1.0,0.0, 0.0,0.0,0.0,1.0};
  GLuint tex ;
  long tex_p[] = {0x1234,0x4567,0x1456, 0x8978} ;
  int loc[] = {8,9,2,3,7,4,5,6,0,1} ;
/*
  glActiveTexture(GL_TEXTURE0) ;
  glBindTexture(0, GL_TEXTURE0);
  glClearColor(0.1,0.2,0.3,1.0) ;
  glViewport(0,0,640,480) ;
  glClear(GL_COLOR_BUFFER_BIT) ;
  glEnable(GL_BLEND) ;
  glDisable(GL_BLEND) ;
  glGenTextures(1, &tex) ;
  glDeleteTextures(1, &tex) ;
  glTexParameteri(1,2,3) ;
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2,
              2, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_p);
  glUniformMatrix4fv(1, 1, GL_FALSE , (GLfloat *) mgu );
  glUniform1i(0, 2) ;
  glUniform4f(3, 1.1, 2.2, 3.3, 4.4) ;
  glUniform2f(3, 5.5, 6.6) ;
  init_jslocobj(loc) ;
  set_webgl_mapping(800,600) ;
*/

float vertices[] = {263.00,405.00,489.00,405.00,489.00,481.00,263.00,481.00};
float textures[] = {1.0,1.0,0.0,1.0,1.0,0.0,0.0,0.0};
glVertexAttribPointer(8,2,5126, 0, 0, (void *)&vertices);
glEnableVertexAttribArray(8);
// glVertexAttribPointer(6,2,5126, 0, 0, (void *)&textures);
// glEnableVertexAttribArray(6);
glDrawArrays(6,0,4);
}
#endif
