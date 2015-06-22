#include "com_stevehavelka_assetbridge_Assetbridge.h"
#include <stdio.h>
#include <stdlib.h>


JNIEXPORT void JNICALL Java_com_stevehavelka_assetbridge_Assetbridge_setassetdir
  (JNIEnv * env, jclass thiz, jstring tmpPath){
  const char *path;
  char evar[200];

  path = (*env)->GetStringUTFChars( env, tmpPath , NULL );
  strcpy(evar, "ASSETDIR=");
  strcat(evar, path);
  putenv(evar);
}
