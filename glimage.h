/*!\file glloadimage.h
 *
 * \brief glImage : lit une image et genere un id de texture
 *
 * \auteur : Vincent Boyer boyer@ai.univ-paris8.fr 
 *
 * \date : November, 2013
 */


#ifndef __GLIMAGE_H__
#define __GLIMAGE_H__

#if defined(__APPLE__) || defined(MACOSX)
# include <OpenGL/gl.h> 
#else
#define  GL_GLEXT_PROTOTYPES
# include <GL/gl.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern GLuint glimageLoadAndBind (const char *);
extern int glimageLoadAndBind2 (const char *,GLuint *);

#ifdef __cplusplus
} // extern C
#endif

#endif
